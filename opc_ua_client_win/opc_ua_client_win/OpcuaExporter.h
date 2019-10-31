#pragma once
#include <map>
#include <vector>
#include <string>
#include "node.h"

// referene 
class server;
class alias;
using namespace std;
namespace OpcUaExporter
{
	struct OpcUaExporter
	{
		std::string Root;
		std::map<std::string, std::string> node_set_attributes;
		std::map < std::string, alias > aliases;
		std::map<UA_UInt16, UA_UInt16> _addr_idx_to_xml_idx;
		server * server;

		OpcUaExporter() {
			node_set_attributes["xmlns:xsi"] = "http://www.w3.org/2001/XMLSchema-instance";
			node_set_attributes["xmlns:uax"] = "http://opcfoundation.org/UA/2008/02/Types.xsd";
			node_set_attributes["xmlns:xsd"] = "http://opcfoundation.org/UA/2008/02/Types.xsd";
			node_set_attributes["xmlns"] = "http://opcfoundation.org/UA/2008/02/Types.xsd";
			Root = "UANodeSet";
		}

		void build_tree()
		{
			/*"""
				Create an XML etree object from a list of nodes; custom namespace uris are optional
				Namespaces used by nodes are always exported for consistency.
				Args:
		node_list: list of Node objects for export
			uris : list of namespace uri strings
			Returns :
				   """
					   self.logger.info('Building XML etree')

					   self._add_namespaces(node_list, uris)

					   # add all nodes in the list to the XML etree
					   for node in node_list :
				   self.node_to_etree(node)

					   # add aliases to the XML etree
					   self._add_alias_els()*/
		}

		void add_namespaces(std::vector<Node>  & nodes,std::vector<std::string> & uris)
		{

		}

		std::map<std::string, std::string> _make_idx_dict(std::vector<std::string> &  idxs
			, std::vector<std::string> & ns_array)
		{

		}

		void build_tree(std::vector<Node> & nodes, std::vector<std::string> & uris)
		{
			add_namespaces(nodes, uris);
			for (int i = 0; i < nodes.size(); ++i)
			{
				node_to_etree(nodes[i]);
			}
		}

		void node_to_etree(Node & node)
		{
			UA_NodeClass nodeClass = node.NodeClass;
			switch (nodeClass)
			{
			default:
				break;
			case UA_NodeClass::UA_NODECLASS_OBJECT:
				break;
			case UA_NodeClass::UA_NODECLASS_OBJECTTYPE:
				break;
			case UA_NodeClass::UA_NODECLASS_VARIABLE:
				break;
			case UA_NodeClass::UA_NODECLASS_VARIABLETYPE:
				break;
			case UA_NodeClass::UA_NODECLASS_REFERENCETYPE:
				break;
			case UA_NodeClass::UA_NODECLASS_DATATYPE:
				break;
			case UA_NodeClass::UA_NODECLASS_METHOD:
				break;
			}

		}

		std::string node_to_string(UA_NodeId & id)
		{
			UA_UInt16 namespaceIndex = id.namespaceIndex;
			if (_addr_idx_to_xml_idx.find(namespaceIndex) != _addr_idx_to_xml_idx.end())
			{
				return  "";
			}

			UA_NodeId newID = { 0 };
			newID.identifier = id.identifier;
			newID.identifierType = id.identifierType;
			newID.namespaceIndex = _addr_idx_to_xml_idx[namespaceIndex];


			return ""; //newID.to_string();
		}

		std::string _bname_to_string(UA_QualifiedName & bName)
		{
			UA_UInt16 namespaceIndex = bName.namespaceIndex;
			if (_addr_idx_to_xml_idx.find(namespaceIndex) != _addr_idx_to_xml_idx.end())
			{
				return  "";
			}

			UA_QualifiedName newName;
			newName.namespaceIndex = bName.namespaceIndex;
			newName.name = bName.name;
			return ""; //bname.to_string()
		}

		// get a list of all indexes used or references by nodes
		std::vector<int> _get_ns_idxs_of_nodes(vector<Node> & nodes)
		{
			std::vector<int> idxs;
			std::vector<int> node_idxs;
			for (int i = 0; i < nodes.size(); ++i)
			{
				Node node = nodes[i];
				node_idxs.push_back(node.nodeId.namespaceIndex);
				try {
					node_idxs.push_back(node.browseName.namespaceIndex);
				}
				catch (exception & ex) {
					// self.logger.exception("Error retrieving browse name of node %s", node)
				}

				for (int j = 0; j < node.references.size(); ++j)
				{
					Node reference = node.references[i];
					node_idxs.push_back(reference.nodeId.namespaceIndex);

					//remove duplicate nodeid.
				}

				for (int z = 0; z < node_idxs.size(); ++z)
				{
					int index = node_idxs[z];
					if (index != 0
						&& std::find(idxs.begin(), idxs.end(), index) != idxs.end())
					{
						idxs.push_back(index);
					}
				}
				
			}
			return idxs;
	
		}

		void _add_idxs_from_uris(std::vector<int> & idxs
			, std::vector<string> & uris
			, std::vector<string> & ns_array)
		{
				for (int i = 0; i < uris.size(); ++i)
				{
					std::string uri = uris[i];
					std::vector<string>::iterator it = std::find(ns_array.begin(), ns_array.end(), uri);
					if (it == ns_array.end())
					{
						continue;
					}
					int targetIndex = it - ns_array.end();
					std::vector<int>::iterator it2 = std::find(idxs.begin(), idxs.end(), targetIndex);
					if (it2 != idxs.end())
					{
						idxs.push_back(targetIndex);
					}
				}

		}
	};
}