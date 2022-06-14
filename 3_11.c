#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>
#include <pthread.h>
void t_func(void *data){
    int id;
    int i=0;
    pthread_t t_id;
    id=*(int *)data;
    printf("TGID(%d) ,PID(%ld), pthread_self(%ld) child\n",getpid(),syscall(__NR_gettid),pthread_self());
    sleep(2);
}
int main(){
    int a, b;
    int pid,status;
    pthread_t pthread;
    pid=pthread_create(&pthread,NULL,t_func,(void*)&a);

    pthread_join(&pthread,(void**)&status);
    
    printf("TGID(%ld) ,PID(%ld) PARENT\n",getpid(),syscall(__NR_gettid));
}