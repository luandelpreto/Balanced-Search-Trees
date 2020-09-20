//=========================================================
// Assert implementation.
//
// Implementation details:
// The  standard  requires  that  header   assert.h  define
// assert(e) as a macro  that  provides  diagnostic informa_
// tion. assert(e)  evaluates e and, if e is  zero,  writes
// diagnostic information on the standard error and  aborts
// execution by calling the standard library function abort.
// The diagnostic information includes  the assertion  that
// failed (the text of e) and the coordinates (the file and
// line number) at which the assert(e) appears. The  format
// of this information is implementation defined.
// assert.h  also  uses,  but does not define,   the  macro
// NDEBUG. If NDEBUG  is defined,  then assert(e)  must  be
// equivalent  to  the  vacuous expression ((void)0).  Thus,
// programmers can turn off assertions by  defining  NDEBUG
// and recompiling. Since e might not be executed, it's  im_
// portant that it never be an  essential computation  that
// has side effects, such as an assignment.
// An expression like e1 || e2 usually appears in  conditio_
// nal contexts, such as  if statements,  but  it can  also
// appear alone as a statement. When it does, the effect is
// equivalent to the statements
//             if (!(e1)) e2;
// The definition of assert uses e1 || e2 because assert(e)
// must expand into an expression, not a statement. e2 is a
// comma expression whose result is a value, which is requi_
// red by the ||  operator,  and the  entire expression  is
// cast  to   void  because the   standard stipulates  that
// assert(e) returns no value.
// Assert mimics the standard's definitions so that the two
// assert.h headers can be  used interchangeably,  which is
// why Assert_Failed appears in except.h.
//=========================================================
#include "assert.h"

const Except_T Assert_Failed = {"Assertion failed"};

void (assert)(int e)
{
    assert(e);
}
