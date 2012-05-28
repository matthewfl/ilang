#include "parserTree.h"
#include "parser.h"
#include "scope.h"
#include "debug.h"

#include <iostream>
using namespace std;

namespace ilang {
  namespace parserNode {
    Head::Head(list<Node*> *declars): Declars(declars) {
      debug(4, "head " << declars->size() );
      debug(4, "running ++++++++++++++++++++++++++++++++++++++++++++" );
      Run();
    }
    void Head::Run () {
      FileScope scope;
      for(list<Node*>::iterator it = Declars->begin(); it !=  Declars->end(); it++) {
	debug(5, "calling run" )
	  (*it)->Run(&scope);
	scope.Debug();
      }
      list<ilang::Value*> v;
      boost::any_cast<Function*>(scope.lookup("main")->Get()->Get())->Call(&scope, v);
      //return RunReturn(new ilang::Value);
    }

    // this does not need to have anything
    void Constant::Run(Scope *scope) {}

    StringConst::StringConst(char *str) :string(str){}
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
      assert(t);
      test=t;
    }
    void IfStmt::Run(Scope *scope) {
      ilang::Value *search = test->GetValue(scope);
      assert(search);
      if(search->isTrue()) {
	if(True) True->Run(scope);
      }else{
	if(False) False->Run(scope);
      }
    }

    ForStmt::ForStmt () {}
    void ForStmt::Run(Scope *scope) {}

    WhileStmt::WhileStmt (Node *test_, Node *exe_): exe(exe_) {
      Value *t=dynamic_cast<Value*>(test);
      assert(t);
      test=t;
    }
    void WhileStmt::Run(Scope *scope) {
      while(test->GetValue(scope)->isTrue())
	exe->Run(scope);
    }

    ReturnStmt::ReturnStmt (Node *r) {
      if(r) {
	ret = dynamic_cast<Value*>(r);
	assert(ret);
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
      list<ilang::Value*> p;
      Call(scope, p);
    }
    ValuePass Function::GetValue(Scope *scope) {
      debug(5, "Function get value");
      return ValuePass(new ilang::Value(this));
    }
    void Function::Call(list<ilang::Value*> params) {
      // this needs to be removed
      Call(NULL, params);
    }
    void Function::Call(Scope *_scope, list<ilang::Value*> &p, ValuePass *_ret) {
      bool returned=false;
      auto returnHandle = [&returned, _ret] (ValuePass *ret) {
	returned=true;
	if(_ret) *_ret = *ret;
	debug(6, "return hook set");
      };
      FunctionScope<decltype(returnHandle)> scope(_scope, returnHandle);
      debug(5,"function called");
      if(params) { // the parser set params to NULL when there are non
	list<Node*>::iterator it = params->begin();
	for(ilang::Value * v : p) {
	  if(it==params->end()) break;
	  assert(dynamic_cast<parserNode::Variable*>(*it));
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
      //cout << "\t\t\t" << name << "\n";
    }
    void Variable::Run (Scope *scope) {
      debug(4,"\t\t\tSetting variable: " << name->front());
    }
    void Variable::Set (Scope *scope, ValuePass var) {
      scope->Debug();
      ilang::Variable *v;
      if(!modifiers->empty())
	v = scope->forceNew(name->front(), *modifiers);
      else
	v = scope->lookup(name->front());
      assert(v);
      v->Set(var);
      debug(4,"Set: " << name->front() << " " << var << " " << v->Get());
      scope->Debug();
    }
    ilang::Variable * Variable::Get(Scope *scope) {
      scope->Debug();
      ilang::Variable *v;
      v = scope->lookup(name->front());
      assert(v);
      debug(4,"Get: " << name->front() << " " << v->Get());
      return v;
    }
    ValuePass Variable::GetValue(Scope *scope) {
      return Get(scope)->Get();
    }
    bool Variable_compare::operator()(Variable *a, Variable *b) {
      return true;
    }

    Call::Call (Variable *call, list<Node*> *args):
      calling(call), params(args) {
      debug(4,"\t\t\tCalling function");
    }
    void Call::Run(Scope *scope) {
      debug(4,"call run");
      GetValue(scope);
    }
    ValuePass Call::GetValue (Scope *scope) {
      ilang::Variable * func = calling->Get(scope);
      assert(func);
      std::list<ValuePass> par;
      for(Node * n : *params) {
	assert(dynamic_cast<parserNode::Value*>(n));
	par.push_back(dynamic_cast<parserNode::Value*>(n)->GetValue(scope));
      }
      ValuePass ret = ValuePass(new ilang::Value);
      boost::any_cast<Function*>(
				 //scope->lookup("something")
				 func
				 ->Get()->Get()) ->Call(scope->fileScope(), par, &ret);
      
      return ret;
    }
    PrintCall::PrintCall(list<Node*> *args):
      Call(NULL, args) {}
    ValuePass PrintCall::GetValue (Scope *scope) {
      debug(5,"made into the print Call");
      //for(list<Node*>::iterator it = params->begin(), end = params->end(); it != end; it++) {
      for(Node *it : *params) {
	assert(dynamic_cast<parserNode::Value*>(it));
	dynamic_cast<parserNode::Value*>((it))->GetValue(scope)->Print();
      }
      debug(5,"made it out of print");
    }

    AssignExpr::AssignExpr (Variable *t, Value *v):target(t), eval(v) {
      assert(eval);
      assert(target);
    }
    void AssignExpr::Run (Scope *scope) {

      ValuePass v = eval->GetValue(scope);

      target->Set(scope, v);
      scope->Debug();
      //return RunReturn(new ilang::Value);
      //ilang::Variable *var = target->Get();
    }
    ValuePass AssignExpr::GetValue (Scope *scope) {
      ValuePass v = eval->GetValue(scope);
      target->Set(scope, v);
      return v;
    }
    MathEquation::MathEquation(Value *r, Value *l, action a) : left(l), right(r), Act(a) {
      assert(left);
      assert(right);
    }
    void MathEquation::Run(Scope *scope) {
      left->Run(scope);
      right->Run(scope);
      // we do not need the value so we should not request the vale from the nodes
    }
    ValuePass MathEquation::GetValue(Scope *scope) {
      ValuePass left = this->left->GetValue(scope);
      ValuePass right = this->right->GetValue(scope);
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
    
    Object::Object (std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj)// : objects(obj) 
    {
      
    }
    void Object::Run(Scope *scope) {
      // should not be doing anything
    }
    ValuePass Object::GetValue(Scope *scope) {
    }

    Class::Class(std::list<Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj)//: parents(p), objects(obj) 
    {
      
    }
    void Class::Run(Scope *scope) {
      // should not be doing anything
    }
    ValuePass Class::GetValue(Scope *scope) {
      
    }


  } // namespace parserTree
} // namespace ilang
