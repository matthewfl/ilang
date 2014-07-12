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
	class ImportScopeFile;
	namespace parserNode {
		class Head;
	};
	struct parser_data {
		ilang::ImportScopeFile *import;
		ilang::parserNode::Head *head;
		const char *fileName;
		int error_count = 0;
	};
	ilang::parserNode::Head * parser (FILE *, ilang::ImportScopeFile *, const char *);
};


#endif // _parser_h
