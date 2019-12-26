// opc_ua_server_win.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "pch.h"
#include <map>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "open62541.h"
#include <memory>
#include <string>
#include <random>
#include <utility>
#include <vector>
#include <Windows.h>
#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;
using namespace std;

#define TRACE_DEBUG printf_s
#define TRACE_ERROR printf_s

bool gRun = true;
std::string gPath = "./opc_server.json";
UA_ServerConfig * PConfig = NULL;
UA_Server *PServer = NULL;
std::random_device Rd;
std::default_random_engine RandGen = std::default_random_engine(Rd());


typedef struct
{
	std::string DisplayName;
	std::string Description;
	uint32_t DataType;
	std::vector<std::string> DataSets;// convert to range of integer/float/double
									  // if needed
	UA_Byte AccessLevel;
} NodeVariable;


std::vector<std::string> NameSpaces;
std::map<std::pair<UA_UInt16, UA_UInt32>, NodeVariable> NumericNodeIdMap;
std::map<std::pair<UA_UInt16, std::string>, NodeVariable> StringNodeIdMap;

namespace Item {
	const std::string NAME_SPACES = "nameSpaces";
	const std::string NODE_IDS = "nodeIds";
	const std::string NS_INDEX = "nsIndex";
	const std::string IDENTIFIER_TYPE = "identifierType";
	const std::string IDENTIFIER = "identifier";
	const std::string DISPLAY_NAME = "displayName";
	const std::string DESCRIPTION = "description";
	const std::string DATA_TYPE = "dataType";
	const std::string DATA_SET = "dataSet";
	const std::string ACCESS_LEVEL = "accessLevel";
}

//static void ParseEntry(
//	cJSON* pJson
//)
//{
	/*cJSON* pArray = cJSON_GetObjectItem(pJson, Item::NAME_SPACES.c_str());
	if (pArray->type != cJSON_Array) {
		TRACE_ERROR("JSON object %s type(%d) is not array.",
			Item::NAME_SPACES.c_str(),
			pArray->type);
		return;
	}
	cJSON* pNameSpace = NULL;
	cJSON_ArrayForEach(pNameSpace, pArray) {
		NameSpaces.push_back(pNameSpace->valuestring);
	}

	pArray = cJSON_GetObjectItem(pJson, Item::NODE_IDS.c_str());
	if (pArray->type != cJSON_Array) {
		TRACE_ERROR("JSON object %s type(%d) is not array.",
			Item::NODE_IDS.c_str(),
			pArray->type);
		return;
	}
	cJSON* pNodeId = NULL;
	cJSON* p = NULL;
	cJSON_ArrayForEach(pNodeId, pArray) {
		p = cJSON_GetObjectItem(pNodeId, Item::IDENTIFIER_TYPE.c_str());
		if (!p) {
			continue;
		}
		uint32_t type = p->valueint;
		p = cJSON_GetObjectItem(pNodeId, Item::NS_INDEX.c_str());
		if (!p) {
			continue;
		}
		uint32_t nsIdx = p->valueint;
		UA_NodeId nodeId = UA_NODEID_NULL;
		p = cJSON_GetObjectItem(pNodeId, Item::IDENTIFIER.c_str());
		if (p && type == UA_NODEIDTYPE_NUMERIC && p->type == cJSON_Number) {
			nodeId = UA_NODEID_NUMERIC(nsIdx, p->valueint);
		}
		else if (p && type == UA_NODEIDTYPE_STRING && p->type == cJSON_String) {
			nodeId = UA_NODEID_STRING(nsIdx, p->valuestring);
		}

		NodeVariable variable;
		p = cJSON_GetObjectItem(pNodeId, Item::DISPLAY_NAME.c_str());
		if (p && p->type == cJSON_String) {
			variable.DisplayName = std::string(p->valuestring);
		}
		p = cJSON_GetObjectItem(pNodeId, Item::DESCRIPTION.c_str());
		if (p && p->type == cJSON_String) {
			variable.Description = std::string(p->valuestring);
		}
		p = cJSON_GetObjectItem(pNodeId, Item::DATA_TYPE.c_str());
		if (p) {
			variable.DataType = p->valueint;
		}
		cJSON* pDataSet = cJSON_GetObjectItem(pNodeId, Item::DATA_SET.c_str());
		if (pDataSet->type != cJSON_Array) {
			TRACE_ERROR("JSON object %s type(%d) is not array.",
				Item::DATA_SET.c_str(),
				pDataSet->type);
			return;
		}
		cJSON_ArrayForEach(p, pDataSet) {
			variable.DataSets.push_back(p->valuestring);
		}
		p = cJSON_GetObjectItem(pNodeId, Item::ACCESS_LEVEL.c_str());
		if (p) {
			variable.AccessLevel = p->valueint;
		}

		if (!UA_NodeId_isNull(&nodeId)
			&& (nodeId.identifierType == UA_NODEIDTYPE_NUMERIC)) {
			std::pair<int, int> key(nodeId.namespaceIndex,
				nodeId.identifier.numeric);
			auto ret = NumericNodeIdMap.insert(std::make_pair(key, variable));
			if (ret.second) {
				TRACE_DEBUG("(%d, %d) => (%s, %s, %d)",
					key.first,
					key.second,
					variable.DisplayName.c_str(),
					variable.Description.c_str(),
					variable.DataType);
			}
			else {
				TRACE_ERROR("(%d, %d) => (%s, %s, %d)",
					key.first,
					key.second,
					variable.DisplayName.c_str(),
					variable.Description.c_str(),
					variable.DataType);
			}
		}
		else if (!UA_NodeId_isNull(&nodeId)
			&& (nodeId.identifierType == UA_NODEIDTYPE_STRING)) {
			std::pair<int, std::string> key(nodeId.namespaceIndex, std::string(
				(char*)nodeId.identifier.string.data));
			auto ret = StringNodeIdMap.insert(std::make_pair(key, variable));
			if (ret.second) {
				TRACE_DEBUG("(%d, %s) => (%s, %s, %d)",
					key.first,
					key.second.c_str(),
					variable.DisplayName.c_str(),
					variable.Description.c_str(),
					variable.DataType);
			}
			else {
				TRACE_ERROR("(%d, %s) => (%s, %s, %d)",
					key.first,
					key.second.c_str(),
					variable.DisplayName.c_str(),
					variable.Description.c_str(),
					variable.DataType);
			}
		}
	}*/
	//}


