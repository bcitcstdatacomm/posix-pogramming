#include <dc_posix/net/dc_if.h>
#include <dc_posix/dc_posix_env.h>
#include <stdio.h>
#include <stdlib.h>


static void error_reporter(const struct dc_error *err);
static void trace_reporter(const struct dc_posix_env *env, const char *file_name,
                           const char *function_name, size_t line_number);


int main(int argc, char *argv[])
{
    dc_error_reporter reporter;
    dc_posix_tracer tracer;
    struct dc_error err;
    struct dc_posix_env env;
    struct if_nameindex *interfaces;

    reporter = error_reporter;
    tracer = trace_reporter;
    tracer = NULL;
    dc_error_init(&err, reporter);
    dc_posix_env_init(&env, tracer);
    interfaces = dc_if_nameindex(&env, &err);

    if(dc_error_has_no_error(&err))
    {
        for(size_t i = 0; interfaces[i].if_name != NULL; i++)
        {
            printf("%u - %s\n", interfaces[i].if_index, interfaces[i].if_name);
        }

        dc_if_freenameindex(&env, interfaces);
    }

    return EXIT_SUCCESS;
}

static void error_reporter(const struct dc_error *err)
{
    fprintf(stderr, "Error: \"%s\" - %s : %s : %d @ %zu\n", err->message, err->file_name, err->function_name, err->errno_code, err->line_number);
}

static void trace_reporter(const struct dc_posix_env *env, const char *file_name,
                           const char *function_name, size_t line_number)
{
    fprintf(stderr, "Entering: %s : %s @ %zu\n", file_name, function_name, line_number);
}
