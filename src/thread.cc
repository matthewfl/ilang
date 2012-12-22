#include "thread.h"
#include "debug.h"


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
    int oldQueueSize = 1;
    MoreThreads();
    while(true) {
      if(m_eventsWaiting == 0 && m_threadCount == m_waitingThread) {
	// there are no events that are currently being waited on and all the threads that are managed by this Event loop are dead
	for(int i = m_threadCount; i > 0; i--) {
	  // if the threads are waiting for an event, give them something so they can noticed that there is nothing left
	  // should cause any possibly "stuck" threads to exit
	  Event e = CreateEvent([](void *data) {});
	  e.Trigger(NULL);
	}
	return; // exit out of the eventLoop / program
      }
      int queueSize = m_queueEvents.size();
      if(oldQueueSize > 5 && queueSize >= oldQueueSize) {
	MoreThreads();
      }
      oldQueueSize = queueSize;
      if(m_waitingThread == 0) {
	// having a thread waiting means that events should be cleared quickly
	MoreThreads();
      }
      std::this_thread::yield();
    }
  }

  void EventPool::MoreThreads() {
    std::thread th(stat_ThreadStart, this);
    th.detach();
  }

  void EventPool::stat_ThreadStart(EventPool *self) {
    self->ThreadStart();
  }

  void EventPool::ThreadStart() {
    ++m_threadCount;
    while(true) {
      if(m_queueEvents.empty()) {
	std::this_thread::yield();
	// at most 10% of threads waiting for work
	if(m_waitingThread * 10 > m_threadCount || m_eventsWaiting == 0) {
	  // kill off thread if there is not another task waiting
	  --m_threadCount;
	  return;
	} else {
	  boost::function0<void> func;
	  ++m_waitingThread;
	  func = GetEventToRun();
	  --m_waitingThread;
	  func();
	}
      }else{
	// There is work in the queue to do
	//debug(0, "----Running event from pool");
	GetEventToRun()();
      }
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
    unsigned long id = m_eventIndex++;
    m_eventsWaiting++;
    assert(id); // if zeon then this has looped arround
    // for the time being, this will be an error, but in the future this should not cause a problem
    m_eventList.insert(std::pair<unsigned long, Event_Callback>(id, back));
    Event e(this, id);
    return e;
  }

  void EventPool::DeleteEvent(unsigned long id) {
    // Want to make sure the event is actually in the map that we are deleting
    assert(m_eventList.erase(id));
    m_eventsWaiting--;
  }

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
    assert(m_pool);
    tbb::interface5::concurrent_hash_map<unsigned long, Event_Callback>::accessor access;
    assert(m_pool->m_eventList.find(access, m_id));
    access->second = back; // not sure if this works like this, but this appears that it will from the documents
  }

} // namespace ilang
