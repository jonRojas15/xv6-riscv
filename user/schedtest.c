#include "kernel/types.h"
#include "user/user.h"

#define NTEST 3

int
main(void)
{
    int start[2], ready[2], results[2];
    int priorities[NTEST] = {5, 20, 40};
    char order[NTEST];
    char signal;
    int i, pid;

    // Create pipes to coordinate the processes.
    if(pipe(start) < 0 ||
       pipe(ready) < 0 ||
       pipe(results) < 0) {
        printf("Pipe creation failed\n");
        exit(1);
    }

    // Create three child processes.
    for(i = 0; i < NTEST; i++) {

        pid = fork();

        if(pid < 0) {
            printf("Fork failed\n");
            exit(1);
        }

        if(pid == 0) {

            close(start[1]);
            close(ready[0]);
            close(results[0]);

            // Assign a different priority to each child.
            if(setpriority(priorities[i]) < 0) {
                printf("Set priority failed\n");
                exit(1);
            }

            // Tell the parent this child is ready.
            signal = 'R';
            if(write(ready[1], &signal, 1) != 1)
                exit(1);

            // Wait until the parent releases all children.
            if(read(start[0], &signal, 1) != 1)
                exit(1);

            // Record the order in which this child runs.
            signal = 'A' + i;

            if(write(results[1], &signal, 1) != 1)
                exit(1);

            close(start[0]);
            close(ready[1]);
            close(results[1]);

            exit(0);
        }
    }

    // Parent process.
    close(start[0]);
    close(ready[1]);
    close(results[1]);

    // Wait until all three children report ready.
    for(i = 0; i < NTEST; i++) {
        if(read(ready[0], &signal, 1) != 1)
            exit(1);
    }

    printf("All three processes are ready.\n");
    printf("Priorities: 5, 20, 40\n");

    // Release all three children.
    if(write(start[1], "GO!", NTEST) != NTEST)
        exit(1);

    close(start[1]);

    // Collect the execution order.
    for(i = 0; i < NTEST; i++) {
        if(read(results[0], &order[i], 1) != 1)
            exit(1);
    }

    printf("Observed execution order: %d, %d, %d\n",
           priorities[order[0] - 'A'],
           priorities[order[1] - 'A'],
           priorities[order[2] - 'A']);

    // Wait for all children to finish.
    for(i = 0; i < NTEST; i++)
        wait(0);

    close(ready[0]);
    close(results[0]);

    exit(0);
}
