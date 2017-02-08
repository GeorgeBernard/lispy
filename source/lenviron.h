#ifndef LENVIRONMENT_HEADER
#define LENVIRONMENT_HEADER

/* Forward declare dependencies */
struct lenv;
struct lval;
typedef struct lenv lenv;
typedef struct lval lval;
typedef lval*(*lbuiltin)(lenv*, lval*);

/*===================================== Struct Definition =====================================*/

struct lenv {
	lenv* par;
	int count;
	char** syms;
	lval** vals;
};

/*===================================== Declared Functions =====================================*/

lenv* lenv_new(void);
void  lenv_del (lenv* e); 
lenv* lenv_copy(lenv* e);
void lenv_def(lenv* e, lval* k, lval* v);
void lenv_put(lenv* e, lval* k, lval* v);
lval* lenv_get(lenv* e, lval* k);
void lenv_add_builtin(lenv* e, char* name, lbuiltin func);
void lenv_add_builtins(lenv* e);

#endif
