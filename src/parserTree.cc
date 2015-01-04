// for string escape
#include <boost/algorithm/string.hpp>

#include <iostream>

#include "parserTree.h"
#include "parser.h"
//#include "scope.h"
#include "object.h"
#include "function.h"
#include "thread.h"
#include "debug.h"
#include "error.h"
#include "tuple.h"
//#include "value_types.h"


using namespace std;

extern "C" int ilang_Assert_fails;

namespace ilang {
	namespace parserNode {
		Head::Head(list<Node*> *declars, ImportScopeFile *import): Import(import), Declars(declars) {
			debug(-5, "head " << declars->size() );
			debug(-5, "running ++++++++++++++++++++++++++++++++++++++++++++" );
		}

		Head::~Head() {
			delete scope;
		}

		void Head::Link () {
			assert(!scope);
			scope = new Scope(ctx);

			assert(ctx.scope == scope);
			if(Import) {
				Import->m_head = this;
				Import->resolve(ctx);
			}
			for(auto it : *Declars) {
				if(dynamic_cast<AssignExpr*>(it)) {
					dynamic_cast<AssignExpr*>(it)->PreRegister(ctx);
				}
			}

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
			ValuePass main = scope->get(ctx, Identifier("main"));
			error(main, "main function not found");
			ilang::Arguments args;
			main->call(ctx, args);
		}

		void Head::Print(Printer *p) {
			// TODO: print out the imports
			for(auto it : *Declars) {
				it->Print(p);
				p->p() << ";\n";
			}
		}

