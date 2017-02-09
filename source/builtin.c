
/*=========================================== Includes ===========================================*/
// Standard Includes
#include <string.h>

// Library Includes
#include "mpc.h"

// Local Includes 
#include "lisputils.h"
#include "lenviron.h"
#include "lvalue.h"
#include "parse.h"

// Header Include
#include "builtin.h"

lval* builtin(lenv* e, lval* a, char* func) {
	if (strcmp("list", func) == 0) { return builtin_list(e, a); }
	if (strcmp("head", func) == 0) { return builtin_head(e, a); }
	if (strcmp("tail", func) == 0) { return builtin_tail(e, a); }
	if (strcmp("join", func) == 0) { return builtin_join(e, a); }
	if (strcmp("eval", func) == 0) { return builtin_eval(e, a); }
	if (strstr("+-/*", func)) { return builtin_op(e, a, func); }
	lval_del(a);
	return lval_err("Unknown Function '%s'!", func);
}

lval* builtin_lambda(lenv* e, lval* a) {
	/*Check Two Arguments, each of which are Q-expr */
	LASSERT_NUM("\\", a, 2);
	LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
	LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

	/* Check first Q-Expression contains only Symbols */
	for (int i = 0; i < a->cell[0]->count; i++) {
		LASSERT(
			a, 
			(a->cell[0]->cell[i]->type == LVAL_SYM),
			"Cannot define non-symbol argument %d. "
			"Got %s, Expected %s.", i, 
			ltype_name(a->cell[0]->cell[i]->type),
			ltype_name(LVAL_SYM)
			);
	}

	/* Pop first two arguments and pass them to lval_lambda */
	lval* formals = lval_pop(a, 0);
	lval* body = lval_pop(a, 0);
	lval_del(a);

	return lval_lambda(formals, body);
}

lval* builtin_var(lenv* e, lval* a, char* func) {
	LASSERT_TYPE(func, a, 0, LVAL_QEXPR);

	/* First argument is symbol list */
	lval* syms = a->cell[0];

	/* Ensure all elements of first list are symbols */
	for (int i = 0; i < syms->count; i++ ) {
		LASSERT_TYPE("def", syms, i, LVAL_SYM);
	}

	/* Check correct number of symbols and values */
	LASSERT(a, (syms->count == a->count-1),
		"Function '%s' passed too many arguments for symbols. "
		"Got %i, Expected %i.", func, syms->count, a->count-1);

	/* Assign copies of values to symbols */
	for(int i = 0; i < syms->count; i++) {
		/* If 'def' define in globally. 
		   If 'put' define in local scope */

		if (strcmp(func, "def") == 0) {
			lenv_def(e, syms->cell[i], a->cell[i+1]);
		}
		if (strcmp(func, "=") == 0) {
			lenv_put(e, syms->cell[i], a->cell[i+1]);
		}
	}

	lval_del(a);
	return lval_sexpr();
}

lval* builtin_put(lenv* e, lval* a) {
	return builtin_var(e, a, "=");
}

lval* builtin_def(lenv* e, lval* a) {
	return builtin_var(e, a, "def");
}


lval* builtin_add(lenv* e, lval* a) {
	return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
	return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
	return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
	return builtin_op(e, a, "/");
}

lval* builtin_head(lenv* e, lval* a) {
	/* Check Error Conditions */
	LASSERT_NUM("head", a, 1);
	LASSERT_TYPE("head", a, 0, LVAL_QEXPR);
	LASSERT_NOT_EMPTY("head", a, 0);

	/* Otherwise take first argument */
	lval* v = lval_take(a, 0);

	/* Delete all elements that are not head and return */
	while (v->count > 1) { lval_del(lval_pop(v, 1)); }
	return v;
}

lval* builtin_tail(lenv* e, lval* a) {
	/* Check Error Conditions */
	LASSERT_NUM("tail", a, 1)
	LASSERT_TYPE("tail", a, 0, LVAL_QEXPR);
	LASSERT_NOT_EMPTY("tail", a, 0);

	/* Otherwise take first argument */
	lval* v = lval_take(a, 0);

	/* Delete first element and return */
	lval_del(lval_pop(v, 0));
	return v;
}

lval* builtin_list(lenv* e, lval* a) {
	a->type = LVAL_QEXPR;
	return a;
}

