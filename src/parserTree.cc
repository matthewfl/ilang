#include "parserTree.h"
#include "parser.h"
#include "scope.h"
#include "object.h"
#include "function.h"
#include "thread.h"
#include "debug.h"
#include "error.h"

// for string escape
#include <boost/algorithm/string.hpp>

#include <iostream>
using namespace std;

extern "C" int ilang_Assert_fails;

namespace ilang {
	namespace parserNode {
		Head::Head(list<Node*> *declars, ImportScopeFile *import): Import(import), Declars(declars) {
			debug(-5, "head " << declars->size() );
			debug(-5, "running ++++++++++++++++++++++++++++++++++++++++++++" );
			//	scope=NULL;
		}

		Head::~Head() {
			delete scope;
		}

		void Head::Link () {
			assert(!scope);
			scope = new Scope(ctx);

			for(list<Node*>::iterator it = Declars->begin(); it !=	Declars->end(); it++) {
				debug(-6, "calling run" )
					(*it)->Run(ctx);
				//scope->Debug();
			}
		}

		void Head::Run() {
			vector<ValuePass> v;
			ValuePass ret;
			//scope->Debug();
			ValuePass main = scope->get(Identifier("main"));
			error(main, "main function not found");
			ilang::Arguments args;
			main->call(args);

			//ilang::Variable * find = scope->lookup("main");
			//error(find, "main function not found");
			//auto main = boost::any_cast<ilang::Function>(find->Get()->Get());
			//main(); // call the main function

			//.ptr(passScope, v, &ret);
			//boost::any_cast<ilang::Function_ptr>(scope->lookup("main")->Get()->Get())(scope, v, &ret);
		}

		void Head::Print(Printer *p) {
			for(auto it : *Declars) {
				it->Print(p);
				p->p() << ";\n";
			}
		}

		Scope *Head::GetScope() {
			assert(0);
			//return scope;
		}

		//static unsigned long _ilang_node_id;
		Node::Node() {}
		// 	_node_id = ++_ilang_node_id;
		// 	_parent = NULL;
		// }

		// void Node::_setParent(Node *n) {
		// 	assert(!_parent);
		// 	_parent = n;
		// }

		// this does not need to have anything
		void Constant::Run(Context &ctx) {}

		StringConst::StringConst(char *str) :string(str){
			delete[] str;
			using boost::replace_all;
			replace_all(string, "\\n", "\n");
			replace_all(string, "\\t", "\t");
			replace_all(string, "\\\\", "\\");
			replace_all(string, "\\\"", "\"");
			replace_all(string, "\\'", "'");

		}
		ValuePass StringConst::GetValue (Context &ctx) {
			debug(-6, "string get value" );
			return valueMaker(string);
		}

		void StringConst::Print(Printer *p) {
			using boost::replace_all;
			std::string result = string;
			replace_all(result, "\n", "\\n");
			replace_all(result, "\t", "\\t");
			replace_all(result, "\\", "\\\\");
			replace_all(result, "\"", "\\\"");
			replace_all(result, "\'", "\\'");
			p->p() << "\"" << result << "\"";
		}

		IntConst::IntConst(long n) : num(n) {}
		ValuePass IntConst::GetValue (Context &ctx) {
			return valueMaker(num);
		}

		void IntConst::Print(Printer *p) {
			p->p() << num;
		}

		FloatConst::FloatConst(double d) : num(d) {}
		ValuePass FloatConst::GetValue (Context &ctx) {
			return valueMaker(num);
		}
		void FloatConst::Print(Printer *p) {
			p->p() << num;
		}

		IfStmt::IfStmt (Node *test_, Node* True_, Node* False_): True(True_), False(False_) {
			Value *t = dynamic_cast<Value*>(test_);
			error(t, "If statement does not contain a test");
			//assert(t);
			test=t;
		}
		void IfStmt::Run(Context &ctx) {
			ValuePass search = test->GetValue(ctx);
			assert(search);
			if(search->isTrue()) {
				if(True) True->Run(ctx);
			}else{
				if(False) False->Run(ctx);
			}
		}

		void IfStmt::Print(Printer *p) {
			p->p() << "if(";
			test->Print(p);
			p->p() << ") ";
			p->up();
			if(True) True->Print(p);
			else p->p() << ';';
			if(False) {
				p->down();
				p->p() << "else";
				p->up();
				False->Print(p);
				p->down();
			}
		}

		ForStmt::ForStmt (Node *pre_, Node *test_, Node *each_, Node *exe_) :pre(pre_), each(each_), exe(exe_) {
			Value *t = dynamic_cast<Value*>(test_);
			error(t, "Test in for statment is not a test");
			//assert(t);
			test = t;
		}
		void ForStmt::Run(Context &ctx) {
			pre->Run(ctx);
			ValuePass t = test->GetValue(ctx);
			while(t->isTrue()) {
				exe->Run(ctx);
				each->Run(ctx);
				ValuePass n = test->GetValue(ctx);
				t = n;
			}
		}

		void ForStmt::Print(Printer *p) {
			p->p() << "for(";
			pre->Print(p);
			p->p() << ';';
			test->Print(p);
			p->p() << ';';
			each->Print(p);
			p->p() << ") ";
			p->up();
			exe->Print(p);
			p->down();
		}

		WhileStmt::WhileStmt (Node *test_, Node *exe_): exe(exe_) {
			Value *t=dynamic_cast<Value*>(test_);
			error(t, "Test in while Statement is not a test");
			//assert(t);
			test=t;
		}
		void WhileStmt::Run(Context &ctx) {
			// note to self: must recall GetValue each time as it is what is actually doing the updating
			ValuePass t = test->GetValue(ctx);
			while(t->isTrue()) {
				exe->Run(ctx);
				ValuePass n = test->GetValue(ctx);
				t = n;
				//test->GetValue(scope)->Print();
			}
		}

		void WhileStmt::Print(Printer *p) {
			p->p() << "while(";
			test->Print(p);
			p->p() << ") ";
			p->up();
			exe->Print(p);
			p->down();
		}