		Scope *Head::GetScope() {
			return scope;
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
		IdentifierSet Constant::UndefinedElements() {
			return IdentifierSet();
		}

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
			test = t;
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

		IdentifierSet IfStmt::UndefinedElements() {
			auto ret = test->UndefinedElements();
			if(True) ret = unionSets(ret, True->UndefinedElements());
			if(False) ret = unionSets(ret, False->UndefinedElements());
			return ret;
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

		IdentifierSet ForStmt::UndefinedElements() {
			// TODO: there should also be some way to determine defined elements that don't need to be captured, but what if they should be overwritten....I suppose there is no harm in capturing more
			IdentifierSet ret = unionSets(pre->UndefinedElements(), test->UndefinedElements(), exe->UndefinedElements(), each->UndefinedElements());
			return ret;
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
			test = t;
		}
		void WhileStmt::Run(Context &ctx) {
			// note to self: must recall GetValue each time as it is what is actually doing the updating
			ValuePass t = test->GetValue(ctx);
			while(t->isTrue()) {
				exe->Run(ctx);
				ValuePass n = test->GetValue(ctx);
				t = n;
			}
		}

		IdentifierSet WhileStmt::UndefinedElements() {
			IdentifierSet ret = unionSets(test->UndefinedElements(), exe->UndefinedElements());
			return ret;
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
			}else
				ret = NULL;
		}
		void ReturnStmt::Run(Context &ctx) {
			if(ret)
				ctx.returned = new ValuePass(ret->GetValue(ctx));
			else
				ctx.returned = (ilang::ValuePass*)1; // eh...haxs
		}

		IdentifierSet ReturnStmt::UndefinedElements() {
			if(ret)
				return ret->UndefinedElements();
			return IdentifierSet();
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
			if(params)
				for(auto it : *params) {
					// argument list can only be variables
					assert(dynamic_cast<parserNode::Variable*>(it));
				}

		}
		void Function::Run(Context &ctx) {
			// This is going to have duplicated code with calling function, minus dealing with the arguments
			Scope local_scope(ctx);
			PreRegister(ctx);

			if(body) {
				for (Node *n : *body) {
					n->Run(ctx);
					if(ctx.returned) break;
				}
			}
		}
		ValuePass Function::GetValue(Context &ctx) {
			// this need to track the scope at this point so that it could be use later in the funciton
			debug(-6, "Function get value");
			errorTrace("Getting value of a function");

			ilang::Function fun(this, ctx); // this calls bind
			auto ret = valueMaker(fun);
			return ret;
		}

		void Function::Constructor(Context &ctx) {
			PreRegister(ctx);
			if(body) {
				for(Node *n : *body) {
					n->Run(ctx);
					if(ctx.returned) break;
				}
			}
		}

		IdentifierSet Function::UndefinedElements() {
			IdentifierSet ret;
			if(body)
				for(auto it = body->rbegin(); it != body->rend(); it++) {
					ret = unionSets(ret, (*it)->UndefinedElements());
				}
			if(params) {
				for(auto it : *params) {
					ret = unionSets(ret, it->UndefinedElements());
					ret.erase(dynamic_cast<Variable*>(it)->GetName());
				}
			}

			return ret;
			// TODO: filter out elements that will be forced New, eg variables with types set for them
		}

		void Function::PreRegister(Context &ctx) {
			if(body) {
				for(auto it : *body) {
					auto v = dynamic_cast<AssignExpr*>(it);
					if(v) v->PreRegister(ctx);
				}
			}
		}

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

		ValuePass VariableScopeModifier::GetValue(Context &ctx) {
			return ValuePass(VariableType(type));
		}

		void VariableScopeModifier::Print(Printer *p) {
			switch(type) {
			case VariableType::t_local:
				p->p() << "Local";
				break;
			case VariableType::t_dynamic:
				p->p() << "Dynamic";
				break;
			}
		}

		Variable::Variable (Identifier n, list<Node*> *mod):
			name(n), modifiers(mod) {
			// TODO: remove this crutch
			if(!modifiers) modifiers = new list<Node*>;
			for(auto it : *modifiers) {
				assert(dynamic_cast<parserNode::Value*>(it));
				auto p = dynamic_cast<parserNode::VariableScopeModifier*>(it);
				if(p) {
						type = p->type;
				}
			}
		}
		void Variable::Run (Context &ctx) {
			// iirc this does not happen as the syntax does not allow for variables to be created without a default value
			error(0, "Variables must always have some value set to them");
		}
		void Variable::Set (Context &ctx, ValuePass var, bool force) {
			errorTrace("Setting value of variable: " << name.str());
			if(force || !modifiers->empty()) {
				assert(ctx.scope);
				assert(dynamic_cast<Scope*>(ctx.scope));
				// TODO: this is wrong, need to set the modifiers
				std::vector<ValuePass> mods;
				mods.reserve(modifiers->size());
				for(auto it : *modifiers) {
					auto g = dynamic_cast<parserNode::Value*>(it)->GetValue(ctx);
					assert(g);
					mods.push_back(g);
				}
				auto pvar = dynamic_cast<Scope*>(ctx.scope)->forceNew(ctx, name, mods);
				assert(var);
				pvar->Set(ctx, var);
			} else {
				ctx.scope->set(ctx, name, var);
			}
		}
		ilang::Variable * Variable::Get(Context &ctx) {
			// this should not happen
			assert(0);
			return NULL;
		}

		ValuePass Variable::GetValue(Context &ctx) {
			// TODO: maybe change it so variables are only on the lhs
			auto ret = ctx.scope->get(ctx, name);
			return ret;
		}

		IdentifierSet Variable::UndefinedElements() {
			IdentifierSet ret;
			ret.insert(name);
			for(auto it : *modifiers) {
				ret = unionSets(ret, it->UndefinedElements());
			}
			return ret;
		}

		void Variable::PreRegister(Context &ctx) {
			vector<ValuePass> mod;
			// this needs to check if it should actually force this to be new
			// eg opt register it?

			if(!modifiers->empty() || !ctx.scope->has(ctx, name)) {
				dynamic_cast<Scope*>(ctx.scope)->forceNew(ctx, name, mod);
			}
		}

		void Variable::Print(Printer *p) {
			if(modifiers) {
				for(auto it : *modifiers) {
					it->Print(p);
					p->p() << " ";
				}
			}
			p->p() << name.str();
		}



		FieldAccess::FieldAccess (Node *obj, Identifier id) : Variable(id, NULL), Obj(NULL) {
			if(obj) {
				assert(dynamic_cast<Value*>(obj));
				Obj = dynamic_cast<Value*>(obj);
			}
		}
		ilang::Variable * FieldAccess::Get(Context &ctx) {
			// bug in this b/c of function pointer
			// we shouldn't be getting the variable direcrly from a variable
			assert(0);
		}

		ValuePass FieldAccess::GetValue(Context &ctx) {
			errorTrace("FieldAccess on " << name.str());
			ValuePass ret;
			if(Obj) {
				_errors.stream() << "\n\tLooking up FieldAccess where there is an object";
				ValuePass obj_val = Obj->GetValue(ctx);
				assert(obj_val);
				try {
					ret = obj_val->get(ctx, name);
				} catch(InvalidOperation e) {
					if(obj_val->type() == typeid(Hashable*)) {
						error(0, "Object " << obj_val << "does not have member " << name.str());
					} else  {
						error(0, "Attempted to access member on non object type " << obj_val->type().name());
					}
				}
				return ret;
			}else{
				ret = ctx.scope->get(ctx, name);
				return ret;
			}
		}

		void FieldAccess::Set(Context &ctx, ValuePass val, bool force) {
			errorTrace("Setting value of variable : " << name.str());
			if (Obj) {
				ValuePass obj_val = Obj->GetValue(ctx);
				try {
					obj_val->set(ctx, name, val);
				} catch(InvalidOperation e) {
					error(0, "Attempted to set " << name.str() << " on non object type");
				}
			}else{
				ctx.scope->set(ctx, name, val);
			}
		}

		IdentifierSet FieldAccess::UndefinedElements() {
			if(Obj)
				return Obj->UndefinedElements();
			IdentifierSet ret;
			ret.insert(name);
			return ret;
		}

		void FieldAccess::Print(Printer *p) {
			if(Obj) {
				Obj->Print(p);
				p->p() << ".";
			}
			p->p() << name.str();
		}



		ArrayAccess::ArrayAccess(Node *obj, Node *look):Variable(NULL,NULL) {
			assert(dynamic_cast<Value*>(obj));
			Obj = dynamic_cast<Value*>(obj);
			assert(dynamic_cast<Value*>(look));
			Lookup = dynamic_cast<Value*>(look);
		}

		ilang::Variable * ArrayAccess::Get(Context &ctx) {
			// TODO: ??
			// shouldn't be getting the variables direcrly any more
			assert(0);
		}

		ValuePass ArrayAccess::GetValue(Context &ctx) {
			errorTrace("Getting element using []: "<<GetFirstName());

			ValuePass obj_val = Obj->GetValue(ctx);
			ValuePass key = Lookup->GetValue(ctx);
			ValuePass ret;
			try {
				ret = obj_val->get(ctx, key);
			} catch(InvalidOperation e) {
				if(obj_val->type() == typeid(Hashable*)) {
					error(0, "Object " << obj_val << " does not have member " << key->cast<string>());
				} else  {
					error(0, "Attempted to access member on non object type " << obj_val);
				}
			}
			return ret;
		}

		void ArrayAccess::Set(Context &ctx, ValuePass var, bool force) {
			assert(!force);
			errorTrace("Setting element using []: " << GetFirstName());
			ValuePass obj_val = Obj->GetValue(ctx);
			ValuePass key = Lookup->GetValue(ctx);
			try {
				obj_val->set(ctx, key, var);
			} catch(InvalidOperation e) {
				error(0, "Attempted to set " << key->cast<string>() << " on non object type");
			}
		}

		std::string ArrayAccess::GetFirstName() {
			return "Array access name";
		}

		IdentifierSet ArrayAccess::UndefinedElements() {
			return unionSets(Obj->UndefinedElements(), Lookup->UndefinedElements());
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

			ValuePass func = calling->GetValue(ctx);

			ilang::Arguments args;
			for(Node *n : *params) {
				// so nasty
				// TODO: support for kwargs
				assert(dynamic_cast<parserNode::Value*>(n));
				args.push(dynamic_cast<parserNode::Value*>(n)->GetValue(ctx));
			}

			ValuePass ret = func->call(ctx, args);
			return ret;
		}

		IdentifierSet Call::UndefinedElements() {
			IdentifierSet ret;
			if(calling) {
				ret = calling->UndefinedElements();
			}
			for(auto it : *params) {
				ret = unionSets(ret, it->UndefinedElements());
			}
			return ret;
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
			return ValuePass();
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
		}
		ValuePass AssignExpr::GetValue (Context &ctx) {
			errorTrace("Assignment expression with gettin value");
			ValuePass v = eval->GetValue(ctx);
			target->Set(ctx, v);
			return v;
		}

		IdentifierSet AssignExpr::UndefinedElements() {
			return unionSets(eval->UndefinedElements(), target->UndefinedElements());
		}
		void AssignExpr::PreRegister(Context &ctx) {
			target->PreRegister(ctx);
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
			errorTrace("Math equation");
			ValuePass left = this->left->GetValue(ctx);
			if(Act == uMinus) {
				auto v = valueMaker(-1);
				return left * v;
			}
			ValuePass right = this->right->GetValue(ctx);
			switch(Act) {
			case add:
				return left + right;
			case subtract:
				return left - right;
			case multiply:
				return left * right;
			case divide:
				return left / right;
			}
			assert(0); // wtf??
		}

		IdentifierSet MathEquation::UndefinedElements() {
			if(right)
				return unionSets(left->UndefinedElements(), right->UndefinedElements());
			return left->UndefinedElements();
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
				break;
			default:
				left->Run(ctx);
				right->Run(ctx);
				break;
			}
		}
		ValuePass LogicExpression::GetValue(Context &ctx) {
			errorTrace("Logic Expression");
			ValuePass left = this->left->GetValue(ctx);
			if(Act == Not)
				return valueMaker(! left->isTrue() );
			// short cut the execuition
			if(Act == And && !left->isTrue())
				return valueMaker(false);
			if(Act == Or && left->isTrue())
				return valueMaker(true);
			ValuePass right = this->right->GetValue(ctx);
			switch(Act) {
			case And:
				return valueMaker(left->isTrue() && right->isTrue());
			case Or:
				return valueMaker(left->isTrue() || right->isTrue());
			case Equal:
				return valueMaker(left == right);
			case Not_Equal:
				return valueMaker(left != right);
			case Less_Equal:
				return valueMaker(left <= right);
			case Greater_Equal:
				return valueMaker(left >= right);
			case Greater:
				return valueMaker(left > right);
			case Less:
				return valueMaker(left < right);
			}
			assert(0);
			//return valueMaker(false);
		}

