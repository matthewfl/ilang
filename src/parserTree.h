#ifndef _ilang_parserTree
#define _ilang_parserTree

#include <list>
#include <map>
#include <string>

//#include <boost/Handle.hpp>

#include "handle.h"
#include "context.h"
#include "variable_new.h"
#include "scope_new.h"
#include "import.h"
#include "print.h"
#include "value.h"

/* There is a problem with the scope getting possibly deleted if a function gets ruturned from a function
 * and it is using the closure, the solution is to change the scope to use a smart pointer to be passed around
 */

namespace ilang {
	class Scope;
	class Object;
	class Class;
	class Modification;
	class Function;
	class Arguments;

	namespace parserNode {

		using std::vector;
		using std::list;

		//using boost::Handle;
		//typedef boost::Handle<ilang::Value_Old> ValuePass;
		using ilang::ValuePass; // defined in variable.h
		//using ilang::ValuePass;
		class Node {
			friend class ilang::Modification;
		private:
			//unsigned long _node_id;
			//Node *_parent;
		protected:
			//void _setParent(Node*);
		public:
			Node();
			//const unsigned long getID() { return _node_id; }
			//const Node * getParent() { return _parent; }
			virtual void Run(Context&)=0;
			void randomsdafasdf(){} // take this out eventually, fixed some random compiler bug or something
			virtual void Print(Printer*) =0;

			virtual IdentifierSet UndefinedElements()=0;
			//virtual void setParent(Node*) =0;
		};

		class Head {
			friend class ilang::Modification;
			friend class ImportCall;
		private:
			// also head by smart point so do not need to delete
			Context ctx;
			Scope *scope = NULL;
			ImportScopeFile *Import;
			std::list<Node*> *Declars;
		public:
			Head(std::list<Node*>*, ImportScopeFile*);
			~Head();
			void Link();
			void Run();
			Scope *GetScope ();
			void Print(Printer*);
		};



		class Value : public Node {
			friend class ilang::Modification;
		public:
			virtual ValuePass GetValue(Context&)=0;
			//virtual ValuePass CallFun(Context&, std::vector<ValuePass> &par); // I feel a little funny about having this here, but this should be the right place
		};
		class Constant : public Value {
			friend class ilang::Modification;
		public:
			void Run(Context&);
			IdentifierSet UndefinedElements() override;
		};



		// I guess we will leave this in, but most things are using Value, not expression
		class Expression : public Value {
			friend class ilang::Modification;
		};

		class Variable;

		class Object : public Value {
			friend class ilang::Modification;
		private:
			std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *objects;
		public:
			Object(std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj);
			void Run(Context&);
			ValuePass GetValue(Context&);
			void Print(Printer*);
			IdentifierSet UndefinedElements();
		};

		class Class : public Value {
			friend class ilang::Modification;
		private:
			std::list<Node*> *parents;
			// NOTE: only useful for iterating over, as pointer type for key
			std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *objects;
		public:
			Class(std::list<Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj);
			void Run(Context&);
			ValuePass GetValue(Context&);
			void Print(Printer *p);
			IdentifierSet UndefinedElements() override;
		};

		class Array : public Value {
			friend class ilang::Modification;
		private:
			std::list<Node*> *elements;
			std::list<Node*> *modifiers;
		public:
			Array(std::list<Node*> *e, std::list<Node*> *m);
			void Run(Context&);
			ValuePass GetValue(Context&);
			void Print(Printer*);
			IdentifierSet UndefinedElements() override;
		};

		class StringConst : public Constant {
			friend class ilang::Modification;
		private:
			//char *string;
			std::string string;
		public:
			StringConst(char *str);
			ValuePass GetValue(Context&);
			void Print(Printer*);
		};

		class IntConst : public Constant {
			friend class ilang::Modification;
		private:
			long num;
		public:
			IntConst(long n);
			ValuePass GetValue(Context&);
			void Print(Printer *p);
		};
		class FloatConst : public Constant {
			friend class ilang::Modification;
		private:
			double num;
		public:
			FloatConst(double d);
			ValuePass GetValue(Context&);
			void Print(Printer*);
		};


		class IfStmt : public Node {
			friend class ilang::Modification;
		private:
			Value *test;
			Node *True, *False;
		public:
			IfStmt(Node*, Node*, Node*);
			void Run(Context&);
			void Print(Printer*);
			IdentifierSet UndefinedElements() override;
		};
		class WhileStmt : public Node {
			friend class ilang::Modification;
		private:
			Value *test;
			Node *exe;
		public:
			WhileStmt(Node*, Node*);
			void Run(Context&);
			void Print(Printer*);
			IdentifierSet UndefinedElements() override;
		};
		class ForStmt : public Node {
			friend class ilang::Modification;
		private:
			Node *pre;
			Value *test;
			Node *each;
			Node *exe;
		public:
			ForStmt(Node*, Node*, Node*, Node*);
			void Run(Context&);
			void Print(Printer*);
			IdentifierSet UndefinedElements() override;
		};

		class ReturnStmt : public Node {
			friend class ilang::Modification;
		private:
			Value *ret;
		public:
			ReturnStmt(Node*);
			void Run(Context&);
			void Print(Printer*);
			IdentifierSet UndefinedElements() override;
		};

		class Function : public Value {
			friend class ilang::Modification;
			friend class ilang::Function;
			friend class ilang::Arguments;
		private:
			std::list<Node*> *body;
			std::list<Node*> *params;
		public:
			Function(std::list<Node*> *p, std::list<Node*> *b);
			void Run(Context&);
			//void Call(std::vector<ilang::Value_Old*>);
			//void Call(Context& _scope_made, Context& _scope_self, std::vector<ValuePass>&, ValuePass *_ret=NULL);
			ValuePass GetValue(Context&);
			void Print(Printer*);
			IdentifierSet UndefinedElements() override;
		};


