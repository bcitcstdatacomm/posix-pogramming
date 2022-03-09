#include <dc_posix/dc_posix_env.h>
#include <dc_posix/dc_string.h>
#include <dc_posix/dc_unistd.h>
#include <dc_posix/sys/dc_msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>


static void create_processes(struct dc_posix_env *env, struct dc_error *err, key_t key);
static void client(struct dc_posix_env *env, struct dc_error *err, int mqid);
static void server(struct dc_posix_env *env, struct dc_error *err, int mqid);

#define SIZE 100

struct message
{
    long mtype;
    char str[SIZE];
};

int main(int argc, char *argv[])
{
    dc_error_reporter reporter;
    dc_posix_tracer tracer;
    struct dc_error err;
    struct dc_posix_env env;
    key_t key;

    reporter = dc_error_default_error_reporter;
    tracer = dc_posix_default_tracer;
    tracer = NULL;
    dc_error_init(&err, reporter);
    dc_posix_env_init(&env, tracer);
    key = 1234;
    create_processes(&env, &err, key);
    dc_error_reset(&err);

    return EXIT_SUCCESS;
}

static void create_processes(struct dc_posix_env *env, struct dc_error *err, key_t key)
{
    int mqid;
    pid_t pid;

    DC_TRACE(env);

    mqid = dc_msgget(env, err, key, IPC_CREAT | S_IRUSR | S_IWUSR);
    pid = dc_fork(env, err);

    if(dc_error_has_error(err))
    {
    }
    else
    {
        if(pid == 0)
        {
            client(env, err, mqid);
        }
        else
        {
            server(env, err, mqid);
        }
    }
}

static void client(struct dc_posix_env *env, struct dc_error *err, int mqid)
{
    struct message msg;

    DC_TRACE(env);
    msg.mtype = 5;
    dc_strcpy(env, msg.str, "hello");
    dc_msgsnd(env, err, mqid, &msg, sizeof(msg), IPC_NOWAIT);
    msg.mtype = 11;
    dc_strcpy(env, msg.str, "world");
    dc_msgsnd(env, err, mqid, &msg, sizeof(msg), IPC_NOWAIT);
}

static void server(struct dc_posix_env *env, struct dc_error *err, int mqid) {
    struct message msg;

    DC_TRACE(env);

    for (;;)
    {
        dc_msgrcv(env, err, mqid, &msg, sizeof(msg), 11, 0);
        printf("got %ld %s\n", msg.mtype, msg.str);
    }
}
