#include "kernel/types.h"
#include "user/user.h"

#define NCHILD 2
#define BUSY_TICKS 120

struct result {
    int id;
    uint first;
};

int
main(void)
{
    int start[2], ready[2], results[2];
    int i, pid;
    int response[NCHILD];
    char signal;
    uint release_time;
    struct result r;

    printf("Interactive priority: %d\n", getpriority());
    printf("Competing CPU priority: 40\n");

    // Create synchronization pipes.
    if(pipe(start) < 0 ||
       pipe(ready) < 0 ||
       pipe(results) < 0) {
        printf("Pipe creation failed\n");
        exit(1);
    }

    for(i = 0; i < NCHILD; i++) {

        pid = fork();

        if(pid < 0) {
            printf("Fork failed\n");
            exit(1);
        }

        if(pid == 0) {

            close(start[1]);
            close(ready[0]);
            close(results[0]);

            // The CPU-intensive child has priority 40.
            // The interactive child inherits shell priority.
            if(i == 1) {
                if(setpriority(40) < 0)
                    exit(1);
            }

            // Tell the parent this child is ready.
            signal = 'R';
            if(write(ready[1], &signal, 1) != 1)
                exit(1);

            // Wait for the start signal.
            if(read(start[0], &signal, 1) != 1)
                exit(1);

            // Record first execution time.
            r.id = i;
            r.first = uptime();

            // Simulate a CPU-intensive workload.
            if(i == 1) {
                while((uint)(uptime() - r.first) < BUSY_TICKS) {
                    // Keep CPU busy.
                }
            }

            // Send timing information to parent.
            if(write(results[1], &r, sizeof(r))
               != sizeof(r))
                exit(1);

            close(start[0]);
            close(ready[1]);
            close(results[1]);

            exit(0);
        }
    }

    // Parent coordinates the experiment.
    close(start[0]);
    close(ready[1]);
    close(results[1]);

    // Wait until both children are ready.
    for(i = 0; i < NCHILD; i++) {
        if(read(ready[0], &signal, 1) != 1)
            exit(1);
    }

    // Record the common release time.
    release_time = uptime();

    if(write(start[1], "GO", NCHILD) != NCHILD)
        exit(1);

    close(start[1]);

    // Collect the results.
    for(i = 0; i < NCHILD; i++) {
        if(read(results[0], &r, sizeof(r))
           != sizeof(r))
            exit(1);

        if(r.id < 0 || r.id >= NCHILD)
            exit(1);

        response[r.id] = r.first - release_time;
    }

    // Wait for both children.
    for(i = 0; i < NCHILD; i++)
        wait(0);

    printf("\nResponse Time Results:\n");

    printf("Interactive process: %d ticks\n",
           response[0]);

    printf("CPU-intensive process: %d ticks\n",
           response[1]);

    close(ready[0]);
    close(results[0]);

    exit(0);
}

