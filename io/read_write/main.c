#include <dc_posix/dc_unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define BUF_SIZE 1024


int main(void)
{
    struct dc_error     err;
    struct dc_posix_env env;
    char                chars[BUF_SIZE];
    ssize_t             nread;
    int                 ret_val;

    dc_error_init(&err, NULL);
    dc_posix_env_init(&env, NULL);
    ret_val = EXIT_SUCCESS;

    while((nread = dc_read(&env, &err, STDIN_FILENO, chars, BUF_SIZE)) > 0)
    {
        if(dc_error_has_error(&err))
        {
            ret_val = 1;
            break;
        }

        dc_write(&env, &err, STDOUT_FILENO, chars, (size_t)nread);

        if(dc_error_has_error(&err))
        {
            ret_val = 2;
            break;
        }
    }

    return ret_val;
}
