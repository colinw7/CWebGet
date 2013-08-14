// TODO: Locking Hierarchy to ensure multiple locks are acquired in the right order

// TODO: Add support for thread specific data (pthread_[set|get]specific)
// TODO: Thread local storage __thread ??

// TODO: thread cleanup (thread_cleanup_[push|pop]) ?

#include <CThread.h>
#include <CFuncs.h>
#include <CThrow.h>

extern "C" {
#include <sys/shm.h>
#include "pthread.h"
}

#include <algorithm>
#include <cstring>
#include <cstdarg>
#include <cerrno>
#include <cstdio>

using std::string;
using std::list;

bool                CThreadArray::debug_       = false;

pthread_attr_t      CThread::attr_;
bool                CThread::attr_inited_      = false;
int                 CThread::num_threads_      = 0;
bool                CThread::debug_            = false;

pthread_mutexattr_t CThreadMutex::attr_;
bool                CThreadMutex::attr_inited_ = false;
bool                CThreadMutex::debug_       = false;

pthread_condattr_t  CThreadCondition::attr_;
bool                CThreadCondition::attr_inited_ = false;
bool                CThreadCondition::debug_       = false;

static pthread_once_t once_control = PTHREAD_ONCE_INIT;

CThreadOnce::
CThreadOnce(OnceProc proc) :
 proc_(proc)
{
  once_ = once_control;
}

void
CThreadOnce::
exec()
{
  pthread_once(&once_, proc_);
}

CThreadArray::
CThreadArray(int n) :
 n_(n), num_active_(0)
{
  thread_mutex_ = new CThreadMutex("thread_mutex");
  alive_mutex_  = new CThreadMutex("alive_mutex");

  thread_cond_ = new CThreadCondition("thread_cond");
}

CThreadArray::
~CThreadArray()
{
  delete thread_mutex_;
  delete alive_mutex_;
  delete thread_cond_;

  for_each(procs_.begin(), procs_.end(), CDeletePointer());

  for_each(alive_threads_.begin(), alive_threads_.end(), CDeletePointer());

  for_each(dead_threads_.begin(), dead_threads_.end(), CDeletePointer());
}

void
CThreadArray::
setDebug(bool debug)
{
  debug_ = debug;
}

bool
CThreadArray::
startThread(CThreadProc proc, void *data)
{
  if (! alive_mutex_->lock())
    return false;

  CThreadArrayCProc *array_proc = new CThreadArrayCProc(this, proc, data);

  addArrayProc(array_proc);

  alive_mutex_->unlock();

  //------

  return startThread1(array_proc);
}

bool
CThreadArray::
startThread(CThreadArrayProc *proc)
{
  return startThread1(proc);
}

bool
CThreadArray::
startThread1(CThreadArrayProc *proc)
{
  if (debug_)
    std::cerr << "startThread1" << std::endl;

  if (! thread_mutex_->lock())
    return false;

  while (num_active_ == n_) {
    thread_cond_->wait(*thread_mutex_);

    if (! alive_mutex_->lock())
      return false;

    joinDeadThreads(NULL);

    alive_mutex_->unlock();
  }

  //------

  ++num_active_;

  //------

  if (! alive_mutex_->lock())
    return false;

  CThread *thread = new CThread();

  addAliveThread(thread);

  proc->setThread(thread);

  alive_mutex_->unlock();

  //------

  thread_mutex_->unlock();

  //------

  if (! thread->start(&CThreadArray::executeCB, (void *) proc))
    endThread(thread);

  return true;
}

bool
CThreadArray::
endThread(CThread *thread)
{
  if (debug_)
    std::cerr << "endThread" << std::endl;

  if (! alive_mutex_->lock())
    return false;

  removeAliveThread(thread);

  alive_mutex_->unlock();

  //------

  if (! thread_mutex_->lock())
    return false;

  --num_active_;

  if (num_active_ == n_ - 1)
    thread_cond_->signal();

  thread_mutex_->unlock();

  thread->exit(NULL);

  return true;
}

bool
CThreadArray::
join(void **data)
{
  if (debug_)
    std::cerr << "join" << std::endl;

  if (! alive_mutex_->lock())
    return false;

  //------

  joinAliveThreads(data);
  joinDeadThreads (data);

  deleteArrayProcs();

  //------

  alive_mutex_->unlock();

  return true;
}

void
CThreadArray::
addArrayProc(CThreadArrayProc *array_proc)
{
  procs_.push_back(array_proc);
}

