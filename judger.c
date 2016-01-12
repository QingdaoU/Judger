#include <python2.7/Python.h>
#include "runner.h"


static PyObject *error;


static PyObject *judger_run(PyObject *self, PyObject *args, PyObject *kwargs) {
    struct config config;
    struct result result;
    config.path = config.in_file = config.out_file = NULL;
    static char *kwargs_list[] = {"path", "in_file", "out_file", "max_cpu_time", "max_memory", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sssii", kwargs_list, &(config.path), &(config.in_file),
                                     &(config.out_file), &(config.max_cpu_time), &(config.max_memory))) {
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

    printf("%s %s %s %d %d", config.path, config.in_file, config.out_file, config.max_cpu_time, config.max_memory);
    run(&config, &result);
    return Py_BuildValue("{s: i, s:l, s:i, s:i, s:i, s:i}",
                         "cpu_time", result.cpu_time, "memory", result.memory, "real_time", result.real_time, "signal",
                         result.signal, "flag", result.flag, "error", result.error);

}


static PyMethodDef judger_methods[] = {
        {"run", (PyCFunction) judger_run, METH_VARARGS | METH_KEYWORDS, NULL},
        {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC initjudger() {
    PyObject *module = Py_InitModule3("judger", judger_methods, NULL);
    error = PyErr_NewException("judger.error", NULL, NULL);
    Py_INCREF(error);
    PyModule_AddObject(module, "error", error);
}