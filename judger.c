#include <python2.7/Python.h>
#include "runner.h"

static PyObject *error;

static PyObject *judger_run(PyObject *self, PyObject *args, PyObject *kwargs){
    static char *kwargs_list[] = {"path", "in_file", "out_file", "max_cpu_time", "max_memory", NULL};
    char *path = NULL, *in_file = NULL, *out_file = NULL;
    int max_cpu_time, max_memory;
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "sssii", kwargs_list, &path, &in_file, &out_file, &max_cpu_time, &max_memory)){
        PyErr_SetString(error, "Invalid args and kwargs");
        return NULL;
    }
    printf("%s %s %s %d %d", path, in_file, out_file, max_cpu_time, max_memory);
    Py_INCREF(Py_None);
    return Py_None;
}


static PyMethodDef judger_methods[] = {
        {"run", (PyCFunction)judger_run, METH_VARARGS | METH_KEYWORDS, NULL},
        {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initjudger(){
    PyObject *module = Py_InitModule3("judger", judger_methods, NULL);
    error = PyErr_NewException("judger.error", NULL, NULL);
    Py_INCREF(error);
    PyModule_AddObject(module, "error", error);
}