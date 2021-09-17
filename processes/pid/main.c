#include <dc_posix/dc_unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    struct dc_posix_env env;
    pid_t               pid;
    pid_t               ppid;

    dc_posix_env_init(&env, NULL);
    pid = dc_getpid(&env);
    ppid = dc_getppid(&env);

    printf("pid        = %d\n", pid);
    printf("parent pid = %d\n", ppid);

    return EXIT_SUCCESS;
}
