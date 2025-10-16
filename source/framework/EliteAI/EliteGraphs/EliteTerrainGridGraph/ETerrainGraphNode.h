#pragma once

#include "../EliteGraph/EGraphNode.h"
namespace Elite
{
	class TerrainGraphNode : public GraphNode
	{
	public:

		TerrainGraphNode(const Vector2& pos);
		TerrainGraphNode(const TerrainGraphNode& other);
		virtual ~TerrainGraphNode() = default;

		TerrainType GetTerrainType() const { return m_Terrain; }
		void SetTerrainType(TerrainType terrain);
		const Color&  GetColor() const;


	protected:
		TerrainType m_Terrain;
	};

}