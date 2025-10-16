#include "stdafx.h"

#include "ETerrainGraphNode.h"

using namespace Elite;

TerrainGraphNode::TerrainGraphNode(const Vector2& pos)
	: GraphNode(pos), m_Terrain(TerrainType::Ground)
{
}

Elite::TerrainGraphNode::TerrainGraphNode(const TerrainGraphNode& other) : GraphNode(other), m_Terrain(other.m_Terrain)
{
}

void Elite::TerrainGraphNode::SetTerrainType(TerrainType terrain)
{
	m_Terrain = terrain;
	GraphNode::SetColor(GetColor());
}

inline const Color& TerrainGraphNode::GetColor() const
{
	switch (m_Terrain)
	{
	case TerrainType::Mud:
		return MUD_NODE_COLOR;
		break;
	case TerrainType::Water:
		return WATER_NODE_COLOR;
		break;
	default:
		return GROUND_NODE_COLOR;
		break;
	}
}
