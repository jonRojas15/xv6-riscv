#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
    struct pstat uproc[NPROC];
    int nprocs;
    int i;
    char *state;
    uint currenttime;

    static char *states[] = {
        [UNUSED]   = "unused",
        [USED]     = "used",
        [SLEEPING] = "sleeping",
        [RUNNABLE] = "runnable",
        [RUNNING]  = "running",
        [ZOMBIE]   = "zombie"
    };

    // Retrieve information about active processes.
    nprocs = getprocs(uproc);

    if (nprocs < 0) {
        printf("ps: getprocs failed\n");
        exit(1);
    }
    // PA2: Get the current system time in clock ticks.
    currenttime = uptime();

    // Print the process table header.
    printf("pid\tstate\t\tsize\tpriority\tage\tppid\tname\n");

    // PA2: Display process information and age.
    for (i = 0; i < nprocs; i++) {

        state = states[uproc[i].state];

        if (uproc[i].state == RUNNABLE) {

            // Calculate how long the process has been waiting.
            uint age = currenttime - uproc[i].readytime;

            printf("%d\t%s\t%d\t%d\t%d\t%d\t%s\n",
                   uproc[i].pid,
                   state,
                   (int)uproc[i].size,
                   uproc[i].priority,
                   age,
                   uproc[i].ppid,
                   uproc[i].name);

        } else {

            // Only RUNNABLE processes have a displayed age.
            printf("%d\t%s\t%d\t%d\t-\t%d\t%s\n",
                   uproc[i].pid,
                   state,
                   (int)uproc[i].size,
                   uproc[i].priority,
                   uproc[i].ppid,
                   uproc[i].name);
        }
    }

    exit(0);
}