		IdentifierSet LogicExpression::UndefinedElements() {
			if(right)
				return unionSets(left->UndefinedElements(), right->UndefinedElements());
			return left->UndefinedElements();
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
			// TODO: have some way to tell apart a += 1 and a = a + 1
			ValuePass val = m_value->GetValue(ctx);
			ValuePass setTo = m_target->GetValue(ctx);
			ValuePass ret;
			if(setTo->type() == typeid(std::string)) {
				error(Act == add, "Can only add to a string");
			}
			switch(Act) {
			case add:
				ret = setTo + val;
				break;
			case subtract:
				ret = setTo - val;
				break;
			case multiply:
				ret = setTo * val;
				break;
			case divide:
				ret = setTo / val;
				break;
			}

			m_target->Set(ctx, ret);
			return ret;

		}

		IdentifierSet SingleExpression::UndefinedElements() {
			return unionSets(m_value->UndefinedElements(), m_target->UndefinedElements());
		}

		void SingleExpression::Print(Printer *p) {
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


		Object::Object(parserNode::Function *func) : function(func) {
			// object created later so we are just going to store the informationa atm
		}

		void Object::Run(Context &ctx) {
			Context obj_ctx(ctx);
			Scope obj_scope(obj_ctx);
			function->Run(obj_ctx);
		}
		ValuePass Object::GetValue(Context &ctx) {
			// will create a new object and return that as when the object is evualiated we do not want to be returing the same old thing
			errorTrace("Creating object");
			debug(-6, "Object getting value");
			Context obj_ctx(ctx);
			Scope obj_scope(obj_ctx);

			function->Constructor(obj_ctx);
			auto obj = make_handle<ilang::Object>(obj_scope, ctx);
			return valueMaker(obj);
		}

		void Object::Print(Printer *p) {
			p->p() << "object ";
			function->Print(p);
		}

		IdentifierSet Object::UndefinedElements() {
			auto ret = function->UndefinedElements();
			ret.erase("this");
			return ret;
		}

		Class::Class(std::list<Node*> *p, parserNode::Function *func) : function(func), parents(p) {
			assert(p);
			assert(func);
			for(auto it : *parents) {
				assert(dynamic_cast<parserNode::Value*>(it));
			}
		}
		void Class::Run(Context &ctx) {
			function->Run(ctx);
		}
		ValuePass Class::GetValue(Context &ctx) {
			errorTrace("Creating class");
			Context cls_ctx(ctx);
			Scope cls_scope(cls_ctx);

			function->Constructor(cls_ctx);
			auto obj = make_handle<ilang::Class>(parents, cls_scope, ctx);
			return valueMaker(obj);
		}


		IdentifierSet Class::UndefinedElements() {
			IdentifierSet ret = function->UndefinedElements();
			for(auto it : *parents) {
				IdentifierSet other = it->UndefinedElements();
				ret.insert(other.begin(), other.end());
			}
			return ret;
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
			function->Print(p);
		}

		Array::Array (std::list<Node*> *e, std::list<Node*> *m) : elements(e), modifiers(m) {
			assert(e);
			if(!modifiers) modifiers = new std::list<Node*>;
			for(auto it : *modifiers) {
				assert(dynamic_cast<parserNode::Value*>(it));
			}
			for(auto it : *elements) {
				assert(dynamic_cast<parserNode::Value*>(it));
			}
		}

		void Array::Run(Context &ctx) {
			errorTrace("Creating Array w/o getting value");
			for(auto it=elements->begin(); it!= elements->end(); it++) {
				(*it)->Run(ctx);
			}
		}

		ValuePass Array::GetValue(Context &ctx) {
			errorTrace("Creating Array");

			auto arr = make_handle<ilang::Array>(modifiers, elements, ctx);
			return valueMaker(arr);
		}

		IdentifierSet Array::UndefinedElements() {
			IdentifierSet ret;
			for(auto it : *elements) {
				auto o = it->UndefinedElements();
				ret.insert(o.begin(), o.end());
			}
			return ret;
		}

		void Array::Print(Printer *p) {
			p->p() << "[";
			if(modifiers && !modifiers->empty()) {
				bool first=true;
				for(auto it : *modifiers) {
					if(!first) p->p() << " ";
					it->Print(p);
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
			bool first=true;
			for(Node *it : *params) {
				if(!first) { p->p() << ", "; }
				first = false;
				it->Print(p);
			}
			p->p() << ")";
		}

		ImportCall::ImportCall(list<Node*> *args) : Call(NULL, args) {
			error(args->size() == 1, "import() expects 1 argument");
			error(dynamic_cast<parserNode::Value*>(args->front()), "import() expects a Value");
		}

		ValuePass ImportCall::GetValue(Context &ctx) {
			ValuePass val = dynamic_cast<Value*>(params->front())->GetValue(ctx);
			std::string name = val->cast<std::string>();

			auto obj = ImportGetByName(name);
			if(!obj) return valueMaker(false);

			return valueMaker(obj);

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
				assert(dynamic_cast<parserNode::Value*>(it));
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
				});
			thread.Trigger(NULL);

			return ValuePass();
		}

		void ThreadGoCall::Print(Printer *p) {
			p->p() << "go(";
			for(auto it : *params) {
				it->Print(p);
			}
			p->p() << ")";
		}

		TupleRHS::TupleRHS(std::list<Node*> *vals) : values(vals) {
			assert(values);
			for(Node *n : *values) {
				assert(dynamic_cast<Value*>(n));
			}
		}

		void TupleRHS::Run(Context &ctx) {
			for(Node *n : *values) {
				n->Run(ctx);
			}
		}

		ValuePass TupleRHS::GetValue(Context &ctx) {
			auto ret = make_handle<ilang::Tuple>();
			for(Node *n : *values) {
				Value *v = dynamic_cast<Value*>(n);
				// if(dynamic_cast<ValueNamed*>(v)) {
				// 	assert(0);
				// }
				ret->push(v->GetValue(ctx));
			}
			return valueMaker(ret);
		}

		IdentifierSet TupleRHS::UndefinedElements() {
			IdentifierSet ret;
			for(Node *n : *values) {
				ret = unionSets(ret, n->UndefinedElements());
			}
			return ret;
		}

		void TupleRHS::Print(Printer *p) {
			p->p() << "(";
			bool first = true;
			for(Node *n : *values) {
				if(!first) p->p() << ", ";
				n->Print(p);
				first = false;
			}
			p->p() << ")";
		}

		TupleLHS::TupleLHS(std::list<Node*> *val) : values(val), Variable(0, NULL) /* gg */ {
			assert(values);
			for(Node *n : *values) {
				assert(dynamic_cast<Variable*>(n));
			}
		}

		ValuePass TupleLHS::GetValue(Context &ctx) {
			// TODO: determine how to make the names be present,
			// or possible all for passing by both of the items, and just
			// not matter, but then there are two value passes on each item
			// I guess that I could create a pair where one is the position
			// and the other is the position....
			assert(0);
			return ValuePass();
		}

		ilang::Variable * TupleLHS::Get(Context &ctx) {
			assert(0);
			// there is not really a variable that is defined for the tuple
		}

		void TupleLHS::Set(Context &ctx, ValuePass var, bool force) {
			Handle<Tuple> tup = var->cast<Tuple*>();
			auto it = tup->begin();
			for(Node *n : *values) {
				Variable *v = dynamic_cast<Variable*>(n);
				if(tup->has(ctx, v->GetName())) {
					ValuePass kv = tup->get(ctx, v->GetName());
					v->Set(ctx, kv);
				} else {
					v->Set(ctx, it->second);
					it++;
				}
			}
		}

		IdentifierSet TupleLHS::UndefinedElements() {
			IdentifierSet ret;
			for(Node *n : *values) {
				ret = unionSets(ret, n->UndefinedElements());
			}
			return ret;
		}

		void TupleLHS::Print(Printer *p) {
			p->p() << "(";
			bool first = true;
			for(Node *n : *values) {
				if(!first) p->p() << ", ";
				n->Print(p);
				first = false;
			}
			p->p() << ")";
		}


	} // namespace parserTree
} // namespace ilang
