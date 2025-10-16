#pragma once
#include <concepts>
#include "../EliteGraph/EGraphNode.h"

namespace Elite
{
	template<typename T_GraphNode>
	concept graphnodetype = requires(T_GraphNode node, const Elite::Vector2 & pos)
	{
		{ T_GraphNode(pos) } -> std::derived_from<GraphNode>;
		
	};

	class GraphNodeFactory
	{
	public:
		GraphNodeFactory() = default;
		virtual ~GraphNodeFactory() = default;

		virtual GraphNode* CreateNode(const Elite::Vector2& pos) const = 0;
		virtual GraphNode* CloneNode(const GraphNode& other) const = 0;
	};

	template<graphnodetype T_GraphNode>
	class GraphNodeFactoryTemplate: public GraphNodeFactory
	{
	public:
		GraphNodeFactoryTemplate() = default;
		virtual ~GraphNodeFactoryTemplate() = default;

		GraphNode* CreateNode(const Elite::Vector2& pos) const  override { return new T_GraphNode(pos); }
		GraphNode* CloneNode(const GraphNode& other) const  override { return  new T_GraphNode((const T_GraphNode&)other); }
	};

	

}