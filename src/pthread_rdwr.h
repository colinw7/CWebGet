#ifndef PTHREAD_RDWR_H
#define PTHREAD_RDWR_H

typedef struct pthread_rdwr_t pthread_rdwr_t;

struct pthread_rdwr_t {
  int             readers_reading;
  int             writer_writing;
  pthread_mutex_t mutex;
  pthread_cond_t  lock_free;
};

typedef void *pthread_rdwrattr_t;

#define pthread_rdwrattr_default NULL;

int pthread_rdwr_init_np   (pthread_rdwr_t *rdwrp, pthread_rdwrattr_t *attrp);
int pthread_rdwr_destroy_np(pthread_rdwr_t *rdwrp);
int pthread_rdwr_rlock_np  (pthread_rdwr_t *rdwrp);
int pthread_rdwr_runlock_np(pthread_rdwr_t *rdwrp);
int pthread_rdwr_wlock_np  (pthread_rdwr_t *rdwrp);
int pthread_rdwr_wunlock_np(pthread_rdwr_t *rdwrp);

#endif