		ReturnStmt::ReturnStmt (Node *r) {
			if(r) {
				ret = dynamic_cast<Value*>(r);
				error(ret, "Return is not given a value");
				///assert(ret);
			}else
				ret = NULL;
		}
		void ReturnStmt::Run(Context &ctx) {
			if(ret)
				ctx.returned = new ValuePass(ret->GetValue(ctx));
			else
				ctx.returned = (ilang::ValuePass*)1; // eh...haxs
		}

		void ReturnStmt::Print(Printer *p) {
			if(ret) {
				p->p() << "return ";
				ret->Print(p);
				p->p() << ";\n";
			}else{
				p->p() << "return;\n";
			}
		}

		Function::Function (list<Node*> *p, list<Node*> *b):body(b), params(p) {
			debug(-5, "\t\t\tfunction constructed" );
		}
		void Function::Run(Context &ctx) {
			// This is going to have duplicated code with calling function, minus dealing with the arguments
			Scope local_scope(ctx);
			//ScopePass local_scope(new Scope(scope));
			if(body) {
				for (Node *n : *body) {
					// TODO: returning block break statement
					n->Run(ctx);
					if(ctx.returned) break;
				}
			}


			//vector<ValuePass> p;
			//Call(scope, ScopePass(), p);
		}
		ValuePass Function::GetValue(Context &ctx) {
			// this need to track the scope at this point so that it could be use later in the funciton
			debug(-6, "Function get value");
			errorTrace("Getting value of a function");

			ilang::Function fun(this, ctx);
			// TODO: call bind??
			return valueMaker(fun);

		}

		//	void Function::Call(ScopePass _scope_made, ScopePass _scope_self, vector<ValuePass> &p, ValuePass *_ret) {
			// bool returned=false;
			// errorTrace("Function called");
			// // could clean this up to use boost::function and then would not have to template the whole class
			// auto returnHandle = [&returned, _ret] (ValuePass *ret) {
			// 	returned=true;
			// 	if(_ret) *_ret = *ret;
			// 	debug(-6, "return hook set");
			// };
			// ScopePass scope = ScopePass(new FunctionScope<decltype(returnHandle)>(_scope_made, _scope_self, returnHandle));

			// debug(-6,"function called");
			// if(params) { // the parser set params to NULL when there are non
			// 	list<Node*>::iterator it = params->begin();
			// 	for(ValuePass v : p) {
			// 		if(it==params->end()) break;
			// 		error(dynamic_cast<parserNode::Variable*>(*it), "Argument to function is not variable name");
			// 		assert(v);
			// 		dynamic_cast<parserNode::Variable*>(*it)->Set(scope, ValuePass(v), true);
			// 		it++;
			// 	}
			// }
		//	assert(0);
			/*

			std::list<std::string> arguments_mod;
			ilang::Variable *args = scope->forceNew("arguments", arguments_mod);
			ilang::Object *arguments_arr = new ilang::Array(p);
			args->Set(ValuePass(new ilang::Value_Old(arguments_arr)));

			if(body) { // body can be null if there is nothing
				for(Node *n : *body) {
					//if(returned) return;
					assert(n);
					debug(-6,"function run");
					n->Run(scope);
				}
				}*/

			// scope deleted
			//delete scope; // not going to be needed when smart pointer
		//}

		void Function::Print(Printer *p) {
			if(!body && !params) {
				p->p() << ";";
				return;
			}
			p->p() << "{";
			if(params && !params->empty()) {
				p->p() << "|";
				bool first=true;
				for(Node *it : *params) {
					if(!first) p->p() << ", ";
					it->Print(p);
					first = false;
				}
				p->p() << "|";
			}
			if(body && !body->empty()) {
				p->up();
				bool first=true;
				for(Node *it : *body) {
					p->line();
					it->Print(p);
					p->p() << ";";
				}
				p->down();
				p->line();
			}
			p->p() << "}";
		}

		Variable::Variable (list<string> *n, list<string> *mod):
			name(n), modifiers(mod) {
			if(!name) name = new list<string>;
			if(!modifiers) modifiers = new list<string>;
			//cout << "\t\t\t" << name << "\n";
		}
		void Variable::Run (Context &ctx) {
			// iirc this does not happen as the syntax does not allow for variables to be created without a default value
			error(0, "Variables must always have some value set to them");
			//debug(-6,"\t\t\tSetting variable: " << name->front());
			//scope->forceNew(GetFirstName(), *modifiers);
		}
		void Variable::Set (Context &ctx, ValuePass var, bool force) {
			errorTrace("Setting value of variable: " << GetFirstName());
			if(force || !modifiers->empty()) {
				assert(ctx.scope);
				assert(dynamic_cast<Scope*>(ctx.scope));
				assert(0); // TODO:
				//dynamic_cast<Scope*>(ctx.scope)->forceNew(GetFirstName(), *modifiers);
			} else {
				ctx.scope->set(Identifier(GetFirstName()), var);
			}
			// scope->Debug();
			// ilang::Variable *v;
			// if(force || !modifiers->empty()) {
			// 	v = scope->forceNew(GetFirstName(), *modifiers);
			// } else {
			// 	//v = scope->lookup(name->front());
			// 	v = Get(scope);
			// }
			// assert(v);
			// // TODO: setting of variables
			// //v->Set(var);
			// debug(-6,"Set: " << GetFirstName() << " " << var << " " << v->Get());
			// scope->Debug();
		}
		ilang::Variable * Variable::Get(Context &ctx) {
			// this should not happen
			assert(0);
			return NULL;
		}

		ValuePass Variable::GetValue(Context &ctx) {
			return ctx.scope->get(Identifier(GetFirstName()));

			// errorTrace("Getting value of variable: "<<GetFirstName());
			// ilang::Variable *v = Get(scope);
			// error(v, "Variable " << GetFirstName() << " Not found"); // maybe an assert, not an error?
			// //assert(v);
			// // TODO:
			// ValuePass p;// = v->Get();
			// // TODO: fix this to make it correct, if there is no value set to a variable then it is an error atm
			// error(p, "Variable " << GetFirstName() << " Not found");
			// return p;
		}
		std::string Variable::GetFirstName() {
			return name->front();
		}

