#include <python2.7/Python.h>
#include "runner.h"


static PyObject *judger_run(PyObject *self, PyObject *args, PyObject *kwargs) {
    struct config config;
    struct result result = {0, 0, 0, 0, 0};
    PyObject *args_list = NULL, *env_list = NULL, *use_sandbox = NULL, *next = NULL, *args_iter = NULL, *env_iter = NULL;
    int count = 0;
    static char *kwargs_list[] = {"path", "in_file", "out_file", "max_cpu_time",
                                  "max_memory", "args", "env", "use_sandbox", NULL};

    config.path = config.in_file = config.out_file = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sssii|OOO", kwargs_list, &(config.path), &(config.in_file),
                                     &(config.out_file), &(config.max_cpu_time), &(config.max_memory),
                                     &args_list, &env_list, &use_sandbox)) {
        PyErr_SetString(PyExc_ValueError, "Invalid args and kwargs");
        return NULL;
    }
    if (config.max_cpu_time <= 1) {
        PyErr_SetString(PyExc_ValueError, "Max cpu time can not less than 1 ms");
        return NULL;
    }
    if (config.max_memory < 16 * 1024 * 1024) {
        PyErr_SetString(PyExc_ValueError, "Max memory can not be less than 16M");
        return NULL;
    }
    if (access(config.path, F_OK) == -1) {
        PyErr_SetString(PyExc_ValueError, "Exec file does not exist");
        return NULL;
    }
    if (access(config.in_file, F_OK) == -1) {
        PyErr_SetString(PyExc_ValueError, "Input file does not exist");
        return NULL;
    }
    if (args_list != NULL) {
        if (!PyList_Check(args_list)) {
            PyErr_SetString(PyExc_ValueError, "args must be a list");
            return NULL;
        }

        args_iter = PyObject_GetIter(args_list);
        while (1) {
            next = PyIter_Next((args_iter));
            if (!next) {
                break;
            }
            if (!PyString_Check(next)) {
                PyErr_SetString(PyExc_ValueError, "arg must be string");
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
            PyErr_SetString(PyExc_ValueError, "env must be a list");
            return NULL;
        }
        env_iter = PyObject_GetIter(env_list);
        while (1) {
            next = PyIter_Next(env_iter);
            if (!next) {
                break;
            }
            if (!PyString_Check(next)) {
                PyErr_SetString(PyExc_ValueError, "env must be string");
                return NULL;
            }
            config.env[count] = PyString_AsString(next);
            count++;
        }
    }
    config.env[count] = NULL;

    if (use_sandbox != NULL) {
        if (!PyBool_Check(use_sandbox)) {
            PyErr_SetString(PyExc_ValueError, "use sandbox must ba a bool");
            return NULL;
        }
        config.use_sandbox = PyObject_IsTrue(use_sandbox);
    }
    else {
        config.use_sandbox = 1;
    }

    run(&config, &result);
    return Py_BuildValue("{s:i, s:l, s:i, s:i, s:i}",
                         "cpu_time", result.cpu_time, "memory", result.memory, "real_time", result.real_time, "signal",
                         result.signal, "flag", result.flag);

}


static PyMethodDef judger_methods[] = {
        {"run", (PyCFunction) judger_run, METH_VARARGS | METH_KEYWORDS, NULL},
        {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC initjudger(void) {
    Py_InitModule3("judger", judger_methods, NULL);
}
