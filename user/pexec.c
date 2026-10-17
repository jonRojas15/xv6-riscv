#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

#define MAXARGS 16

int
main(int argc, char *argv[])
{
    int i, rc;
    char *newargv[MAXARGS];
    int priority;

    // Check that a priority and command were provided.
    if(argc < 3 || argc > MAXARGS + 1){
        printf("Usage: pexec <priority> <command> [args...]\n");
        exit(1);
    }

    // Convert the priority argument into an integer.
    priority = atoi(argv[1]);

    // Set the priority of the current process.
    if(setpriority(priority) < 0){
        printf("pexec: invalid priority (use 0-49)\n");
        exit(1);
    }

    // Prepare the arguments for the new program.
    for(i = 2; i < argc; i++){
        newargv[i - 2] = argv[i];
    }

    newargv[argc - 2] = 0;

    // Create a child process.
    rc = fork();

    if(rc < 0){
        printf("pexec: fork failed\n");
        exit(1);
    }

    // Child executes the requested command.
    if(rc == 0){
        exec(newargv[0], newargv);

        printf("pexec: exec failed\n");
        exit(1);
    }

    // Parent waits for the child to finish.
    wait(0);

    exit(0);
}
