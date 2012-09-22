#include "parserTree.h"
#include "parser.h"
#include "scope.h"
#include "object.h"
#include "function.h"
#include "debug.h"
#include "error.h"

// for string escape
#include <boost/algorithm/string.hpp>

#include <iostream>
using namespace std;

namespace ilang {
  namespace parserNode {
    Head::Head(list<Node*> *declars, ImportScopeFile *import): Import(import), Declars(declars) {
      debug(4, "head " << declars->size() );
      debug(4, "running ++++++++++++++++++++++++++++++++++++++++++++" );
      scope=NULL;
    }
    void Head::Link () {
      if(scope) return;
      scope = new FileScope;
      Import->resolve(scope);

      for(list<Node*>::iterator it = Declars->begin(); it !=  Declars->end(); it++) {
	debug(5, "calling run" )
	  (*it)->Run(scope);
	//scope->Debug();
      }
      
      //for(
            
      //vector<ilang::Value*> v;
      //boost::any_cast<ilang::Function_ptr>(scope->lookup("main")->Get()->Get())(scope, v, NULL);
      
    }
    
    void Head::Run() {
      vector<ValuePass> v;
      ValuePass ret;
      boost::any_cast<ilang::Function_ptr>(scope->lookup("main")->Get()->Get())(scope, v, &ret);
    }

    // this does not need to have anything
    void Constant::Run(Scope *scope) {}

    StringConst::StringConst(char *str) :string(str){
      delete[] str;
      using boost::replace_all;
      replace_all(string, "\\n", "\n");
      replace_all(string, "\\t", "\t");
      replace_all(string, "\\\\", "\\");
      replace_all(string, "\\\"", "\"");
      replace_all(string, "\\'", "'"); 
      
    }
    ValuePass StringConst::GetValue (Scope *scope) {
      debug(5, "string get value" );
      return ValuePass(new ilang::Value(std::string(string)));
    }

    IntConst::IntConst(long n) : num(n) {}
    ValuePass IntConst::GetValue (Scope *scope) {
      return ValuePass(new ilang::Value(num));
    }

    FloatConst::FloatConst(double d) : num(d) {}
    ValuePass FloatConst::GetValue (Scope *scope) {
      return ValuePass(new ilang::Value(num));
    }
    

    IfStmt::IfStmt (Node *test_, Node* True_, Node* False_): True(True_), False(False_) {
      Value *t = dynamic_cast<Value*>(test_);
      error(t, "If statement does not contain a test");
      //assert(t);
      test=t;
    }
    void IfStmt::Run(Scope *scope) {
      ValuePass search = test->GetValue(scope);
      assert(search);
      if(search->isTrue()) {
	if(True) True->Run(scope);
      }else{
	if(False) False->Run(scope);
      }
    }

    ForStmt::ForStmt (Node *pre_, Node *test_, Node *each_, Node *exe_) :pre(pre_), each(each_), exe(exe_) {
      Value *t = dynamic_cast<Value*>(test_);
      error(t, "Test in for statment is not a test");
      //assert(t);
      test = t;
    }
    void ForStmt::Run(Scope *scope) {
      for( pre->Run(scope) ; test->GetValue(scope)->isTrue() ; each->Run(scope) )
	exe->Run(scope);
    }

    WhileStmt::WhileStmt (Node *test_, Node *exe_): exe(exe_) {
      Value *t=dynamic_cast<Value*>(test_);
      error(t, "Test in while Statement is not a test");
      //assert(t);
      test=t;
    }
    void WhileStmt::Run(Scope *scope) {
      // note to self: must recall GetValue each time as it is what is actually doing the updating
      while(test->GetValue(scope)->isTrue()) {
	exe->Run(scope);
	//test->GetValue(scope)->Print();
      }
    }

    ReturnStmt::ReturnStmt (Node *r) {
      if(r) {
	ret = dynamic_cast<Value*>(r);
	error(ret, "Return is not given a value");
	///assert(ret);
      }else
	ret = NULL;
    }
    void ReturnStmt::Run(Scope *scope) {
      ValuePass v;
      if(ret) v = ret->GetValue(scope);
      scope->ParentReturn(&v); // this should avoid copying any smart pointers and run much faster
    }