void
CThreadArray::
deleteArrayProcs()
{
  for_each(procs_.begin(), procs_.end(), CDeletePointer());

  procs_.clear();
}

bool
CThreadArray::
joinDeadThreads(void **data)
{
  CThread **dead_threads;
  int       num_dead_threads;

  getDeadThreads(&dead_threads, &num_dead_threads);

  while (num_dead_threads > 0) {
    alive_mutex_->unlock();

    dead_threads[0]->join(data);

    if (! alive_mutex_->lock())
      return false;

    delete [] dead_threads;

    getDeadThreads(&dead_threads, &num_dead_threads);
  }

  delete [] dead_threads;

  return true;
}

bool
CThreadArray::
getDeadThreads(CThread ***dead_threads, int *num_dead_threads)
{
  *dead_threads     = NULL;
  *num_dead_threads = dead_threads_.size();

  if (*num_dead_threads > 0) {
    *dead_threads = new CThread * [*num_dead_threads];

    list<CThread *>::iterator pthread1 = dead_threads_.begin();
    list<CThread *>::iterator pthread2 = dead_threads_.end();

    int i = 0;

    for ( ; pthread1 != pthread2; ++pthread1)
      (*dead_threads)[i++] = *pthread1;
  }

  dead_threads_.clear();

  return true;
}

void
CThreadArray::
addAliveThread(CThread *thread)
{
  alive_threads_.push_back(thread);
}

void
CThreadArray::
removeAliveThread(CThread *thread)
{
  alive_threads_.remove(thread);

  dead_threads_.push_back(thread);
}

bool
CThreadArray::
joinAliveThreads(void **data)
{
  CThread **alive_threads;
  int       num_alive_threads;

  getAliveThreads(&alive_threads, &num_alive_threads);

  while (num_alive_threads > 0) {
    alive_mutex_->unlock();

    alive_threads[0]->join(data);

    if (! alive_mutex_->lock())
      return false;

    delete [] alive_threads;

    getAliveThreads(&alive_threads, &num_alive_threads);
  }

  return true;
}

bool
CThreadArray::
getAliveThreads(CThread ***alive_threads, int *num_alive_threads)
{
  *alive_threads     = NULL;
  *num_alive_threads = alive_threads_.size();

  if (*num_alive_threads > 0) {
    *alive_threads = new CThread * [*num_alive_threads];

    list<CThread *>::iterator pthread1 = alive_threads_.begin();
    list<CThread *>::iterator pthread2 = alive_threads_.end();

    int i = 0;

    for ( ; pthread1 != pthread2; ++pthread1)
      (*alive_threads)[i++] = *pthread1;
  }

  alive_threads_.clear();

  return true;
}

void *
CThreadArray::
executeCB(void *data)
{
  CThreadArrayProc *proc = (CThreadArrayProc *) data;

  return proc->executeCB();
}

void *
CThreadArrayProc::
executeCB()
{
  void *retval = execute();

  sleep(1);

  array_->endThread(thread_);

  return retval;
}

void *
CThreadArrayCProc::
execute()
{
  return (*proc_)(data_);
}

CThread::
CThread() :
 thread_(0)
{
}

CThread::
CThread(pthread_t thread) :
 thread_(thread)
{
}

CThread::
~CThread()
{
  --num_threads_;

  if (debug_)
    std::cerr << num_threads_ << " Threads" << std::endl;
}

bool
CThread::
start()
{
  return start(&CThread::executeCB, (void *) this);
}

bool
CThread::
start(CThreadProc proc, void *data)
{
  if (thread_ != 0)
    return false;

  int error = pthread_create(&thread_, getAttr(), proc, data);

  if (error != 0) {
    CTHROW(string("pthread_create: ") + strerror(error));

    return false;
  }

  resetAttr();

  ++num_threads_;

  if (debug_)
    std::cerr << num_threads_ << " Threads" << std::endl;

  return true;
}

void *
CThread::
executeCB(void *data)
{
  CThread *thread = (CThread *) data;

  return thread->execute();
};

