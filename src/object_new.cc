#include "object_new.h"

using namespace ilang;

Class_new::Class_new() {

}

Class_new::Class_new(std::list<ilang::parserNode::Node*> *p, std::map<ilang::parserNode::Variable*, ilang::parserNode::Node*> *obj, ScopePass scope) {
	assert(p && obj && scope);
	for(auto it : *p) {

	}

}
