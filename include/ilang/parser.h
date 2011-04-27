#ifndef _parser_h
#define _parser_h

union YYSTYPE;
struct YYLTYPE;

#define YY_DECL int yylex (YYSTYPE * yylval_param,YYLTYPE * yylloc_param , void * yyscanner)
//#define YY_DECL int yylex (YYSTYPE * yylval_param,YYLTYPE * yylloc_param)

//extern YY_DECL ;
YY_DECL ;

#include <stdio.h>

namespace ilang {
  class Import;
  namespace parserNode {
    class Head;
  };
  struct parser_data {
    ilang::Import *import;
    ilang::parserNode::Head *head;
  };
  ilang::parserNode::Head * parser (FILE *, ilang::Import *);
};


#endif // _parser_h