bool
CThread::
join(void **data)
{
  if (thread_ == 0)
    return false;

  int error = pthread_join(thread_, data);

  if (error != 0 && error != EINVAL) {
    CTHROW(string("pthread_join: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThread::
detach()
{
  if (thread_ == 0)
    return false;

  int error = pthread_detach(thread_);

  if (error != 0) {
    CTHROW(string("pthread_detach: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThread::
cancel()
{
  if (thread_ == 0)
    return false;

  int error = pthread_cancel(thread_);

  if (error == ESRCH) error = 0;

  if (error != 0) {
    CTHROW(string("pthread_cancel: ") + strerror(error));
    return false;
  }

  return true;
}

void
CThread::
testcancel()
{
  pthread_testcancel();
}

bool
CThread::
kill(int signal)
{
  if (thread_ == 0)
    return false;

  int error = pthread_kill(thread_, signal);

  if (error != 0) {
    CTHROW(string("pthread_kill: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThread::
setCancelState(int state)
{
  int last_state;

  if (thread_ == 0)
    return false;

  int error = pthread_setcancelstate(state, &last_state);

  if (error != 0) {
    CTHROW(string("pthread_setcancelstate: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThread::
setCancelType(int type)
{
  int last_type;

  if (thread_ == 0)
    return false;

  int error = pthread_setcanceltype(type, &last_type);

  if (error != 0) {
    CTHROW(string("pthread_setcanceltype: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThread::
operator==(const CThread &thread)
{
  return pthread_equal(thread_, thread.thread_);
}

void
CThread::
exit(void *data)
{
  pthread_exit(data);
}

bool
CThread::
setDetachState(int state)
{
  int error = pthread_attr_setdetachstate(getAttr(), state);

  if (error != 0) {
    CTHROW(string("pthread_attr_setdetachstate: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThread::
getDetachState(int *state)
{
  int error = pthread_attr_getdetachstate(getAttr(), state);

  if (error != 0) {
    CTHROW(string("pthread_attr_getdetachstate: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThread::
setScope(int scope)
{
#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
  int error = pthread_attr_setscope(getAttr(), scope);

  if (error != 0) {
    CTHROW(string("pthread_attr_setscope: ") + strerror(error));
    return false;
  }

  return true;
#else
  return false;
#endif
}

bool
CThread::
getScope(int *scope)
{
#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
  int error = pthread_attr_getscope(getAttr(), scope);

  if (error != 0) {
    CTHROW(string("pthread_attr_getscope: ") + strerror(error));
    return false;
  }

  return true;
#else
  return false;
#endif
}

bool
CThread::
getStack(void **addr, uint *size)
{
  size_t size1;

  int error = pthread_attr_getstack(getAttr(), addr, &size1);

  if (error != 0) {
    CTHROW(string("pthread_attr_getstack: ") + strerror(error));
    return false;
  }

  *size = size1;

  return false;
}

bool
CThread::
setStack(void *addr, uint size)
{
  int error = pthread_attr_setstack(getAttr(), addr, size);

  if (error != 0) {
    CTHROW(string("pthread_attr_setstack: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThread::
setSchedulePolicy(int policy, int priority)
{
  if (! setInheritSchedule(PTHREAD_EXPLICIT_SCHED))
    return false;

  int error = pthread_attr_setschedpolicy(getAttr(), policy);

  if (error != 0) {
    CTHROW(string("pthread_attr_setschedpolicy: ") + strerror(error));
    return false;
  }

  if (priority == -1)
    priority = getAvgPriority(policy);

  struct sched_param sched_param;

  sched_param.sched_priority = priority;

  error = pthread_attr_setschedparam(getAttr(), &sched_param);

  if (error != 0) {
    CTHROW(string("pthread_attr_setschedparam: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThread::
getSchedulePolicy(int *policy)
{
  int error = pthread_attr_getschedpolicy(getAttr(), policy);

  if (error != 0) {
    CTHROW(string("pthread_attr_getschedpolicy: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThread::
setInheritSchedule(int inherit)
{
  int error = pthread_attr_setinheritsched(getAttr(), inherit);

  if (error != 0) {
    CTHROW(string("pthread_attr_setinheritsched: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThread::
getInheritSchedule(int *inherit)
{
  int error = pthread_attr_getinheritsched(getAttr(), inherit);

  if (error != 0) {
    CTHROW(string("pthread_attr_getinheritsched: ") + strerror(error));
    return false;
  }

  return true;
}

int
CThread::
getMinPriority(int policy)
{
  return sched_get_priority_min(policy);
}

int
CThread::
getMaxPriority(int policy)
{
  return sched_get_priority_max(policy);
}

int
CThread::
getAvgPriority(int policy)
{
  int max_priority = sched_get_priority_max(policy);
  int min_priority = sched_get_priority_min(policy);

  return (max_priority + min_priority)/2;
}

bool
CThread::
updateSchedulePolicy(int policy, int priority)
{
  struct sched_param sched_param;

  sched_param.sched_priority = priority;

  int error = pthread_setschedparam(thread_, policy, &sched_param);

  if (error != 0) {
    CTHROW(string("pthread_setschedparam: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThread::
getSchedulePolicy(int *policy, int *priority)
{
  struct sched_param sched_param;

  int error = pthread_getschedparam(thread_, policy, &sched_param);

  if (error != 0) {
    CTHROW(string("pthread_getschedparam: ") + strerror(error));
    return false;
  }

  *priority = sched_param.sched_priority;

  return true;
}

pthread_attr_t *
CThread::
getAttr()
{
  if (! attr_inited_)
    resetAttr();

  return &attr_;
}

void
CThread::
resetAttr()
{
  pthread_attr_init(&attr_);

  attr_inited_ = true;
}

bool
CThread::
blockSignals(int signum, ...)
{
  va_list vargs;

  sigset_t sigs;

  va_start(vargs, signum);

  while (signum != 0) {
    sigaddset(&sigs, signum);

    signum = va_arg(vargs, int);
  }

  va_end(vargs);

  int error = pthread_sigmask(SIG_BLOCK, &sigs, NULL);

  if (error != 0) {
    CTHROW(string("pthread_sigmask: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThread::
atFork(void (*prepareProc)(),  void (*parentProc)(), void (*childProc)())
{
  int error = pthread_atfork(prepareProc, parentProc, childProc);

  if (error != 0) {
    CTHROW(string("pthread_atfork: ") + strerror(error));
    return false;
  }

  return true;
}

pthread_t
CThread::
getSelf()
{
  return pthread_self();
}

void
CThread::
testCancel()
{
  pthread_testcancel();
}

#ifdef _POSIX_THREAD_PROCESS_SHARED

CThreadSharedMutex::
CThreadSharedMutex()
{
  shared_mem_id_ = shmget(IPC_PRIVATE, sizeof(pthread_mutex_t), 0660);

  shared_mem_ptr_ = (void *) shmat(shared_mem_id_, NULL, 0);

  mutex_ = (pthread_mutex_t *) shared_mem_ptr_;

  pthread_mutexattr_t attr_t;

  pthread_mutexattr_init(&attr_t);

  pthread_mutexattr_setpshared(&attr_t, PTHREAD_PROCESS_SHARED);

  int error = pthread_mutex_init(mutex_, &attr_t);

  if (error != 0) {
    CTHROW(string("pthread_mutex_init: ") + strerror(error));
    return;
  }

  locked_ = false;
}

CThreadSharedMutex::
~CThreadSharedMutex()
{
  if (locked_)
    unlock();

  int error = pthread_mutex_destroy(mutex_);

  if (error != 0)
    CTHROW(string("pthread_mutex_destroy: ") + strerror(error));
}

bool
CThreadSharedMutex::
lock()
{
  int error = pthread_mutex_lock(mutex_);

  if (error != 0) {
    CTHROW(string("pthread_mutex_lock: ") + strerror(error));

    return false;
  }

  locked_ = true;

  return true;
}

bool
CThreadSharedMutex::
try_lock()
{
  int error = pthread_mutex_trylock(mutex_);

  if (error != 0)
    return false;

  return true;
}

bool
CThreadSharedMutex::
unlock()
{
  int error = pthread_mutex_unlock(mutex_);

  if (error != 0) {
    CTHROW(string("pthread_mutex_unlock: ") + strerror(error));

    return false;
  }

  locked_ = false;

  return true;
}

#endif

CThreadMutex::
CThreadMutex(const char *id) :
 id_(id)
{
  int error = pthread_mutex_init(&mutex_, getAttr());

  if (error != 0) {
    CTHROW(string("pthread_mutex_init: ") + strerror(error));
    return;
  }

  locked_ = false;
}

CThreadMutex::
~CThreadMutex()
{
  if (locked_)
    unlock();

  int error = pthread_mutex_destroy(&mutex_);

  if (error != 0)
    CTHROW(string("pthread_mutex_destroy: ") + strerror(error));
}

bool
CThreadMutex::
lock()
{
  if (debug_ && id_ != NULL)
    std::cerr << id_ << " try lock" << std::endl;

  int error = pthread_mutex_lock(&mutex_);

  if (error != 0) {
    CTHROW(string("pthread_mutex_lock: ") + strerror(error));

    return false;
  }

  locked_ = true;

  if (debug_ && id_ != NULL)
    std::cerr << id_ << " locked" << std::endl;

  return true;
}

bool
CThreadMutex::
try_lock()
{
  int error = pthread_mutex_trylock(&mutex_);

  if (error != 0)
    return false;

  return true;
}

bool
CThreadMutex::
unlock()
{
  int error = pthread_mutex_unlock(&mutex_);

  if (error != 0) {
    CTHROW(string("pthread_mutex_unlock: ") + strerror(error));

    return false;
  }

  locked_ = false;

  if (debug_ && id_ != NULL)
    std::cerr << id_ << " unlocked" << std::endl;

  return true;
}

bool
CThreadMutex::
setPriorityProtect()
{
#if _POSIX_THREAD_PRIO_PROTECT > 0 && _NEVER
  int protocol;

  int error = pthread_mutex_getprotocol(getAttr(), &protocol);

  if (error != 0) {
    CTHROW(string("pthread_mutex_getprotocol: ") + strerror(error));
    return false;
  }

  if (protocol != PTHREAD_PRIO_PROTECT) {
    error = pthread_mutex_setprotocol(getAttr(), PTHREAD_PRIO_PROTECT);

    if (error != 0) {
      CTHROW(string("pthread_mutex_setprotocol: ") + strerror(error));
      return false;
    }
  }

  int priority = CThread::getMaxPriority(SCHED_FIFO);

  if (! setPriorityCeiling(priority))
    return false;

  return true;
#else
  return false;
#endif
}

bool
CThreadMutex::
setPriorityInherit()
{
#if _POSIX_THREAD_PRIO_INHERIT > 0
  if (! setProtocol(PTHREAD_PRIO_INHERIT))
    return false;

  return true;
#else
  return false;
#endif
}

bool
CThreadMutex::
getProtocol(int *protocol)
{
#if _POSIX_THREAD_PRIO_INHERIT > 0
  int error = pthread_mutexattr_getprotocol(getAttr(), protocol);

  if (error != 0) {
    CTHROW(string("pthread_mutexattr_getprotocol: ") + strerror(error));
    return false;
  }

  return true;
#else
  assert(protocol);

  return false;
#endif
}

bool
CThreadMutex::
setProtocol(int protocol)
{
#if _POSIX_THREAD_PRIO_INHERIT > 0
  int old_protocol;

  if (! getProtocol(&old_protocol))
    return false;

  if (protocol != old_protocol) {
    int error = pthread_mutexattr_setprotocol(getAttr(), protocol);

    if (error != 0) {
      CTHROW(string("pthread_mutexattr_setprotocol: ") + strerror(error));
      return false;
    }
  }

  return true;
#else
  assert(protocol);

  return false;
#endif
}

bool
CThreadMutex::
getPriorityCeiling(int *ceiling)
{
#if _POSIX_THREAD_PRIO_INHERIT > 0
  int error = pthread_mutexattr_getprioceiling(getAttr(), ceiling);

  if (error != 0) {
    CTHROW(string("pthread_mutexattr_getprioceiling: ") + strerror(error));
    return false;
  }

  return true;
#else
  assert(ceiling);

  return false;
#endif
}

bool
CThreadMutex::
setPriorityCeiling(int ceiling)
{
#if _POSIX_THREAD_PRIO_INHERIT > 0
  int old_ceiling;

  if (! getPriorityCeiling(&old_ceiling))
    return false;

  if (ceiling != old_ceiling) {
    int error = pthread_mutexattr_setprioceiling(getAttr(), ceiling);

    if (error != 0) {
      CTHROW(string("pthread_mutexattr_setprioceiling: ") + strerror(error));
      return false;
    }
  }

  return true;
#else
  assert(ceiling);

  return false;
#endif
}

bool
CThreadMutex::
getProcessShared(int *shared)
{
  int error = pthread_mutexattr_getpshared(getAttr(), shared);

  if (error != 0) {
    CTHROW(string("pthread_mutexattr_getpshared: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThreadMutex::
setProcessShared(int shared)
{
  int error = pthread_mutexattr_setpshared(getAttr(), shared);

  if (error != 0) {
    CTHROW(string("pthread_mutexattr_setpshared: ") + strerror(error));
    return false;
  }

  return true;
}

pthread_mutexattr_t *
CThreadMutex::
getAttr()
{
  if (! attr_inited_)
    resetAttr();

  return &attr_;
}

void
CThreadMutex::
resetAttr()
{
  pthread_mutexattr_init(&attr_);

  attr_inited_ = true;
}

CThreadReadWriteMutex::
CThreadReadWriteMutex()
{
  int error = pthread_rdwr_init_np(&mutex_, NULL);

  if (error != 0) {
    CTHROW(string("pthread_rdwr_init_np: ") + strerror(error));
    return;
  }

  read_locked_  = false;
  write_locked_ = false;
}

CThreadReadWriteMutex::
~CThreadReadWriteMutex()
{
  if (read_locked_)
    read_unlock();

  if (write_locked_)
    write_unlock();

  int error = pthread_rdwr_destroy_np(&mutex_);

  if (error != 0)
    CTHROW(string("pthread_rdwr_destroy_np: ") + strerror(error));
}

bool
CThreadReadWriteMutex::
read_lock()
{
  int error = pthread_rdwr_rlock_np(&mutex_);

  if (error != 0) {
    CTHROW(string("pthread_rdwr_rlock_np: ") + strerror(error));

    return false;
  }

  read_locked_ = true;

  return true;
}

bool
CThreadReadWriteMutex::
write_lock()
{
  int error = pthread_rdwr_wlock_np(&mutex_);

  if (error != 0) {
    CTHROW(string("pthread_rdwr_wlock_np: ") + strerror(error));

    return false;
  }

  write_locked_ = true;

  return true;
}

bool
CThreadReadWriteMutex::
read_unlock()
{
  int error = pthread_rdwr_runlock_np(&mutex_);

  if (error != 0) {
    CTHROW(string("pthread_rdwr_runlock_np: ") + strerror(error));

    return false;
  }

  read_locked_ = false;

  return true;
}

bool
CThreadReadWriteMutex::
write_unlock()
{
  int error = pthread_rdwr_wunlock_np(&mutex_);

  if (error != 0) {
    CTHROW(string("pthread_rdwr_wunlock_np: ") + strerror(error));

    return false;
  }

  write_locked_ = false;

  return true;
}

CThreadCondition::
CThreadCondition(const char *id) :
 id_(id)
{
  int error = pthread_cond_init(&cond_, getAttr());

  if (error != 0)
    CTHROW(string("pthread_cond_init: ") + strerror(error));

  resetAttr();
}

CThreadCondition::
~CThreadCondition()
{
  int error = pthread_cond_destroy(&cond_);

  if (error != 0)
    CTHROW(string("pthread_cond_destroy: ") + strerror(error));
}

bool
CThreadCondition::
wait(CThreadMutex &mutex)
{
  if (! mutex.locked())
    return false;

  int error = pthread_cond_wait(&cond_, mutex.getMutexP());

  if (error != 0) {
    CTHROW(string("pthread_cond_wait: ") + strerror(error));

    return false;
  }

  return true;
}

bool
CThreadCondition::
timed_wait(CThreadMutex &mutex, struct timespec *time)
{
  if (! mutex.locked())
    return false;

  int error = pthread_cond_timedwait(&cond_, mutex.getMutexP(), time);

  if (error != 0) {
    CTHROW(string("pthread_cond_wait: ") + strerror(error));

    return false;
  }

  return true;
}

bool
CThreadCondition::
signal()
{
  int error = pthread_cond_signal(&cond_);

  if (error != 0) {
    CTHROW(string("pthread_cond_signal: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThreadCondition::
broadcast()
{
  int error = pthread_cond_broadcast(&cond_);

  if (error != 0) {
    CTHROW(string("pthread_cond_broadcast: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThreadCondition::
getProcessShared(int *shared)
{
  int error = pthread_condattr_getpshared(getAttr(), shared);

  if (error != 0) {
    CTHROW(string("pthread_condattr_getpshared: ") + strerror(error));
    return false;
  }

  return true;
}

bool
CThreadCondition::
setProcessShared(int shared)
{
  int error = pthread_condattr_setpshared(getAttr(), shared);

  if (error != 0) {
    CTHROW(string("pthread_condattr_setpshared: ") + strerror(error));
    return false;
  }

  return true;
}

pthread_condattr_t *
CThreadCondition::
getAttr()
{
  if (! attr_inited_)
    resetAttr();

  return &attr_;
}

void
CThreadCondition::
resetAttr()
{
  pthread_condattr_init(&attr_);

  attr_inited_ = true;
}

bool
CThreadFile::
lock()
{
  flockfile(fp_);

  locked_ = true;

  return true;
}

bool
CThreadFile::
try_lock()
{
  int error = ftrylockfile(fp_);

  if (error != 0)
    return false;

  locked_ = true;

  return true;
}

bool
CThreadFile::
unlock()
{
  funlockfile(fp_);

  locked_ = false;

  return true;
}
