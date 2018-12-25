#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>

union semun
{
  int val;
  struct semid_ds * buf;
  unsigned short * array;
  struct seminfo * _buf;
};

//define the opration P
void P(int sem_id,int index)
{
  struct sembuf sem;
  sem.sem_num = index;
  sem.sem_op = -1;
  sem.sem_flg = SEM_UNDO;
  semop(sem_id,&sem,1);
  return;
}

//define the opration V
void V(int sem_id,int index)
{
  struct sembuf sem;
  sem.sem_num = index;
  sem.sem_op = 1;
  sem.sem_flg = SEM_UNDO;
  semop(sem_id,&sem,1);
  return ;
}

//set the num of the special semaphore
void setSemValue(int sem_id,int index,int num)
{
  union semun sem_union;
  sem_union.val = num;
  semctl(sem_id,index,SETVAL,sem_union);
}

//create an shared memory
//create an semaphore
//define the opration P and the opration V
int main(void)
{
  //create an semaphore and shared memory
  int key = ftok("/",0);
  int sem_id = semget((key_t)key,2,0666|IPC_CREAT);
  int shm_id = shmget((key_t)key,100,IPC_CREAT|0666);
  printf(" i am the read childe , pid = %d , ppid = %d\nkey = %d, sem_id = %d, shm_id = %d \n",key,sem_id,shm_id,getpid(),getppid());
  char * smap = (char *)shmat(shm_id,NULL,0);
/*  for(int i = 0;i<100;i++)
  {
    P(sem_id,1);
    int index = i%10;
    printf("%d  ",smap[index]);
    V(sem_id,0);
  }*/

  FILE * f_read = fopen("input.txt","rb");
  fseek(f_read,0,SEEK_END);
  long length = ftell(f_read);
  fseek(f_read,0,SEEK_SET);
  fclose(f_read);
  printf(" i am the read childe ,%ld",length);
  FILE * f_write = fopen("output.txt","wb");
  for(int i = 0;i<length;i++)
  {
    P(sem_id,1);
    int index = i%10;
    fwrite(smap+index,sizeof(char),1,f_write);
    V(sem_id,0);
  }
  fclose(f_write);
  shmdt(smap);
  return 0;
}
