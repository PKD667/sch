/*
Here we have the python/C interface.
*/
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <python3.10/Python.h>

// Global variables to store references to the imported module and handle_request function
PyObject* module = NULL;
PyObject* request_handler = NULL;
PyObject* request_composer = NULL;



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
    request_handler = PyObject_GetAttrString(module, "handle_request");
    if (request_handler == NULL || !PyCallable_Check(request_handler)) {
        // An error occurred
        PyErr_Print();
        Py_XDECREF(request_handler);
        Py_DECREF(module);
        return 0;
    }

    request_composer = PyObject_GetAttrString(module, "create_request");
    if (request_composer == NULL || !PyCallable_Check(request_composer)) {
        // An error occurred
        PyErr_Print();
        Py_XDECREF(request_composer);
        Py_DECREF(module);
        return 0;
    }

    return 1;
}

// Wrapper function for the Python handle_request function
const char* handle_request(const char* request) {
  if (module == NULL || request_handler == NULL) {
    fprintf(stderr, "Error: Python module or function not loaded\n");
    return NULL;
  }

  // Convert the request string to a Python string object
  PyObject* py_request = PyUnicode_FromString(request);

  // Call the handle_request function with the request string as an argument
  PyObject* result = PyObject_CallFunctionObjArgs(request_handler, py_request, NULL);
  Py_DECREF(py_request);

  if (result == NULL) {
    // An error occurred
    PyErr_Print();
    return NULL;
  }

  // Convert the result to a C string
  const char* response = PyUnicode_AsUTF8(result);
  Py_DECREF(result);

  return response;
}

const char* create_request(const char* text,const char* cid) {
  if (module == NULL || request_composer == NULL) {
    fprintf(stderr, "Error: Python module or function not loaded\n");
    return NULL;
  }

  // Convert the request string to a Python string object
  // Convert the request string to a Python string object
  PyObject* py_request = PyUnicode_FromString(request);

  // Call the handle_request function with the request string as an argument
  PyObject* result = PyObject_CallFunctionObjArgs(request_handler, py_request, NULL);
  Py_DECREF(py_request);

  if (result == NULL) {
    // An error occurred
    PyErr_Print();
    return NULL;
  }

  // Convert the result to a C string
  const char* response = PyUnicode_AsUTF8(result);
  Py_DECREF(result);

  return response;
}


