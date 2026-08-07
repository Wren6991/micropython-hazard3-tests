// Minimal newlib reentrancy stubs
//
// newlib's libm (linked for soft-float math) references __errno and
// _impure_ptr, which normally live in newlib's libc. This port is built
//  with -nostdlib, so supply the bare minimum needed by libm here.

#include <reent.h>

struct _reent reent_stub_reent;

struct _reent *_impure_ptr = &reent_stub_reent;

int *__errno(void) {
    return &_impure_ptr->_errno;
}
