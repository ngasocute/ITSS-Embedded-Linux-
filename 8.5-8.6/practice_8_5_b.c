#include "shared_mutex.h"
#include <stdio.h>

int cleanup(shared_mutex_t*mutex) {
  // Mutex destruction completely cleans it from system memory.
  if (shared_mutex_destroy(*mutex)) {
    return 1;
  }
  return 0;
}

main(){
    shared_mutex_t mutex = shared_mutex_init("/my-mutex");
    if (mutex.ptr == NULL) {
        return 1;
    }
    
    if (mutex.created) {
        printf("The mutex was just created\n");
    }
    
    printf("Waiting for unlock the mutex\n");
    pthread_mutex_lock(mutex.ptr);
    printf("the mutex is unlock\n");
    pthread_mutex_unlock(mutex.ptr);
    
    cleanup(&mutex);
}