		void Variable::Print(Printer *p) {
			if(modifiers) {
				for(std::string it : *modifiers) {
					p->p() << it << " ";
				}
			}
			bool first=true;
			for(std::string it : *name) {
				p->p() << (first ? "" : ".") << it;
				first = false;
			}
		}



		FieldAccess::FieldAccess (Node *obj, std::string id) :Variable(NULL, NULL), identifier(id), Obj(NULL) {
			if(obj) {
				assert(dynamic_cast<Value*>(obj));
				Obj = dynamic_cast<Value*>(obj);
			}
		}
		ilang::Variable * FieldAccess::Get(Context &ctx) {
			// bug in this b/c of function pointer
			assert(0);
			// errorTrace("FieldAccess on " << GetFirstName());
			// ilang::Variable *v;
			// if(Obj) {
			// 	_errors.stream() << "\n\t\tLooking where there is an object";
			// 	// this might cause problems if what is returned is not an object
			// 	ValuePass obj_val = Obj->GetValue(scope);
			// 	assert(obj_val);
			// 	boost::any & a = obj_val->Get();
			// 	error(a.type() == typeid(ilang::Object*), "trying to find fields in value that does not contain fields, essently not an object\n\t\twas looking for: " << GetFirstName());
			// 	ilang::Object *obj = boost::any_cast<ilang::Object*>(a);
			// 	v = obj->operator[](identifier);
			// 	if(v->Get()->Get().type() == typeid(ilang::Function)) {
			// 		// so that the reference to the object is keep around
			// 		// Need to fix this so that clean up works
			// 		// TODO: make this work with this new function system

			// 		// boost::any_cast<ilang::Function>( &(v->Get()->Get()) )->object = obj_val;
			// 	}
			// }else{
			// 	v = scope->lookup(identifier);
			// }
			// error(v, "Did not find " << GetFirstName());
			// return v;
		}

		ValuePass FieldAccess::GetValue(Context &ctx) {
			errorTrace("FieldAccess on " << GetFirstName());
			if(Obj) {
				_errors.stream() << "\n\tLooking up FieldAccess where there is an object";
				ValuePass obj_val = Obj->GetValue(ctx);
				assert(obj_val);
				ValuePass ret = obj_val->get(valueMaker(identifier));
				// TODO: functions binding???
				// should put functions bunding to scopes on their get value
				// rather than the field access
				return ret;

				// boost::any & a = obj_val->Get();
				// error(a.type() == typeid(ilang::Object*), "Trying to find field in value that does not contain field, essently not an object\n\tWas looking in: " << GetFirstName());
				// ilang::Object *obj = boost::any_cast<ilang::Object*>(a);
				// ilang::Variable *v = obj->operator[](identifier);
				// ValuePass ret = v->Get();
				// if(ret->Get().type() == typeid(ilang::Function)) {
				// 	// doing it this way creates a new pointer to the function and then sets the new function to point back to the object, otherwise the object can have smart pointers to itself which causes it to no be deleted from memory

				// 	ilang::Function fun = boost::any_cast<ilang::Function>(ret->Get());
				// 	ilang::Function fun_new = fun.bind(obj_val);
				// 	ret = ValuePass(new ilang::Value_Old(fun_new));
				// }
			//return ret;
			}else{
				return ctx.scope->get(Identifier(identifier));
				// ilang::Variable *v = scope->lookup(identifier);
				// error(v, "Did not find " << GetFirstName());

				// //ValuePass ret = v->Get();
				// return valueMaker(false); // TODO:
			}
		}

		void FieldAccess::Set(Context &ctx, ValuePass val, bool force) {
			errorTrace("Setting value of variable : " << GetFirstName());
			if (Obj) {
				ValuePass obj_val = Obj->GetValue(ctx);
				ValuePass key = valueMaker(identifier);
				obj_val->set(key, val);
			}else{
				Identifier name(identifier);
				ctx.scope->set(name, val);
			}

			// ilang::Variable *v;
			// if(Obj) {
			// 	_errors.stream() << "\n\tLooking up FieldAccess of object to set";
			// 	ValuePass obj_val = Obj->GetValue(scope);
			// 	assert(obj_val);
			// 	boost::any & a = obj_val->Get();
			// 	error(a.type() == typeid(ilang::Object*), "Trying to find field in value that does not contain field, essently not an object\n\tWas looking in: " << GetFirstName());
			// 	ilang::Object *obj = boost::any_cast<ilang::Object*>(a);
			// 	v = obj->operator[](identifier);
			// }else{
			// 	if(force || !modifiers->empty()) {
			// 		v = scope->forceNew(GetFirstName(), *modifiers);
			// 	}else{
			// 		v = scope->lookup(identifier);
			// 	}
			// }
			// assert(v);
			// v->Set(var);

		}

		std::string FieldAccess::GetFirstName() {
			return identifier;
		}

		void FieldAccess::Print(Printer *p) {
			if(Obj) {
				Obj->Print(p);
				p->p() << ".";
			}
			p->p() << identifier;
		}



		ArrayAccess::ArrayAccess(Node *obj, Node *look):Variable(NULL,NULL) {
			assert(dynamic_cast<Value*>(obj));
			Obj = dynamic_cast<Value*>(obj);
			assert(dynamic_cast<Value*>(look));
			Lookup = dynamic_cast<Value*>(look);
		}

		ilang::Variable * ArrayAccess::Get(Context &ctx) {
			// TODO: ??
			assert(0);
			// errorTrace("Getting element using []: "<<GetFirstName());
			// ValuePass obj_val = Obj->GetValue(scope);
			// boost::any & a = obj_val->Get();
			// error(a.type() == typeid(ilang::Object*), "Not of an object or array type");
			// ilang::Object *obj = boost::any_cast<ilang::Object*>(a);
			// ValuePass val = Lookup->GetValue(scope);
			// ilang::Variable *var = obj->operator[](val);
			// if(var->Get()->Get().type() == typeid(ilang::Function)) {
			// 	// TODO: fix to use new function interface

			// 	//boost::any_cast<ilang::Function>( &(var->Get()->Get()) )->object = obj_val;
			// }
			// return var;
		}

