#include "kernel/types.h"
#include "user/user.h"

#define NCHILD 2
#define HIGH_TICKS 120

struct result {
    int id;
    uint first;
    uint finish;
};

int
main(void)
{
    int start[2], ready[2], results[2];
    int priorities[NCHILD] = {5, 40};
    struct result r, samples[NCHILD];
    char signal;
    int pid, i, order[NCHILD];
    uint release_time;

    if(pipe(start) < 0 ||
       pipe(ready) < 0 ||
       pipe(results) < 0){
        printf("agingtest: pipe failed\n");
        exit(1);
    }

    // Create low- and high-priority processes.
    for(i = 0; i < NCHILD; i++){

        pid = fork();

        if(pid < 0){
            printf("agingtest: fork failed\n");
            exit(1);
        }

        if(pid == 0){

            close(start[1]);
            close(ready[0]);
            close(results[0]);

            // Assign this child's priority.
            if(setpriority(priorities[i]) < 0)
                exit(1);

            // Inform the parent that we are ready.
            signal = 'R';
            if(write(ready[1], &signal, 1) != 1)
                exit(1);

            // Wait for the common start signal.
            if(read(start[0], &signal, 1) != 1)
                exit(1);

            r.id = i;
            r.first = uptime();

            // The high-priority process stays CPU-bound.
            if(i == 1){
                while((uint)(uptime() - r.first) < HIGH_TICKS){
                    // Busy waiting.
                }
            }

            r.finish = uptime();

            // Send timing results to the parent.
            if(write(results[1], &r, sizeof(r))
               != (int)sizeof(r))
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

    // Wait until both children are ready.
    for(i = 0; i < NCHILD; i++){
        if(read(ready[0], &signal, 1) != 1)
            exit(1);
    }

    printf("Both processes ready.\n");
    printf("Priorities: low=5, high=40\n");

    release_time = uptime();

    // Release both children.
    if(write(start[1], "GO", NCHILD) != NCHILD)
        exit(1);

    close(start[1]);

    // Collect results in completion order.
    for(i = 0; i < NCHILD; i++){
        if(read(results[0], &r, sizeof(r))
           != (int)sizeof(r))
            exit(1);

        if(r.id < 0 || r.id >= NCHILD)
            exit(1);

        order[i] = r.id;
        samples[r.id] = r;
    }

    for(i = 0; i < NCHILD; i++)
        wait(0);

    printf("\nResults (clock ticks):\n");

    for(i = 0; i < NCHILD; i++){
        printf("Priority %d: response=%d, turnaround=%d\n",
               priorities[i],
               (int)(samples[i].first - release_time),
               (int)(samples[i].finish - release_time));
    }

    printf("Completion order: %d, %d\n",
           priorities[order[0]],
           priorities[order[1]]);

    if(samples[0].first < samples[1].finish){
        printf("Low-priority process ran before high finished.\n");
    } else {
        printf("Low-priority process waited until high finished.\n");
    }

    close(ready[0]);
    close(results[0]);

    exit(0);
}
