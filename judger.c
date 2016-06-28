#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <python2.7/Python.h>
#include "runner.h"

#define RaiseValueError(msg)  {PyErr_SetString(PyExc_ValueError, msg); return NULL;}


static PyObject *judger_run(PyObject *self, PyObject *args, PyObject *kwargs) {
    struct config config;
    struct result result = {0, 0, 0, 0, 0, 0};
    PyObject *args_list = NULL, *env_list = NULL, *use_sandbox = NULL, *use_nobody = NULL,
            *next = NULL, *args_iter = NULL, *env_iter = NULL, *log_path = NULL;
    int count = 0;
    struct passwd *passwd;
    static char *kwargs_list[] = {"path", "in_file", "out_file", "max_cpu_time",
                                  "max_memory", "args", "env", "use_sandbox", "use_nobody", "log_path", NULL};

    config.path = config.in_file = config.out_file = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sssil|OOOOO", kwargs_list, &(config.path), &(config.in_file),
                                     &(config.out_file), &(config.max_cpu_time), &(config.max_memory),
                                     &args_list, &env_list, &use_sandbox, &use_nobody, &log_path)) {
        RaiseValueError("Invalid args and kwargs");
    }
    if (config.max_cpu_time < 1 && config.max_cpu_time != CPU_TIME_UNLIMITED) {
        RaiseValueError("max_cpu_time must > 1 ms or unlimited");
    }
    // by default, max_real_time = max_cpu_time * 3
    config.max_real_time = config.max_cpu_time * 3;
    if (config.max_memory < 16 * 1024 * 1024 && config.max_memory != MEMORY_UNLIMITED) {
        RaiseValueError("max_memory must > 16M or unlimited");
    }
    if (access(config.path, F_OK) == -1) {
        RaiseValueError("Exec file does not exist");
    }
    if (access(config.in_file, F_OK) == -1) {
        RaiseValueError("in_file does not exist");
    }
    config.err_file = config.out_file;
    config.args[count++] = config.path;
    if (args_list != NULL) {
        if (!PyList_Check(args_list)) {
            RaiseValueError("args must be a list");
        }

        args_iter = PyObject_GetIter(args_list);
        while (1) {
            next = PyIter_Next((args_iter));
            if (!next) {
                break;
            }
            if (!PyString_Check(next) && !PyUnicode_Check(next)) {
                RaiseValueError("arg in args must be a string");
            }
            config.args[count] = PyString_AsString(next);
            count++;
            if(count > 95) {
                RaiseValueError("Number of args must < 95");
            }
        }
    }
    config.args[count] = NULL;

    count = 0;
    if (env_list != NULL) {
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
            config.env[count] = PyString_AsString(next);
            count++;
            if(count > 95) {
                RaiseValueError("Number of env must < 95");
            }
        }
    }
    config.env[count] = NULL;

    if (use_sandbox != NULL) {
        if (!PyBool_Check(use_sandbox)) {
            RaiseValueError("use_sandbox must ba a bool");
        }
        config.use_sandbox = PyObject_IsTrue(use_sandbox);
    }
    else {
        config.use_sandbox = 1;
    }

    if (use_nobody != NULL) {
        if (!PyBool_Check(use_nobody)) {
            RaiseValueError("use_nobody must be a bool");
        }
        if (PyObject_IsTrue(use_nobody)) {
            passwd = getpwnam("nobody");
            if(passwd == NULL) {
                RaiseValueError("get nobody user info failed");
            }
            config.gid = passwd->pw_gid;
            config.uid = passwd->pw_uid;
        }
        else {
            config.uid = config.gid = -1;
        }
    }
    else {
        config.uid = config.gid = -1;
    }

    if (log_path != NULL) {
        if (!PyString_Check(log_path)) {
            RaiseValueError("log path must be a string");
        }
        config.log_path = PyString_AsString(log_path);
    }
    else {
        config.log_path = "judger.log";
    }

    if((config.uid != -1 || config.gid != -1) && getuid() != 0) {
        RaiseValueError("Root user is required when use_nobody=True");
    }

    run(&config, &result);
    return Py_BuildValue("{s:l, s:i, s:i, s:i, s:i, s:i}",
                         "cpu_time", result.cpu_time, "memory", result.memory, "real_time", result.real_time, "signal",
                         result.signal, "flag", result.flag, "exit_status", result.exit_status);

}


static PyMethodDef judger_methods[] = {
        {"run", (PyCFunction) judger_run, METH_KEYWORDS, NULL},
        {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC initjudger(void) {
    PyObject *module = Py_InitModule3("judger", judger_methods, NULL);
    PyModule_AddIntConstant(module, "CPU_TIME_UNLIMITED", CPU_TIME_UNLIMITED);
    PyModule_AddIntConstant(module, "MEMORY_UNLIMITED", MEMORY_UNLIMITED);
}
