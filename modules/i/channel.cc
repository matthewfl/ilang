#include "ilang.h"
#include "variable_new.h"
#include "error.h"
#include "thread.h"

#include "handle.h"

#include <tbb/concurrent_queue.h>

namespace {
	using namespace std;
	using namespace ilang;
	class threadChannel : public ilang::C_Class {
	private:
		tbb::concurrent_bounded_queue<ValuePass> m_queue;
		ilang::Event waiting;
		ValuePass push(Context &ctx, Arguments &args) {
			error(args.size() == 1, "channel.push expects one argument");
			auto v = args[0];
			waiting.Trigger(NULL);
			m_queue.push(v);
			return ValuePass();
		}
		ValuePass pop(Context &ctx, Arguments &args) {
			error(args.size() == 0, "channel.pop expects zero arguments");
			ValuePass ret;
			//m_queue.pop(ret); // blocks until there is something to pop
			while(!m_queue.try_pop(ret)) {
				// there was nothing to pop from the queue
				if(m_queue.capacity() != 0) // if zero size then we spin on the queue otherwise the push call blocks?
					waiting.Wait();
			}
			return ret;
		}
		ValuePass size(Context &ctx, Arguments &args) {
			error(args.size() == 0, "channel.size expects zero arguments");
			long size = m_queue.size();
			return valueMaker(size);
		}
		ValuePass setLimit(Context &ctx, Arguments &args) {
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
		threadChannel(Context &ctx, Arguments &args) {
			Init();
			int len = 10;
			if(args.size() == 1)
				len = args[0]->cast<int>();
			m_queue.set_capacity(len);
		}
		threadChannel(int size) {
			Init();
			m_queue.set_capacity(size);
		}
	};

	ValuePass createChannel(Context &ctx, Arguments &args) {
		error(args.size() <= 1, "channel.create expects 1 or no arguments");
		int channelLength = 10;
		if(args.size()) {
			error(args[0]->type() == typeid(long), "channel.create expects 1 argument");
			channelLength = args[0]->cast<int>();
		}
		auto ch = make_handle<threadChannel>(channelLength);
		return valueMaker(static_pointer_cast<Hashable>(ch));
	}

	ILANG_LIBRARY_NAME("i/channel",
										 ILANG_FUNCTION("create", createChannel)
										 ILANG_CLASS("channel", threadChannel)
										 )
}
