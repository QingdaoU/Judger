/*
Lua Binding:
    The MIT License (MIT)
    Copyright (c) <2016> <avaicode>

Judger:
    The Star And Thank Author License (SATA)
    Copyright (c) <Qingdao University Online Judge Dev Team> <info@qduoj.com>
*/

/*
Usage:
    In Lua: require "judger"
    In C: luaL_requiref(L, "judger", luaopen_judger, 1)
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include "../../src/runner.h"

typedef struct ConfItem {
    const char* name;
    int value;
} ConfItem;
static ConfItem conf_item[] = {
    {"_VERSION", VERSION},
    {"_UNLIMITED", UNLIMITED},
    {"_RESULT_WRONG_ANSWER", WRONG_ANSWER},
    {"_RESULT_SUCCESS", SUCCESS},
    {"_RESULT_CPU_TIME_LIMIT_EXCEEDED", CPU_TIME_LIMIT_EXCEEDED},
    {"_RESULT_REAL_TIME_LIMIT_EXCEEDED", REAL_TIME_LIMIT_EXCEEDED},
    {"_RESULT_MEMORY_LIMIT_EXCEEDED", MEMORY_LIMIT_EXCEEDED},
    {"_RESULT_RUNTIME_ERROR", RUNTIME_ERROR},
    {"_RESULT_SYSTEM_ERROR", SYSTEM_ERROR},
    {"_ERROR_INVALID_CONFIG", INVALID_CONFIG},
    {"_ERROR_FORK_FAILED", FORK_FAILED},

    {"_ERROR_PTHREAD_FAILED", PTHREAD_FAILED},
    {"_ERROR_WAIT_FAILED", WAIT_FAILED},
    {"_ERROR_ROOT_REQUIRED", ROOT_REQUIRED},
    {"_ERROR_LOAD_SECCOMP_FAILED", LOAD_SECCOMP_FAILED},
    {"_ERROR_SETRLIMIT_FAILED", SETRLIMIT_FAILED},
    {"_ERROR_DUP2_FAILED", DUP2_FAILED},
    {"_ERROR_SETUID_FAILED", SETUID_FAILED},
    {"_ERROR_EXECVE_FAILED", EXECVE_FAILED},
    {"_ERROR_SPJ_ERROR", SPJ_ERROR}
};
static const int conf_item_size = sizeof(conf_item) / sizeof(ConfItem);

typedef struct ArgMask {
    const char* name;
    int t;  //Should be lua_type
    int nil_accept;
} ArgMask;
static ArgMask arg_mask[] = {
    {"max_cpu_time", LUA_TNUMBER, 0},
    {"max_real_time", LUA_TNUMBER, 0},
    {"max_memory", LUA_TNUMBER, 0},
    {"max_process_number", LUA_TNUMBER, 0},
    {"max_output_size", LUA_TNUMBER, 0},
    {"exe_path", LUA_TSTRING, 0},
    {"input_path", LUA_TSTRING, 0},
    {"output_path", LUA_TSTRING, 0},
    {"error_path", LUA_TSTRING, 0},
    {"log_path", LUA_TSTRING, 0},
    {"seccomp_rule_name", LUA_TSTRING, 1},
    {"args", LUA_TTABLE, 0},
    {"env", LUA_TTABLE, 0},
    {"uid", LUA_TNUMBER, 0},
    {"gid", LUA_TNUMBER, 0}
};
static const int arg_mask_size = sizeof(arg_mask) / sizeof(ArgMask);

static int global_judge(lua_State* L);

static const struct luaL_Reg judger[] = {
    {"run", global_judge},
    {NULL, NULL}
};

static char* _strdup(const char* s) {
    char* t = NULL;
    if (s && (t = (char*)malloc(strlen(s) + 1)))
        strcpy(t, s);
    return t;
}

static void destory_config(struct config* config) {
    int i = 0;

    free(config->exe_path);
    free(config->input_path);
    free(config->output_path);
    free(config->error_path);
    free(config->log_path);
    free(config->seccomp_rule_name);

    for (; i < 256; ++i) {
        free(config->args[i]);
        free(config->env[i]);
    }
}

static int lua_checkstringtable(lua_State* L, char** dst, int max_size, int* dst_size) {
    int table_index = lua_gettop(L);
    int table_size = 0;

    lua_pushnil(L);
    while (0 != lua_next(L, table_index)) {
        if (lua_type(L, -1) != LUA_TSTRING)
            return 0;
        if (lua_type(L, -2) != LUA_TNUMBER)
            return 0;

        if (table_size == max_size)
            return 0;

        dst[table_size] = _strdup(lua_tostring(L, -1));
        table_size++;

        lua_pop(L, 1);
    }

    if (dst_size != NULL)
        *dst_size = table_size;

    return 1;
}

static int global_judge(lua_State* L) {
    int i = 0, table_size = 0;
    struct config config;
    struct result result;
    void* arg_refs[arg_mask_size];

    arg_refs[0]  = &config.max_cpu_time;
    arg_refs[1]  = &config.max_real_time;
    arg_refs[2]  = &config.max_memory;
    arg_refs[3]  = &config.max_process_number;
    arg_refs[4]  = &config.max_output_size;
    arg_refs[5]  = &config.exe_path;
    arg_refs[6]  = &config.input_path;
    arg_refs[7]  = &config.output_path;
    arg_refs[8]  = &config.error_path;
    arg_refs[9]  = &config.log_path;
    arg_refs[10] = &config.seccomp_rule_name;
    arg_refs[11] = &config.args;
    arg_refs[12] = &config.env;
    arg_refs[13] = &config.uid;
    arg_refs[14] = &config.gid;
    memset(&config, 0, sizeof(config));

    luaL_checktype(L, 1, LUA_TTABLE);
    for (; i < arg_mask_size; ++i) {
        lua_pushstring(L, arg_mask[i].name);
        lua_gettable(L, -2);
        if (lua_type(L, -1) != arg_mask[i].t && !(arg_mask[i].nil_accept && lua_type(L, -1) == LUA_TNIL))
            return destory_config(&config), luaL_error(L, "bad argument %s", arg_mask[i].name);


        switch (lua_type(L, -1)) {
        case LUA_TNUMBER:
            *(int*)arg_refs[i] = lua_tointeger(L, -1);
            break;
        case LUA_TSTRING:
            *(char**)arg_refs[i] = _strdup(lua_tostring(L, -1));
            break;
        case LUA_TTABLE:
            if (lua_checkstringtable(L, (char**)arg_refs[i], 256 - 1, &table_size) == 0) {
                destory_config(&config);
                return luaL_error(L, "bad argument %s (item must be a string)", arg_mask[i].name);
            }
            *((char**)arg_refs[i] + table_size) = NULL;
            break;
        case LUA_TNIL:
            *(char**)arg_refs[i] = NULL;
            break;
        }

        lua_pop(L, 1);
    }

    run(&config, &result);

    lua_newtable(L);
    lua_pushstring(L, "cpu_time"); lua_pushnumber(L, result.cpu_time); lua_rawset(L, -3);
    lua_pushstring(L, "real_time"); lua_pushnumber(L, result.real_time); lua_rawset(L, -3);
    lua_pushstring(L, "memory"); lua_pushnumber(L, result.memory); lua_rawset(L, -3);
    lua_pushstring(L, "signal"); lua_pushnumber(L, result.signal); lua_rawset(L, -3);
    lua_pushstring(L, "exit_code"); lua_pushnumber(L, result.exit_code); lua_rawset(L, -3);
    lua_pushstring(L, "error"); lua_pushnumber(L, result.error); lua_rawset(L, -3);
    lua_pushstring(L, "result"); lua_pushnumber(L, result.result); lua_rawset(L, -3);
    assert(lua_gettop(L) == 2);

    destory_config(&config);
    return 1;
}


int luaopen_judger(lua_State* L) {
    int i = 0;

#ifdef LUA51
    luaL_register(L, "judger", judger);
#else
    luaL_newlib(L, judger);
#endif

    for (; i < conf_item_size; ++i) {
        lua_pushstring(L, conf_item[i].name);
        lua_pushnumber(L, conf_item[i].value);
        lua_rawset(L, -3);
    }

    return 1;
}
