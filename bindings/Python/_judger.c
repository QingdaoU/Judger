#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <python2.7/Python.h>
#include "../../src/runner.h"


#define RaiseValueError(msg)\
    { \
        PyErr_SetString(PyExc_ValueError, msg); \
        return NULL; \
    }


static PyObject *judger_run(PyObject *self, PyObject *args, PyObject *kwargs) {
    struct config _config;
    struct result _result = {0, 0, 0, 0, 0, 0, 0};

    PyObject *args_list, *env_list, *rule_name, *args_iter, *env_iter, *next;

    int count = 0, i = 0;

    static char *kwargs_list[] = {"max_cpu_time", "max_real_time", "max_memory",
                                  "max_process_number", "max_output_size",
                                  "exe_path", "input_path", "output_path",
                                  "error_path", "args", "env", "log_path",
                                  "seccomp_rule_name", "uid", "gid", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iililssssOOsOii", kwargs_list,
                                     &(_config.max_cpu_time), &(_config.max_real_time), &(_config.max_memory),
                                     &(_config.max_process_number), &(_config.max_output_size),
                                     &(_config.exe_path), &(_config.input_path), &(_config.output_path),
                                     &(_config.error_path), &args_list, &env_list, &(_config.log_path),
                                     &rule_name, &(_config.uid), &(_config.gid))) {
        RaiseValueError("Invalid args and kwargs");
    }

    if (!PyList_Check(args_list)) {
        // fixme decref
        RaiseValueError("args must be a list");
    }
    _config.args[count++] = _config.exe_path;
    args_iter = PyObject_GetIter(args_list);
    for(i = 0;i < ARGS_MAX_NUMBER;i++) {
        next = PyIter_Next(args_iter);
        if (!next) {
            break;
        }
        if (!PyString_Check(next)) {
            // Py_DECREF(next);
            RaiseValueError("arg item must be a string");
        }
        _config.args[count] = PyString_AsString(next);
        // Py_DECREF(next);
        count++;
    }
    _config.args[count] = NULL;
    // Py_DECREF(args_list);
    // Py_DECREF(args_iter);

    count = 0;

    if (!PyList_Check(env_list)) {
        // fixme decref
        RaiseValueError("env must be a list");
    }
    env_iter = PyObject_GetIter(env_list);
    for(i = 0;i < ENV_MAX_NUMBER; i++) {
        next = PyIter_Next(env_iter);
        if (!next) {
            break;
        }
        if (!PyString_Check(next)) {
            // Py_DECREF(next);
            RaiseValueError("env item must be a string");
        }
        _config.env[count] = PyString_AsString(next);
        // Py_DECREF(next);
        count++;
    }
    _config.env[count] = NULL;
    // Py_DECREF(env_list);
    // Py_DECREF(env_iter);
    
    if (PyString_Check(rule_name)) {
        _config.seccomp_rule_name = PyString_AsString(rule_name);
        // Py_DECREF(rule_path);
    }
    else {
        if (rule_name == Py_None) {
            // Py_DECREF(rule_path);
            _config.seccomp_rule_name = NULL;
        }
        else {
            // fixme decref
            RaiseValueError("seccomp_rule_name must be string or None");
        }
    }

    void *handler = dlopen("/usr/lib/judger/libjudger.so", RTLD_LAZY);
    int (*judger_run)(struct config *, struct result *);

    if (!handler) {
        // fixme decref
        RaiseValueError("dlopen error")
    }
    judger_run = dlsym(handler, "run");
    judger_run(&_config, &_result);

    return Py_BuildValue("{s:l, s:l, s:i, s:i, s:i, s:i, s:i}",
                         "cpu_time", _result.cpu_time,
                         "memory", _result.memory,
                         "real_time", _result.real_time,
                         "signal", _result.signal,
                         "exit_code", _result.exit_code,
                         "error", _result.error,
                         "result", _result.result);
}


static PyMethodDef judger_methods[] = {
    {"run", (PyCFunction) judger_run, METH_KEYWORDS, NULL},
    {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC init_judger(void) {
    PyObject *module = Py_InitModule3("_judger", judger_methods, NULL);
    PyModule_AddIntConstant(module, "VERSION", VERSION);
    PyModule_AddIntConstant(module, "UNLIMITED", UNLIMITED);
    PyModule_AddIntConstant(module, "RESULT_WRONG_ANSWER", WRONG_ANSWER);
    PyModule_AddIntConstant(module, "RESULT_SUCCESS", SUCCESS);
    PyModule_AddIntConstant(module, "RESULT_CPU_TIME_LIMIT_EXCEEDED", CPU_TIME_LIMIT_EXCEEDED);
    PyModule_AddIntConstant(module, "RESULT_REAL_TIME_LIMIT_EXCEEDED", REAL_TIME_LIMIT_EXCEEDED);
    PyModule_AddIntConstant(module, "RESULT_MEMORY_LIMIT_EXCEEDED", MEMORY_LIMIT_EXCEEDED);
    PyModule_AddIntConstant(module, "RESULT_RUNTIME_ERROR", RUNTIME_ERROR);
    PyModule_AddIntConstant(module, "RESULT_SYSTEM_ERROR", SYSTEM_ERROR);

    PyModule_AddIntConstant(module, "ERROR_INVALID_CONFIG", INVALID_CONFIG);
    PyModule_AddIntConstant(module, "ERROR_FORK_FAILED", FORK_FAILED);
    PyModule_AddIntConstant(module, "ERROR_PTHREAD_FAILED", PTHREAD_FAILED);
    PyModule_AddIntConstant(module, "ERROR_WAIT_FAILED", WAIT_FAILED);
    PyModule_AddIntConstant(module, "ERROR_ROOT_REQUIRED", ROOT_REQUIRED);
    PyModule_AddIntConstant(module, "ERROR_LOAD_SECCOMP_FAILED", LOAD_SECCOMP_FAILED);
    PyModule_AddIntConstant(module, "ERROR_SETRLIMIT_FAILED", SETRLIMIT_FAILED);
    PyModule_AddIntConstant(module, "ERROR_DUP2_FAILED", DUP2_FAILED);
    PyModule_AddIntConstant(module, "ERROR_SETUID_FAILED", SETUID_FAILED);
    PyModule_AddIntConstant(module, "ERROR_EXECVE_FAILED", EXECVE_FAILED);
    PyModule_AddIntConstant(module, "ERROR_SPJ_ERROR", SPJ_ERROR);
}
