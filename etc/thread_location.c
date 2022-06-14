#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
void* thread_start(void *arg){
    char buf[102400];
    memset(buf,0,sizeof(buf));
    strcpy(buf,"hello world!");
    printf("%s","Thread end\n");
    return NULL;
}


int main(int argc,char** argv){
    pthread_t thread[100];
    int i,cnt;
    cnt=atoi(argv[1]);
    for(i=0;i<cnt;i++){
        pthread_create(&thread[i],NULL,thread_start,NULL);
        sleep(1);
    }
    pause();
}


