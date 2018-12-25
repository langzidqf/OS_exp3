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
  char * smap;
  //create an semaphore and shared memory
  int key = ftok("/",0);
  int sem_id = semget((key_t)key,2,0666|IPC_CREAT);
  int shm_id = shmget((key_t)key,100,IPC_CREAT|0666);
  printf("i am the write child pid = %d, ppid = %d\nkey = %d, sem_id = %d, shm_id = %d \n, ",key,sem_id,shm_id,getpid(),getppid());

  //bind the shared memory to virtual space
  smap = (char *)shmat(shm_id,NULL,0);
/*  for(int i = 0;i<100;i++)
  {
    P(sem_id,0);
    int index = (i) % 10;
    smap[index] = i;
    V(sem_id,1);
  }*/
  FILE * f_read = fopen("input.txt","rb");
  fseek(f_read,0,SEEK_END);
  long length = ftell(f_read);
  fseek(f_read,0,SEEK_SET);
  for(int i = 0;i<length;i++)
  {
    P(sem_id,0);
    int index = i%10;
    fread(smap + index,sizeof(char),1,f_read);
    V(sem_id,1);
  }
  fclose(f_read);
  shmdt(smap);
  return 0;
}
