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

    // Print the process table header.
    printf("pid\tstate\t\tsize\tpriority\tppid\tname\n");
    // Display information for each process.
    for (i = 0; i < nprocs; i++) {
        state = states[uproc[i].state];

        printf("%d\t%s\t%d\t%d\t\t%d\t%s\n",
               uproc[i].pid,
               state,
               (int)uproc[i].size,
               uproc[i].priority,
               uproc[i].ppid,
               uproc[i].name);
    }

    exit(0);
}

