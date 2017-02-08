#ifndef LISPY_MAIN_HEADER
#define LISPY_MAIN_HEADER

/*============================== Declared Parsers ==============================*/
struct mpc_parser_t;
typedef struct mpc_parser_t mpc_parser_t;

mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Lispy;

#endif
