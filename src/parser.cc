#include "parser.h"
#include <stdio.h>
#include "debug.h"

// copied from http://flex.sourceforge.net/manual/Reentrant-Functions.html
#define yyscan_t void*

char *yyget_text ( yyscan_t scanner );
int yyget_leng ( yyscan_t scanner );
FILE *yyget_in ( yyscan_t scanner );
FILE *yyget_out ( yyscan_t scanner );
int yyget_lineno ( yyscan_t scanner );
//YY_EXTRA_TYPE yyget_extra ( yyscan_t scanner );
int	 yyget_debug ( yyscan_t scanner );

void yyset_debug ( int flag, yyscan_t scanner );
void yyset_in	 ( FILE * in_str , yyscan_t scanner );
void yyset_out	( FILE * out_str , yyscan_t scanner );
void yyset_lineno ( int line_number , yyscan_t scanner );
//void yyset_extra ( YY_EXTRA_TYPE user_defined , yyscan_t scanner );

int yylex_init( yyscan_t* );
int yylex_destroy (yyscan_t );

int yyparse (void * yyscanner, ilang::parser_data * parser_handle);

extern int yydebug;

namespace ilang {
	ilang::parserNode::Head * parser (FILE *file, ilang::ImportScopeFile *import, const char *name) {
		yydebug =0;
		yyscan_t scanner;
		ilang::parser_data data;
		data.import = import;
		data.fileName = name;
		data.head = NULL;
		yylex_init(&scanner);
		yyset_in(file, scanner);
		yyparse(scanner, &data);
		yylex_destroy(scanner);
		assert(data.error_count == 0);
		return data.head;
	}
} // namespace ilang

// #include <iostream>
// #include "parser.tab.hh"

// void yyerror(YYLTYPE *loc, void *, ilang::parser_data*, const char *msg) {
//   using namespace std;
//   //cerr << "error: " << msg << endl << yylloc.first_line;
//   cerr << "error: " << msg << endl << loc->first_line;
// }