static void ParseEntry(
	json jSon
)
{
	std::string dump = jSon["nameSpaces"].dump();

	for (json::iterator it = jSon["nameSpaces"].begin(); it != jSon["nameSpaces"].end(); ++it) {
		//std::cout << *it << '
		NameSpaces.push_back(*it);
	}

	int index = 0;
	for (json::iterator it = jSon["nodeIds"].begin(); it != jSon["nodeIds"].end(); ++it) {
		//std::cout << *it << '
	   // NameSpaces.push_back(*it);

		//std::string temp = *it;
		json nodeJson = *it;
		std::string test= std::to_string(index++);
		int nsIndex = nodeJson[Item::NS_INDEX.c_str()].get<int>();
		int type = nodeJson[Item::IDENTIFIER_TYPE.c_str()].get<int>();
		std::string displayName = nodeJson[Item::DISPLAY_NAME.c_str()];
		std::string description = nodeJson[Item::DESCRIPTION.c_str()].dump();
		uint32_t dataType = nodeJson[Item::DATA_TYPE.c_str()].get<int>();
		std::vector<std::string>  dataSet = nodeJson[Item::DATA_SET.c_str()];
		int accessLevel = nodeJson[Item::ACCESS_LEVEL.c_str()].get<int>();

		UA_NodeId nodeId = UA_NODEID_NULL;

		if (type == UA_NODEIDTYPE_NUMERIC)
		{
			int value = nodeJson[Item::IDENTIFIER.c_str()].get<int>();
			nodeId = UA_NODEID_NUMERIC(nsIndex, value);

		}
		else if (type == UA_NODEIDTYPE_STRING)
		{
			char * value = (char*)nodeJson[Item::IDENTIFIER.c_str()].dump().c_str();
			nodeId = UA_NODEID_STRING(nsIndex, value);
		}

		NodeVariable variable;
		variable.DisplayName = displayName;
		variable.Description = description;
		variable.DataType = dataType;
		variable.DataSets = dataSet;
		variable.AccessLevel = accessLevel;
		if (nodeId.identifierType == UA_NODEIDTYPE_NUMERIC)
		{
			std::pair<int, int> key(nodeId.namespaceIndex, nodeId.identifier.numeric);
			auto ret = NumericNodeIdMap.insert(std::make_pair(key, variable));
			if (ret.second)
			{
				TRACE_DEBUG("(%d, %d) => (%s, %s, %d)\n",
					key.first,
					key.second,
					variable.DisplayName.c_str(),
					variable.Description.c_str(),
					variable.DataType);
			}
			else
			{
				TRACE_ERROR("(%d, %d) => (%s, %s, %d)\n",
					key.first,
					key.second,
					variable.DisplayName.c_str(),
					variable.Description.c_str(),
					variable.DataType);
			}
		}
		else if (nodeId.identifierType == UA_NODEIDTYPE_STRING)
		{
			std::pair<int, std::string> key(nodeId.namespaceIndex, std::string(
				(char*)nodeId.identifier.string.data));
			auto ret = StringNodeIdMap.insert(std::make_pair(key, variable));
			if (ret.second) {
				TRACE_DEBUG("(%d, %s) => (%s, %s, %d)\n",
					key.first,
					key.second.c_str(),
					variable.DisplayName.c_str(),
					variable.Description.c_str(),
					variable.DataType);
			}
			else {
				TRACE_ERROR("(%d, %s) => (%s, %s, %d)\n",
					key.first,
					key.second.c_str(),
					variable.DisplayName.c_str(),
					variable.Description.c_str(),
					variable.DataType);
			}
		}
	}
}

