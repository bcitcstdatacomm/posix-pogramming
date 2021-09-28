#include <dc_posix/dc_signal.h>
#include <stdio.h>
#include <stdlib.h>


struct sig_info
{
    int sig;
    const char *name;
};


static void error_reporter(const struct dc_error *err);
static void trace_reporter(const struct dc_posix_env *env, const char *file_name,
                           const char *function_name, size_t line_number);
static void set_signal_handler(struct dc_posix_env *env, struct dc_error *err, const struct sig_info *signal_info);
static void print_handler(int sig_num);


int main(void)
{
    dc_posix_tracer tracer;
    dc_error_reporter reporter;
    struct dc_error err;
    struct dc_posix_env env;
    struct sigaction query_action;
    struct sig_info signals[] =
    {
        { SIGFPE, "SIGFPE" },
        { SIGILL, "SIGILL" },
        { SIGSEGV, "SIGSEGV" },
        { SIGBUS, "SIGBUS" },
        { SIGABRT, "SIGABRT" },
        { SIGTRAP, "SIGTRAP" },
        { SIGTERM, "SIGTERM" },
//        { SIGKILL, "SIGKILL" },
        { SIGINT, "SIGINT" },
        { SIGQUIT, "SIGQUIT" },
        { SIGHUP, "SIGHUP" },
        { SIGALRM, "SIGALRM" },
        { SIGVTALRM, "SIGVTALRM" },
        { SIGURG, "SIGURG" },
        { SIGCHLD, "SIGCHLD" },
//        { SIGSTOP, "SIGSTOP" },
        { SIGCONT, "SIGCONT" },
        { SIGTTIN, "SIGTTIN" },
        { SIGTTOU, "SIGTTOU" },
        { SIGPIPE, "SIGPIPE" },
        { SIGXCPU, "SIGXCPU" },
        { SIGXFSZ, "SIGXFSZ" },
        { SIGUSR1, "SIGUSR1" },
        { SIGUSR2, "SIGUSR2" },
    };
    size_t signals_count;

    reporter = error_reporter;
    tracer = trace_reporter;
    tracer = NULL;
    dc_error_init(&err, reporter);
    dc_posix_env_init(&env, tracer);

    signals_count = sizeof signals / sizeof signals[0];

    for(size_t i = 0; i < signals_count; i++)
    {
        set_signal_handler(&env, &err, &signals[i]);
    }

    for(size_t i = 0; i < signals_count; i++)
    {
        dc_raise(&env, &err, signals[i].sig);
    }

    return EXIT_SUCCESS;
}


static void set_signal_handler(struct dc_posix_env *env, struct dc_error *err, const struct sig_info *signal_info)
{
    struct sigaction old_action;

    dc_sigaction(env, err, signal_info->sig, NULL, &old_action);

    if(old_action.sa_handler != SIG_IGN)
    {
        struct sigaction new_action;

        new_action.sa_handler = print_handler;
        sigemptyset(&new_action.sa_mask);
        new_action.sa_flags = 0;
        dc_sigaction(env, err, signal_info->sig, &new_action, NULL);
    }
}


static void print_handler(int sig_num)
{
    printf("Got signal: %d\n", sig_num);
}

static void error_reporter(const struct dc_error *err)
{
    fprintf(stderr, "Error: \"%s\" - %s : %s @ %zu\n", err->message, err->file_name, err->function_name, err->line_number);
}

static void trace_reporter(const struct dc_posix_env *env, const char *file_name,
                           const char *function_name, size_t line_number)
{
    fprintf(stderr, "Entering: %s : %s @ %zu\n", file_name, function_name, line_number);
}