		ValuePass ArrayAccess::GetValue(Context &ctx) {
			errorTrace("Getting element using []: "<<GetFirstName());

			ValuePass obj_val = Obj->GetValue(ctx);
			ValuePass key = Lookup->GetValue(ctx);
			ValuePass ret = obj_val->get(key);
			return ret;

			// boost::any & a = obj_val->Get();
			// error(a.type() == typeid(ilang::Object*), "Not of an object or array type");
			// ilang::Object *obj = boost::any_cast<ilang::Object*>(a);
			// ValuePass val = Lookup->GetValue(scope);
			// ilang::Variable *var = obj->operator[](val);
			// ValuePass ret = var->Get();
			// if(ret->Get().type() == typeid(ilang::Function)) {
			// 	// TODO: fix to use new function interface
			// 	ilang::Function fun = boost::any_cast<ilang::Function>(ret->Get());
			// 	ilang::Function fun_new = fun.bind(obj_val);
			// 	ret = ValuePass(new ilang::Value_Old(fun_new));
			// }
			// return ret;
		}

		void ArrayAccess::Set(Context &ctx, ValuePass var, bool force) {
			assert(!force);
			errorTrace("Setting element using []: " << GetFirstName());
			ValuePass obj_val = Obj->GetValue(ctx);
			ValuePass key = Lookup->GetValue(ctx);
			obj_val->set(key, var);
			// boost::any & a = obj_val->Get();
			// error(a.type() == typeid(ilang::Object*), "Not of an object or array type");
			// ilang::Object *obj = boost::any_cast<ilang::Object*>(a);
			// ValuePass index = Lookup->GetValue(scope);
			// ilang::Variable *v = obj->operator[](index);
			// v->Set(var);
		}

		std::string ArrayAccess::GetFirstName() {
			return "Array access name";
		}

		void ArrayAccess::Print(Printer *p) {
			Obj->Print(p);
			p->p() << "[";
			Lookup->Print(p);
			p->p() << "]";
		}

		Call::Call (Value *call, list<Node*> *args):
			calling(call), params(args) {
			debug(-5,"\t\t\tCalling function");
		}
		void Call::Run(Context &ctx) {
			debug(-6,"call run");
			ValuePass ret = GetValue(ctx);
		}
		ValuePass Call::GetValue (Context &ctx) {
			errorTrace("Calling function");
			// new stuff

			ValuePass func = calling->GetValue(ctx);
			// boost::any &an = func->Get();
			// error(an.type() == typeid(ilang::Function),
			// 			"Calling a non function " << an.type().name());
			// ilang::Function *function = boost::any_cast<ilang::Function>(&an);

			ilang::Arguments args;
			for(Node *n : *params) {
				// so nasty
				// TODO: support for kwargs
				assert(dynamic_cast<parserNode::Value*>(n));
				args.push(dynamic_cast<parserNode::Value*>(n)->GetValue(ctx));
			}

			ValuePass ret = func->call(args);
			return ret;






			// old
			// std::vector<ValuePass> par;
			// for(Node * n : *params) {
			// 	assert(dynamic_cast<parserNode::Value_Old*>(n));
			// 	par.push_back(dynamic_cast<parserNode::Value_Old*>(n)->GetValue_Old(scope));
			// }
			// ValuePass ret = ValuePass(new ilang::Value_Old);
			// ValuePass func = calling->GetValue(scope);
			// boost::any &an = func->Get();
			// // TODO: this error is overly cryptic
			// error(an.type() == typeid(ilang::Function), "Calling a non function " << an.type().name());
			// ilang::Function *function = boost::any_cast<ilang::Function>(&an);
			// if(function->native) {
			// 	function->ptr(scope, par, &ret);
			// }else if(function->object) {
			// 	assert(function->object->Get().type() == typeid(ilang::Object*));
			// 	ScopePass obj_scope = ScopePass(new ObjectScope(boost::any_cast<ilang::Object*>(function->object->Get())));
			// 	function->ptr(obj_scope, par, &ret);
			// }else{
			// 	function->ptr(ScopePass(), par, &ret);
			// }
			// return ret;

		}

		void Call::Print(Printer *p) {
			calling->Print(p);
			p->p() << "(";
			bool first=true;
			for(auto it : *params) {
				if(!first) p->p() << ", ";
				it->Print(p);
				first = false;
			}
			p->p() << ")";
		}

		PrintCall::PrintCall(list<Node*> *args):
			Call(NULL, args) {}

		ValuePass PrintCall::GetValue (Context &ctx) {
			errorTrace("Print call");
			debug(-6,"made into the print Call");
			for(Node *it : *params) {
				assert(dynamic_cast<parserNode::Value*>(it));
				dynamic_cast<parserNode::Value*>((it))->GetValue(ctx)->Print();
			}
			debug(-5,"made it out of print");
		}

		void PrintCall::Print (Printer *p) {
			p->p() << "Print(";
			bool first = true;
			for(Node *it : *params) {
				if(!first) p->p() << ", ";
				it->Print(p);
				first = false;
			}
			p->p() << ")";
		}


		AssignExpr::AssignExpr (Variable *t, Value *v):target(t), eval(v) {
			assert(eval);
			assert(target);
		}
		void AssignExpr::Run (Context &ctx) {
			errorTrace("Assignment expression");
			ValuePass v = eval->GetValue(ctx);

			target->Set(ctx, v);
			//	scope->Debug();
		}
		ValuePass AssignExpr::GetValue (Context &ctx) {
			errorTrace("Assignment expression with gettin value");
			ValuePass v = eval->GetValue(ctx);
			target->Set(ctx, v);
			return v;
		}

		void AssignExpr::Print(Printer *p) {
			target->Print(p);
			p->p() << " = ";
			eval->Print(p);
		}

