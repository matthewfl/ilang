#include "tuple.h"
#include "value_types.h"
#include "parserTree.h"
#include "function.h"

using namespace ilang;
using namespace std;

Tuple::Tuple() {
}

Tuple::Tuple(const Tuple &o) {
	m_args = o.m_args;
	m_next_identifier = o.m_next_identifier;
}

Tuple::Tuple(std::vector<ValuePass> pargs) {
	for(auto it : pargs)
		push(it);
}

void Tuple::push(ilang::ValuePass value) {
	m_args.insert(make_pair(Identifier(m_next_identifier++), value));
}

void Tuple::set(Context &ctx, Identifier key, ilang::ValuePass value) {
	m_args.insert(make_pair(key, value));
}

void Tuple::set(Identifier key, ilang::ValuePass value) {
	m_args.insert(make_pair(key, value));
}

ValuePass Tuple::get(Identifier i) {
	auto it = m_args.find(i);
	if(it != m_args.end()) {
		return it->second;
	}
	assert(0);
}

// void Tuple::populate(Context &ctx, Function *func) {
// 	if(!func->native && func->func->params) {
// 		// there is some function with arguments that can be used to determine what to set values to

// 		auto it = func->func->params->begin(); // wtf was I thinking
// 		auto end = func->func->params->end();
// 		for(auto ait : m_args) {
// 			if(it == end) break;
// 			if(ait.first.isInt()) {
// 				dynamic_cast<parserNode::Variable*>(*it)->Set(ctx, ait.second, true);
// 			}
// 			it++;
// 		}
// 	}
// }

ValuePass Tuple::get(Context &ctx, Identifier i) {
	auto it = m_args.find(i);
	if(it != m_args.end()) {
		return it->second;
	}
	if(i == "length") {
		return valueMaker(size());
	}
	assert(0);
}

bool Tuple::has(Context &ctx, Identifier i) {
	return m_args.find(i) != m_args.end();
}

size_t Tuple::size() {
	return m_args.size();
}

Handle<Variable> Tuple::getVariable(Context &ctx, Identifier i) {
	return make_variable(get(ctx, i));
}
