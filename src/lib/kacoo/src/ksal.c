#include <ksal.h>

#if defined(__WIN32__)
#include "../sysdeps/win32/ksal.c"
#elif defined (_WIN32_WCE)
#include "../sysdeps/winCE/ksal.c"
#elif defined(__IVISA__)
#include "../sysdeps/ivisa/ksal.c"
#elif defined(__UNIX__)
#include "../sysdeps/unix/ksal.c"
#elif defined(__LIB__)
#include "../sysdeps/lib/ksal.c"
#else
#error What platform do you want?
#endif

