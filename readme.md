# SCH

This project is an attempt at creating a JSON-only TCP protocol. It is a work in progress and is not yet ready for production use.

## Design

This protocol currently uses a basic client-server model, but the finalversion will be more flexible and will allow for more complex topologies. The server is a simple TCP server that listens on a port and accepts incoming connections. The server can handle multiple clients at the same time.

## Language

The "base" part of the server and client are written in C but in order to allow for more flexibility, request handling is done in python. This allows for more complex request handling and easier integration with other systems.

## Protocol Specifications

The SCH protocol uses a simple JSON format for communication. The basic pattern is as follows:

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
Find more details in the [specification file](doc/spec.md).

## Installation

The protocol and server are not yet ready for production use. However, you can try the package like this:

```bash
make
./bin/sch
```
