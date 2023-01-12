import json

CONNECTIONS = {}

base_request = {
    "method" : "",
    "status" : "OK",
    "version" : "1.0",
    "headers" : {},   
    "body" : {}
}


def do_TEST(request :dict) -> dict:
    response = base_request
    response["body"] = {
        "text" : "Hi ! This is a test response."
    }
    response["method"] = "TEST"
    return json.dumps(response)

def do_INIT(request :dict) -> dict:
    cid = request["headers"]["Connection-ID"]
    CONNECTIONS[cid] = {
        "status" : "OPEN",
    }

    response = base_request
    response["headers"]["Connection-ID"] = cid
    response["body"] = { "text" : "Connection established." }
    response["method"] = "INIT"

    return response

def do_CHAT(request :dict) -> dict:
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
    "CHAT" : do_CHAT
}

def handle_request(request :str) -> str: 
    json_request = json.loads(request)

    print(f'Method: {json_request["method"]}')
    print(f'Status: {json_request["status"]}')
    print(f'Headers: ')
    for key in json_request["headers"]:
        print(f'\t{key} : {json_request["headers"][key]}')
    print(f'Body: {json.dumps(json_request["body"],indent=4)}')

    fun = methods[json_request["method"]]
    response = fun(json_request)

    if response == None:
        return None

    return json.dumps(response)

def create_request(body,method,cid,headers={}):
    request = base_request
    request["method"] = method
    if type(request) == dict:
        request["body"] = body
    elif type(request) == str:
        request["body"]["text"] = body
    else:
        # throw type exception
        raise TypeError("Invalid type for request variable. Expected dict or str, got {}".format(type(request)))
    
    request["headers"] = headers
    request["headers"]["Connection-ID"] = cid

    
    request_str = json.dumps(request)
    return request_str


