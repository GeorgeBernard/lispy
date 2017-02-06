
/*=========================================== Includes ===========================================*/
// Standard Includes
#include <string.h>

// Local Includes 
#include "lisputils.h"
#include "lenviron.h"
#include "lvalue.h"

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