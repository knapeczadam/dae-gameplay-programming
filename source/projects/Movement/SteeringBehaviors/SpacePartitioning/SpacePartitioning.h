/*=============================================================================*/
// Copyright 2019-2020
// Authors: Yosha Vandaele
/*=============================================================================*/
// SpacePartitioning.h: Contains Cell and Cellspace which are used to partition a space in segments.
// Cells contain pointers to all the agents within.
// These are used to avoid unnecessary distance comparisons to agents that are far away.

// Heavily based on chapter 3 of "Programming Game AI by Example" - Mat Buckland
/*=============================================================================*/

#pragma once
#include <list>
#include <vector>
#include <iterator>
#include "framework\EliteMath\EVector2.h"
#include "framework\EliteGeometry\EGeometry2DTypes.h"

class SteeringAgent;

// --- Cell ---
// ------------
struct Cell
{
	Cell(float left, float bottom, float width, float height);

	std::vector<Elite::Vector2> GetRectPoints() const;
	
	// all the agents currently in this cell
	std::list<SteeringAgent*> agents;
	Elite::Rect boundingBox;
};

// --- Partitioned Space ---
// -------------------------
class CellSpace
{
public:
	struct DebugProperties
	{
		int  bottomLeftIdx  {-1};
		int  bottomRightIdx {-1};
		int  topLeftIdx     {-1};
		int  topRightIdx    {-1};
		int  colDiff        {-1};
		bool agentInOneCell {false};
	};
	
	CellSpace(float width, float height, int rows, int cols, int maxEntities);
	~CellSpace();
	void AddAgent(SteeringAgent* agent);
	void AgentPositionChanged(SteeringAgent* agent, Elite::Vector2 oldPos);

	void RegisterNeighbors(const SteeringAgent* pAgent, float neighborhoodRadius);
	const std::vector<SteeringAgent*>& GetNeighbors() const { return m_Neighbors; }
	int GetNrOfNeighbors() const { return m_NrOfNeighbors; }
	void SetFirstAgent(SteeringAgent* pAgent) { m_pFirstAgent = pAgent; }
	DebugProperties GetDebugProperties() const { return m_DebugProperties; }
	void SetCanRenderPartitions(bool canRender) { m_CanRenderPartitions = canRender; }

	//empties the cells of entities
	void EmptyCells();
	
	void RenderCells()const;
	void DrawCell(int rIdx, int cIdx, size_t nrOfAgents) const;
	void DrawNumber(int rIdx, int cIdx, size_t nrOfAgents) const;

private:
	// Cells and properties
	std::vector<Cell> m_Cells {};

	// World size
	float m_SpaceWidth  {0};
	float m_SpaceHeight {0};

	int   m_NrOfRows    {0};
	int   m_NrOfCols    {0};

	float m_CellWidth   {0};
	float m_CellHeight  {0};

	// Members to avoid memory allocation on every frame
	std::vector<SteeringAgent*> m_Neighbors           {};
	int                         m_NrOfNeighbors       {0};
	SteeringAgent*              m_pFirstAgent         {nullptr};
	DebugProperties             m_DebugProperties     {};
	bool                        m_CanRenderPartitions {false};

	// Helper functions
	int PositionToIndex(const Elite::Vector2 pos) const;
};
