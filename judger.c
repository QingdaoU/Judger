#include <python2.7/Python.h>
#include "runner.h"


static PyObject *error;


static PyObject *judger_run(PyObject *self, PyObject *args, PyObject *kwargs) {
    struct config config;
    struct result result = {0, 0, 0, 0, 0};
    PyObject *args_list = NULL, *env_list = NULL, *next = NULL, *args_iter = NULL, *env_iter = NULL;
    int count = 0;
    static char *kwargs_list[] = {"path", "in_file", "out_file", "max_cpu_time", "max_memory", "args", "env", NULL};

    config.path = config.in_file = config.out_file = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sssii|OO", kwargs_list, &(config.path), &(config.in_file),
                                     &(config.out_file), &(config.max_cpu_time), &(config.max_memory),
                                     &args_list, &env_list)) {
        PyErr_SetString(error, "Invalid args and kwargs");
        return NULL;
    }
    if (config.max_cpu_time <= 1) {
        PyErr_SetString(error, "Max cpu time can not less than 1 ms");
        return NULL;
    }
    if (config.max_memory < 16 * 1024 * 1024) {
        PyErr_SetString(error, "Max memory can not be less than 16M");
        return NULL;
    }
    if (access(config.path, F_OK) == -1) {
        PyErr_SetString(error, "Exec file does not exist");
        return NULL;
    }
    if (access(config.in_file, F_OK) == -1) {
        PyErr_SetString(error, "Input file does not exist");
        return NULL;
    }
    if (args_list != NULL) {
        if (!PyList_Check(args_list)) {
            PyErr_SetString(error, "args must be a list");
            return NULL;
        }

        args_iter = PyObject_GetIter(args_list);
        while (1) {
            next = PyIter_Next((args_iter));
            if (!next) {
                break;
            }
            if (!PyString_Check(next)) {
                PyErr_SetString(error, "arg must be string");
                return NULL;
            }
            config.args[count] = PyString_AsString(next);
            count++;
        }
    }
    config.args[count] = NULL;

    count = 0;
    if (env_list != NULL) {
        if (!PyList_Check(env_list)) {
            PyErr_SetString(error, "env must be a list");
            return NULL;
        }
        env_iter = PyObject_GetIter(env_list);
        while (1) {
            next = PyIter_Next(env_iter);
            if (!next) {
                break;
            }
            if (!PyString_Check(next)) {
                PyErr_SetString(error, "env must be string");
                return NULL;
            }
            config.env[count] = PyString_AsString(next);
            count++;
        }
    }
    config.env[count] = NULL;

    run(&config, &result);
    return Py_BuildValue("{s:i, s:l, s:i, s:i, s:i, s:i}",
                         "cpu_time", result.cpu_time, "memory", result.memory, "real_time", result.real_time, "signal",
                         result.signal, "flag", result.flag, "error", result.error);

}


static PyMethodDef judger_methods[] = {
        {"run", (PyCFunction) judger_run, METH_VARARGS | METH_KEYWORDS, NULL},
        {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC initjudger(void) {
    PyObject *module = Py_InitModule3("judger", judger_methods, NULL);
    error = PyErr_NewException("judger.error", NULL, NULL);
    Py_INCREF(error);
    PyModule_AddObject(module, "error", error);
}