    Function::Function (list<Node*> *p, list<Node*> *b):body(b), params(p) {
      debug(5, "\t\t\tfunction constructed" );
    }
    void Function::Run(Scope *scope) {
      vector<ValuePass> p;
      Call(scope, NULL, p);
    }
    ValuePass Function::GetValue(Scope *this_scope) {
      // this need to track the scope at this point so that it could be use later in the funciton
      debug(5, "Function get value");
      errorTrace("Getting value of a function");
      Function *self = this;
      //ilang::Function_ptr f = boost::bind(&Function::Call, this, scope, _1, _2, _3);
      ilang::Function_ptr f = [self, this_scope](Scope *scope, std::vector<ValuePass> & args, ValuePass *ret) {
	self->Call((Scope*)this_scope, scope, args, ret);
      };
      return ValuePass(new ilang::Value(f));
      //return ValuePass(new ilang::Value(this));
    }
    /*void Function::Call(vector<ilang::Value*> params) {
      // this needs to be removed
      Call(NULL, NULL, params);
      }*/
    void Function::Call(Scope *_scope_made, Scope *_scope_self, vector<ValuePass> &p, ValuePass *_ret) {
      bool returned=false;
      errorTrace("Function called");
      auto returnHandle = [&returned, _ret] (ValuePass *ret) {
	returned=true;
	if(_ret) *_ret = *ret;
	debug(6, "return hook set");
      };
      //FunctionScope<decltype(returnHandle)> scope(_scope_made, _scope_call, returnHandle);
      FunctionScope<decltype(returnHandle)> scope(_scope_made, _scope_self, returnHandle);
      
      debug(5,"function called");
      if(params) { // the parser set params to NULL when there are non
	list<Node*>::iterator it = params->begin();
	for(ValuePass v : p) {
	  if(it==params->end()) break;
	  error(dynamic_cast<parserNode::Variable*>(*it), "Argument to function is not variable name");
	  assert(v);
	  dynamic_cast<parserNode::Variable*>(*it)->Set(&scope, ValuePass(v));
	  it++;
	}
      }
      if(body) { // body can be null if there is nothing
	for(Node *n : *body) {
	  if(returned) return;
	  assert(n);
	  debug(6,"function run");
	  n->Run(&scope);
	}
      }
    }

    Variable::Variable (list<string> *n, list<string> *mod):
      name(n), modifiers(mod) {
      if(!name) name = new list<string>;
      if(!modifiers) modifiers = new list<string>;
      //cout << "\t\t\t" << name << "\n";
    }
    void Variable::Run (Scope *scope) {
      // iirc this does not happen as the syntax does not allow for variables to be created without a default value
      error(0, "Variables must always have some value set to them");
      debug(4,"\t\t\tSetting variable: " << name->front());
      scope->forceNew(GetFirstName(), *modifiers);
    }
    void Variable::Set (Scope *scope, ValuePass var, bool force) {
      errorTrace("Setting value of variable: " << GetFirstName());
      scope->Debug();
      ilang::Variable *v;
      if(force || !modifiers->empty()) {
	v = scope->forceNew(GetFirstName(), *modifiers);
      } else {
	//v = scope->lookup(name->front());
	v = Get(scope);
      }
      assert(v);
      v->Set(var);
      debug(4,"Set: " << GetFirstName() << " " << var << " " << v->Get());
      scope->Debug();
    }
    ilang::Variable * Variable::Get(Scope *scope) {
      // this should not happen
      assert(0);
      scope->Debug();
      ilang::Variable *v;
      v = scope->lookup(name->front());
      assert(v);
      // TODO: this needs to get taken out
      if(name->size() > 1) {
	for(auto it=++(name->begin());it != name->end(); it++) {
	  ilang::Object *obj = boost::any_cast<ilang::Object*>(v->Get()->Get());
	  v = obj->operator[](*it);
	  assert(v);
	}
      }
      debug(4,"Get: " << name->front() << " " << v->Get());
      return v;
    }
    
    ValuePass Variable::GetValue(Scope *scope) {
      errorTrace("Getting value of variable: "<<GetFirstName());
      ilang::Variable *v = Get(scope);
      error(v, "Variable " << GetFirstName() << " Not found"); // maybe an assert, not an error?
      //assert(v);
      auto p = v->Get();
      // TODO: fix this to make it correct, if there is no value set to a variable then it is an error atm
      error(p, "Variable " << GetFirstName() << " Not found");
      return p;
    }
    std::string Variable::GetFirstName() {
      return name->front();
    }

