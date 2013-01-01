#include "thread.h"
#include "debug.h"

#include <iostream>
using namespace std;

#include <boost/date_time/posix_time/posix_time.hpp>


/** Threads are created at the start of the process
 *  Threads will keep asking the event loop for more work
 *  When there is no more work, a thread will check if there are too many othre threads also waiting for
 *  If there are already enough other threads waiting for work, then the thread will terminate itself
 *
 *  The main thread will make sure that the queue is not becoming too full with work otherwise it can start more threads
 */

namespace ilang {
  static EventPool _global_EventPool;
  EventPool *global_EventPool() {
    return &_global_EventPool;
  }
  boost::thread_specific_ptr<s_eventData> Thread_current_event;


  EventPool::EventPool() {
    m_eventsWaiting = 0;
    m_eventIndex = 1;
    m_queueEvents.set_capacity(200);
    m_eventList.rehash(500);

    m_threadCount = 0;
    m_waitingThread = 0;
  }

  void EventPool::Run() {
    //std::thread th(stat_ThreadStart, this);
    //th.join();
    int oldQueueSize = 0;
    MoreThreads();
    while(true) {
      //boost::this_thread::sleep(10); //_for(std::chrono::milliseconds(10));
      //boost::this_thread::yield();
      boost::this_thread::sleep(boost::posix_time::millisec(10));
      if(m_eventsWaiting == 0) {
	if(m_threadCount == m_waitingThread) {
	  // there are no events that are currently being waited on and all the threads that are managed by this Event loop are dead
	  for(int i = m_threadCount; i > 0; i--) {
	    // if the threads are waiting for an event, give them something so they can noticed that there is nothing left
	    // should cause any possibly "stuck" threads to exit
	    Event e = CreateEvent([](void *data) {});
	    e.Trigger(NULL);
	  }
	  return; // exit out of the eventLoop / program
	}
      }else{
	int queueSize = m_queueEvents.size();
	if(oldQueueSize > 5 && queueSize != 0 && queueSize >= oldQueueSize) {
	  MoreThreads();
	}
	oldQueueSize = queueSize;
	if(m_waitingThread == 0) {
	  // having a thread waiting means that events should be cleared quickly
	  MoreThreads();
	}
      }
      cout << "\t\t" << m_eventsWaiting << "\t" << m_threadCount << "\t" << m_waitingThread << endl;

      //std::this_thread::yield();
    }
  }

  void EventPool::MoreThreads() {
    //std::thread th(stat_ThreadStart, this);
    //th.detach();
    boost::thread th(stat_ThreadStart, this);
    th.detach();
  }

  void EventPool::stat_ThreadStart(EventPool *self) {
    self->ThreadStart();
  }

  void EventPool::ThreadStart() {
    ++m_threadCount;
    while(true) {
      //cout << "loop top\n";
      if(m_queueEvents.empty()) {
	//	std::this_thread::yield();
	//boost::this_thread::sleep(10); //_for(std::chrono::milliseconds(5));
	boost::this_thread::sleep(boost::posix_time::millisec(5));
	// at most 10% of threads waiting for work
	if(true || m_waitingThread * 10 > m_threadCount || m_eventsWaiting == 0) {
	  // kill off thread if there is not another task waiting
	  --m_threadCount;
	  return;
	}else{
	  boost::function0<void> func;
	  ++m_waitingThread;
	  //func = GetEventToRun();
	  //func();
	  boost::this_thread::yield();
	  --m_waitingThread;
	}
      }else{
	// There is work in the queue to do
	//debug(0, "----Running event from pool");
	//	GetEventToRun();
	RunEvents();
      }
      //cout << "loop bottom\n";
    }
  }

  void EventPool::TriggerEvent(unsigned long id, void *data) {
    s_eventTrigger trigger;
    trigger.id = id;
    trigger.data = data;
    m_queueEvents.push(trigger);
    if(m_queueEvents.size() > 150) {
      MoreThreads();
    }
  }

  Event EventPool::CreateEvent(Event_Callback back) {
    m_eventsWaiting++;
    unsigned long id = m_eventIndex++;
    assert(id); // if zeon then this has looped arround
    // for the time being, this will be an error, but in the future this should not cause a problem
    s_eventData *eve = new s_eventData;
    eve->id = id;
    eve->callback = back;
    makecontext(&eve->context, [](void) -> void {
	Thread_current_event->callback(Thread_current_event->data);
	Thread_current_event->done = true;
      }, 0);
    m_eventList.insert(std::pair<unsigned long, s_eventData*>(id, eve));
    //m_eventList.insert(std::pair<unsigned long, Event_Callback>(id, back));
    Event e(this, id);
    return e;
  }

  /*  void EventPool::WaitEvent(Event &e) {
    Event origional_even = e;
    ucontext_t context;// = new ucontext_t;
    void *_data;
    Event new_event = CreateEvent([context, &_data](void *data) {
	//origional_even.Trigger(data);
	_data = data;
	setcontext(context);
      });
    getcontext(context);
    if(new_event == e) {
      e = origional_even;
      e.Trigger(_data);
      return;
    }else{
      e = new_event;
    }
    }*/

  void EventPool::DeleteEvent(unsigned long id) {
    // Want to make sure the event is actually in the map that we are deleting
    assert(m_eventList.erase(id));
    m_eventsWaiting--;
  }

  /*
  boost::function0<void> EventPool::GetEventToRun() {
    while(true) {
      s_eventTrigger event;
      m_queueEvents.pop(event);
      tbb::interface5::concurrent_hash_map<unsigned long, Event_Callback>::accessor access;
      if(m_eventList.find(access, event.id)) {
	Event_Callback func = access->second;
	// need to delete the data from the list
	assert(m_eventList.erase(access));
	m_eventsWaiting--;
	return [func, event]() -> void {
	  func(event.data);
	};
      } // else there were no functions tied to this callback
    }
    //return []() -> void {};
    }*/

  void EventPool::RunEvents() {
    tbb::interface5::concurrent_hash_map<unsigned long, s_eventData*>::accessor access;
    s_eventTrigger event;
    m_queueEvents.pop(event);
    if(m_eventList.find(access, event.id)) {
      s_eventData *eve = access->second;
      eve->data = event.data;
      Thread_current_event.reset(eve);
      swapcontext(&eve->back, &eve->context);
      Thread_current_event.release();
      // might still want to remove from the hasmap
      if(eve->done) {
	// delete the even and the stack
	assert(m_eventList.erase(access));
	delete eve;
	m_eventsWaiting--;
      }
    }
  }


  Event::Event(EventPool *pool,  unsigned long id) : m_pool(pool), m_id(id) {}
  Event::Event() : m_pool(NULL), m_id(0) {}
  void Event::Trigger(void *data) {
    if(m_pool) m_pool->TriggerEvent(m_id, data);
  }
  void Event::Cancel() {
    if(m_pool) m_pool->DeleteEvent(m_id);
  }
  void Event::SetCallback(Event_Callback back) {
    // first check that the event is actually still in the list
    assert(0);
    /*assert(m_pool);
    tbb::interface5::concurrent_hash_map<unsigned long, Event_Callback>::accessor access;
    assert(m_pool->m_eventList.find(access, m_id));
    access->second = back; // not sure if this works like this, but this appears that it will from the documents
    */
  }

} // namespace ilang
