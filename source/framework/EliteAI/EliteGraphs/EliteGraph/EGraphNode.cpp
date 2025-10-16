#include "stdafx.h"
#include "EGraphNode.h"

Elite::GraphNode::GraphNode(const Elite::Vector2& pos)
	: m_Position{ pos }, m_Color(DEFAULT_NODE_COLOR)
	, m_Id{ invalid_node_id }
{
}