		MathEquation::MathEquation(Value *l, Value *r, action a) : left(l), right(r), Act(a) {
			assert(left);
			if(a != uMinus) assert(right);
		}
		void MathEquation::Run(Context &ctx) {
			errorTrace("Running math equation w/o getting value");
			left->Run(ctx);
			if(Act != uMinus) right->Run(ctx);
			// we do not need the value so we should not request the vale from the nodes
		}
		ValuePass MathEquation::GetValue(Context &ctx) {
			/*			errorTrace("Math equation");
			ValuePass left = this->left->GetValue(scope);
			ValuePass right;
			if(Act != uMinus) right = this->right->GetValue(scope);
			/* when trying for speed boost
				 auto left_type = left->Get().type();
				 auto right_type = right->Get().type();
			/ /
			switch(Act) {
			case uMinus:
				if(left->Get().type() == typeid(std::string)) {
					error(0, "can not uMinus a string type");
				}else if(left->Get().type() == typeid(double)) {
					return ValuePass(new ilang::Value_Old(- boost::any_cast<double>(left->Get())));
				}else if(left->Get().type() == typeid(long)) {
					return ValuePass(new ilang::Value_Old(- boost::any_cast<long>(left->Get())));
				}
				break;
			case add:
				if(left->Get().type() == typeid(std::string) || right->Get().type() == typeid(std::string)) {
					return ValuePass(new ilang::Value_Old(std::string(left->str() + right->str())));
				}else if(left->Get().type() == typeid(double)) {
					if(right->Get().type() == typeid(double))
						return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) + boost::any_cast<double>(right->Get())));
					else if(right->Get().type() == typeid(long))
						return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) + boost::any_cast<long>(right->Get())));
				}else if(left->Get().type() == typeid(long)) {
					if(right->Get().type() == typeid(double))
						return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) + boost::any_cast<double>(right->Get())));
					else if(right->Get().type() == typeid(long))
						return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) + boost::any_cast<long>(right->Get())));
				}
				break;
			case subtract:
				if(left->Get().type() == typeid(double)) {
					if(right->Get().type() == typeid(double))
						return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) - boost::any_cast<double>(right->Get())));
					else if(right->Get().type() == typeid(long))
						return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) - boost::any_cast<long>(right->Get())));
				}else if(left->Get().type() == typeid(long)) {
					if(right->Get().type() == typeid(double))
						return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) - boost::any_cast<double>(right->Get())));
					else if(right->Get().type() == typeid(long))
						return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) - boost::any_cast<long>(right->Get())));
				}
				break;
			case multiply:
				if(left->Get().type() == typeid(double)) {
					if(right->Get().type() == typeid(double))
						return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) * boost::any_cast<double>(right->Get())));
					else if(right->Get().type() == typeid(long))
						return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) * boost::any_cast<long>(right->Get())));
				}else if(left->Get().type() == typeid(long)) {
					if(right->Get().type() == typeid(double))
						return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) * boost::any_cast<double>(right->Get())));
					else if(right->Get().type() == typeid(long))
						return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) * boost::any_cast<long>(right->Get())));
				}
				break;
			case divide:
				if(left->Get().type() == typeid(double)) {
					if(right->Get().type() == typeid(double))
						return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) / boost::any_cast<double>(right->Get())));
					else if(right->Get().type() == typeid(long))
						return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) / boost::any_cast<long>(right->Get())));
				}else if(left->Get().type() == typeid(long)) {
					if(right->Get().type() == typeid(double))
						return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) / boost::any_cast<double>(right->Get())));
					else if(right->Get().type() == typeid(long))
						return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) / boost::any_cast<long>(right->Get())));
				}
				break;
			}
			error(0, "Problem with math equation");
*/
		}

		void MathEquation::Print(Printer *p) {
			if(Act != uMinus) left->Print(p);
			switch(Act) {
			case add:
				p->p() << " + ";
				break;
			case subtract:
				p->p() << " - ";
				break;
			case multiply:
				p->p() << " * ";
				break;
			case divide:
				p->p() << " / ";
				break;
			case uMinus:
				p->p() << "-";
				break;
			}
			if(Act == uMinus) left->Print(p);
			else right->Print(p);
		}

