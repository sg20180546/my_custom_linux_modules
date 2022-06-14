#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/unistd.h>

void interrupt_handler(int sig){
    printf("TGID(%ld) PID(%ld) interrupt_handler\n",getpid(),syscall(__NR_gettid));
}

int main(){
    sigset_t s;
    signal(SIGINT,interrupt_handler);
     printf("TGID(%ld) PID(%ld) parent\n",getpid(),syscall(__NR_gettid));
    
    sigsuspend(&s);
}