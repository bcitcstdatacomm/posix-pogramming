#include <dc_posix/dc_netdb.h>
#include <dc_posix/dc_posix_env.h>
#include <dc_posix/arpa/dc_inet.h>
#include <dc_posix/sys/dc_socket.h>
#include <stdio.h>
#include <stdlib.h>


static void error_reporter(const struct dc_error *err);
static void trace_reporter(const struct dc_posix_env *env, const char *file_name,
                           const char *function_name, size_t line_number);
static void quit_handler(int sig_num);


int main(int argc, char *argv[])
{
    dc_error_reporter reporter;
    dc_posix_tracer tracer;
    struct dc_error err;
    struct dc_posix_env env;
    const char *host_name;
    struct addrinfo *result;

    reporter = error_reporter;
    tracer = trace_reporter;
    tracer = NULL;
    dc_error_init(&err, reporter);
    dc_posix_env_init(&env, tracer);

    host_name = argv[1];
    dc_getaddrinfo(&env, &err, host_name, NULL, NULL, &result);

    if(dc_error_has_no_error(&err))
    {
        size_t i;

        i = 1;

        for(struct addrinfo *info = result; info != NULL; info = info->ai_next)
        {
            printf("%zu\n", i);
            printf("\tai_flags      %d\n", info->ai_flags);
            printf("\tai_family     %d\n", info->ai_family);
            printf("\tai_socktype   %d\n", info->ai_socktype);
            printf("\tai_protocol   %d\n", info->ai_protocol);
            printf("\tai_addrlen    %d\n", info->ai_addrlen);
            printf("\tsa_family     %d\n", info->ai_addr->sa_family);

            if(info->ai_addr->sa_family == AF_INET)
            {
                struct sockaddr_in *addr;
                char ip_address[INET_ADDRSTRLEN + 1];

                addr = (struct sockaddr_in *)info->ai_addr;
                dc_inet_ntop(&env, &err, AF_INET, &addr->sin_addr, ip_address, INET_ADDRSTRLEN);
                printf("\tIPv4 Address %s\n", ip_address);
            }
            else if(info->ai_addr->sa_family == AF_INET6)
            {
                struct sockaddr_in6 *addr;
                char ip_address[INET6_ADDRSTRLEN + 1];

                addr = (struct sockaddr_in6 *)info->ai_addr;
                dc_inet_ntop(&env, &err, AF_INET6, &addr->sin6_addr, ip_address, INET6_ADDRSTRLEN);
                printf("\tIPv6 Address  %s\n", ip_address);
                printf("\tsin6_flowinfo %d\n", addr->sin6_flowinfo);
                printf("\tsin6_scope_id %d\n", addr->sin6_scope_id);
            }
            else if(info->ai_addr->sa_family == AF_UNIX)
            {
                printf("\tUnix\n");
            }
            else if(info->ai_addr->sa_family == AF_UNSPEC)
            {
                printf("\tUnspecified\n");
            }
            else
            {
                printf("\tUnknown\n");
            }

            printf("\tai_canonname %s\n", info->ai_canonname);
        }

        dc_freeaddrinfo(&env, result);
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