		LogicExpression::LogicExpression(Value *l, Value *r, action a): left(l), right(r), Act(a) {
			assert(left);
			if(a != Not) assert(right) ;
		}
		void LogicExpression::Run (Context &ctx) {
			errorTrace("Logic Expression w/o getting value");
			switch(Act) {
			case And:
				if(left->GetValue(ctx)->isTrue())
					right->Run(ctx);
				break;
			case Or:
				if(!left->GetValue(ctx)->isTrue())
					right->Run(ctx);
				break;
			case Not:
				left->Run(ctx);
				// no right
			default:
				left->Run(ctx);
				right->Run(ctx);
			}
		}
		ValuePass LogicExpression::GetValue(Context &ctx) {
			errorTrace("Logic Expression");
			assert(0);
			ValuePass left = this->left->GetValue(ctx);
			if(Act == Not)
				return valueMaker(! left->isTrue() );
			ValuePass right = this->right->GetValue(ctx);
			// cout << "logic check " << left->Get().type().name() << " " << right->Get().type().name() << endl;
			// cout << left->str() << " " << right->str() << endl
			//		 << (left->Get().type() == typeid(bool)) << " " << (right->Get().type() == typeid(long)) << endl;
			switch(Act) {
			case And:
				if(left->isTrue())
					if(right->isTrue())
						return valueMaker(true);
				return valueMaker(false);
				break;
			case Or:
				return valueMaker(left->isTrue() || right->isTrue());
			}
			// case Equal:
			// 	if(left->Get().type() == typeid(std::string) && right->Get().type() == typeid(std::string))
			// 		return ValuePass(new ilang::Value_Old(boost::any_cast<std::string>(left->Get()) == boost::any_cast<std::string>(right->Get())));
			// 	if(left->Get().type() == typeid(double)) {
			// 		if(right->Get().type() == typeid(double))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) == boost::any_cast<double>(right->Get())));
			// 		else if(right->Get().type() == typeid(long))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) == boost::any_cast<long>(right->Get())));
			// 		else
			// 			return ValuePass(new ilang::Value_Old(false));
			// 	}else if(left->Get().type() == typeid(long)) {
			// 		if(right->Get().type() == typeid(double))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) == boost::any_cast<double>(right->Get())));
			// 		else if(right->Get().type() == typeid(long))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) == boost::any_cast<long>(right->Get())));
			// 		else
			// 			return ValuePass(new ilang::Value_Old(false));
			// 	}
			// 	break;
			// case Not_Equal:
			// 	if(left->Get().type() == typeid(std::string) && right->Get().type() == typeid(std::string))
			// 		return ValuePass(new ilang::Value_Old(boost::any_cast<std::string>(left->Get()) != boost::any_cast<std::string>(right->Get())));
			// 	if(left->Get().type() == typeid(double)) {
			// 		if(right->Get().type() == typeid(double))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) != boost::any_cast<double>(right->Get())));
			// 		else if(right->Get().type() == typeid(long))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) != boost::any_cast<long>(right->Get())));
			// 		else
			// 			return ValuePass(new ilang::Value_Old(false));
			// 	}else if(left->Get().type() == typeid(long)) {
			// 		if(right->Get().type() == typeid(double))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) != boost::any_cast<double>(right->Get())));
			// 		else if(right->Get().type() == typeid(long))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) != boost::any_cast<long>(right->Get())));
			// 		else
			// 			return ValuePass(new ilang::Value_Old(false));
			// 	}
			// 	break;
			// case Less_Equal:
			// 	if(left->Get().type() == typeid(double)) {
			// 		if(right->Get().type() == typeid(double))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) <= boost::any_cast<double>(right->Get())));
			// 		else if(right->Get().type() == typeid(long))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) <= boost::any_cast<long>(right->Get())));
			// 		else
			// 			return ValuePass(new ilang::Value_Old(false));
			// 	}else if(left->Get().type() == typeid(long)) {
			// 		if(right->Get().type() == typeid(double))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) <= boost::any_cast<double>(right->Get())));
			// 		else if(right->Get().type() == typeid(long))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) <= boost::any_cast<long>(right->Get())));
			// 		else
			// 			return ValuePass(new ilang::Value_Old(false));
			// 	}
			// 	break;
			// case Greater_Equal:
			// 	if(left->Get().type() == typeid(double)) {
			// 		if(right->Get().type() == typeid(double))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) >= boost::any_cast<double>(right->Get())));
			// 		else if(right->Get().type() == typeid(long))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) >= boost::any_cast<long>(right->Get())));
			// 		else
			// 			return ValuePass(new ilang::Value_Old(false));
			// 	}else if(left->Get().type() == typeid(long)) {
			// 		if(right->Get().type() == typeid(double))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) >= boost::any_cast<double>(right->Get())));
			// 		else if(right->Get().type() == typeid(long))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) >= boost::any_cast<long>(right->Get())));
			// 		else
			// 			return ValuePass(new ilang::Value_Old(false));
			// 	}
			// 	break;
			// case Less:
			// 	if(left->Get().type() == typeid(double)) {
			// 		if(right->Get().type() == typeid(double))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) < boost::any_cast<double>(right->Get())));
			// 		else if(right->Get().type() == typeid(long))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) < boost::any_cast<long>(right->Get())));
			// 		else
			// 			return ValuePass(new ilang::Value_Old(false));
			// 	}else if(left->Get().type() == typeid(long)) {
			// 		if(right->Get().type() == typeid(double))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) < boost::any_cast<double>(right->Get())));
			// 		else if(right->Get().type() == typeid(long))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) < boost::any_cast<long>(right->Get())));
			// 		else
			// 			return ValuePass(new ilang::Value_Old(false));
			// 	}
			// 	break;
			// case Greater:
			// 	if(left->Get().type() == typeid(double)) {
			// 		if(right->Get().type() == typeid(double))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) > boost::any_cast<double>(right->Get())));
			// 		else if(right->Get().type() == typeid(long))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<double>(left->Get()) > boost::any_cast<long>(right->Get())));
			// 		else
			// 			return ValuePass(new ilang::Value_Old(false));
			// 	}else if(left->Get().type() == typeid(long)) {
			// 		if(right->Get().type() == typeid(double))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) > boost::any_cast<double>(right->Get())));
			// 		else if(right->Get().type() == typeid(long))
			// 			return ValuePass(new ilang::Value_Old(boost::any_cast<long>(left->Get()) > boost::any_cast<long>(right->Get())));
			// 		else
			// 			return ValuePass(new ilang::Value_Old(false));
			// 	}
			// 	break;
			// }
			return valueMaker(false);
		}

		void LogicExpression::Print(Printer *p) {
			if(Act == Not) {
				p->p() << "!";
				left->Print(p);
				return;
			}
			left->Print(p);
			switch(Act) {
			case Equal:
				p->p() << " == ";
				break;
			case Not_Equal:
				p->p() << " != ";
				break;
			case Less_Equal:
				p->p() << " <= ";
				break;
			case Greater_Equal:
				p->p() << " >= ";
				break;
			case Less:
				p->p() << " < ";
				break;
			case Greater:
				p->p() << " < ";
				break;
			case And:
				p->p() << " && ";
				break;
			case Or:
				p->p() << " || ";
				break;
			}
			right->Print(p);
		}

		SingleExpression::SingleExpression(Variable *target, Value *value, action a) : m_target(target), m_value(value), Act(a) {
			assert(target);
			assert(value);
		}

		void SingleExpression::Run(Context &ctx) {
			ValuePass v = GetValue(ctx);
		}

