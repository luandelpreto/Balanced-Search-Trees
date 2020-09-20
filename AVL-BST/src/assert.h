//=========================================================
// Assert Interface.
// This interface  defines  assert(e) as specified  by  the
// standard, except that  an assertion  failure raises  the
// exception  Assert_Failed  instead of aborting  execution,
// and does not provide the text of the assertion e.
//=========================================================
#undef assert
#ifdef NDEBUG
#define assert(e) ((void)0)
#else
#include "except.h"
extern void assert(int e);
#define assert(e) ((void)((e)||(RAISE(Assert_Failed),0)))
#endif
