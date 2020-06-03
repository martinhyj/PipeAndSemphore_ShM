//Created by martin in 2020 0518
//

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include <sys/mman.h>   //共享内存
#include <semaphore.h>  //信号量
#include <fcntl.h>      //O_* 宏定义

#define Maxsize 100000    //The random float's num
#define maxsize 25000     //every child process' float's num

double sum(double *num, int size);  // figure the sum

int turn=1;


int main()
{
  //signal and srand
  //signal(SIGCHLD,SIG_IGN);
  srand(time(NULL));

  //variable
  double num[Maxsize+1];
  double numsum[5];
  int *wait_child;

  //Get the randomly nums
  for(int j=0; j<Maxsize; j++)
    {
      num[j]=(double)rand();
    }

  //Create semaphore,named "lock", value=1
  sem_t *semaph_lock_c=sem_open("lock",O_CREAT|O_RDWR,0666,1);

  //The children processes code start
  pid_t pid;
  for(int i=1; i<5; i++){
    if((pid=fork())==0){

      //open and map
       int shmem_fd_c=shm_open("data_buffer",O_CREAT | O_RDWR,0666);
       if(shmem_fd_c<0){
           perror("shm_open");
           exit(-1);
       }
       double *ptr=mmap(NULL, sizeof(double)*4+sizeof(int)*2, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd_c, 0);

       double sum41=sum(num, i);
       printf("%dChild_%d sum: %8f\n", getpid(), i, sum41);

       //write the data to share mermory, use the semaphore "lock" directly
       sleep(2);
       sem_wait(semaph_lock_c);    
       *(ptr+i-1)=sum41;
       sem_post(semaph_lock_c);

       //relieve map
       if(munmap(ptr,sizeof(double)*4+sizeof(int)*2)<0){
           perror("munmap");
       }     
//Question: use the code will error: shm_unlink: No such file or directory
//        if(shm_unlink("data_buffer")<0){
//           perror("shm_unlink");
//          exit(-1);
//         }
       
       return 0;
    }

   }
   //Children processes code end

  //Create the shared mermory space and map
  int shmem_fd =shm_open("data_buffer", O_RDWR | O_CREAT, 0666);
  ftruncate(shmem_fd, sizeof(double)*4+ sizeof(int)*2);
  double *ptr=mmap(NULL, sizeof(double)*4+sizeof(int)*2, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);

  //wait the child processes finished
  sleep(5);
  printf("%d Parent direct sum: %8f, sum from children: %8f\n", getpid(), sum(num, 0), sum(ptr,5));  

  //remove the share mermory and semaphore
  if(sem_unlink("lock")<0){
    perror("sem_unlink");
    exit(-1);
    }
  munmap(ptr,sizeof(double)*4+sizeof(int)*2);
  if(shm_unlink("data_buffer")<0) {
    perror("shm_unlink");
    exit(-1);
    }
  wait(wait_child);

  return 0;

}

//sum
double sum(double *num, int size)
{
  double sum=0;
  int i;

  if(size==0) i=0, size=100000;
  if(size==1) i=0, size=25000;
  if(size==2) i=25000, size=50000;
  if(size==3) i=50000, size=75000;
  if(size==4) i=75000, size=100000;
  if(size==5) i=0, size=4;
  for(i; i<size; i++)
  {
   sum+=num[i];
   }
 
  return sum;    
}
