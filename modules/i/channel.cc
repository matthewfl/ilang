#include "ilang.h"
#include "variable.h"
#include "error.h"
#include "thread.h"

#include <tbb/concurrent_queue.h>

namespace {
	using namespace std;
	using namespace ilang;
	class threadChannel : public ilang::C_Class {
	private:
		tbb::concurrent_bounded_queue<ValuePass> m_queue;
		ilang::Event waiting;
		ValuePass push(Arguments &args) {
			error(args.size() == 1, "channel.push expects one argument");
			m_queue.push(args[0]);
			waiting.Trigger(NULL);
			return ValuePass();
		}
		ValuePass pop(Arguments &args) {
			error(args.size() == 0, "channel.pop expects zero arguments");
			ValuePass ret;
			//m_queue.pop(ret); // blocks until there is something to pop
			while(!m_queue.try_pop(ret)) {
				// there was nothing to pop from the queue
				ilang::global_EventPool()->WaitEvent(waiting);
			}
			return ret;
		}
		ValuePass size(Arguments &args) {
			error(args.size() == 0, "channel.size expects zero arguments");
			long size = m_queue.size();
			return valueMaker(size);
		}
		ValuePass setLimit(Arguments &args) {
			error(args.size() == 1, "channel.setLimit expects one argument");
			error(args[0]->type() == typeid(long), "channel.setLimit expects a number");
			m_queue.set_capacity(args[0]->cast<long>());
			return ValuePass();
		}
		void Init() {
			reg("push", &threadChannel::push);
			reg("pop", &threadChannel::pop);
			reg("size", &threadChannel::size);
			reg("setLimit", &threadChannel::setLimit);
		}
	public:
		threadChannel() {
			Init();
			m_queue.set_capacity(10);
		}
		threadChannel(int size) {
			Init();
			m_queue.set_capacity(size);
		}
	};

	ValuePass createChannel(Arguments &args) {
		error(args.size() <= 1, "channel.create expects 1 or no arguments");
		int channelLength = 10;
		if(args.size()) {
			error(args[0]->type() == typeid(long), "channel.create expects 1 argument");
			channelLength = args[0]->cast<int>();
		}
		threadChannel *ch = new threadChannel(channelLength);
		auto obj = make_shared<ilang::Object>(ch);
		return valueMaker(obj);
	}

	ILANG_LIBRARY_NAME("i/channel",
										 ILANG_FUNCTION("create", createChannel)
										 ILANG_CLASS("channel", threadChannel)
										 )
}