    /*
      // this should not be needed, the only time that something different is needed is when it is being called of some object
    ValuePass Variable::CallFun(Scope *scope, std::vector<ValuePass> &par) {
      ilang::Variable * func = Get(scope);
      assert(func);
      boost::any & an = func->Get()->Get();
      assert(an.type() == typeid(ilang::Function_ptr));
       
      ValuePass ret = ValuePass(new ilang::Value);
    
      boost::any_cast<ilang::Function_ptr>(an)(NULL, par, &ret);

      return ret;
    }
    */

    bool Variable_compare::operator()(Variable *a, Variable *b) {
      auto a_it = a->name->begin();
      auto b_it = b->name->begin();
      while(a_it != a->name->end() && b_it != b->name->end()) {
	if(*a_it == *b_it) continue;
	return *a_it > *b_it;
      }
      if(a_it == a->name->end()) {
	if(b_it != b->name->end()) {
	  return false;
	}
      }else{ // a_it != a->name->end()
	if(b_it == b->name->end()) {
	  return true;
	}
      }
      return true; // just something if they are "equal"
    }

    

    FieldAccess::FieldAccess (Node *obj, std::string id) :Variable(NULL, NULL), identifier(id), Obj(NULL) {
      if(obj) {
	assert(dynamic_cast<Value*>(obj));
	Obj = dynamic_cast<Value*>(obj);
      }
    }
    ilang::Variable * FieldAccess::Get(Scope *scope) {
      errorTrace("FieldAccess on " << GetFirstName());
      ilang::Variable *v;
      if(Obj) {
	_errors.stream() << "\n\t\tLooking where there is an object";
	// this might cause problems if what is returned is not an object
	ValuePass obj_val = Obj->GetValue(scope);
	assert(obj_val);
	boost::any & a = obj_val->Get();
	error(a.type() == typeid(ilang::Object*), "trying to find fields in value that does not contain fields, essently not an object\n\t\twas looking for: " << GetFirstName());
	ilang::Object *obj = boost::any_cast<ilang::Object*>(a);
	v = obj->operator[](identifier);
      }else{
	v = scope->lookup(identifier);
      }
      error(v, "Did not find " << GetFirstName());
      return v;
    }
    
    std::string FieldAccess::GetFirstName() {
      return identifier;
    }

    ValuePass FieldAccess::CallFun(Scope *scope, vector<ValuePass> &par) {
      debug(4, "field access callfun");
      errorTrace("Calling function of object/class");
      ilang::Variable * func = Get(scope);
      error(func, "Unable to find function " << GetFirstName());
      boost::any & an = func->Get()->Get();
      error(an.type() == typeid(ilang::Function_ptr), GetFirstName()<<" is not a function");

      ValuePass ret = ValuePass(new ilang::Value);
      if(Obj) {
	// this bugged up at one point, I assume that it had to deal with the smart pointer deleting what it was pointing at before the ->Get() could be called, there are going to be other errors like this I assume
	ValuePass obj_val = Obj->GetValue(scope);
	assert(obj_val); 
	boost::any & o = obj_val->Get();
	assert(o.type() == typeid(ilang::Object*));
	ObjectScope obj_scope(boost::any_cast<ilang::Object*>(o));
	
	boost::any_cast<ilang::Function_ptr>(an)(&obj_scope, par, &ret);
      }else{
	boost::any_cast<ilang::Function_ptr>(an)(NULL, par, &ret);
      }
      return ret;
    }

    ArrayAccess::ArrayAccess(Node *obj, Node *look):Variable(NULL,NULL) {
      assert(dynamic_cast<Value*>(obj));
      Obj = dynamic_cast<Value*>(obj);
      assert(dynamic_cast<Value*>(look));
      Lookup = dynamic_cast<Value*>(look);
    }