static void ReadEntryPoint()
{
	std::ifstream i((char *)gPath.c_str());

	if (!i.is_open())
	{
		return;
	}

	json j;
	i >> j;
	i.close();
	ParseEntry(j);

}

static int ServerInit()
{
	PConfig = UA_ServerConfig_new_default();
	if (!PConfig) {
		return UA_STATUSCODE_BADUNEXPECTEDERROR;
	}

	if (!NameSpaces.empty()) {
		PConfig->applicationDescription.applicationUri = UA_String_fromChars(
			const_cast<char*>(NameSpaces.front().c_str()));
	}

	PServer = UA_Server_new(PConfig);
	if (!PServer) {
		return UA_STATUSCODE_BADUNEXPECTEDERROR;
	}

	if (NameSpaces.size() > 1) {
		for (size_t i = 1; i < NameSpaces.size(); ++i) {
			UA_Server_addNamespace(PServer, NameSpaces[i].c_str());
		}
	}
	return UA_STATUSCODE_GOOD;
}

static void NodeSetInit()
{

	UA_VariableAttributes attr = UA_VariableAttributes_default;
	UA_Int32 myInteger = 42;
	UA_Variant_setScalar(&attr.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
	attr.description = UA_LOCALIZEDTEXT((char*)"en-US", (char*) "the answer");
	attr.displayName = UA_LOCALIZEDTEXT((char*)"en-US", (char*)"the answer");
	attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
	attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

	/* Add the variable node to the information model */
	UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, (char*) "the.answer");
	UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, (char*) "the answer");
	UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
	UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
	UA_Server_addVariableNode(PServer, myIntegerNodeId, parentNodeId,
		parentReferenceNodeId, myIntegerName,
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);

	int errCount = 0;
	for (auto it = NumericNodeIdMap.begin(); it != NumericNodeIdMap.end();
		++it) {
		UA_VariableAttributes attr = UA_VariableAttributes_default;
		attr.displayName = UA_LOCALIZEDTEXT(
			const_cast<char*>(""),
			const_cast<char*>(it->second.DisplayName.c_str()));
		attr.description = UA_LOCALIZEDTEXT(
			const_cast<char*>(""),
			const_cast<char*>(it->second.Description.c_str()));
		attr.dataType = UA_TYPES[it->second.DataType].typeId;
		attr.accessLevel = it->second.AccessLevel;
		UA_NodeId nodeId = UA_NODEID_NUMERIC(it->first.first, it->first.second);
		UA_QualifiedName nodeName =
			UA_QUALIFIEDNAME(it->first.first,
				const_cast<char*>(it->second.DisplayName.c_str()));
		UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
		UA_NodeId parentRefNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
		UA_StatusCode code = UA_Server_addVariableNode(PServer,
			nodeId,
			parentNodeId,
			parentRefNodeId,
			nodeName,
			UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
			attr,
			NULL,
			NULL);

		if (code != 0)
		{
			printf_s("[%d] code =%s\n", ++errCount, UA_StatusCode_name(code));
		}
	}
	for (auto it = StringNodeIdMap.begin(); it != StringNodeIdMap.end(); ++it) {
		UA_VariableAttributes attr = UA_VariableAttributes_default;
		attr.displayName = UA_LOCALIZEDTEXT(
			const_cast<char*>(""),
			const_cast<char*>(it->second.DisplayName.c_str()));
		attr.description = UA_LOCALIZEDTEXT(
			const_cast<char*>(""),
			const_cast<char*>(it->second.Description.c_str()));
		attr.dataType = UA_TYPES[it->second.DataType].typeId;
		attr.accessLevel = it->second.AccessLevel;
		UA_NodeId nodeId =
			UA_NODEID_STRING(it->first.first,
				const_cast<char*>(it->first.second.c_str()));
		UA_QualifiedName nodeName =
			UA_QUALIFIEDNAME(it->first.first,
				const_cast<char*>(it->second.DisplayName.c_str()));
		UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
		UA_NodeId parentRefNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
		UA_Server_addVariableNode(PServer,
			nodeId,
			parentNodeId,
			parentRefNodeId,
			nodeName,
			UA_NODEID_NUMERIC(0,
				UA_NS0ID_BASEDATAVARIABLETYPE),
			attr,
			NULL,
			NULL);
	}
}

