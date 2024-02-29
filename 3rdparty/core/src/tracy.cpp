//
//          Tracy profiler
//         ----------------
//
// For fast integration, compile and
// link with this source file (and none
// other) in your executable (or in the
// main DLL / shared object on multi-DLL
// projects).
//

// Define TRACY_ENABLE to enable profiler.
#include "tracy.hpp"
#include <Tracy/common/TracySystem.cpp>

#ifdef TRACY_ENABLE

#ifdef _MSC_VER
#  pragma warning(push, 0)
#endif

#include <Tracy/common/tracy_lz4.cpp>
#include <Tracy/client/TracyProfiler.cpp>
#include <Tracy/client/TracyCallstack.cpp>
#include <Tracy/client/TracySysPower.cpp>
#include <Tracy/client/TracySysTime.cpp>
#include <Tracy/client/TracySysTrace.cpp>
#include <Tracy/common/TracySocket.cpp>
#include <Tracy/client/tracy_rpmalloc.cpp>
#include <Tracy/client/TracyDxt1.cpp>
#include <Tracy/client/TracyAlloc.cpp>
#include <Tracy/client/TracyOverride.cpp>

#if TRACY_HAS_CALLSTACK == 2 || TRACY_HAS_CALLSTACK == 3 || TRACY_HAS_CALLSTACK == 4 || TRACY_HAS_CALLSTACK == 6
#  include <Tracy/libbacktrace/alloc.cpp>
#  include <Tracy/libbacktrace/dwarf.cpp>
#  include <Tracy/libbacktrace/fileline.cpp>
#  include <Tracy/libbacktrace/mmapio.cpp>
#  include <Tracy/libbacktrace/posix.cpp>
#  include <Tracy/libbacktrace/sort.cpp>
#  include <Tracy/libbacktrace/state.cpp>
#  if TRACY_HAS_CALLSTACK == 4
#    include <Tracy/libbacktrace/macho.cpp>
#  else
#    include <Tracy/libbacktrace/elf.cpp>
#  endif
#  include <Tracy/common/TracyStackFrames.cpp>
#endif

#ifdef _MSC_VER
#  pragma comment(lib, "ws2_32.lib")
#  pragma comment(lib, "dbghelp.lib")
#  pragma comment(lib, "advapi32.lib")
#  pragma comment(lib, "user32.lib")
#  pragma warning(pop)
#endif

#endif
