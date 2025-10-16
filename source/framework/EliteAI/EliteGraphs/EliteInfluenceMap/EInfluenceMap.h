#pragma once

namespace Elite
{
	class GraphNode;
	class InfluenceNode;

	class InfluenceMap
	{
	public:
		InfluenceMap(int columns, int rows, float cellSize);
		virtual ~InfluenceMap();

		float GetMomentum() const { return m_Momentum; }
		void SetMomentum(float momentum) { m_Momentum = momentum; }

		float GetDecay() const { return m_Decay; }
		void SetDecay(float decay) { m_Decay = decay; }

		float GetPropagationInterval() const { return m_PropagationInterval; }
		void SetPropagationInterval(float propagationInterval) { m_PropagationInterval = propagationInterval; }
		
		void Update(float deltaTime);
		void Render() const;

		float GetInfluenceAtPosition(Elite::Vector2 & pos) const;
		void SetInfluenceAtPosition(Elite::Vector2& pos, float influence, bool additive = false);

	private:
		const float DC = 1.41421356f;
		const Elite::Color m_NegativeColor{ 1.f, 0.2f, 0.f };
		const Elite::Color m_NeutralColor{ 0.f, 0.f, 0.f };
		const Elite::Color m_PositiveColor{ 0.f, 0.2f, 1.f };

		//GRID options
		int m_numColumns;
		int m_numRows;
		float m_cellSize;

		float m_MaxAbsInfluence = 100.f;

		float m_Momentum = 0.8f; // a higher momentum means a higher tendency to retain the current influence
		float m_Decay = 0.1f; // determines the decay in influence over distance

		float m_PropagationInterval = .05f; //in Seconds
		float m_TimeSinceLastPropagation = 0.0f;

		std::vector<float> m_InfluenceBackBuffer;
		std::vector<float> m_InfluenceFrontBuffer;
		const float m_influenceCosts[9] = { DC,1,DC, 1, 0, 1, DC, 1, DC };

		bool IsInBounds(Vector2& position) const;
		int GetIndexAtPosition(Vector2& position) const;
		std::vector<int> GetNeighboringIndices(int nodeIndex) const;
		Elite::Color GetNodeColorFromInfluence(float influence) const;
		void SwapBuffers();
	};
}