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
static void display_signal_information(struct dc_posix_env *env, struct dc_error *err, const struct sig_info *signal_info);


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
        display_signal_information(&env, &err, &signals[i]);
    }

    return EXIT_SUCCESS;
}


static void display_signal_information(struct dc_posix_env *env, struct dc_error *err, const struct sig_info *signal_info)
{
    struct sigaction old_action;

    printf("%s = %d -> ", signal_info->name, signal_info->sig);
    dc_sigaction(env, err, signal_info->sig, NULL, &old_action);

    if(old_action.sa_handler == SIG_IGN)
    {
        printf("ignored");
    }
    else if(old_action.sa_handler == SIG_DFL)
    {
        printf("default handler");
    }
    else if(old_action.sa_handler == NULL)
    {
        printf("unhandled");
    }
    else
    {
        printf("custom handler");
    }

    printf("\n");
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
