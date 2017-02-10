#ifndef BUILTIN_HEADER
#define BUILTIN_HEADER

/* Forward declare dependencies */
struct lenv;
struct lval;
typedef struct lenv lenv;
typedef struct lval lval;

/*===================================== Declared Functions =====================================*/

/* Builtin Infrastructure */
lval* builtin(lenv* e, lval* a, char* func);

/* Variable Setters */
lval* builtin_var(lenv* e, lval* a, char* func);
lval* builtin_def(lenv* e, lval* a);
lval* builtin_put(lenv* e, lval* a);

/* Lambda expressions */
lval* builtin_lambda(lenv* e, lval* a);

/* List Operators */
lval* builtin_list(lenv* e, lval* a);
lval* builtin_head(lenv* e, lval* a);
lval* builtin_tail(lenv* e, lval* a);
lval* builtin_eval(lenv* e, lval* a);
lval* builtin_join(lenv* e, lval* a);

/* Arithmetic Operators */
lval* builtin_op(lenv* e, lval* a, char* op);
lval* builtin_add(lenv* e, lval* a);
lval* builtin_sub(lenv* e, lval* a);
lval* builtin_mul(lenv* e, lval* a);
lval* builtin_div(lenv* e, lval* a);

lval* builtin_mod(lenv* e, lval* a);
lval* builtin_exp(lenv* e, lval* a);

/* Ordering Operators */
lval* builtin_ord(lenv* e, lval* a, char* op);
lval* builtin_gt(lenv* e, lval* a);
lval* builtin_lt(lenv* e, lval* a);
lval* builtin_ge(lenv* e, lval* a);
lval* builtin_le(lenv* e, lval* a);

/* Equality Operators */
lval* builtin_cmp(lenv* e, lval* a, char* op);
lval* builtin_eq(lenv* e, lval* a);
lval* builtin_ne(lenv* e, lval* a);

/* Conditional branching */
lval* builtin_if(lenv* e, lval* a);

/* Load, print, and error functions */
lval* builtin_load  (lenv* e, lval* a);
lval* builtin_print (lenv* e, lval* a);
lval* builtin_error (lenv* e, lval* a);

#endif