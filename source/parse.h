#ifndef LISPY_PARSE_HEADER
#define LISPY_PARSE_HEADER

/* Forward Declared Dependencies */
struct mpc_parser_t;
typedef struct mpc_parser_t mpc_parser_t;
struct lenv;
typedef struct lenv lenv;


/*============================== Declared Parsers ==============================*/

mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Lispy;

/*============================= Declared Functions =============================*/

int parse(int argc, char* argv[]);

void REPL_loop(lenv* e);
void REPL_args(lenv* e, int argc, char* argv[]);



#endif
