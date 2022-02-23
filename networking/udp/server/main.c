#include <dc_network/options.h>
#include <dc_posix/dc_netdb.h>
#include <dc_posix/dc_posix_env.h>
#include <dc_posix/dc_unistd.h>
#include <dc_posix/dc_signal.h>
#include <dc_posix/dc_string.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_posix/arpa/dc_inet.h>
#include <dc_posix/sys/dc_socket.h>
#include <dc_util/networking.h>
#include <dc_util/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


static void error_reporter(const struct dc_error *err);
static void trace_reporter(const struct dc_posix_env *env, const char *file_name,
                           const char *function_name, size_t line_number);
static void quit_handler(int sig_num);
void echo_data(struct dc_posix_env *env, struct dc_error *err, int in_fd, int out_fd, size_t size);


static volatile sig_atomic_t exit_flag;


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

    if(argc == 1)
    {
        host_name = "localhost";
    }
    else
    {
        host_name = argv[1];
    }

    dc_memset(&env, &hints, 0, sizeof(hints));
    hints.ai_family =  PF_INET; // PF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_CANONNAME;
    dc_getaddrinfo(&env, &err, host_name, NULL, &hints, &result);

    if(dc_error_has_no_error(&err))
    {
        int server_socket_fd;

        server_socket_fd = dc_socket(&env, &err, result->ai_family, result->ai_socktype, result->ai_protocol);

        if(dc_error_has_no_error(&err))
        {
            struct sockaddr *sockaddr;
            in_port_t port;
            in_port_t converted_port;
            socklen_t sockaddr_size;

            sockaddr = result->ai_addr;

            if(argc < 3)
            {
                port = 4981;
            }
            else
            {
                port = dc_uint16_from_str(&env, &err, argv[2], 10);
            }

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
                dc_network_opt_ip_so_reuse_addr(&env, &err, server_socket_fd, true);

                if(dc_error_has_no_error(&err))
                {
                    printf("binding to port %d\n", port);
                    dc_bind(&env, &err, server_socket_fd, sockaddr, sockaddr_size);

                    if(dc_error_has_no_error(&err))
                    {
                        struct sigaction old_action;

                        dc_sigaction(&env, &err, SIGINT, NULL, &old_action);

                        if(old_action.sa_handler != SIG_IGN)
                        {
                            struct sigaction new_action;

                            exit_flag = 0;
                            new_action.sa_handler = quit_handler;
                            sigemptyset(&new_action.sa_mask);
                            new_action.sa_flags = 0;
                            dc_sigaction(&env, &err, SIGINT, &new_action, NULL);

                            while(!(exit_flag) && dc_error_has_no_error(&err))
                            {
                                struct sockaddr *clientaddr;
                                socklen_t clientaddr_size;
                                uint16_t client_port;
                                char *client_ip_address;
                                char buffer[1024];

                                if(sockaddr->sa_family == AF_INET)
                                {
                                    clientaddr_size = sizeof(struct sockaddr_in);
                                }
                                else
                                {
                                    clientaddr_size = sizeof(struct sockaddr_in6);
                                }

                                clientaddr = dc_calloc(&env, &err, 1, clientaddr_size);
                                printf("accepting\n");
                                dc_recvfrom(&env, &err, server_socket_fd, buffer, 1024, MSG_WAITALL, clientaddr, &clientaddr_size);
                                client_ip_address = dc_inet_ntop_compat(&env, &err, clientaddr);
                                client_port = dc_inet_get_port(&env, &err, clientaddr);
                                printf("IP: %s port: %d\n", client_ip_address, ntohs(client_port));
                                free(clientaddr);
                            }
                        }
                    }
                }
            }
        }

        if(dc_error_has_no_error(&err))
        {
            dc_close(&env, &err, server_socket_fd);
        }
    }

    return EXIT_SUCCESS;
}

// Look at the code in the client, you could do the same thing
void echo_data(struct dc_posix_env *env, struct dc_error *err, int in_fd, int out_fd, size_t size)
{
    // more efficient would be to allocate the buffer in the caller (main) so we don't have to keep
    // mallocing and freeing the same data over and over again.
    char *data;
    ssize_t count;

    data = dc_malloc(env, err, size);

    while(!(exit_flag) && (count = dc_read(env, err, in_fd,data, size)) > 0 && dc_error_has_no_error(err))
    {
        dc_write(env, err, STDOUT_FILENO, data, (size_t)count);
        dc_write(env, err, out_fd, data, (size_t)count);

        if(data[count - 1] == '\n')
        {
            break;
        }
    }

    dc_free(env, data, size);
}

static void quit_handler(int sig_num)
{
    exit_flag = 1;
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
