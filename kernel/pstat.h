#ifndef _PSTAT_H_
#define _PSTAT_H_

//Process states shared by kernel and user programs.
enum procstate{
  UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE
};

//PA1: CPU usage information.
struct rusage{
  uint cputime;
};

//PA2: Process information for the ps command.
struct pstat{
  int pid;
  enum procstate state;
  uint64 size;
  int ppid;
  char name[16];
};

#endif
