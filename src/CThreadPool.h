#ifndef CTHREAD_POOL_H
#define CTHREAD_POOL_H

#include <CThread.h>

class CThreadPoolProc {
 public:
  CThreadPoolProc() { }

  virtual ~CThreadPoolProc() { }

  virtual void *execute() = 0;
};

class CThreadPool {
 public:
  CThreadPool(unsigned int num_threads, unsigned int max_num_workers, bool do_not_block);
 ~CThreadPool();

  bool addThread(CThreadPoolProc *proc);
  bool addThread(CThreadProc proc, void *data);

  bool destroy(bool wait);

  static void *process(void *data);

  bool processNext();

  bool isDead() const;

 private:
  unsigned int                   max_num_procs_;
  bool                           do_not_block_;
  bool                           in_destroy_;
  bool                           shutdown_;
  CThreadMutex                   mutex_;
  CThreadCondition               not_empty_cond_; // signal when queue not empty
  CThreadCondition               not_full_cond_;  // signal when queue not full
  CThreadCondition               empty_cond_;     // signal when queue is empty
  std::vector<CThread *>         threads_;
  std::vector<CThreadPoolProc *> procs_;
};

class CThreadPoolCProc : public CThreadPoolProc {
 public:
  CThreadPoolCProc(CThreadProc proc, void *data) :
   proc_(proc), data_(data) {
  }

  void *execute() { return (*proc_)(data_); }

 private:
  CThreadProc  proc_;
  void        *data_;
};

#endif
