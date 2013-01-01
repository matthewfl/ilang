#ifndef _ilang_thread
#define _ilang_thread

#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_queue.h>
//#include <thread>
#include <boost/thread.hpp>
#include <atomic>
#include <boost/function.hpp>
#include <ucontext.h>


namespace ilang {
  class ThreadPool;
  class Thread;
  class EventPool;
  class Event;

  typedef boost::function1<void, void*> Event_Callback;


  struct s_eventData {
    //Event_Callback callback;
    ucontext_t context;
    ucontext_t back;
    bool done = false;
    unsigned long id;
    void *data = NULL;
    char stack[16384];
    Event_Callback callback;
    s_eventData() {
      // move this into source file
      getcontext(&context);
      context.uc_stack.ss_sp = stack;
      context.uc_stack.ss_size = sizeof(stack);
      context.uc_link = &back;
    }
  };

  //extern boost::thread_specific_ptr<s_eventData> Thread_current_event;

  class EventPool {
    friend class Event;
    friend class ThreadPool;
  private:
    struct s_eventTrigger {
      unsigned long id;
      void *data;
    };


    tbb::concurrent_bounded_queue<s_eventTrigger> m_queueEvents;
    tbb::interface5::concurrent_hash_map<unsigned long, s_eventData*> m_eventList;
    std::atomic<unsigned long> m_eventsWaiting;//(0);
    std::atomic<unsigned long> m_eventIndex;//(1);


    void DeleteEvent(unsigned long id);
    //boost::function0<void> GetEventToRun(); // blocks until there is an event function
    void RunEvents();
  public:
    EventPool();
    Event CreateEvent(Event_Callback);
    void TriggerEvent(unsigned long id, void *data);
    //void WaitEvent(Event &e);
    Event GetEvent(unsigned long id);

    void Run();
  private:
    std::atomic<unsigned long> m_threadCount;
    std::atomic<unsigned long> m_waitingThread;

    void MoreThreads();
    void ThreadStart();
    static void stat_ThreadStart(EventPool*);

  };

  class Event {
    friend class EventPool;
  private:
    unsigned long m_id;
    EventPool *m_pool;

    Event(EventPool *, unsigned long);
  public:
    Event();
    void Trigger(void *data);
    void Cancel();
    void SetCallback(Event_Callback back);
    Event_Callback GetCallback();
    unsigned long GetId() { return m_id; }
  };


  EventPool *global_EventPool();
} // namespace ilang


#endif // _ilang_thread
