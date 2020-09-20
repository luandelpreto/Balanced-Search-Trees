//=========================================================
// Exception handling implementation.
//
// Implementation details:
// The TRY clause pushes a new Except_Frame onto the  excep_
// tion stack  and calls  setjmp.  Except_raise,  which  is
// called by RAISE and  RERAISE,  fills  in  the  exception,
// file and  line  fields in  the   top  frame,   pops  the
// Except_Frame off the exception stack, and calls  longjmp.
// EXCEPT clauses test the exception field of this frame to
// determine which handler applies. The FINALLY clause  exe_
// cutes its clean-up  code and reraises the  exception sto_
// red in the popped frame.
// If an exception occurs and  control reaches  an  END_TRY
// clause without handling it, the exception is reraised.
//
// The first return from setjmp  is  zero,  so it  sets the
// Except_flag  to  Except_entered,  which indicates that a
// TRY statement has been  entered and an  exception  frame
// has been pushed  onto  the exception  stack.  Subsequent
// returns from  setjmp  set Except_flag  to  Except_raised,
// which indicates that an exception occurred. Handlers set
// Except_flag to  Except_handled to indicate  that they've
// handled the exception.
//
// The if statement right at the first line  of the  EXCEPT
// macro pops the exception stack only in the  first EXCEPT
// clause. If no exception occurs while executing S, Except_
// flag  remains Except_entered , so  when control  reaches
// the if statement, the exception stack is popped.  The se_
// cond and subsequent  EXCEPT  clauses follow handlers  in
// which  Except_flag  has  been changed to  Except_handled.
// For these, the exception  stack has already  been popped,
// and this starting if statement protects against  popping
// it again.
//
// In  the FINALLY  clause,  Except_flag  is  changed  from
// Except_entered to Except_finalized to  indicate that  an
// exception did not occur but  that a FINALLY  clause  did
// appear. If an exception occurred, Except_flag is left at
// Except_raised so that it can be reraised after the clean-
// up code has been executed in FINALLY.
//
// The exception is reraised by testing whether Except_flag
// is equal to Except_raise in the expansion of END_TRY. if
// an  exception   did  not  occur,   Except_flag  will  be
// Except_entered or Except_finalized.
//=========================================================
#include "except.h"
#include "assert.h"
#include <stdio.h>
#include <stdlib.h>

Except_Frame *Except_stack = 0;

void Except_raise(const Except_T *e, const char *restrict file, unsigned long line)
{
    assert(e);

    Except_Frame *p = Except_stack;
    if (!p) {
        fprintf(stderr, "Uncaught exception ");
        if (e->reason)
            fprintf(stderr, "%s", e->reason);
        else
            fprintf(stderr, " at 0x%p", (void *) e);
        if (file && line > 0)
            fprintf(stderr, " raised at %s: %lu\n", file, line);

        fprintf(stderr, "aborting...\n");
        fflush(stderr);
        abort();
    }
    p->exception = e;
    p->file = file;
    p->line = line;
    Except_stack = Except_stack->prev;
    longjmp(p->env, Except_raised);
}
