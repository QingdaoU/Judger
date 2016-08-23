#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <python2.7/Python.h>
#include "../../src/runner.h"


#define RaiseValueError(msg)  {PyErr_SetString(PyExc_ValueError, msg); return NULL;}


static PyObject *judger_run(PyObject *self, PyObject *args, PyObject *kwargs) {
    struct config _config;
    struct result _result = {0, 0, 0, 0, 0, 0, 0};

    PyObject *args_list = NULL, *env_list = NULL, *rule_path = NULL, *args_iter = NULL, *env_iter = NULL, *next = NULL;

    int count = 0;

    static char *kwargs_list[] = {"max_cpu_time", "max_real_time", "max_memory",
                                  "max_process_number", "max_output_size",
                                  "exe_path", "input_path", "output_path",
                                  "error_path", "args", "env", "log_path",
                                  "seccomp_rule_so_path", "uid", "gid", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iililssssOOsOii", kwargs_list,
                                     &(_config.max_cpu_time), &(_config.max_real_time), &(_config.max_memory),
                                     &(_config.max_process_number), &(_config.max_output_size),
                                     &(_config.exe_path), &(_config.input_path), &(_config.output_path),
                                     &(_config.error_path), &args_list, &env_list, &(_config.log_path),
                                     &rule_path, &(_config.uid), &(_config.gid))) {
        RaiseValueError("Invalid args and kwargs");
    }

    if (!PyList_Check(args_list)) {
        RaiseValueError("args must be a list");
    }

    _config.args[count++] = _config.exe_path;
    args_iter = PyObject_GetIter(args_list);
    while (1) {
        next = PyIter_Next((args_iter));
        if (!next) {
            break;
        }
        if (!PyString_Check(next)) {
            RaiseValueError("arg in args must be a string");
        }
        _config.args[count] = PyString_AsString(next);
        count++;
    }

    _config.args[count] = NULL;

    count = 0;
    if (!PyList_Check(env_list)) {
        RaiseValueError("env must be a list");
    }
    env_iter = PyObject_GetIter(env_list);
    while (1) {
        next = PyIter_Next(env_iter);
        if (!next) {
            break;
        }
        if (!PyString_Check(next) && !PyUnicode_Check(next)) {
            RaiseValueError("env item must be a string");
        }
        _config.env[count] = PyString_AsString(next);
        count++;
    }
    _config.env[count] = NULL;
    if (PyString_Check(rule_path)) {
        _config.seccomp_rule_so_path = PyString_AsString(rule_path);
    }
    else {
        if (rule_path == Py_None) {
            _config.seccomp_rule_so_path = NULL;
        }
        else {
            RaiseValueError("seccomp_rule_so_path must be string or None");
        }
    }

    void *handler = dlopen("/usr/lib/judger/libjudger.so", RTLD_LAZY);
    int (*judger_run)(struct config *, struct result *);

    if (!handler) {
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
    PyModule_AddIntConstant(module, "UNLIMITED", UNLIMITED);
    PyModule_AddIntConstant(module, "SUCCESS", SUCCESS);
    PyModule_AddIntConstant(module, "CPU_TIME_LIMIT_EXCEEDED", CPU_TIME_LIMITED);
    PyModule_AddIntConstant(module, "REAL_TIME_LIMIT_EXCEEDED", REAL_TIME_LIMIT_EXCEEDED);
    PyModule_AddIntConstant(module, "MEMORY_LIMIT_EXCEEDED", MEMORY_LIMIT_EXCEEDED);
    PyModule_AddIntConstant(module, "RUNTIME_ERROR", RUNTIME_ERROR);
    PyModule_AddIntConstant(module, "SYSTEM_ERROR", SYSTEM_ERROR);
}
