#pragma once

#include <open62541/client_config_default.h>

#include <vector>
#include <string>

namespace OpcUaExporter
{
	struct Node
	{

		UA_NodeId nodeId;

		Node * parent;

		UA_QualifiedName browseName;

		/// @brief List child nodes by specified reference
  /// @return zero or more child nodes.
		//std::vector<Node> GetChildren(const OpcUa::ReferenceId & refid) const;

		/// @brief Get ghildren by hierarchal referencies.
  /// @return One or zero chilren nodes.

		std::vector<Node> references;

		Node GetChild(const  UA_QualifiedName & path) const;
		Node GetChild(const std::vector<std::string> & path) const;
		Node GetChild(const UA_QualifiedName & browsename) const;

		//UA_VARIANT Value;
		UA_DataValue  DataValue;

		UA_NodeClass NodeClass;

	};
}