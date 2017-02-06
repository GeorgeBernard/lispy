
// Standard Library Includes
#include <stdio.h>
#include <stdlib.h>

// Internal Includes
#include "mpc.h"
#include "lisputils.h"
#include "lvalue.h"
#include "lenviron.h"
#include "builtin.h"

/* If we are compiling on Windows compile these functions */
#ifdef _WIN32

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt){
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer)+1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy)-1] = '\0';
	return cpy;
}

/* Fake add history function */
void add_history(char * unused) {}

/* Otherwise include the editline headers */
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

/* =================================== Main =================================== */


int main(int argc, char* argv[]) {

	/* Create Some Parsers*/
	mpc_parser_t* Number   = mpc_new("number");
	mpc_parser_t* Symbol   = mpc_new("symbol");
	mpc_parser_t* Sexpr	   = mpc_new("sexpr");
	mpc_parser_t* Qexpr    = mpc_new("qexpr");
	mpc_parser_t* Expr 	   = mpc_new("expr");
	mpc_parser_t* Lispy	   = mpc_new("lispy");

	/* Define them with the following Language */
	mpca_lang(MPCA_LANG_DEFAULT, 
		"														 \
			number 	 : /-?[0-9]+/ ;								 \
			symbol   : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;		 \
			sexpr    : '(' <expr>* ')' ;                         \
			qexpr	 : '{' <expr>* '}' ;                         \
			expr  	 : <number> | <symbol> | <sexpr> | <qexpr> ; \
			lispy 	 : /^/ <expr>* /$/ ;						 \
		",
		 Number, Symbol, Sexpr, Qexpr, Expr, Lispy);  

	/* Print Version and Exit Information */
	puts("Lispy Version 0.0.0.0.6\n");
	puts("Press Ctrl+C to Exit\n");

	/* Initialize environment */
	lenv* e = lenv_new();
	lenv_add_builtins(e);

	while(1) {

		/* Output our prompt and add to History */
		char * input = readline("lispy> ");
		add_history(input);

		/* Attempt to parse the user input */
		mpc_result_t r;
		if(mpc_parse("<stdin>", input, Lispy, &r)) {

			/* On success print result and delete the AST */
			lval*  x = lval_eval(e, lval_read(r.output));
			lval_println(x);
			lval_del(x);
			mpc_ast_delete(r.output);
			
		} 

		else {

			/* Otherwise print and delete the Error */
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}

		/*Free retrieved input*/
		free(input);
	}

	lenv_del(e);

	/* Undefine and Delete our Parsers */
	mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

	return 0;
}

