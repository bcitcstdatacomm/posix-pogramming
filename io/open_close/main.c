#include <dc_posix/dc_fcntl.h>
#include <dc_posix/dc_unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define BUF_SIZE 1024


int main(void)
{
    struct dc_error     err;
    struct dc_posix_env env;
    int                 fd;
    char                chars[BUF_SIZE];
    int                 ret_val;

    dc_error_init(&err, NULL);
    dc_posix_env_init(&env, NULL);
    fd      = dc_open(&env, &err, "./makefile", DC_O_RDONLY, 0);
    ret_val = EXIT_SUCCESS;

    if(dc_error_has_no_error(&err))
    {
        ssize_t nread;

        while((nread = dc_read(&env, &err, fd, chars, BUF_SIZE)) > 0)
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

        dc_close(&env, &err, fd);
    }
    else
    {
        ret_val = 3;
    }

    return ret_val;
}