		class Variable : public Value {
			friend class ilang::Modification;
		private:
			friend class Variable_compare;
			friend class ::ilang::Object;
			friend class ::ilang::Class;

			// TODO: variables won't have access list for names
			// just a single item, otherwise it is a field access
			std::list<std::string> *name;
		protected:
			std::list<Node*> *modifiers;
		public:
			Variable (std::list<std::string> *n, std::list<Node*> *mod);
			void Run(Context&);
			virtual void Set(Context&, ValuePass var, bool force = false);
			// not sure if I want to make this virtual, but I believe that this will be the most effective way to make this work easily
			virtual ilang::Variable * Get(Context&);
			ValuePass GetValue(Context&);
			virtual std::string GetFirstName();
			//virtual ValuePass CallFun (Scope*, std::vector<ValuePass> &par);
			void Print(Printer*);
			IdentifierSet UndefinedElements() override;
			void PreRegister(Context &ctx);
		};


		class FieldAccess : public Variable {
			friend class ilang::Modification;
		private:
			std::string identifier;
			Value *Obj;
		public:
			FieldAccess(Node*, std::string);
			//void Run(Scope*);
			//void Set(Scope*, ValuePass var);
			void Set(Context&, ValuePass val, bool force = false);
			ilang::Variable * Get(Context&);
			ValuePass GetValue(Context&);
			virtual std::string GetFirstName();
			//virtual ValuePass CallFun (Context&, std::vector<ValuePass> &par);
			void Print(Printer*);
			IdentifierSet UndefinedElements() override;
		};

		class ArrayAccess : public Variable {
			friend class ilang::Modification;
		private:
			Value *Obj;
			Value *Lookup;
		public:
			ArrayAccess(Node*, Node*);
			ilang::Variable * Get(Context&);
			ValuePass GetValue(Context&);
			void Set(Context&, ValuePass var, bool force = false); // forced is ignored as it makes no since
			virtual std::string GetFirstName();
			void Print(Printer*);

			IdentifierSet UndefinedElements() override;
		};

		class Call : public Value {
			friend class ilang::Modification;
		private:
			Value *calling;
		protected:
			std::list<Node*> *params;
		public:
			Call(Value *call, std::list<Node*> *args);
			void Run(Context&);
			ValuePass GetValue(Context&);
			void Print(Printer*);
			IdentifierSet UndefinedElements() override;
		};
		class PrintCall : public Call {
			friend class ilang::Modification;
		public:
			PrintCall(std::list<Node*> *args);
			ValuePass GetValue (Context&); // returns null
			void Print(Printer*);
		};
		// class NewCall : public Call {
		// 	friend class ilang::Modification;
		// public:
		// 	NewCall(std::list<Node*> *args);
		// 	ValuePass GetValue(Context&);
		// 	void Print(Printer*);
		// };

		class AssertCall : public Call {
			friend class ilang::Modification;
		private:
			int lineN;
			std::string fileName;
		public:
			AssertCall(int line, const char *name, std::list<Node*> *args);
			ValuePass GetValue(Context&);
			void Print(Printer*);
		};

		class ImportCall : public Call {
			friend class ilang::Modification;
		public:
			ImportCall(std::list<Node*> *args);
			ValuePass GetValue(Context&);
			void Print(Printer*);
		};

		class ThreadGoCall : public Call {
			friend class ilang::Modification;
		public:
			ThreadGoCall(std::list<Node*> *args);
			ValuePass GetValue(Context&);
			void Print(Printer*);
		};


		class AssignExpr : public Expression {
			friend class ilang::Modification;
		private:
			Variable *target;
			Value *eval;
		public:
			AssignExpr (Variable *target, Value *value);
			void Run(Context& scope);
			ValuePass GetValue(Context& scope);
			void Print(Printer*);
			IdentifierSet UndefinedElements() override;
			void PreRegister(Context &ctx);
		};
		class MathEquation : public Expression {
			friend class ilang::Modification;
		public:
			enum action {
				add,
				subtract,
				multiply,
				divide,
				uMinus
			};
			MathEquation(Value *l, Value *r, action a);
			void Run(Context& sope);
			ValuePass GetValue(Context& scope);
			void Print(Printer*);
			IdentifierSet UndefinedElements() override;
		private:
			Value *left, *right;
			action Act;
		};
		class LogicExpression : public Expression {
			friend class ilang::Modification;
		public:
			enum action {
				Equal,
				Not_Equal,
				Less_Equal,
				Greater_Equal,
				Less,
				Greater,
				And,
				Or,
				Not
			};
			LogicExpression(Value *l, Value *r, action a);
			void Run(Context& scope);
			ValuePass GetValue(Context& scope);
			void Print(Printer*);
			IdentifierSet UndefinedElements() override;
		private:
			Value *left, *right;
			action Act;
		};

		class SingleExpression : public Expression{
			friend class ilang::Modification;
		public:
			enum action {
				add,
				subtract,
				multiply,
				divide
			};
			SingleExpression(Variable *target, Value *value, action a);
			void Run(Context& scope);
			ValuePass GetValue(Context& scope);
			void Print(Printer*);
			IdentifierSet UndefinedElements() override;
		private:
			Variable *m_target;
			Value *m_value;
			action Act;
		};

	} // namespace parserNode
} // namespace ilang

#endif // _ilang_parserTree
