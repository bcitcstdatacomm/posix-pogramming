#include <dc_posix/dc_netdb.h>
#include <dc_posix/dc_posix_env.h>
#include <dc_posix/dc_fcntl.h>
#include <dc_posix/dc_ndbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static int store(const struct dc_posix_env *env, struct dc_error *err, DBM *db, const char *name, const char *phone_number, int type);
static datum fatch(const struct dc_posix_env *env, struct dc_error *err, DBM *db, const char *name);
static void display(const char *name, datum *content);
static void error_reporter(const struct dc_error *err);
static void trace_reporter(const struct dc_posix_env *env, const char *file_name,
                           const char *function_name, size_t line_number);


#define DB_NAME "phones"


int main(int argc, char *argv[])
{
    dc_error_reporter reporter;
    dc_posix_tracer tracer;
    struct dc_error err;
    struct dc_posix_env env;
    const char *host_name;
    struct addrinfo *result;
    DBM *db;
    int ret_code;
    datum content;

    reporter = error_reporter;
    tracer = trace_reporter;
    tracer = NULL;
    dc_error_init(&err, reporter);
    dc_posix_env_init(&env, tracer);

    // Open the database and store the record
    db       = dc_dbm_open(&env, &err, DB_NAME, DC_O_RDWR | DC_O_CREAT, 0600);
    ret_code = EXIT_SUCCESS;

    if(dc_error_has_no_error(&err))
    {
        store(&env, &err, db, "Foo", "123-4567", DBM_REPLACE);

        if(dc_error_has_error(&err))
        {
            goto close_db;
        }

        store(&env, &err, db, "Bar", "876-5432", DBM_REPLACE);

        if(dc_error_has_error(&err))
        {
            goto close_db;
        }

        store(&env, &err, db, "Bar", "111-1111", DBM_INSERT);

        if(dc_error_has_error(&err))
        {
            goto close_db;
        }

        content = fatch(&env, &err, db, "Foo");
        display("Foo", &content);
        content = fatch(&env, &err, db, "Bar");
        display("Bar", &content);
        content = fatch(&env, &err, db, "bar");
        display("bar", &content);

        close_db:
        dc_dbm_close(&env, &err, db);
    }
    else
    {
        fprintf(stderr, "Cannot crate database");
        ret_code = EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int store(const struct dc_posix_env *env, struct dc_error *err, DBM *db, const char *name, const char *phone_number, int type)
{
    int ret_val;

    datum key = {(void *)name, strlen(name) + 1};
    datum value = {(void *)phone_number, strlen(phone_number) + 1};
    ret_val = dc_dbm_store(env, err, db, key, value, type);

    return ret_val;
}

static datum fatch(const struct dc_posix_env *env, struct dc_error *err, DBM *db, const char *name)
{
    datum key = {(void *)name, strlen(name) + 1};
    datum content;

    content = dc_dbm_fetch(env, err, db, key);

    return content;
}

static void display(const char *name, datum *content)
{
    if(content->dsize > 0)
    {
        printf("%s: %s\n", name, (char *)content->dptr);
    }
    else
    {
        printf("%s: Not found\n", name);
    }
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
