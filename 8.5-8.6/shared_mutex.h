#ifndef SHARED_MUTEX_H
#define SHARED_MUTEX_H

#define _BSD_SOURCE // for ftruncate
#include <pthread.h> // pthread_mutex_t, pthread_mutexattr_t,
                     // pthread_mutexattr_init, pthread_mutexattr_setpshared,
                     // pthread_mutex_init, pthread_mutex_destroy

// Structure of a shared mutex.
typedef struct shared_mutex_t {
  pthread_mutex_t *ptr; // Pointer to the pthread mutex and
                        // shared memory segment.
  int shm_fd;           // Descriptor of shared memory object.
  char* name;           // Name of the mutex and associated
                        // shared memory object.
  int created;          // Equals 1 (true) if initialization
                        // of this structure caused creation
                        // of a new shared mutex.
                        // Equals 0 (false) if this mutex was
                        // just retrieved from shared memory.
} shared_mutex_t;

// Initialize a new shared mutex with given `name`. If a mutex
// with such name exists in the system, it will be loaded.
// Otherwise a new mutes will by created.
//
// In case of any error, it will be printed into the standard output
// and the returned structure will have `ptr` equal `NULL`.
// `errno` wil not be reset in such case, so you may used it.
//
// **NOTE:** In case when the mutex appears to be uncreated,
// this function becomes *non-thread-safe*. If multiple threads
// call it at one moment, there occur several race conditions,
// in which one call might recreate another's shared memory
// object or rewrite another's pthread mutex in the shared memory.
// There is no workaround currently, except to run first
// initialization only before multi-threaded or multi-process
// functionality.
shared_mutex_t shared_mutex_init(char *name);

// Close access to the shared mutex and free all the resources,
// used by the structure.
//
// Returns 0 in case of success. If any error occurs, it will be
// printed into the standard output and the function will return -1.
// `errno` wil not be reset in such case, so you may used it.
//
// **NOTE:** It will not destroy the mutex. The mutex would not
// only be available to other processes using it right now,
// but also to any process which might want to use it later on.
// For complete desctruction use `shared_mutex_destroy` instead.
//
// **NOTE:** It will not unlock locked mutex.
int shared_mutex_close(shared_mutex_t mutex);

// Close and destroy shared mutex.
// Any open pointers to it will be invalidated.
//
// Returns 0 in case of success. If any error occurs, it will be
// printed into the standard output and the function will return -1.
// `errno` wil not be reset in such case, so you may used it.
//
// **NOTE:** It will not unlock locked mutex.
int shared_mutex_destroy(shared_mutex_t mutex);



#include <errno.h> // errno, ENOENT
#include <fcntl.h> // O_RDWR, O_CREATE
#include <limits.h> // NAME_MAX
#include <sys/mman.h> // shm_open, shm_unlink, mmap, munmap,
                      // PROT_READ, PROT_WRITE, MAP_SHARED, MAP_FAILED
#include <unistd.h> // ftruncate, close
#include <stdio.h> // perror
#include <stdlib.h> // malloc, free
#include <string.h> // strcpy

shared_mutex_t shared_mutex_init(char *name) {
  shared_mutex_t mutex = {NULL, 0, NULL, 0};
  errno = 0;

  // Open existing shared memory object, or create one.
  // Two separate calls are needed here, to mark fact of creation
  // for later initialization of pthread mutex.
  mutex.shm_fd = shm_open(name, O_RDWR, 0660);
  if (errno == ENOENT) {
    mutex.shm_fd = shm_open(name, O_RDWR|O_CREAT, 0660);
    mutex.created = 1;
  }
  if (mutex.shm_fd == -1) {
    perror("shm_open");
    return mutex;
  }

  // Truncate shared memory segment so it would contain
  // pthread_mutex_t.
  if (ftruncate(mutex.shm_fd, sizeof(pthread_mutex_t)) != 0) {
    perror("ftruncate");
    return mutex;
  }

  // Map pthread mutex into the shared memory.
  void *addr = mmap(
    NULL,
    sizeof(pthread_mutex_t),
    PROT_READ|PROT_WRITE,
    MAP_SHARED,
    mutex.shm_fd,
    0
  );
  if (addr == MAP_FAILED) {
    perror("mmap");
    return mutex;
  }
  pthread_mutex_t *mutex_ptr = (pthread_mutex_t *)addr;

  // If shared memory was just initialized -
  // initialize the mutex as well.
  if (mutex.created) {
    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr)) {
      perror("pthread_mutexattr_init");
      return mutex;
    }
    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)) {
      perror("pthread_mutexattr_setpshared");
      return mutex;
    }
    if (pthread_mutex_init(mutex_ptr, &attr)) {
      perror("pthread_mutex_init");
      return mutex;
    }
  }
  mutex.ptr = mutex_ptr;
  mutex.name = (char *)malloc(NAME_MAX+1);
  strcpy(mutex.name, name);
  return mutex;
}

int shared_mutex_close(shared_mutex_t mutex) {
  if (munmap((void *)mutex.ptr, sizeof(pthread_mutex_t))) {
    perror("munmap");
    return -1;
  }
  mutex.ptr = NULL;
  if (close(mutex.shm_fd)) {
    perror("close");
    return -1;
  }
  mutex.shm_fd = 0;
  free(mutex.name);
  return 0;
}

int shared_mutex_destroy(shared_mutex_t mutex) {
  if ((errno = pthread_mutex_destroy(mutex.ptr))) {
    perror("pthread_mutex_destroy");
    return -1;
  }
  if (munmap((void *)mutex.ptr, sizeof(pthread_mutex_t))) {
    perror("munmap");
    return -1;
  }
  mutex.ptr = NULL;
  if (close(mutex.shm_fd)) {
    perror("close");
    return -1;
  }
  mutex.shm_fd = 0;
  if (shm_unlink(mutex.name)) {
    perror("shm_unlink");
    return -1;
  }
  free(mutex.name);
  return 0;
}
#endif // SHARED_MUTEX_H