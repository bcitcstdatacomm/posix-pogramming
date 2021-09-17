#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


extern char **environ;


int main(void)
{
    char **envp;

    envp = environ;

    for(char *var; (var = *envp) != NULL; envp++)
    {
        printf("%s\n", var);
    }

    return EXIT_SUCCESS;
}
