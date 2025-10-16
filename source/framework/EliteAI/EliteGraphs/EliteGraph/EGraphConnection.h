#pragma once
#include "EGraphEnums.h"
#include "../EliteGraphUtilities/EGraphVisuals.h"
namespace Elite
{
	class GraphConnection
	{
	public:
		GraphConnection(int fromId = invalid_node_id, int toIndex = invalid_node_id, float cost = 1.f, const Color& color = DEFAULT_CONNECTION_COLOR)
			:m_FromId{ fromId }
			, m_ToId{ toIndex }
			,m_Cost{cost}
			,m_Color{color}
		{}
		virtual ~GraphConnection() = default;

		int GetFromNodeId() const { return m_FromId; }
		void SetFromNodeId(int id) { m_FromId = id; }

		int GetToNodeId() const { return m_ToId; }
		void SetToNodeId(int id) { m_ToId = id; }

		float GetCost() const { return m_Cost; }
		void SetCost(float newCost) { m_Cost = newCost; }

		const Color& GetColor() const { return m_Color; }
		void SetColor(const Color& color) { m_Color = color; }

		bool IsValid() const { return (m_FromId != invalid_node_id && m_ToId != invalid_node_id); }

		bool operator==(const GraphConnection& rhs) const;
		bool operator!=(const GraphConnection& rhs) const;

	protected:
		Color m_Color{ DEFAULT_CONNECTION_COLOR };
		int m_FromId;
		int m_ToId;
		float m_Cost;

	};
}

