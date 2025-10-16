#pragma once

#include  <type_traits>
#include "EGraphVisuals.h"


namespace Elite
{
	class GraphNode;
	class GraphConnection;
	class Graph;
	class GridGraph;
	class TerrainGraphNode;

	struct GraphRenderingOptions
	{
		GraphRenderingOptions() = default;
		GraphRenderingOptions(bool renderNodes = true, bool renderNodesTxt = false, bool renderConnections = true, bool renderConnectionsTxt = false):
			renderNodes{ renderNodes },
			renderConnections{ renderConnections },
			renderNodesTxt{renderNodesTxt},
			renderConnectionsTxt{ renderConnectionsTxt }
		{
		}
		bool renderNodes;
		bool renderNodesTxt;
		bool renderConnections;
		bool renderConnectionsTxt;
	};

	class GraphRenderer final
	{
	public:
		GraphRenderer() = default;
		~GraphRenderer() = default;

		void RenderGraph(Graph* pGraph, const GraphRenderingOptions& renderOptions) const;
		
		void RenderGraph(GridGraph* pGraph, bool renderNodes, bool renderNodeTxt, bool renderConnections, bool renderConnectionsCosts) const;

		void HighlightNodes(GridGraph* pGraph, std::vector<GraphNode*> path, Color col = HIGHLIGHTED_NODE_COLOR) const;

		void SetNumberPrintPrecision(int precision) { m_FloatPrintPrecision = precision; }

	private:
		void RenderCircleNode(Vector2 pos, std::string text = "", float radius = DEFAULT_NODE_RADIUS, Elite::Color col = DEFAULT_NODE_COLOR, float depth = 0.0f) const;
		void RenderRectNode(Vector2 pos, std::string text = "", float width = DEFAULT_NODE_RADIUS, Elite::Color col = DEFAULT_NODE_COLOR, float depth = 0.0f) const;
		void RenderConnection(GraphConnection* con, Elite::Vector2 toPos, Elite::Vector2 fromPos, std::string text, Elite::Color col = DEFAULT_CONNECTION_COLOR, float depth = 0.0f) const;

		// Get correct color/text depending on the pNode/pConnection type
		Elite::Color GetNodeColor(GraphNode* pNode) const;

		Elite::Color GetConnectionColor(GraphConnection* pConnection) const;

		std::string GetNodeText(GraphNode* pNode) const;
		
		std::string GetConnectionText(GraphConnection* pConnection) const;

		//C++ make the class non-copyable
		GraphRenderer(const GraphRenderer&) = delete;
		GraphRenderer& operator=(const GraphRenderer&) = delete;

		// variables
		int m_FloatPrintPrecision = 1;
	};




}