void UpdateTask()
{
	for (auto it = NumericNodeIdMap.begin(); it != NumericNodeIdMap.end();
		++it) {
		UA_Variant variant;
		UA_Variant_init(&variant);
		UA_NodeId nodeId = UA_NODEID_NUMERIC(it->first.first, it->first.second);

		switch (it->second.DataType) {
		case UA_TYPES_FLOAT:
		{
			std::uniform_real_distribution<float> dis(0, 100);
			UA_Float floatVar = dis(RandGen);
			UA_Variant_setScalar(&variant, &floatVar,
				&UA_TYPES[it->second.DataType]);
		}
		break;
		case UA_TYPES_DOUBLE:
		{
			std::uniform_real_distribution<double> dis(0, 100);
			UA_Double doubleVar = dis(RandGen);
			UA_Variant_setScalar(&variant, &doubleVar,
				&UA_TYPES[it->second.DataType]);
		}
		break;
		case UA_TYPES_UINT32:
		{
			std::uniform_int_distribution<UA_UInt32> dis(0, 100);
			UA_UInt32 uint32Var = dis(RandGen);
			UA_Variant_setScalar(&variant, &uint32Var,
				&UA_TYPES[it->second.DataType]);
		}
		break;
		case UA_TYPES_STRING:
		{
			if (!it->second.DataSets.empty()) {
				std::uniform_int_distribution<size_t> dis(0,
					it->second.DataSets.size() -
					1);
				size_t i = dis(RandGen);
				UA_String nodeValue =
					UA_STRING(const_cast<char*>(it->second.DataSets[i].c_str()));
				UA_Variant_setScalar(&variant, &nodeValue,
					&UA_TYPES[it->second.DataType]);
			}
		}
		break;
		default:
			TRACE_ERROR("Input an invalid type");
			return;
		}
		UA_StatusCode retval = UA_Server_writeValue(PServer, nodeId, variant);
	}
}

void RepeatCallback(UA_Server* server, void* data)
{
	if (PServer == server) {
		UpdateTask();
	}
}

static void StopServer(int signum)
{
	gRun = false;
}


int main()
{

	ReadEntryPoint();
	ServerInit();
	NodeSetInit();
	UA_UInt64 callbackId;
	UA_Server_addRepeatedCallback(PServer,
		RepeatCallback,
		NULL,
		10000,
		&callbackId);
	UA_Server_run(PServer, &gRun);
	UA_Server_delete(PServer);
	PServer = NULL;
	UA_ServerConfig_delete(PConfig);
	PConfig = NULL;
	return UA_STATUSCODE_GOOD;
}


// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的秘訣: 
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案