		ValuePass SingleExpression::GetValue(Context &ctx) {
			errorTrace("Single Expression");
			assert(0);
			ValuePass val = m_value->GetValue(ctx);
			ValuePass setTo = m_target->GetValue(ctx);
			ValuePass ret;
			// if(setTo->Get().type() == typeid(std::string)) {
			// 	error(Act == add, "Can only add to a string");
			// 	ret = ValuePass(new ilang::Value_Old(boost::any_cast<std::string>(setTo->Get()) + val->str()));
			// 	m_target->Set(scope, ret);
			// 	return ret;
			// }
			// if(val->Get().type() == typeid(long)) {
			// 	long n = boost::any_cast<long>(val->Get());
			// 	switch(Act) {
			// 	case add:
			// 		if(setTo->Get().type() == typeid(long)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<long>(setTo->Get()) + n));
			// 		}else if(setTo->Get().type() == typeid(double)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<double>(setTo->Get()) + n));
			// 		}else{
			// 			error(0, "Can't perform math on non numbers");
			// 		}
			// 		break;
			// 	case subtract:
			// 		if(setTo->Get().type() == typeid(long)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<long>(setTo->Get()) - n));
			// 		}else if(setTo->Get().type() == typeid(double)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<double>(setTo->Get()) - n));
			// 		}else{
			// 			error(0, "Can't perform math on non numbers");
			// 		}
			// 		break;
			// 	case multiply:
			// 		if(setTo->Get().type() == typeid(long)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<long>(setTo->Get()) * n));
			// 		}else if(setTo->Get().type() == typeid(double)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<double>(setTo->Get()) * n));
			// 		}
			// 		break;
			// 	case divide:
			// 		if(setTo->Get().type() == typeid(long)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<long>(setTo->Get()) / n));
			// 		}else if(setTo->Get().type() == typeid(double)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<double>(setTo->Get()) / n));
			// 		}else{
			// 			error(0, "Can't perform math on non numbers");
			// 		}
			// 		break;
			// 	default:
			// 		assert(0);
			// 	}
			// 	m_target->Set(scope, ret);
			// 	return ret;
			// }else if(val->Get().type() == typeid(double)) {
			// 	double n = boost::any_cast<double>(val->Get());
			// 	switch(Act) {
			// 	case add:
			// 		if(setTo->Get().type() == typeid(double)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<double>(setTo->Get()) + n));
			// 		}else if(setTo->Get().type() == typeid(long)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<long>(setTo->Get()) + n));
			// 		}else{
			// 			error(0, "Can't perform math on non numbers");
			// 		}
			// 		break;
			// 	case subtract:
			// 		if(setTo->Get().type() == typeid(double)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<double>(setTo->Get()) - n));
			// 		}else if(setTo->Get().type() == typeid(long)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<long>(setTo->Get()) -n));
			// 		}else{
			// 			error(0, "Can't perform math on non numbers");
			// 		}
			// 		break;
			// 	case multiply:
			// 		if(setTo->Get().type() == typeid(double)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<double>(setTo->Get()) * n));
			// 		}else if(setTo->Get().type() == typeid(long)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<long>(setTo->Get()) * n));
			// 		}else{
			// 			error(0, "Can't perform math on non numbers");
			// 		}
			// 		break;
			// 	case divide:
			// 		if(setTo->Get().type() == typeid(double)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<double>(setTo->Get()) / n));
			// 		}else if(setTo->Get().type() == typeid(long)) {
			// 			ret = ValuePass(new ilang::Value_Old(boost::any_cast<long>(setTo->Get()) / n));
			// 		}else{
			// 			error(0, "Can't perform math on non numbers");
			// 		}
			// 		break;
			// 	default:
			// 		assert(0);
			// 	}
			// 	m_target->Set(scope, ret);
			// 	return ret;
			// }else{
			// 	error(0, "Did not have type of number");
			// }
		}

		void SingleExpression::Print(Printer *p) {
			assert(0);
			m_target->Print(p);
			switch(Act) {
			case add:
				p->p() << " += ";
				break;
			case subtract:
				p->p() << " -= ";
				break;
			case multiply:
				p->p() << " *= ";
				break;
			case divide:
				p->p() << " /= ";
				break;
			default:
				assert(0);
			}
			m_value->Print(p);
		}


		Object::Object (std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj) : objects(obj)
		{
			// object created later so we are just going to store the informationa atm
		}
		void Object::Run(Context &ctx) {
			// could possibly go through all the elements and call run
			// should this assert that the types on the return match??
			for(auto it : *objects) {
				it.second->Run(ctx);
			}
		}
		ValuePass Object::GetValue(Context &ctx) {
			// will create a new object and return that as when the object is evualiated we do not want to be returing the same old thing
			errorTrace("Creating object");
			debug(-6, "Object getting value");
			// can use scope.get() to access the pointer as the scope is not keep around after the class/object is created
			auto obj = make_shared<ilang::Object>(objects, ctx);
			return valueMaker(obj);
			//ilang::Value_Old *val = new ilang::Value_Old(obj);
			//return ValuePass(val);
		}

		void Object::Print(Printer *p) {
			p->p() << "object { ";
			p->up();
			bool first=true;
			for(auto it : *objects) {
				if(!first) p->p() << ", ";
				p->line();
				it.first->Print(p);
				p->p() << ": ";
				it.second->Print(p);
				first = false;
			}
			p->down();
			p->line() << "}";
		}

		Class::Class(std::list<Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj): parents(p), objects(obj)
		{
			assert(p);
			assert(obj);
		}
		void Class::Run(Context &ctx) {
			for(auto it : *objects) {
				it.second->Run(ctx);
			}
		}
		ValuePass Class::GetValue(Context &ctx) {
			errorTrace("Creating class");
			auto cls = make_shared<ilang::Class>(parents, objects, ctx);
			return valueMaker(cls);
			//ilang::Class *c = new ilang::Class(parents, objects, scope);
			//ilang::Value_Old *val = new ilang::Value_Old(c);
			//return ValuePass(val);
		}

		void Class::Print(Printer *p) {
			p->p() << "class ";
			if(parents && !parents->empty()) {
				p->p() << "(";
				bool first = true;
				for(Node *it : *parents) {
					if(!first) p->p() << ", ";
					it->Print(p);
					first = false;
				}
				p->p() << ") ";
			}
			p->up();
			p->p() << "{";
			bool first=true;
			for(auto it : *objects) {
				if(!first) p->p() << ", ";
				p->line();
				it.first->Print(p);
				p->p() << ": ";
				it.second->Print(p);
				first = false;
			}
			p->down();
			p->line() << "}";
		}

		Array::Array (std::list<Node*> *e, std::list<string> *m) : elements(e), modifiers(m) {
			assert(e);
			if(!modifiers) modifiers = new std::list<string>;
		}

		void Array::Run(Context &ctx) {
			errorTrace("Creating Array w/o getting value");
			for(auto it=elements->begin(); it!= elements->end(); it++) {
				(*it)->Run(ctx);
			}
		}

