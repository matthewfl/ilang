#ifndef _ilang_tuple
#define _ilang_tuple

#include "hashable.h"
#include <map>

namespace ilang {
	// create an instance of the kwarg helper that will be passable by reference
	// so that we can pass it to the inject function which takes all values by reference
#define ILANG_KWARG(kw) ([]() -> Tuple_kwarg_helper& { static Tuple_kwarg_helper tkh( #kw ); return tkh; }())
  struct Tuple_kwarg_helper {
    const Identifier id;
    Tuple_kwarg_helper(char *n) : id(n) {}
  };

  class Tuple : public Hashable {
  private:

    std::map<ilang::Identifier, ilang::ValuePass> m_args;
    unsigned long m_next_identifier = 0;

    template<typename T> void unwrap(T t) {
      push(valueMaker(t));
    }
    void unwrap(ilang::ValuePass t) {
      push(t);
    }
    template<typename T> void unwrap (Tuple_kwarg_helper h, T t) {
      set(h.id, valueMaker(t));
    }
    void unwrap(Tuple_kwarg_helper h, ValuePass t) {
      set(h.id, t);
    }
    template<typename T, typename... types> void unwrap(Tuple_kwarg_helper h, T t, types... values) {
      unwrap(h, t);
      unwrap(values...);

    }
    template<typename T, typename... types> void unwrap(T t, types... values) {
      unwrap(t);
      unwrap(values...);
    }
    //void populate(Context &ctx, Function*);

    void injector(std::map<Identifier, ValuePass>::iterator it) {}
		template<typename T, typename... types> void injector(std::map<Identifier, ValuePass>::iterator it, Tuple_kwarg_helper h, T &t, types & ... values) {
			auto fi = m_args.find(h.id);
			if(fi != m_args.end()) {
				fi->second->inject(t);
			}else{
				it->second->inject(t);
				it++;
			}
			injector(it, values...);
		}
		template<typename T, typename... types> void injector(std::map<Identifier, ValuePass>::iterator it, T &t, types & ... values) {
      assert(it != end());
      it->second->inject(t);
      it++;
      injector(it, values...);
    }

    friend class Function;
  public:
    void set(Context &ctx, Identifier, ilang::ValuePass) override;
    bool has(Context &ctx, Identifier) override;
    ValuePass get(Context &ctx, Identifier i) override;

    Handle<Variable> getVariable(Context &ctx, Identifier i);

    void push(ilang::ValuePass);
    void set(Identifier, ValuePass);
    ValuePass get(Identifier);

    // hm...constructing the context here
    ValuePass operator[](Identifier i) { return get(i); }
    ValuePass operator[](unsigned long i) { return get(Identifier(i)); }

    auto begin() { return m_args.begin(); }
    auto end() { return m_args.end(); }

    size_t size();

    Tuple();
    Tuple(const Tuple&);
    Tuple(std::vector<ValuePass> pargs);
    template<typename... types> Tuple(types... values) {
      unwrap(values...);
    }

    template <typename... types> void inject(types & ... values) {
      // int a; std::stinrg b; args.inject(a, b);
      injector(begin(), values...);
    }
  };

	// a class for being on the left hand side of an expression
	// will have variables instead of values as it is what is being set
	class TupleLHS {
		friend class Tuple;

	};
}

#endif // _ilang_tuple
