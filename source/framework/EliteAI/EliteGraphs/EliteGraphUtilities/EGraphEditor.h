#pragma once
#include "../EliteGraph/EGraphEnums.h"

namespace Elite
{
	class Graph;
	class GridGraph;
	class TerrainGridGraph;

	class GraphEditor final
	{
	public:
		GraphEditor() = default;
		~GraphEditor() = default;

		bool UpdateGraph(Graph* pGraph);

		bool UpdateGraph(GridGraph* pGraph);

		bool UpdateGraph(TerrainGridGraph* pGraph);

	private:
		const float MOUSE_MOVE_TRESHOLD = 4.0f;
		int m_SelectedNodeIdx = -1;
		int m_SelectedTerrainType = (int)TerrainType::Ground;

		Elite::Vector2 m_MousePos;
		Elite::Vector2 m_initialMousePos;
		bool m_IsLeftMouseBtnPressed = false;
		bool m_mouseHasMoved = false;
	};


}


