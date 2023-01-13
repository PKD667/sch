#pragma once

#include "python3.10/Python.h"
#include "server.h"

int load_python_module(const char* module_name);
PyDictObject* handle_request(const char* request_str, connection* conn);