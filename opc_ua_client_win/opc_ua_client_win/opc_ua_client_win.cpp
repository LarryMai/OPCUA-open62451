// opc_ua_client_win.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include "open62541.h"
#include <memory>
#include <string>
#include <Windows.h>


using namespace std;
#define TRACE_DEBUG 

bool gRun = false;

static void StopClient()
{
	gRun = false;
}


static void DeleteSubscriptionCallback(UA_Client* client,
	UA_UInt32 subscriptionId,
	void* subscriptionContext)
{
	TRACE_DEBUG("Subscription Id %u was deleted", subscriptionId);
}

static void InactiveCb(UA_Client* client)
{
	TRACE_DEBUG("client(%p)", client);
}

static void HandlerV1Changed(UA_Client* client,
	UA_UInt32 subId,
	void* subContext,
	UA_UInt32 monId,
	void* monContext,
	UA_DataValue* value)
{
	TRACE_DEBUG("client(%p)", client);
	float v1 = *((float*)(value->value.data));
	TRACE_DEBUG("The v1 has changed to (%.3f)!", v1);
	TRACE_DEBUG("%s", subContext);
	TRACE_DEBUG("%s", monContext);
	//if (gAgent) {
	//	char cmd[255];
	//	sprintf(cmd, "{\"v1\":%.3f}", v1);
	//	CPA_direct_dev_property_post(gAgent, cmd, 0, NULL);
	//}
}

static void ClientStateCb(UA_Client* client, UA_ClientState clientState)
{
	TRACE_DEBUG("client(%p) state = %d",
		client,
		clientState);
	switch (clientState) {
	case UA_CLIENTSTATE_SESSION:
	{
		TRACE_DEBUG("A session with the server is open");
		/* A new session was created. We need to create the subscription. */
		/* Create a subscription */
		UA_CreateSubscriptionRequest
			request = UA_CreateSubscriptionRequest_default();
		UA_CreateSubscriptionResponse response = UA_Client_Subscriptions_create(
			client,
			request,
			NULL,
			NULL,
			DeleteSubscriptionCallback);
		if (response.responseHeader.serviceResult == UA_STATUSCODE_GOOD) {
			TRACE_DEBUG("Create subscription succeeded, id %u",
				response.subscriptionId);
		}
		else {
			return;
		}
		/* Add a MonitoredItem */
		UA_MonitoredItemCreateRequest monRequest =
			UA_MonitoredItemCreateRequest_default(UA_NODEID_STRING(1, (char *)"v1"));
		UA_MonitoredItemCreateResult monResponse =
			UA_Client_MonitoredItems_createDataChange(client,
				response.subscriptionId,
				UA_TIMESTAMPSTORETURN_BOTH,
				monRequest,
				NULL,
				HandlerV1Changed,
				NULL);
		if (monResponse.statusCode == UA_STATUSCODE_GOOD) {
			TRACE_DEBUG(
				"Monitoring UA_NS0ID_SERVER_SERVERSTATUS_CURRENTTIME', id %u",
				monResponse.monitoredItemId);
		}
	}
	break;
	case UA_CLIENTSTATE_SESSION_RENEWED:
		TRACE_DEBUG("A session with the server is open (renewed)");
		/* The session was renewed. We don't need to recreate the subscription. */
		break;
	}

}

static void SubscriptionInactivityCb(UA_Client* client,
	UA_UInt32 subscriptionId,
	void* subContext)
{
	TRACE_DEBUG("client(%p) subscriptionId = %d, context(%p)",
		client,
		subscriptionId,
		subContext);
}


int main()
{
	UA_ClientConfig myConfig = UA_ClientConfig_default;
	myConfig.subscriptionInactivityCallback = SubscriptionInactivityCb;
	myConfig.stateCallback = ClientStateCb;
	myConfig.inactivityCallback = InactiveCb;

	TRACE_DEBUG("%d, %d",
		myConfig.outStandingPublishRequests,
		myConfig.connectivityCheckInterval);
	UA_Client* client = UA_Client_new(myConfig);

	string opc_ua_server_path = "opc.tcp://192.168.100.58:4840";
		                        //"opc.tcp://ge831412:62567/Quickstarts/BoilerServer";
	UA_StatusCode retval = UA_Client_connect(client,opc_ua_server_path.c_str());
	TRACE_DEBUG("UA_Client_connect return %s", UA_StatusCode_name(retval));
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}

	UA_Variant value;
	UA_Variant_init(&value);

	UA_NodeId nodeId = UA_NODEID_STRING(1, (char*)"v1");
	retval = UA_Client_readValueAttribute(client, nodeId, &value);
	TRACE_DEBUG("UA_Client_readValueAttribute(v1) return %s",
		UA_StatusCode_name(retval));
	char cmd[255];
	if (retval == UA_STATUSCODE_GOOD) {
		TRACE_DEBUG("v1: %.3f", *((float*)value.data));
		sprintf(cmd, "{\"v1\":%.3f}", *((float*)value.data));
		//CPA_direct_dev_property_post(gAgent, cmd, 0, NULL);
	}
	nodeId = UA_NODEID_STRING(1, (char*)"switch");
	retval = UA_Client_readValueAttribute(client, nodeId, &value);
	TRACE_DEBUG("UA_Client_readValueAttribute(switch) return %s",
		UA_StatusCode_name(retval));
	if (retval == UA_STATUSCODE_GOOD) {
		TRACE_DEBUG("switch: %1.0f", *((float*)value.data));
		sprintf(cmd, "{\"switch\":%1.0f}", *((float*)value.data));
		//CPA_direct_dev_property_post(gAgent, cmd, 0, NULL);
	}
	nodeId = UA_NODEID_STRING(1, (char*)"model");
	retval = UA_Client_readValueAttribute(client, nodeId, &value);
	TRACE_DEBUG("UA_Client_readValueAttribute(model) return %s",
		UA_StatusCode_name(retval));
	if (retval == UA_STATUSCODE_GOOD) {
		TRACE_DEBUG("model: %s", ((UA_String*)value.data)->data);
		sprintf(cmd, "{\"model\":\"%s\"}", ((UA_String*)value.data)->data);
		//CPA_direct_dev_property_post(gAgent, cmd, 0, NULL);
	}
	int i = 0;

	while (gRun) {
		UA_Client_runAsync(client, 1000);
	}  
	
	/* Clean up */
	UA_Variant_deleteMembers(&value);
	UA_Client_delete(client); /* Disconnects the client internally */

	return UA_STATUSCODE_GOOD;
}
	