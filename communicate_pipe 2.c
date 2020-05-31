//Created by litao in 2020 0518
//This program make child processes have communication with thier parents process by pipe

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>

#define Maxsize 100000
#define maxsize 25000

double sum(double *Num, int size);


int main()
{
  //signal and srand
  //signal(SIGCHLD,SIG_IGN);
  srand(time(NULL));

  //variable
  int fd_pipes[2];
  double Num[Maxsize+1];
  double _2500Num[5];
  double SumBuffer[1];

  //Get the randomly Nums
  for(int j=0; j<Maxsize; j++)
    {
      Num[j]=(double)rand();
    }

  //create the pipe
  if(pipe(fd_pipes)<0){
    perror("Create pipe");
    exit(-1);
  } 

     
  // fork the four child processes
  //chilren processes start
  pid_t pid;
  for(int i=1; i<5; i++){
    if((pid=fork())==0){
       double _2500Sum=sum(Num, i);
       printf("%dChild_%d sum: %f\n", getpid(), i, _2500Sum);
       SumBuffer[0]=_2500Sum;
       int data =write(fd_pipes[1], SumBuffer, sizeof(SumBuffer)); // write data to pipe
       return 0;
    }
   }
//children processes end

  //wait the child processes finished
  sleep(5);
  //read the child data form the pipe
  int data=read(fd_pipes[0], _2500Num, sizeof(SumBuffer)*4);
  printf("%d Parent direct sum: %f, sum from children: %f\n", getpid(), sum(Num, 0), sum(_2500Num,5));
  
  return 0;
}

//sum
double sum(double *Num, int size)
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
   sum+=Num[i];
   }
 
  return sum;    
}
