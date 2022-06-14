#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>
int main(){
    int pid;
    
    if(fork()==0){
        printf("TGID(%d) PID(%d) child\n",getpid(),syscall(__NR_gettid));
    }else{
        printf("TGID(%d) PID(%d) parent\n",getpid(),syscall(__NR_gettid));
    }
}