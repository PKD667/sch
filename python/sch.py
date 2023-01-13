import json,os

CONNECTIONS = {}

base_request = {
    "method" : "",
    "status" : "OK",
    "version" : "1.0",
    "headers" : {},   
    "body" : {}
}


def do_TEST(request :dict,connection) -> dict:
    response = base_request
    response["body"] = {
        "text" : "Hi ! This is a test response."
    }
    response["method"] = "TEST"
    
    os.write(connection["socket"],json.dumps(response).encode())

    return

def do_INIT(request :dict,connection) -> dict:
    cid = request["headers"]["Connection-ID"]
    CONNECTIONS[cid] = {
        "status" : "OPEN",
    }

    response = base_request
    response["headers"]["Connection-ID"] = cid
    response["body"] = { "text" : "Connection established." }
    response["method"] = "INIT"

    os.write(connection["socket"],json.dumps(response).encode())

    return 

def do_CHAT(request :dict,connection) -> dict:
    cid = request["headers"]["Connection-ID"]

    if (not cid in CONNECTIONS) or (CONNECTIONS[cid]["status"] != "OPEN"):
        print("Connection not found or closed")
        return None

    response = base_request
    response["headers"]["Connection-ID"] = cid
    response["body"] = { "text" : "Connection established." }
    response["method"] = "INIT"

    return None



methods = {
    "TEST" : do_TEST,
    "INIT" : do_INIT,
    "CHAT" : 420
}

def handle_request(request :str,connection :dict) -> dict: 
    json_request = json.loads(request)

    print(f'Method: {json_request["method"]}')
    print(f'Status: {json_request["status"]}')
    print(f'Headers: ')
    for key in json_request["headers"]:
        print(f'\t{key} : {json_request["headers"][key]}')
    print(f'Body: {json.dumps(json_request["body"],indent=4)}')

    fun = methods[json_request["method"]]
    if fun != 420:
        print("No associated method")
        fun(json_request,connection)

    print(f"Request handled successfully , returning {json.dumps(json_request,indent=4)}")
    return json_request

