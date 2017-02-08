#ifndef LISP_UTILITIES_HEADER
#define LISP_UTILITIES_HEADER

/*======================================= Assert Macros =======================================*/

#define LASSERT(args, cond, fmt, ...) \
	if (!(cond)) { \
		lval* err = lval_err(fmt, ##__VA_ARGS__); \
		lval_del(args); \
		return err; \
	}

#define LASSERT_TYPE(func, args, index, expect) \
	LASSERT(args, args->cell[index]->type == expect, \
		"Function '%s' passed incorrect type for argument %d. Got %s, Expected %s.", \
		func, index, ltype_name(args->cell[index]->type), ltype_name(expect));

#define LASSERT_NUM(func, args, num) \
	LASSERT(args, args->count == num, \
		"Function '%s' passed incorrect number of arguments. Got %d, Expected %d.", \
		func, args->count, num);

#define LASSERT_NOT_EMPTY(func, args, index) \
	LASSERT(args, args->cell[index]->count != 0, \
		"Function '%s' passed {} for argument %s.", \
		func, index); 

/*=================================== Lval Type Enumeration ===================================*/

/* Create Enumeration of Possible lval Types */
enum { 
	LVAL_ERR, 
	LVAL_NUM,
	LVAL_SYM,
	LVAL_STR, 
	LVAL_FUN,
	LVAL_SEXPR,
	LVAL_QEXPR
	};

/* Error Related Functions */
char* ltype_name(int t);

#endif
