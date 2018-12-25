#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
//#include <linux/sem.h>
#include <sys/wait.h>

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

void del_semvalue(int sem_id,int index)
{
  union semun sem_union;
  semctl(sem_id,index,IPC_RMID,sem_union);
}


int main(void)
{
  struct shmid_ds buf;
  int key = ftok("/",0);
  int sem_id = semget((key_t)key,2,0666|IPC_CREAT);
  int shm_id = shmget((key_t)key,100,IPC_CREAT|0666);
  printf("key = %d, sem_id = %d, shm_id = %d \n",key,sem_id,shm_id);

  setSemValue(sem_id,0,10);
  setSemValue(sem_id,1,0);

  pid_t pid[2];
  int i = 0;
  for(i = 0;i<2;i++)
  {
    if((pid[i] = fork())==0)
    break;
  }

  if(pid[0] > 0 && pid[1]>0)
  {
    wait(&pid[0]);
    wait(&pid[1]);
    shmctl(shm_id,IPC_RMID,&buf);
    del_semvalue(sem_id,0);
    del_semvalue(sem_id,1);

    printf("\nI am the father pid = %d\nsuccessd!",getpid());
    return 0;
  }
  else if(i==1)
  {
    execlp("./shm_w",NULL);
    printf("error i = 1");
  }
  else if(i == 0)
  {
    execlp("./shm_r",NULL);
    printf("error i = 2");
  }
}