lval* builtin_eval(lenv* e, lval* a) {
	LASSERT_NUM("eval", a, 1);
	LASSERT_TYPE("eval", a, 0, LVAL_QEXPR);

	lval* x = lval_take(a, 0);
	x->type = LVAL_SEXPR;
	return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {

	for (int i = 0; i < a->count; i++) {
		LASSERT_TYPE("join", a, i, LVAL_QEXPR);
	}

	lval* x = lval_pop(a, 0);

	while (a->count != 0) {
		x = lval_join(x, lval_pop(a,0));
	}

	lval_del(a);
	return x;
}

lval* builtin_op(lenv* e, lval* a, char* op){

	/* Ensure all arguments are numbers */
	for (int i = 0; i < a->count; i++) {
		if(a->cell[i]->type != LVAL_NUM) {
			LASSERT_TYPE(op, a, i, LVAL_NUM);
		}
	}

	/* Pop the first element */
	lval* x = lval_pop(a, 0);

	/* If no arguments and sub then perform unary negation */
	if ( (strcmp(op, "-") == 0) && (a->count == 0) ) {
		x->num = -x->num;
	}

	/* While there are still elements remaining */
	while (a->count > 0) {

		/* Pop the next element */
		lval* y = lval_pop(a, 0);

		if ( strcmp(op, "+") == 0 ) { x->num += y->num; }
		if ( strcmp(op, "-") == 0 ) { x->num -= y->num; }
		if ( strcmp(op, "*") == 0 ) { x->num *= y->num; }
		if ( strcmp(op, "/") == 0 ) { 
			if (y->num == 0) {
				lval_del(x); 
				lval_del(y);
				x = lval_err("Division by Zero!"); 
				break;
			}
			x->num /= y->num; 
		}

		lval_del(y);
	}

	lval_del(a);

	return x;
}

/*====================================== Ordering Operators ======================================*/

lval* builtin_ord(lenv* e, lval* a, char* op) {
	// Assert two arguments, both numbers
	LASSERT_NUM(op, a, 2);
	LASSERT_TYPE(op, a, 0, LVAL_NUM);
	LASSERT_TYPE(op, a, 1, LVAL_NUM);

	// Based on operator simply perform that comparison
	int r;
	if (strcmp(op, ">") == 0) {
		r = (a->cell[0]->num > a->cell[1]->num);
	}
	if (strcmp(op, "<") == 0) {
		r = (a->cell[0]->num < a->cell[1]->num);
	}
	if (strcmp(op, ">=") == 0) {
		r = (a->cell[0]->num >= a->cell[1]->num);
	}
	if (strcmp(op, "<=") == 0) {
		r = (a->cell[0]->num <= a->cell[1]->num);
	}

	// Cleanup and return
	lval_del(a);
	return lval_num(r);

}

lval* builtin_gt(lenv* e, lval* a){
	return builtin_ord(e, a, ">");
}

lval* builtin_lt(lenv* e, lval* a){
	return builtin_ord(e, a, "<");
}

lval* builtin_ge(lenv* e, lval* a){
	return builtin_ord(e, a, ">=");
}

lval* builtin_le(lenv* e, lval* a){
	return builtin_ord(e, a, "<=");
}

/*====================================== Equality Operators ======================================*/

lval* builtin_cmp(lenv* e, lval* a, char* op){
	// Assert two arguments
	LASSERT_NUM(op, a, 2);

	int r;
	if (strcmp(op, "==") == 0) {
		r = lval_eq(a->cell[0], a->cell[1]);
	}
	if (strcmp(op, "!=") == 0) {
		r = !lval_eq(a->cell[0], a->cell[1]);
	}

	lval_del(a);
	return lval_num(r);
}

lval* builtin_eq(lenv* e, lval* a){
	return builtin_cmp(e, a, "==");
}

lval* builtin_ne(lenv* e, lval* a){
	return builtin_cmp(e, a, "!=");
}

/*==================================== Conditional branching ====================================*/

lval* builtin_if(lenv* e, lval* a) {
	/* Assert Three Arguments, First is number, followed by two Q-expr */
	LASSERT_NUM("if", a, 3);
	LASSERT_TYPE("if", a, 0, LVAL_NUM);	
	LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
	LASSERT_TYPE("if", a, 2, LVAL_QEXPR);	

	/* Mark both Expressions as evaluable (i.e. S-expr) */
	lval* x;
	a->cell[1]->type = LVAL_SEXPR;
	a->cell[2]->type = LVAL_SEXPR;

	if (a->cell[0]->num) {
		/* If condition is true evaluate first expression */
		x = lval_eval(e, lval_pop(a, 1));
	} else {
		x = lval_eval(e, lval_pop(a, 2));
	}

	// Cleanup and return
	lval_del(a);
	return x;
}

lval* builtin_load (lenv* e, lval* a) {
	// Assert 1 String argument
	LASSERT_NUM("load", a, 1);
	LASSERT_TYPE("load", a, 0, LVAL_STR);

	/* Parse File given by string name */
	mpc_result_t r;
	if (mpc_parse_contents(a->cell[0]->str, Lispy, &r)) {

		/* Read contents */
		lval* expr = lval_read(r.output);
		mpc_ast_delete(r.output);

		/* Evaluate each Expression */
		while (expr->count) {
			lval* x = lval_eval(e, lval_pop(expr, 0));
			/* If Evaluation leads to error print it */
			if (x->type == LVAL_ERR) { lval_println(x); }
			lval_del(x);
		}

		/* Delete expressions and arguments */
		lval_del(expr);
		lval_del(a);

		/* Return empty list */
		return lval_sexpr();

	} else {
		/* Get Parse Error as String */
		char* err_msg = mpc_err_string(r.error);
		mpc_err_delete(r.error);

		/* Create new error message using it */
		lval* err = lval_err("Could not load Library %s", err_msg);
		free(err_msg);
		lval_del(a);

		/* Cleanup and return error */
		return err;
	}
}

lval* builtin_print (lenv* e, lval* a) {

	/* Print each argument followed by a space */
	for ( int i = 0; i < a->count; i++) {
		lval_print(a->cell[i]); putchar(' ');
	}

	/* Print a newline and delete arguments */
	putchar('\n');
	lval_del(a);

	return lval_sexpr();
}


lval* builtin_error (lenv* e, lval* a) {
	// Assert one string arg
	LASSERT_NUM("error", a , 1);
	LASSERT_TYPE("error", a, 0, LVAL_STR);

	/* Construct error from first arg */
	lval* err = lval_err(a->cell[0]->str);

	// Cleanup and return
	lval_del(a);
	return err;
}