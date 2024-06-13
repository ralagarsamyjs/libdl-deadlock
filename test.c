#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *persistencestore_handle = NULL;
void *mutextest_handle = NULL;

void* load_library_persistence(void* arg) {
    const char* lib_name = (const char*) arg;
    printf("load_library_persistence()->PID<%d><%d> calling dlopen()\n", getpid(), gettid());

    // Open the shared library
    void* persistencestore_handle = dlopen(lib_name, RTLD_LAZY);
    if (!persistencestore_handle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return NULL;
    }
    printf("load_library_persistence()->PID<%d><%d> calling dlclose()\n", getpid(), gettid());
    
    // Clear any existing errors
    dlerror();
    
    dlclose(persistencestore_handle);
    persistencestore_handle = NULL;
    return NULL;
}

void* load_library_mutextest(const char* arg) {
    const char* lib_name = (const char*) arg;
    printf("load_library_mutextest()->PID<%d><%d> calling dlopen()\n", getpid(), gettid());

    // Open the shared library
    mutextest_handle = dlopen(lib_name, RTLD_LAZY);
    if (!mutextest_handle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return NULL;
    }
    printf("load_library_mutextest()->PID<%d><%d> calling dlclose()\n", getpid(), gettid());

    // Clear any existing errors
    dlerror();
    return NULL;
}

void* mutextest_access_symboltable(void *arg){
    char *error;
    void (*testing)();

    printf("mutextest_access_symboltable()->PID<%d><%d> entered \n", getpid(), gettid());

    *(void **) (&testing) = dlsym(mutextest_handle, "testing");
        // Check for any errors
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }
    printf("mutextest_access_symboltable()->PID<%d><%d>calling testing() \n", getpid(), gettid());
    // use the syymbol
    testing();
    dlclose(mutextest_handle);
    mutextest_handle = NULL;
    return NULL;
}


int main() {
    pthread_t thread1, thread2;
    const char* libpersistence_name = "libpersistencestore.so";
    const char* libmutextest_name = "libmutextest.so";

    printf("main:: calling load_library_mutextest(libmutextest_name) \n");
    load_library_mutextest(libmutextest_name);

    printf("main:: spawn threads ....\n");
    pthread_create(&thread1, NULL, load_library_persistence, (void*) libpersistence_name);
    sleep(2);
    pthread_create(&thread2, NULL, mutextest_access_symboltable, (void*) NULL);

    printf("main:: waiting for child threads to complete \n");
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
