#pragma once
#include "../EliteGraph/EGraphNode.h"
namespace Elite
{
	class NavGraphNode : public GraphNode
	{
	public:
		NavGraphNode(const Vector2& pos = ZeroVector2)
			: GraphNode(pos), m_LineIdx(0)
		{
		}
		NavGraphNode(int lineIdx, const Vector2& pos = ZeroVector2)
			: GraphNode(pos), m_LineIdx(lineIdx)
		{
		}
		virtual ~NavGraphNode() = default;
		int GetLineIndex() const { return m_LineIdx; };
	protected:
		int m_LineIdx;
	};
}