		ValuePass Array::GetValue(Context &ctx) {
			errorTrace("Creating Array");
			auto arr = make_shared<ilang::Array>(elements, modifiers, ctx);
			return valueMaker(arr);
			//ilang::Object *arr = new ilang::Array(elements, modifiers, scope);
			//ilang::Value_Old *val = new ilang::Value_Old(arr);
			//return ValuePass(val);
		}

		void Array::Print(Printer *p) {
			p->p() << "[";
			if(modifiers && !modifiers->empty()) {
				bool first=true;
				for(std::string it : *modifiers) {
					if(!first) p->p() << " ";
					p->p() << it;
					first = false;
				}
				p->p() << "| ";
			}
			bool first=true;
			for(Node *it : *elements) {
				if(!first) p->p() << ", ";
				it->Print(p);
				first = false;
			}
			p->p() << "]";
		}

		NewCall::NewCall(std::list<Node*> *args): Call(NULL, args) {
			// might change the grammer to reflect that this needs one element
			// might in the future allow for arguments to be passed to classes when they are getting constructed through additional arguments

			error(args->size() == 1, "New only takes one argument");
			error(dynamic_cast<Value_Old*>(args->front()), "First argument to New needs to be a Value_old");
		}
		ValuePass NewCall::GetValue(Context &ctx) {
			errorTrace("New Call");
			assert(0); // TODO:
			// ValuePass a = dynamic_cast<Value*>(params->front())->GetValue(scope);
			// error(a->Get().type() == typeid(ilang::Class*), "Can not create something with new that is not a class");
			// ilang::Value_Old *val = new ilang::Value_Old( boost::any_cast<ilang::Class*>(a->Get())->NewClass(a) ); // returns an Object*
			// //ilang::Value_Old *val = new ilang::Value_Old( new ilang::Object(a) );
			// // TODO: make this call an init function that is defined in the class
			// // does this need to call the init function, as default values can be set and no arguments can be passed when the new function is called
			// return ValuePass(val);
		}

		void NewCall::Print (Printer *p) {
			p->p() << "new(";
			params->front()->Print(p);
			p->p() << ")";
		}

		AssertCall::AssertCall(int line, const char *name, list<Node*> *args): Call(NULL, args), lineN(line), fileName(name) {
			for(auto it : *args) {
				error(dynamic_cast<Value*>(it), "Arguments to assert need to have some Value");
			}
		}
		ValuePass AssertCall::GetValue(Context &ctx) {
			for(auto it : *params) {
				ValuePass val = dynamic_cast<Value*>(it)->GetValue(ctx);
				if(!val->isTrue()) {
					// used for unit testing
					if(ilang_Assert_fails) {
						ilang_Assert_fails++;
						return ValuePass();
					}
					error_print_trace();
					cerr << "\n\nfailang assert " << fileName << ":" << lineN << endl;
					exit(2);
				}
			}
			return ValuePass();
		}
		void AssertCall::Print (Printer *p) {
			p->p() << "assert(";
			for(Node *it : *params) {
				it->Print(p); // problem with printing commas between
			}
			p->p() << ")";
		}

		ImportCall::ImportCall(list<Node*> *args) : Call(NULL, args) {
			error(args->size() == 1, "import() expects 1 argument");
			error(dynamic_cast<Value_Old*>(args->front()), "import() expects a Value_old");
		}

		ValuePass ImportCall::GetValue(Context &ctx) {
			ValuePass val = dynamic_cast<Value*>(params->front())->GetValue(ctx);
			//error(val->Get().type() == typeid(std::string), "import() expects a string");
			std::string name = val->cast<std::string>(); //boost::any_cast<std::string>(val->Get());

			auto obj = ImportGetByName(name);
			if(!obj) return valueMaker(false);

			//if(obj == NULL) return ValuePass(new ilang::Value_Old(false));

			return valueMaker(obj); //ValuePass(new ilang::Value_Old(obj));

		}

		void ImportCall::Print(Printer *p) {
			p->p() << "import(";
			params->front()->Print(p);
			p->p() << ")";
		}


		ThreadGoCall::ThreadGoCall(list<Node*> *args) : Call(NULL, args) {
			// first argument is the function that will be called
			// following arguments are values to be passed to the function, but they will be evaluated in the current thread
			error(args->size() >= 1, "go call expects at least 1 argument " << args->size());
			for(auto it : *args) {
				assert(dynamic_cast<Value_Old*>(it));
			}
		}

		ValuePass ThreadGoCall::GetValue(Context &ctx) {
			// TODO: rewrite, this is shit
			ValuePass calling_val = dynamic_cast<Value*>(params->front())->GetValue(ctx);
			error(calling_val->type() == typeid(ilang::Function), "go expects argument to be a function");
			// TODO: should allocate this function along with the arguments
			ilang::Function calling = *calling_val->cast<ilang::Function*>();
			//ilang::Function calling = boost::any_cast<ilang::Function>(calling_val->Get());
			ilang::Arguments *arguments = new ilang::Arguments;
			auto it = params->begin();
			it++;
			while(it != params->end()) {
				arguments->push(dynamic_cast<Value*>(*it)->GetValue(ctx));
				it++;
			}
			assert(ilang::global_EventPool());
			Event thread = ilang::global_EventPool()->CreateEvent([arguments, calling](void *data) {

					Context ctx;
					const_cast<ilang::Function*>(&calling)->call(ctx, *arguments);

					delete arguments;


					// ValuePass ret = ValuePass(new ilang::Value_Old);

					// if(calling.object) {
					// 	assert(calling.object->Get().type() == typeid(ilang::Object*));

					// 	ScopePass obj_scope = ScopePass(new ObjectScope(boost::any_cast<ilang::Object*>(calling.object->Get())));
					// 	calling.ptr(obj_scope, *arguments, &ret);
					// }else{
					// 	calling.ptr(ScopePass(), *arguments, &ret);
					// }
					// delete arguments;

					// return value is ignored
				});
			thread.Trigger(NULL);
		}

		void ThreadGoCall::Print(Printer *p) {
			p->p() << "go(";
			for(auto it : *params) {
				it->Print(p);
			}
			p->p() << ")";
		}


	} // namespace parserTree
} // namespace ilang
