# Protocol Specifications

This is the specification file for the sch protocol

### Table

 * (Basic request design)[#Ba]

 * Specific usages


## Basic request design

The request are direcly send in json like shown in the basic pattern : 
```json
{
    "method" : "TEST",
    "status" : "OK",
    "version" : "1.0",
    "id" : "SHA-256",
    "headers" : {
        "client" : "CURL 1.0",
        "host" : "0.0.0.0"
    },   
    "body" : {
        "Hi ! How are you ?"
    }
}
```

Responses are in the same format but will some changes : 
```json
{
    "method" : "TEST",
    "version" : "1.0",
    "id" : "SHA-256",
    "lid" : "SHA-256",
    "status" : "OK",
    "headers" : {
        "client" : "SCH SERVER 0.8",
        "host" : "example.com"
    },    
    "body" : {
        "text" : "Fine. And you ?"
    }
}   
```

The `"id"` field will be a sha-256 hash of the entire json string (inline) without the id obv.