    ilang::Variable * ArrayAccess::Get(Scope *scope) {
      errorTrace("Getting element using []: "<<GetFirstName());
      boost::any & a = Obj->GetValue(scope)->Get();
      error(a.type() == typeid(ilang::Object*), "Not of an object or array type");
      ilang::Object *obj = boost::any_cast<ilang::Object*>(a);
      ValuePass val = Lookup->GetValue(scope);
      return obj->operator[](val);

      /*if(val.type() == typeid(long)) {
	assert(dynamic_cast<ilang::Array*>(obj));
	return dynamic_cast<ilang::Array*>(obj)->operator[](boost::any_cast<long>(val));
	
      }else if(val.type() == typeid(std::string)) {
	return obj->operator[](boost::any_cast<std::string>(val));
	}*/
    }


    Call::Call (Value *call, list<Node*> *args):
      calling(call), params(args) {
      debug(4,"\t\t\tCalling function");
    }
    void Call::Run(Scope *scope) {
      debug(4,"call run");
      GetValue(scope);
    }
    ValuePass Call::GetValue (Scope *scope) {
      //ilang::Variable * func = calling->Get(scope);
      //assert(func);
      errorTrace("Calling function");
      std::vector<ValuePass> par;
      for(Node * n : *params) {
	assert(dynamic_cast<parserNode::Value*>(n));
	par.push_back(dynamic_cast<parserNode::Value*>(n)->GetValue(scope));
      }
      return calling->CallFun(scope, par);
      /*
      ValuePass ret = ValuePass(new ilang::Value);
      boost::any & an = calling->GetValue(scope)->Get();//func->Get()->Get();

      assert(an.type() == typeid(ilang::Function_ptr));
      boost::any_cast<ilang::Function_ptr>(an)(NULL, par, &ret);
      // scope->fileScope();

      //assert(an.type() == typeid(Function*));
      //boost::any_cast<Function*>(an) ->Call(scope->fileScope(), par, &ret);
      
      return ret;
      */
    }
    PrintCall::PrintCall(list<Node*> *args):
      Call(NULL, args) {}
    ValuePass PrintCall::GetValue (Scope *scope) {
      errorTrace("Print call");
      debug(5,"made into the print Call");
      //for(list<Node*>::iterator it = params->begin(), end = params->end(); it != end; it++) {
      for(Node *it : *params) {
	assert(dynamic_cast<parserNode::Value*>(it));
	dynamic_cast<parserNode::Value*>((it))->GetValue(scope)->Print();
      }
      debug(5,"made it out of print");
    }

    ValuePass Value::CallFun(Scope *scope, std::vector<ValuePass> &par) {
      errorTrace("Generic version of calling function on value");
      boost::any & a = GetValue(scope)->Get();
      assert(a.type() == typeid(ilang::Function_ptr));
      
      ValuePass ret = ValuePass(new ilang::Value);
      
      boost::any_cast<ilang::Function_ptr>(a)(NULL, par, &ret);

      return ret;
    }

    

