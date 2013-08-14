#include <CThreadPool.h>
#include <CThread.h>
#include <CFuncs.h>
#include <algorithm>

CThreadPool::
CThreadPool(unsigned int num_threads, unsigned int max_num_procs, bool do_not_block) :
 max_num_procs_(max_num_procs), do_not_block_(do_not_block),
 in_destroy_(false), shutdown_(false)
{
  for (unsigned int i = 0; i < num_threads; ++i) {
    CThread *thread = new CThread;

    thread->start(&CThreadPool::process, (void *) this);

    threads_.push_back(thread);
  }
}

CThreadPool::
~CThreadPool()
{
  for_each(threads_.begin(), threads_.end(), CDeletePointer());
}

bool
CThreadPool::
addThread(CThreadProc proc, void *data)
{
  // TODO: cleanup

  CThreadPoolCProc *cproc = new CThreadPoolCProc(proc, data);

  return addThread(cproc);
}

bool
CThreadPool::
addThread(CThreadPoolProc *proc)
{
  mutex_.lock();

  unsigned int num_procs = procs_.size();

  /* no space and this caller doesn't want to wait */

  if (num_procs == max_num_procs_ && do_not_block_) {
    mutex_.unlock();
    return false;
  }

  /* wait for free space */

  while (num_procs == max_num_procs_ && ! isDead()) {
    not_full_cond_.wait(mutex_);

    num_procs = procs_.size();
  }

  /* the pool is in the process of being destroyed */

  if (isDead()) {
    mutex_.unlock();
    return false;
  }

  /* add pool proc */

  procs_.push_back(proc);

  if (procs_.size() == 1)
    not_empty_cond_.broadcast();

  mutex_.unlock();

  return true;
}

bool
CThreadPool::
destroy(bool wait)
{
  mutex_.lock();

  /* Is a shutdown_ already in progress? */

  if (isDead()) {
    mutex_.unlock();
    return false;
  }

  in_destroy_ = true;

  /* If the wait flag is set, wait for procs to drain queue */

  if (wait) {
    unsigned int num_procs = procs_.size();

    while (num_procs > 0) {
      empty_cond_.wait(mutex_);

      num_procs = procs_.size();
    }
  }

  shutdown_ = true;

  mutex_.unlock();

  /* Wake up any procs so they recheck shutdown flag */

  not_empty_cond_.broadcast();
  not_full_cond_ .broadcast();

  /* Wait for procs to exit */

  unsigned int num_threads = threads_.size();

  for (unsigned int i = 0; i < num_threads; ++i)
    threads_[i]->join(NULL);

  /* Now free pool structures */

  return true;
}

void *
CThreadPool::
process(void *data)
{
  CThreadPool *pool = (CThreadPool *) data;

  for (;;)
    pool->processNext();

  return NULL;
}

bool
CThreadPool::
processNext()
{
  mutex_.lock();

  /* Wait for procs to become available */

  unsigned int num_procs = procs_.size();

  while (num_procs == 0 && ! shutdown_) {
    not_empty_cond_.wait(mutex_);

    num_procs = procs_.size();
  }

  /* Has a shutdown started while i was sleeping? */

  if (shutdown_) {
    mutex_.unlock();

    CThread::exit(NULL);
  }

  /* Get to work, dequeue the next item */

  CThreadPoolProc *proc = procs_.back();

  procs_.pop_back();

  /* Handle waiting add_work threads */

  if (! do_not_block_) {
    num_procs = procs_.size();

    if (num_procs == (max_num_procs_ - 1))
      not_full_cond_.broadcast();
  }

  /* Handle waiting destroyer threads */

  num_procs = procs_.size();

  if (num_procs == 0)
    empty_cond_.signal();

  mutex_.unlock();

  /* Do this work item */

  proc->execute();

  return true;
}

bool
CThreadPool::
isDead() const
{
  return (shutdown_ || in_destroy_);
}
