#include <dc_posix/dc_netdb.h>
#include <dc_posix/dc_posix_env.h>
#include <dc_posix/dc_unistd.h>
#include <dc_posix/dc_signal.h>
#include <dc_posix/dc_string.h>
#include <dc_posix/sys/dc_socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>


static void error_reporter(const struct dc_error *err);
static void trace_reporter(const struct dc_posix_env *env, const char *file_name,
                           const char *function_name, size_t line_number);


int main(int argc, char *argv[])
{
    dc_error_reporter reporter;
    dc_posix_tracer tracer;
    struct dc_error err;
    struct dc_posix_env env;
    const char *host_name;
    struct addrinfo hints;
    struct addrinfo *result;

    reporter = error_reporter;
    tracer = trace_reporter;
    tracer = NULL;
    dc_error_init(&err, reporter);
    dc_posix_env_init(&env, tracer);

    host_name = argv[1];
    dc_memset(&env, &hints, 0, sizeof(hints));
    hints.ai_family =  PF_INET; // PF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
//    hints.ai_flags = AI_CANONNAME;
    dc_getaddrinfo(&env, &err, host_name, NULL, &hints, &result);

    if(dc_error_has_no_error(&err))
    {
        int socket_fd;

        socket_fd = dc_socket(&env, &err, result->ai_family, result->ai_socktype, result->ai_protocol);

        if(dc_error_has_no_error(&err))
        {
            struct sockaddr *sockaddr;
            in_port_t port;
            in_port_t converted_port;
            socklen_t sockaddr_size;

            sockaddr = result->ai_addr;
            port = 80;
            converted_port = htons(port);

            if(sockaddr->sa_family == AF_INET)
            {
                struct sockaddr_in *addr_in;

                addr_in = (struct sockaddr_in *)sockaddr;
                addr_in->sin_port = converted_port;
                sockaddr_size = sizeof(struct sockaddr_in);
            }
            else
            {
                if(sockaddr->sa_family == AF_INET6)
                {
                    struct sockaddr_in6 *addr_in;

                    addr_in = (struct sockaddr_in6 *)sockaddr;
                    addr_in->sin6_port = converted_port;
                    sockaddr_size = sizeof(struct sockaddr_in6);
                }
                else
                {
                    DC_ERROR_RAISE_USER(&err, "sockaddr->sa_family is invalid", -1);
                    sockaddr_size = 0;
                }
            }

            if(dc_error_has_no_error(&err))
            {
                dc_connect(&env, &err, socket_fd, sockaddr, sockaddr_size);

                if(dc_error_has_no_error(&err))
                {
                    dc_getsockname(&env, &err, socket_fd, sockaddr, &sockaddr_size);

                    if(dc_error_has_no_error(&err))
                    {
                        if(sockaddr->sa_family == AF_INET)
                        {
                            struct sockaddr_in *addr;
                            char ip_address[INET_ADDRSTRLEN + 1];

                            addr = (struct sockaddr_in *)sockaddr;
                            inet_ntop(AF_INET, &addr->sin_addr, ip_address, INET_ADDRSTRLEN);
                            printf("\tIPv4 Address %s\n", ip_address);
                        }
                        else if(result->ai_addr->sa_family == AF_INET6)
                        {
                            struct sockaddr_in6 *addr;
                            char ip_address[INET6_ADDRSTRLEN + 1];

                            addr = (struct sockaddr_in6 *)sockaddr;
                            inet_ntop(AF_INET6, &addr->sin6_addr, ip_address, INET6_ADDRSTRLEN);
                            printf("\tIPv6 Address  %s\n", ip_address);
                        }
                    }
                }
            }
        }

        if(dc_error_has_no_error(&err))
        {
            dc_close(&env, &err, socket_fd);
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
