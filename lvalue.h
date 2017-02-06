#ifndef LVALUE_HEADER
#define LVALUE_HEADER

/* Forward declare dependencies */
typedef struct mpc_ast_t mpc_ast_t;

struct lenv;
struct lval;
typedef struct lenv lenv;
typedef struct lval lval;
typedef lval*(*lbuiltin)(lenv*, lval*);

/*===================================== Struct Definitions =====================================*/

struct lval
{
	int type;

	/* Basic */
	long num;
	char* err;
	char* sym;

	/* Function */
	lbuiltin builtin;
	lenv* env;
	lval* formals;
	lval* body;
	
	/* Expression */
	int count;
	struct lval** cell;
};

/*===================================== Declared Functions =====================================*/

/* lval printers */
void lval_print(lval* v);
void lval_println(lval* v);
void lval_expr_print(lval* v, char open, char close);

/* lval constructors */
lval* lval_num(long x);
lval* lval_err(char* fmt, ...);
lval* lval_sym(char* m);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_fun(lbuiltin func);
lval* lval_lambda(lval* formals, lval* body);

/* Function caller */
lval* lval_call(lenv* e, lval* f, lval* a);

/* lval Destructor */
void lval_del(lval* v);

/* lval Copier */
lval* lval_copy(lval* v);

/* Lval Readers */
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);

/* Lval operators */
lval* lval_add(lval* v, lval* x);
lval* lval_join(lval* x, lval* y);
lval* lval_eval_sexpr(lenv* e, lval* v);
lval* lval_eval(lenv* e, lval* v);
lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);

#endif
