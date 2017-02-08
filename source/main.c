
// Standard Library Includes
#include <stdio.h>
#include <stdlib.h>

// Internal Includes
#include "mpc.h"
#include "lisputils.h"
#include "lvalue.h"
#include "lenviron.h"
#include "builtin.h"

// Header Include
#include "main.h"

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
	Number   = mpc_new("number");
	Symbol   = mpc_new("symbol");
	String   = mpc_new("string");
	Comment  = mpc_new("comment");
	Sexpr	 = mpc_new("sexpr");
	Qexpr    = mpc_new("qexpr");
	Expr 	 = mpc_new("expr");
	Lispy	 = mpc_new("lispy");

	/* Define them with the following Language */
	mpca_lang(MPCA_LANG_DEFAULT, 
		"														 \
			number 	 : /-?[0-9]+/ ;								 \
			symbol   : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;		 \
			string   : /\"(\\\\.|[^\"])*\"/ ;                    \
			comment  : /;[^\\r\\n]*/ ;							 \
			sexpr    : '(' <expr>* ')' ;                         \
			qexpr	 : '{' <expr>* '}' ;                         \
			expr  	 : <number>  | <symbol> | <string>           \
					 | <comment> | <sexpr>  | <qexpr>;           \
			lispy 	 : /^/ <expr>* /$/ ;						 \
		",
		 Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);  

	/* Initialize environment */
	lenv* e = lenv_new();
	lenv_add_builtins(e);

	/* Interactive Prompt */
	if ( argc == 1 ) {

		/* Print Version and Exit Information */
		puts("Lispy Version 0.0.0.1.0\n");
		puts("Press Ctrl+C to Exit\n");

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
	}

	/* Supplied with list of files */
	if (argc >= 2) {

		/* loop over each supplied filename (starting from 1) */
		for (int i = 1; i < argc; i++) {

			/* Argument list with a single argument, the filename */
			lval* args = lval_add(lval_sexpr(), lval_str(argv[i]));

			/* Pass to builtin load and get the result */
			lval* x = builtin_load(e, args);

			/* If the result is an error be sure to print it */
			if (x->type == LVAL_ERR) { lval_println(x); }
			lval_del(x);
		}
	}

	lenv_del(e);

	/* Undefine and Delete our Parsers */
	mpc_cleanup(8, 
		Number, Symbol, String, Comment, 
		Sexpr,  Qexpr,  Expr,   Lispy);

	return 0;
}

