#! /usr/bin/env python3

import os
import subprocess
import sys
import argparse
import re
from glob import glob
from collections import defaultdict
import multiprocessing
from multiprocessing.pool import ThreadPool
import threading

from test_utils import (
    base_path,
    pyboard,
    test_instance_description,
    test_instance_epilog,
    test_directory_description,
    get_test_instance,
    TEST_TIMEOUT,
)

if os.name == "nt":
    MICROPYTHON = os.getenv(
        "MICROPY_MICROPYTHON", "../ports/windows/build-standard/micropython.exe"
    )
    CPYTHON3 = os.getenv("MICROPY_CPYTHON3", "python3")
else:
    MICROPYTHON = os.getenv("MICROPY_MICROPYTHON", "../ports/unix/build-standard/micropython")
    CPYTHON3 = os.getenv("MICROPY_CPYTHON3", "python3")

MICROPYTHON_CMD = [MICROPYTHON, "-X", "emit=bytecode"]
CPYTHON3_CMD = [CPYTHON3, "-BS"]


injected_bench_code = b"""
import time

class bench_class:
    ITERS = 20000000

    @staticmethod
    def run(test):
        t = time.ticks_us()
        test(bench_class.ITERS)
        t = time.ticks_diff(time.ticks_us(), t)
        s, us = divmod(t, 1_000_000)
        print("{}.{:06}".format(s, us))

import sys
sys.modules['bench'] = bench_class
"""


def execbench(test_instance, filename, iters):
    with open(filename, "rb") as f:
        pyfile = f.read()
    code = (injected_bench_code + pyfile).replace(b"20000000", str(iters).encode("utf-8"))
    return test_instance.exec(code, timeout=TEST_TIMEOUT).replace(b"\r\n", b"\n")


def run_tests(test_instance, test_dict, iters, num_threads=1, pyb_pool=None):
    # Assign each worker thread its own pyboard instance for parallel exec: targets.
    _worker_pyb = threading.local()
    _pyb_iter = iter(pyb_pool) if pyb_pool else None

    def _init_worker():
        if _pyb_iter is not None:
            _worker_pyb.pyb = next(_pyb_iter)

    def run_one_test(entry):
        _, test_file = entry
        this_test_instance = (
            getattr(_worker_pyb, "pyb", None) if _pyb_iter is not None else test_instance
        )
        error_info = None
        if isinstance(this_test_instance, list):
            # run on PC
            try:
                output_mupy = subprocess.check_output(this_test_instance + [test_file])
            except subprocess.CalledProcessError:
                output_mupy = b"CRASH"
        else:
            # run on pyboard
            this_test_instance.enter_raw_repl()
            try:
                output_mupy = execbench(this_test_instance, test_file, iters)
            except pyboard.PyboardError as er:
                output_mupy = b"CRASH"
                error_info = er

        raw_output = output_mupy
        try:
            timing = float(output_mupy.strip())
        except ValueError:
            timing = -1
        return test_file, timing, raw_output, error_info

    def report(test_file, timing, raw_output, error_info):
        if timing == -1:
            print("    %s: -1 (full output follows)" % test_file)
            if error_info is not None:
                for part in error_info.args:
                    if isinstance(part, bytes):
                        print(part.decode("utf-8", "replace"))
                    else:
                        print(part)
            else:
                print(raw_output.decode("utf-8", "replace"))
        else:
            print("    %.3fs %s" % (timing, test_file))

    flat_tests = [
        (base_test, test_file[0])
        for base_test, tests in sorted(test_dict.items())
        for test_file in tests
    ]

    if not isinstance(test_instance, list) and pyb_pool is None:
        num_threads = 1
    if pyb_pool is not None:
        num_threads = min(num_threads, len(pyb_pool))

    if num_threads > 1:
        pool = ThreadPool(num_threads, initializer=_init_worker)
        # imap_unordered with chunksize=1 pulls one test at a time from a
        # shared queue, so a single long-running test can't strand a whole
        # chunk of subsequent tests behind it on one worker's pyboard.
        for result in pool.imap_unordered(run_one_test, flat_tests, chunksize=1):
            report(*result)
    else:
        for entry in flat_tests:
            report(*run_one_test(entry))

    return True


def main():
    cmd_parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=f"""Run and manage tests for MicroPython.

{test_instance_description}
{test_directory_description}
""",
        epilog=f"""{test_instance_epilog}- cpython - use CPython to run the benchmarks instead\n""",
    )
    cmd_parser.add_argument(
        "-t", "--test-instance", default="unix", help="the MicroPython instance to test"
    )
    cmd_parser.add_argument(
        "-b", "--baudrate", default=115200, help="the baud rate of the serial device"
    )
    cmd_parser.add_argument("-u", "--user", default="micro", help="the telnet login username")
    cmd_parser.add_argument("-p", "--password", default="python", help="the telnet login password")
    cmd_parser.add_argument(
        "-d", "--test-dirs", nargs="*", help="input test directories (if no files given)"
    )
    cmd_parser.add_argument(
        "-I",
        "--iters",
        type=int,
        default=200_000,
        help="number of test iterations, only for remote instances (default 200,000)",
    )
    cmd_parser.add_argument(
        "-j",
        "--jobs",
        default=multiprocessing.cpu_count(),
        metavar="N",
        type=int,
        help="Number of tests to run simultaneously",
    )
    cmd_parser.add_argument("files", nargs="*", help="input test files")
    args = cmd_parser.parse_args()

    if args.test_instance == "cpython":
        test_instance = CPYTHON3_CMD
    else:
        # Note pyboard support is copied over from run-tests.py, not tests, and likely needs revamping
        test_instance = get_test_instance(
            args.test_instance, args.baudrate, args.user, args.password
        )
        if test_instance is None:
            test_instance = MICROPYTHON_CMD

    # For exec: targets, spawn additional instances for parallel test execution.
    pyb_pool = None
    if args.test_instance.startswith("exec:") and args.jobs > 1:
        pyb_pool = [test_instance] + [
            get_test_instance(args.test_instance, args.baudrate, args.user, args.password)
            for _ in range(args.jobs - 1)
        ]

    if len(args.files) == 0:
        if args.test_dirs:
            test_dirs = tuple(args.test_dirs)
        else:
            test_dirs = ("internal_bench",)

        tests = sorted(
            test_file
            for test_files in (glob("{}/*.py".format(dir)) for dir in test_dirs)
            for test_file in test_files
        )
    else:
        # tests explicitly given
        tests = sorted(args.files)

    test_dict = defaultdict(lambda: [])
    for t in tests:
        m = re.match(r"(.+?)-(.+)\.py", t)
        if not m:
            continue
        test_dict[m.group(1)].append([t, None])

    try:
        if not run_tests(test_instance, test_dict, args.iters, args.jobs, pyb_pool):
            sys.exit(1)
    finally:
        if pyb_pool:
            for p in pyb_pool:
                p.close()
        elif not isinstance(test_instance, list):
            test_instance.close()


if __name__ == "__main__":
    main()
