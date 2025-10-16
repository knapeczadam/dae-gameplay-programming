#include "stdafx.h"

#include <iomanip>
#include "EGraphRenderer.h"
#include "../EliteGraph/EGraphNode.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraph.h"
#include "../EliteGridGraph/EGridGraph.h"

namespace Elite
{
	void GraphRenderer::RenderCircleNode(Vector2 pos, std::string text /*= ""*/, float radius /*= 3.0f*/, Elite::Color col /*= DEFAULT_NODE_COLOR*/, float depth /*= 0.0f*/) const
	{
		DEBUGRENDERER2D->DrawSolidCircle(pos, radius, { 0,0 }, col, depth);
		const auto stringOffset = Vector2{ -0.5f, 1.f };
		DEBUGRENDERER2D->DrawString(pos + stringOffset, text.c_str());
	}

	void GraphRenderer::RenderRectNode(Vector2 pos, std::string text /*= ""*/, float width /* = 3.0f*/, Elite::Color col /*= DEFAULT_NODE_COLOR*/, float depth /*= 0.0f*/) const
	{
		Vector2 verts[4]
		{
			Vector2(pos.x - width / 2.0f, pos.y - width / 2.0f),
			Vector2(pos.x - width / 2.0f, pos.y + width / 2.0f),
			Vector2(pos.x + width / 2.0f, pos.y + width / 2.0f),
			Vector2(pos.x + width / 2.0f, pos.y - width / 2.0f)
		};

		DEBUGRENDERER2D->DrawSolidPolygon(&verts[0], 4, col, depth);

		const auto stringOffset = Vector2{ -0.5f, 1.f };
		DEBUGRENDERER2D->DrawString(pos + stringOffset, text.c_str());
	}

	

	void GraphRenderer::RenderConnection(GraphConnection* con, Elite::Vector2 toPos, Elite::Vector2 fromPos, std::string text, Elite::Color col, float depth/*= 0.0f*/) const
	{
		auto center = toPos + (fromPos - toPos) / 2;

		DEBUGRENDERER2D->DrawSegment(toPos, fromPos, col, depth);
		DEBUGRENDERER2D->DrawString(center, text.c_str());
	}

	Elite::Color GraphRenderer::GetNodeColor(GraphNode* pNode) const
	{
		return pNode->GetColor();
	}

	Elite::Color GraphRenderer::GetConnectionColor(GraphConnection* connection) const
	{
		return connection->GetColor();
	}

	
	void GraphRenderer::RenderGraph(Graph* pGraph, const GraphRenderingOptions& options) const
	{
		const float depth = DEBUGRENDERER2D->NextDepthSlice();
		for (auto node : pGraph->GetAllNodes())
		{
			if (options.renderNodes)
			{
				std::string nodeTxt = "";
				if (options.renderNodesTxt)
					nodeTxt = GetNodeText(node);

				RenderCircleNode(node->GetPosition(), nodeTxt, DEFAULT_NODE_RADIUS, GetNodeColor(node), depth);
			}

			if (options.renderConnections)
			{
				//Connections
				for (auto con : pGraph->GetConnectionsFromNode(node->GetId()))
				{
					std::string conTxt = "";
					if (options.renderConnectionsTxt)
						conTxt = GetConnectionText(con);
					Elite::Vector2 fromPos, toPos;
					fromPos = pGraph->GetNode(con->GetFromNodeId())->GetPosition();
					toPos = pGraph->GetNode(con->GetToNodeId())->GetPosition();
					RenderConnection(con, fromPos, toPos, conTxt, GetConnectionColor(con), depth);
				}
			}
		}
	}

	std::string GraphRenderer::GetNodeText(GraphNode* pNode) const
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(m_FloatPrintPrecision) << pNode->GetId();
		return ss.str();
	}

	std::string GraphRenderer::GetConnectionText(GraphConnection* pConnection) const
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(m_FloatPrintPrecision) << pConnection->GetCost();
		return ss.str();
	}


	void GraphRenderer::RenderGraph(
		GridGraph* pGraph,
		bool renderNodes,
		bool renderNodeNumbers,
		bool renderConnections,
		bool renderConnectionsCosts) const
	{
		if (renderNodes)
		{
			//Nodes/Grid
			for (auto r = 0; r < pGraph->m_NrOfRows; ++r)
			{
				for (auto c = 0; c < pGraph->m_NrOfColumns; ++c)
				{
					int idx = pGraph->GetNodeId(c, r);
					Vector2 cellPos{ pGraph->GetNodePos(idx) };
					int cellSize = pGraph->m_CellSize;

					//Node
					std::string nodeTxt{};
					if (renderNodeNumbers)
						nodeTxt = GetNodeText(pGraph->GetNode(c,r));

					RenderRectNode(cellPos, nodeTxt, float(cellSize), GetNodeColor(pGraph->GetNode(c,r)), DEBUGRENDERER2D->NextDepthSlice());
				}
			}
		}

		if (renderConnections)
		{
			for (auto node : pGraph->GetAllNodes())
			{
				//Connections
				for (auto con : pGraph->GetConnectionsFromNode(node->GetId()))
				{
					std::string conTxt{ };
					if (renderConnectionsCosts)
						conTxt = GetConnectionText(con);

					RenderConnection(con, pGraph->GetNodePos(con->GetToNodeId()), pGraph->GetNodePos(con->GetFromNodeId()), conTxt, GetConnectionColor(con), DEBUGRENDERER2D->NextDepthSlice());
				}
			}
		}
	}


	void GraphRenderer::HighlightNodes(GridGraph* pGraph, std::vector<GraphNode*> path, Color col /*= HIGHLIGHTED_NODE_COLOR*/) const
	{
		for (auto& node : path)
		{
			//Node
			RenderCircleNode(
				pGraph->GetNodePos(node->GetId()),
				"",
				3.1f,
				col,
				DEBUGRENDERER2D->NextDepthSlice()
			);
		}
	}
}