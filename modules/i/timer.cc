#include "ilang.h"
#include "error.h"

#include "../../deps/libuv/include/uv.h"


namespace {
  using namespace std;
  using namespace ilang;

  class timerManager : public ilang::C_Class {
  private:
    struct timerData {
      ValuePass m_function;
      timerManager *parent;
      uv_timer_t m_timer;
      bool doDelete = false;
    };
    timerData *m_data;
    long m_timeout;
    bool m_interval;
    ValuePass stopTimer(std::vector<ValuePass> &args) {
      uv_timer_stop(&m_data->m_timer);
      m_data->doDelete = true;
      return ValuePass(new ilang::Value(true));
    }

    static void timer_callback(uv_timer_t *handle, int status) {
      timerData *data = (timerData*)handle->data;
      ilang::Function func = boost::any_cast<ilang::Function>(data->m_function->Get());
      vector<ValuePass> params;
      ValuePass ret = ValuePass(new ilang::Value);
      if(func.object) {
	ScopePass obj_scope = ScopePass(new ObjectScope(boost::any_cast<ilang::Object*>(func.object->Get())));
	func.ptr(obj_scope, params, &ret);
      }else{
	func.ptr(ScopePass(), params, &ret);
      }
      // ignore the return type

      if(!uv_timer_get_repeat(&data->m_timer)) {
	if(data->doDelete) {
	  delete data;
	}else{
	  data->doDelete = true;
	}
      }
    }

    void Init() {
      reg("stop", &timerManager::stopTimer);
    }

  public:
    timerManager(long time, ValuePass func, bool interval = false) :
      m_timeout(time), m_interval(interval)
    {
      assert(func->Get().type() == typeid(ilang::Function));
      m_data = new timerData;
      m_data->parent = this;
      m_data->m_function = func;

      uv_timer_init(uv_default_loop(), &m_data->m_timer);
      m_data->m_timer.data = m_data;
      uv_timer_start(&m_data->m_timer, timer_callback, time, interval ? time : 0);

      Init();
    }
    virtual ~timerManager() {
      if(m_data->doDelete) {
	// the timer must be stopped then
	delete m_data;
      }else{
	m_data->doDelete = true;
      }
      //uv_timer_stop(&m_timer);
    }
  };

  ValuePass setTimer(std::vector<ValuePass> &args) {
    error(args.size() == 2, "Expects 2 arguments");
    error(args[0]->Get().type() == typeid(long), "first argument to setTimeout should be a time in ms");
    error(args[1]->Get().type() == typeid(ilang::Function), "second argument to setTimeout should be a function");
    timerManager *time = new timerManager(boost::any_cast<long>(args[0]->Get()), args[1], false);

    return ValuePass(new ilang::Value(new ilang::Object(time)));
  }

  ValuePass setInterval(std::vector<ValuePass> &args) {
    error(args.size() == 2, "Expects 2 arguments");
    error(args[0]->Get().type() == typeid(long), "first argument to setInterval should be a time in ms");
    error(args[1]->Get().type() == typeid(ilang::Function), "second argument to setInterval should be a function");
    timerManager *time = new timerManager(boost::any_cast<long>(args[0]->Get()), args[1], true);

    return ValuePass(new ilang::Value(new ilang::Object(time)));
  }


  ILANG_LIBRARY_NAME("i/timer",
		     ILANG_FUNCTION("setTimeout", setTimer)
		     ILANG_FUNCTION("setInterval", setInterval)
		     )

} // unnammed namespace
