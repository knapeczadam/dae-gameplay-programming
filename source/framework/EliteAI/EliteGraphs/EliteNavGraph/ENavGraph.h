#pragma once

#include "framework/EliteAI/EliteGraphs/EliteGraph/EGraph.h"
#include "./ENavGraphNode.h"
namespace Elite
{
	class GraphNodeFactory;
	class NavGraph final: public Graph
	{
	public:
		NavGraph(const std::vector<Polygon>& colliderShapes, float widthWorld, float heightWorld, float playerRadius);
		NavGraph(const NavGraph& other);
		virtual ~NavGraph();

		std::shared_ptr<NavGraph> Clone();
		int GetNodeIdFromLineIndex(int lineIdx) const;
		Polygon* GetNavMeshPolygon() const;


	private:
		//--- Datamembers ---
		Polygon* m_pNavMeshPolygon = nullptr; //Polygon that represents navigation mesh

		void CreateNavigationGraph();


	private:
		NavGraph& operator=(const NavGraph& other) = delete;
		NavGraph(NavGraph&& other) = delete;
		NavGraph& operator=(NavGraph&& other) = delete;
		
	};
}

