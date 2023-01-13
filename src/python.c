/*
Here we have the python/C interface.
*/
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <python3.10/Python.h>


#include "../include/server.h"
#include "../include/cutils.h"

// Global variables to store references to the imported module and handle_request function
PyObject* module = NULL;
PyObject* function = NULL;


// Function to initialize the Python interpreter and import the module containing the handle_request function
int load_python_module(const char* module_name) {

    setenv("PYTHONPATH", "./python", 1);

    // Initialize the Python interpreter
    Py_Initialize();

    // Import the module
    PyObject* py_module_name = PyUnicode_FromString(module_name);
    module = PyImport_Import(py_module_name);
    Py_DECREF(py_module_name);

    if (module == NULL) {
        // An error occurred
        PyErr_Print();
        return 0;
    }

    // Get a reference to the handle_request function
    function = PyObject_GetAttrString(module, "handle_request");
    if (function == NULL || !PyCallable_Check(function)) {
        // An error occurred
        PyErr_Print();
        Py_XDECREF(function);
        Py_DECREF(module);
        return 0;
    }

    return 1;
}

PyDictObject* handle_request(const char* request_str, connection* conn) {
    if (module == NULL) {
        fprintf(stderr, "Error: Python module not loaded\n");
        return NULL;
    }
    
    // Convert the request string to a Python string object
    PyObject* py_request = PyUnicode_FromString(request_str);

    // Create a Python dictionary to hold the connection struct data
    PyObject* py_conn = PyDict_New();
    PyDict_SetItemString(py_conn, "address", PyUnicode_FromString(inet_ntoa(conn->address.sin_addr)));
    PyDict_SetItemString(py_conn, "port", PyLong_FromLong(conn->address.sin_port));
    PyDict_SetItemString(py_conn, "socket", PyLong_FromLong(conn->socket));
    PyDict_SetItemString(py_conn, "cuuid", PyUnicode_FromString(conn->cuuid));

    dbg(1,"Python: Calling handle_request function with request string");
    // Call the handle_request function with the request string and conn dict as arguments
    PyObject* result = PyObject_CallFunctionObjArgs(function, py_request, py_conn, NULL);
    Py_DECREF(py_request);
    Py_DECREF(py_conn);

    if (result == NULL) {
        msg(ERROR,"Python: handle_request function returned NULL");
        PyErr_Print();
        exit(1);
    }
    PyDictObject* dict = NULL;
    if (PyDict_Check(result)) {
      PyDictObject* dict = (PyDictObject*) result;
      // do something with the dictionary
    } else {
        fprintf(stderr, "Error: handle_request did not return a dictionary\n");
        exit(1);
    }

    return dict;
}
