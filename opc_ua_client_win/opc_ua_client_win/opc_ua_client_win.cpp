// opc_ua_client_win.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "pch.h"

#include <open62541/client_config_default.h>
#include <open62541/client.h>
#include <open62541/client_config.h>
#include <iostream>
#include <stdio.h>
#include <memory>
#include <string>
#include "CJson/cJSON.h"

using namespace std;
#define TRACE_DEBUG 





int main()
{

	UA_Client* client = UA_Client_new();
	UA_ClientConfig_setDefault(UA_Client_getConfig(client));
	string opcUaServerPath = "opc.tcp://DESKTOP-TLOIROF:4840/";
		                        //"opc.tcp://ge831412:62567/Quickstarts/BoilerServer";

	string userName = "user1";
	string password = "password";
	/* Listing endpoints */
	UA_EndpointDescription* endpointArray = NULL;
	size_t endpointArraySize = 0;
	UA_StatusCode retval = UA_Client_getEndpoints(client, "opc.tcp://localhost:4840",
		&endpointArraySize, &endpointArray);
	retval = UA_Client_connect_username(client, opcUaServerPath.c_str(),userName.c_str(), password.c_str());
	TRACE_DEBUG("UA_Client_connect return %s", UA_StatusCode_name(retval));
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Array_delete(endpointArray, endpointArraySize, &UA_TYPES[UA_TYPES_ENDPOINTDESCRIPTION]);
		UA_Client_delete(client);
		return EXIT_FAILURE;
	}
	 char strJson []= "{\"name\" : \"Mars\",\"mass\":639e21,\"moons\":[{\"name\":\"Phobos\",\"size\":70},{\"name\":\"Deimos\",\"size\":39}]}";

	printf("Planet:\n");
	// First, parse the whole thing
	cJSON *root = cJSON_Parse(strJson);
	char *name = cJSON_GetObjectItem(root, "name")->valuestring;
	double mass = cJSON_GetObjectItem(root, "mass")->valuedouble;
	printf("%s, %.2e kgs\n", name, mass); // Note the format! %.2e will print a number with scientific notation and 2 decimals
	// transverse all client.

	UA_Client_disconnect(client);
	UA_Client_delete(client);
	return UA_STATUSCODE_GOOD;
}
	