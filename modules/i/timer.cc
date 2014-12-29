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
		ValuePass stopTimer(Context &ctx, Arguments &args) {
			uv_timer_stop(&m_data->m_timer);
			m_data->doDelete = true;
			return valueMaker(true);
		}

		static void timer_callback(uv_timer_t *handle, int status) {
			timerData *data = (timerData*)handle->data;
			ilang::Function func = *data->m_function->cast<Function*>();
			//vector<ValuePass> params;
			Context ctx; // this is a callback so there isn't really any scope ctx for this
			ValuePass ret = func(ctx);
			// ValuePass ret = ValuePass(new ilang::Value);
			// if(func.object) {
			// 	ScopePass obj_scope = ScopePass(new ObjectScope(boost::any_cast<ilang::Object*>(func.object->Get())));
			// 	func.ptr(obj_scope, params, &ret);
			// }else{
			// 	func.ptr(ScopePass(), params, &ret);
			// }
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
			assert(func->type() == typeid(ilang::Function));
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

	ValuePass setTimer(Context &ctx, Arguments &args) {
		error(args.size() == 2, "Expects 2 arguments");
		error(args[0]->type() == typeid(long), "first argument to setTimeout should be a time in ms");
		error(args[1]->type() == typeid(ilang::Function), "second argument to setTimeout should be a function");
		timerManager *time = new timerManager(args[0]->cast<long>(), args[1], false);

		// TODO: returning class instance
		//auto obj = make_handle<ilang::Object>(time);
		return valueMaker(false);
	}

	ValuePass setInterval(Context &ctx, Arguments &args) {
		error(args.size() == 2, "Expects 2 arguments");
		error(args[0]->type() == typeid(long), "first argument to setInterval should be a time in ms");
		error(args[1]->type() == typeid(ilang::Function), "second argument to setInterval should be a function");
		timerManager *time = new timerManager(args[0]->cast<long>(), args[1], true);

		return valueMaker(Handle<Hashable>(time));
		//auto obj = make_handle<ilang::Object>(time);
		//return valueMaker(false); //obj);
	}


	ILANG_LIBRARY_NAME("i/timer",
										 ILANG_FUNCTION("setTimeout", setTimer)
										 ILANG_FUNCTION("setInterval", setInterval)
										 )

} // unnammed namespace
