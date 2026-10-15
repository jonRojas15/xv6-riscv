#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
    int pid;

    // Test 1: Check initial priority.
    printf("Initial priority: %d\n", getpriority());

    // Test 2: Change priority to 12.
    if(setpriority(12) < 0){
        printf("Failed to set priority\n");
        exit(1);
    }

    printf("New priority: %d\n", getpriority());

    // Test 3: Check invalid priority values.
    printf("Set priority -1: %d\n", setpriority(-1));
    printf("Set priority 50: %d\n", setpriority(50));

    printf("Priority after invalid attempts: %d\n",
           getpriority());

    // Test 4: Check priority inheritance.
    pid = fork();

    if(pid < 0){
        printf("Fork failed\n");
        exit(1);
    }

    if(pid == 0){
        printf("Child inherited priority: %d\n",
               getpriority());

        // Run ps to display process priorities.
        char *args[] = {"ps", 0};
        exec("ps", args);

        printf("Failed to execute ps\n");
        exit(1);
    }

    wait(0);

    printf("Parent priority: %d\n", getpriority());

    exit(0);
}