    AssignExpr::AssignExpr (Variable *t, Value *v):target(t), eval(v) {
      assert(eval);
      assert(target);
    }
    void AssignExpr::Run (Scope *scope) {
      errorTrace("Assignment expression");
      ValuePass v = eval->GetValue(scope);

      target->Set(scope, v);
      scope->Debug();
      //return RunReturn(new ilang::Value);
      //ilang::Variable *var = target->Get();
    }
    ValuePass AssignExpr::GetValue (Scope *scope) {
      errorTrace("Assignment expression with gettin value");
      ValuePass v = eval->GetValue(scope);
      target->Set(scope, v);
      return v;
    }
    MathEquation::MathEquation(Value *l, Value *r, action a) : left(l), right(r), Act(a) {
      assert(left);
      assert(right);
    }
    void MathEquation::Run(Scope *scope) {
      errorTrace("Running math equation w/o getting value");
      left->Run(scope);
      right->Run(scope);
      // we do not need the value so we should not request the vale from the nodes
    }
    ValuePass MathEquation::GetValue(Scope *scope) {
      errorTrace("Math equation");
      ValuePass left = this->left->GetValue(scope);
      ValuePass right = this->right->GetValue(scope);
      /* when trying for speed boost
	auto left_type = left->Get().type();
	auto right_type = right->Get().type();
       */
      switch(Act) {
      case add:
	if(left->Get().type() == typeid(std::string) && right->Get().type() == typeid(std::string)) {
	  return ValuePass(new ilang::Value(std::string(boost::any_cast<std::string>(left->Get()) + boost::any_cast<std::string>(right->Get()))));
	}else if(left->Get().type() == typeid(double)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) + boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long)) 
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) + boost::any_cast<long>(right->Get())));
	}else if(left->Get().type() == typeid(long)) {
	  if(right->Get().type() == typeid(double)) 
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) + boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long))
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) + boost::any_cast<long>(right->Get())));
	}
	break;
      case subtract:
	if(left->Get().type() == typeid(double)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) - boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long))
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) - boost::any_cast<long>(right->Get())));
	}else if(left->Get().type() == typeid(long)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) - boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long))
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) - boost::any_cast<long>(right->Get())));
	}
	break;
      case multiply:
	if(left->Get().type() == typeid(double)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) * boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long))
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) * boost::any_cast<long>(right->Get())));
	}else if(left->Get().type() == typeid(long)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) * boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long))
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) * boost::any_cast<long>(right->Get())));
	}
	break;
      case devide:
	if(left->Get().type() == typeid(double)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) / boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long))
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) / boost::any_cast<long>(right->Get())));
	}else if(left->Get().type() == typeid(long)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) / boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long))
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) / boost::any_cast<long>(right->Get())));
	}
	break;
      }
    }

    LogicExpression::LogicExpression(Value *l, Value *r, action a): left(l), right(r), Act(a) {
      assert(left);
      assert(right);
    }
    void LogicExpression::Run (Scope *scope) {
      errorTrace("Logic Expression w/o getting value");
      switch(Act) {
      case And:
	if(left->GetValue(scope)->isTrue())
	  right->Run(scope);
	break;
      case Or:
	if(!left->GetValue(scope)->isTrue())
	  right->Run(scope);
	break;
      default:
	left->Run(scope);
	right->Run(scope);
      }
    }
    ValuePass LogicExpression::GetValue(Scope *scope) {
      errorTrace("Logic Expression");
      ValuePass left = this->left->GetValue(scope);
      ValuePass right = this->right->GetValue(scope);
      switch(Act) {
      case And:
	if(left->isTrue())
	  if(right->isTrue())
	    return ValuePass(new ilang::Value(true));
	return ValuePass(new ilang::Value(false));
	break;
      case Or:
	if(left->isTrue() || right->isTrue()) return ValuePass(new ilang::Value(true));
	return ValuePass(new ilang::Value(false));
      case Equal:
	if(left->Get().type() == typeid(std::string) && right->Get().type() == typeid(std::string))
	  return ValuePass(new ilang::Value(boost::any_cast<std::string>(left->Get()) == boost::any_cast<std::string>(right->Get())));
	if(left->Get().type() == typeid(double)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) == boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long)) 
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) == boost::any_cast<long>(right->Get())));
	  else
	    return ValuePass(new ilang::Value(false));
	}else if(left->Get().type() == typeid(long)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) == boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long)) 
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) == boost::any_cast<long>(right->Get())));
	  else
	    return ValuePass(new ilang::Value(false));
	}
	break;
      case Not_Equal:
	if(left->Get().type() == typeid(std::string) && right->Get().type() == typeid(std::string))
	  return ValuePass(new ilang::Value(boost::any_cast<std::string>(left->Get()) != boost::any_cast<std::string>(right->Get())));
	if(left->Get().type() == typeid(double)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) != boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long)) 
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) != boost::any_cast<long>(right->Get())));
	  else
	    return ValuePass(new ilang::Value(false));
	}else if(left->Get().type() == typeid(long)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) != boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long)) 
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) != boost::any_cast<long>(right->Get())));
	  else
	    return ValuePass(new ilang::Value(false));
	}
	break;
      case Less_Equal:
	if(left->Get().type() == typeid(double)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) <= boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long)) 
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) <= boost::any_cast<long>(right->Get())));
	  else
	    return ValuePass(new ilang::Value(false));
	}else if(left->Get().type() == typeid(long)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) <= boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long)) 
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) <= boost::any_cast<long>(right->Get())));
	  else
	    return ValuePass(new ilang::Value(false));
	}
	break;
      case Greater_Equal:
	if(left->Get().type() == typeid(double)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) >= boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long)) 
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) >= boost::any_cast<long>(right->Get())));
	  else
	    return ValuePass(new ilang::Value(false));
	}else if(left->Get().type() == typeid(long)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) >= boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long)) 
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) >= boost::any_cast<long>(right->Get())));
	  else
	    return ValuePass(new ilang::Value(false));
	}
	break;
      case Less:
	if(left->Get().type() == typeid(double)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) < boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long)) 
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) < boost::any_cast<long>(right->Get())));
	  else
	    return ValuePass(new ilang::Value(false));
	}else if(left->Get().type() == typeid(long)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) < boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long)) 
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) < boost::any_cast<long>(right->Get())));
	  else
	    return ValuePass(new ilang::Value(false));
	}
	break;
      case Greater:
	if(left->Get().type() == typeid(double)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) > boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long)) 
	    return ValuePass(new ilang::Value(boost::any_cast<double>(left->Get()) > boost::any_cast<long>(right->Get())));
	  else
	    return ValuePass(new ilang::Value(false));
	}else if(left->Get().type() == typeid(long)) {
	  if(right->Get().type() == typeid(double))
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) > boost::any_cast<double>(right->Get())));
	  else if(right->Get().type() == typeid(long)) 
	    return ValuePass(new ilang::Value(boost::any_cast<long>(left->Get()) > boost::any_cast<long>(right->Get())));
	  else
	    return ValuePass(new ilang::Value(false));
	}
	break;
      }
    }
    
    Object::Object (std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj) : objects(obj) 
    {
      // object created later so we are just going to store the informationa atm
    }
    void Object::Run(Scope *scope) {
      // should not be doing anything
    }
    ValuePass Object::GetValue(Scope *scope) {
      // will create a new object and return that as when the object is evualiated we do not want to be returing the same old thing
      errorTrace("Creating object");
      debug(4, "Object getting value")
      ilang::Object *obj = new ilang::Object(objects, scope);
      ilang::Value *val = new ilang::Value(obj);
      return ValuePass(val);
    }

    Class::Class(std::list<Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj): parents(p), objects(obj) 
    {
      assert(p);
      assert(obj);
    }
    void Class::Run(Scope *scope) {
      // should not be doing anything
    }
    ValuePass Class::GetValue(Scope *scope) {
      errorTrace("Creating class");
      ilang::Class *c = new ilang::Class(parents, objects, scope);
      ilang::Value *val = new ilang::Value(c);
      return ValuePass(val);
    }

    Array::Array (std::list<Node*> *e, std::list<string> *m) : elements(e), modifiers(m) {
      assert(e);
      if(!modifiers) modifiers = new std::list<string>;
    }

    void Array::Run(Scope *scope) {
      errorTrace("Creating Array w/o getting value");
      for(auto it=elements->begin(); it!= elements->end(); it++) {
	(*it)->Run(scope);
      }
    }

    ValuePass Array::GetValue(Scope *scope) {
      errorTrace("Creating Array");
      /*
      vector<ValuePass> ret;
      ret.reserve(elements->size());
      for(auto it=elements->begin(); it!= elements->end(); it++) {
	Value *v = dynamic_cast<parserNode::Value*>(*it);
	assert(v);
	ret.push_back(v->GetValue(scope));
      }
      return ValuePass(new ilang::Value(ret));
      */
      ilang::Array *arr = new ilang::Array(elements, modifiers, scope);
      ilang::Value *val = new ilang::Value(arr);
      return ValuePass(val);
    }

    NewCall::NewCall(std::list<Node*> *args): Call(NULL, args) {
      // might change the grammer to reflect that this needs one element
      // might in the future allow for arguments to be passed to classes when they are getting constructed through additional arguments
      
      error(args->size() == 1, "New only takes one argument");
      error(dynamic_cast<Value*>(args->front()), "First argument to New needs to be a value");
    }
    ValuePass NewCall::GetValue(Scope *scope) {
      errorTrace("New Call");
      ValuePass a = dynamic_cast<Value*>(params->front())->GetValue(scope);
      error(a->Get().type() == typeid(ilang::Class*), "Can not create something with new that is not a class");
      //ilang::Value *val = new ilang::Value( boost::any_cast<ilang::Class*>(a)->NewClass() ); // returns an Object*
      ilang::Value *val = new ilang::Value( new ilang::Object(a) );
      // TODO: make this call an init function that is defined in the class
      // does this need to call the init function, as default values can be set and no arguments can be passed when the new function is called
      return ValuePass(val);
    }


  } // namespace parserTree
} // namespace ilang
