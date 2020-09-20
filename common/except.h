//==========================================================
// Interface for Exception handling.
// This interface wraps the setjmp/longjmp facility in a set
// of macros  and functions  that collaborate  to provide  a
// structured exception facility.
//
// Exception handlers manipulate the addresses of exceptions.
// Exceptions must  be global  or static  variables so  that
// their  addresses  identify   them   uniquely.  It  is  an
// unchecked runtime error to declare an exception as  a  lo_
// cal variable or as a parameter.
// An exception e is raised  by the  RAISE macro  or by  the
// function Except_raise.
// Handlers  are   instantiated   by   the   TRY-EXCEPT  and
// TRY-FINALLY statements, which are implemented with macros.
// These  statements  handle  nested exceptions  and  manage
// exception-state data.
//
// The syntax of the TRY-EXCEPT statement is
// TRY
//     S
// EXCEPT(e1)
//     S1
// EXCEPT(e2)
//     S2
// ...
// EXCEPT(en)
//     Sn
// ELSE
//     S0
// END_TRY
//
// This TRY-EXCEPT statement establishes handlers for  excep_
// tions named e1, e2, ..., en, and executes the statement S.
// If no exceptions are raised by S, the handlers are disman_
// tled and execution continues at the statement  after  the
// END_TRY. If S raises and exception  e where e is  one  of
// e1-en,  the  execution of  S is  interrupted and  control
// transfers immediately to the statements following the  re_
// levant EXCEPT clause. The  handlers are  dismantled,  the
// handler statements Si in  the EXCEPT clause are  executed,
// and execution continues after the END_TRY.
// If S raises an exception that is  not one of  e1-en,  the
// handlers are  dismantled, the statements  following  ELSE
// are executed, and execution  continues after the  END_TRY.
// The ELSE clause is optional.
// If S raises an exception that  is not handled by  one  of
// the Si, the handlers are dismantled, and the exception is
// passed to  the  handlers  established by  the  previously
// executed TRY-EXCEPT or TRY-FINALLY statement.
// TRY-END_TRY is  syntactically  equivalent to a  statement.
// TRY introduces a new scope,  which ends at the  correspon_
// ding END_TRY.
//
// The syntax of the TRY-FINALLY statement is
// TRY
//     S
// FINALLY
//     S1
// END_TRY
//
// If no  exceptions  are  raised by S, S1 is  executed  and
// execution  continues at  the statement after the  END_TRY.
// If  S  raises  an  exception,   the  execution  of  S  is
// interrupted  and  control  transfers  immediately  to  S1.
// After S1 is executed, the exception that caused its execu_
// tion is reraised so that it can be handled by a  previous_
// ly instantiated handler. Note that S1 is executed in both
// cases. Handlers can  reraise exceptions  explicitly  with
// the RERAISE macro.
// The TRY-FINALLY statement is equivalent to
// TRY
//     S
// ELSE
//     S1
//     RERAISE;
// END_TRY
// S1
//
// If S1 in a TRY-FINALLY  statement or any of the  handlers
// in  a  TRY-EXCEPT statement  raises an  exception,  it is
// handled by the previously instantiated handler.
//
// The RETURN macro is used instead of return statements  in_
// side TRY statements. It is an unchecked  runtime error to
// execute the C  return  statement inside a  TRY-EXCEPT  or
// TRY-FINALLY statement. If any of the statements in any of
// these two statements must  do a  return, they must do  so
// with  this  macro instead  of  with  the usual  C  return
// statement.
//==========================================================
#ifndef EXCEPTION_H
#define EXCEPTION_H 1

#include <setjmp.h>
#include <stddef.h>

enum { Except_entered = 0,
	   Except_raised,
	   Except_handled,
	   Except_finalized,
   };

#define T Except_T
typedef struct T {
	const char *reason;
} T;

typedef struct Except_Frame Except_Frame;
struct Except_Frame {
	Except_Frame *prev;
	jmp_buf env;
	const char *file;
	unsigned long line;
	const T *exception;
};

extern Except_Frame *Except_stack;
extern const Except_T Assert_Failed;

void Except_raise(const T *e, const char *restrict file, unsigned long line);

#define RAISE(e) Except_raise(&(e), __FILE__, __LINE__+0UL)
#define RERAISE Except_raise(Except_frame.exception, Except_frame.file, Except_frame.line)
#define RETURN switch(Except_stack = Except_stack->prev, 0) default: return

#define TRY do {                                         \
	Except_Frame Except_frame;                           \
	Except_frame.prev = Except_stack;                    \
	Except_stack = &Except_frame;                        \
	volatile int Except_flag = setjmp(Except_frame.env); \
	if (Except_flag == Except_entered) {

#define EXCEPT(e)                                        \
		if (Except_flag == Except_entered)               \
			Except_stack = Except_stack->prev;           \
	}                                                    \
	else if (Except_frame.exception == &(e)) {           \
		Except_flag = Except_handled;

#define ELSE                                             \
		if (Except_flag == Except_entered)               \
			Except_stack = Except_stack->prev;           \
	}                                                    \
	else {                                               \
		Except_flag = Except_handled;

#define FINALLY                                          \
		if (Except_flag == Except_entered)               \
			Except_stack = Except_stack->prev;           \
	} {                                                  \
		if (Except_flag == Except_entered)               \
			Except_flag = Except_finalized;

#define END_TRY                                          \
		if (Except_flag == Except_entered)               \
			Except_stack = Except_stack->prev;           \
	}                                                    \
	if (Except_flag == Except_raised) {                  \
		RERAISE;                                         \
	}                                                    \
} while(0)

#undef T
#endif
