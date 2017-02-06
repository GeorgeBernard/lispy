#ifndef BUILTIN_HEADER
#define BUILTIN_HEADER

/* Forward declare dependencies */
struct lenv;
struct lval;
typedef struct lenv lenv;
typedef struct lval lval;

/*===================================== Declared Functions =====================================*/

lval* builtin(lenv* e, lval* a, char* func);
lval* builtin_op(lenv* e, lval* a, char* op);
lval* builtin_var(lenv* e, lval* a, char* func);
lval* builtin_def(lenv* e, lval* a);
lval* builtin_put(lenv* e, lval* a);
lval* builtin_lambda(lenv* e, lval* a);
lval* builtin_head(lenv* e, lval* a);
lval* builtin_tail(lenv* e, lval* a);
lval* builtin_list(lenv* e, lval* a);
lval* builtin_eval(lenv* e, lval* a);
lval* builtin_add(lenv* e, lval* a);
lval* builtin_sub(lenv* e, lval* a);
lval* builtin_mul(lenv* e, lval* a);
lval* builtin_div(lenv* e, lval* a);
lval* builtin_join(lenv* e, lval* a);

#endif