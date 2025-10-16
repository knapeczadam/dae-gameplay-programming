#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\Movement\SteeringBehaviors\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
    boundingBox.bottomLeft = Elite::Vector2{left, bottom};
    boundingBox.width = width;
    boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
    const auto left = boundingBox.bottomLeft.x;
    const auto bottom = boundingBox.bottomLeft.y;
    const auto width = boundingBox.width;
    const auto height = boundingBox.height;

    std::vector<Elite::Vector2> rectPoints =
    {
        {left, bottom},
        {left, bottom + height},
        {left + width, bottom + height},
        {left + width, bottom},
    };

    return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
    : m_SpaceWidth(width)
      , m_SpaceHeight(height)
      , m_NrOfRows(rows)
      , m_NrOfCols(cols)
      , m_Neighbors(maxEntities)
{
    m_CellWidth = m_SpaceWidth / static_cast<float>(cols);
    m_CellHeight = m_SpaceHeight / static_cast<float>(rows);

    for (int rIdx{0}; rIdx < m_NrOfRows; ++rIdx)
    {
        for (int cIdx{0}; cIdx < m_NrOfCols; ++cIdx)
        {
            const float left{m_CellWidth * static_cast<float>(cIdx)};
            const float bottom{m_CellHeight * static_cast<float>(rIdx)};
            m_Cells.emplace_back(left, bottom, m_CellWidth, m_CellHeight);
        }
    }
}

CellSpace::~CellSpace()
{
    EmptyCells();
}

/**
 * \brief Add an agent to the correct cell
 * \param agent 
 */
void CellSpace::AddAgent(SteeringAgent* agent)
{
    m_Cells[PositionToIndex(agent->GetPosition())].agents.push_back(agent);
}

/**
 * \brief Move the agent to the correct cell
 * \param agent 
 * \param oldPos 
 */
void CellSpace::AgentPositionChanged(SteeringAgent* agent, Elite::Vector2 oldPos)
{
    if (PositionToIndex(oldPos) != PositionToIndex(agent->GetPosition()))
    {
        m_Cells[PositionToIndex(oldPos)].agents.remove(agent);
        m_Cells[PositionToIndex(agent->GetPosition())].agents.push_back(agent);
    }
}

/**
 * \brief Register the neighbors for the provided agent
 * Only check the cells that are within the radius of the neighborhood
 * \param pAgent 
 * \param neighborhoodRadius 
 */
void CellSpace::RegisterNeighbors(const SteeringAgent* pAgent, float neighborhoodRadius)
{
    const Elite::Vector2 agentPos {pAgent->GetPosition()};
    const Elite::Vector2 maxPos{m_SpaceWidth, m_SpaceHeight};

    // agent bounding box
    const Elite::Vector2 bottomLeft{Clamp(agentPos - Elite::Vector2{neighborhoodRadius, neighborhoodRadius}, {}, maxPos)};
    const Elite::Vector2 bottomRight{Clamp(agentPos + Elite::Vector2{neighborhoodRadius, -neighborhoodRadius}, {}, maxPos)};
    const Elite::Vector2 topLeft{Clamp(agentPos + Elite::Vector2{-neighborhoodRadius, neighborhoodRadius}, {}, maxPos)};
    const Elite::Vector2 topRight{Clamp(agentPos + Elite::Vector2{neighborhoodRadius, neighborhoodRadius}, {}, maxPos)};
    
    const int bottomLeftIdx{PositionToIndex(bottomLeft)};
    const int bottomRightIdx{PositionToIndex(bottomRight)};
    const int topLeftIdx{PositionToIndex(topLeft)};
    const int topRightIdx{PositionToIndex(topRight)};
    
    const int colDiff{bottomRightIdx - bottomLeftIdx};
    const bool agentInOneCell{bottomLeftIdx == bottomRightIdx and bottomRightIdx == topLeftIdx and topLeftIdx == topRightIdx};

    // DEBUG PROPERTIES
    if (pAgent == m_pFirstAgent)
    {
        m_DebugProperties.bottomLeftIdx = bottomLeftIdx;
        m_DebugProperties.bottomRightIdx = bottomRightIdx;
        m_DebugProperties.topLeftIdx = topLeftIdx;
        m_DebugProperties.topRightIdx = topRightIdx;
        m_DebugProperties.colDiff = colDiff;
        m_DebugProperties.agentInOneCell = agentInOneCell;
    }

    int nrOfNeighbors{0};
    
    for (int rIdx{bottomLeftIdx}; rIdx <= topRightIdx; rIdx += m_NrOfRows)
    {
        for (int cIdx{0}; cIdx <= colDiff; ++cIdx)
        {
            const int currentIdx{rIdx + cIdx};
            const Elite::Rect& cellBox{m_Cells[currentIdx].boundingBox};
            const Elite::Vector2 cellBottomRight{cellBox.bottomLeft + Elite::Vector2{cellBox.width, 0.0f}};
            const Elite::Vector2 cellTopLeft{cellBox.bottomLeft + Elite::Vector2{0.0f, cellBox.height}};
            const Elite::Vector2 cellTopRight{cellBox.bottomLeft + Elite::Vector2{cellBox.width, cellBox.height}};

            float cx{agentPos.x};
            float cy{agentPos.y};

            if (agentPos.x < cellBox.bottomLeft.x)
            {
                cx = cellBox.bottomLeft.x;
            }
            else if (agentPos.x > cellBottomRight.x)
            {
                cx = cellBottomRight.x;
            }
            if (agentPos.y < cellBox.bottomLeft.y)
            {
                cy = cellBox.bottomLeft.y;
            }
            else if (agentPos.y > cellTopLeft.y)
            {
                cy = cellTopLeft.y;
            }

            const Elite::Vector2 closestPoint{cx, cy};
            
            const float radiusSq{ neighborhoodRadius * neighborhoodRadius };
            const bool isOverlapping {DistanceSquared(closestPoint, agentPos) < radiusSq};
            
            if (not isOverlapping) continue;
            
            const Elite::Vector2 boundingBox[] {cellBox.bottomLeft, cellBottomRight, cellTopRight, cellTopLeft};
            
            if (pAgent == m_pFirstAgent and m_CanRenderPartitions)
            DEBUGRENDERER2D->DrawSolidPolygon(boundingBox, 4, {1.0f, 1.0f, 0.0f}, DEBUGRENDERER2D->NextDepthSlice());
           
            for (const auto& agent : m_Cells[rIdx + cIdx].agents)
            {
                if (agent != pAgent)
                {
                    const float distance{DistanceSquared(agent->GetPosition(), agentPos)};
                    if (distance < neighborhoodRadius * neighborhoodRadius)
                    {
                        m_Neighbors[nrOfNeighbors] = agent;
                        ++nrOfNeighbors;
                    }
                }
            }
        }
    }
    m_NrOfNeighbors = nrOfNeighbors;
}

void CellSpace::EmptyCells()
{
    for (Cell& c : m_Cells)
        c.agents.clear();
}

void CellSpace::DrawCell(int rIdx, int cIdx, const size_t nrOfAgents) const
{
    const float frIdx{static_cast<float>(rIdx)};
    const float fcIdx{static_cast<float>(cIdx)};
    const Elite::Vector2 bottomLeft{m_CellWidth * fcIdx, m_CellHeight * frIdx};
    const Elite::Vector2 bottomRight{m_CellWidth * (fcIdx + 1), m_CellHeight * frIdx};
    const Elite::Vector2 topLeft{m_CellWidth * fcIdx, m_CellHeight * (frIdx + 1)};
    const Elite::Vector2 topRight{m_CellWidth * (fcIdx + 1), m_CellHeight * (frIdx + 1)};
    const Elite::Vector2 cellBoundaries[]
    {
        bottomLeft, bottomRight, topRight, topLeft
    };
            
    // Green cell if there are agents in it, red otherwise
    Elite::Color color{1.0f, 0.0f, 0.0f}; // red
    if (nrOfAgents) color = {0.0f, 1.0f, 0.0f, 0.4f}; // green
    DEBUGRENDERER2D->DrawSolidPolygon(cellBoundaries, 4, color, 0.5f);
}

void CellSpace::DrawNumber(int rIdx, int cIdx, const size_t nrOfAgents) const
{
    const float frIdx{static_cast<float>(rIdx)};
    const float fcIdx{static_cast<float>(cIdx)};
    constexpr float offset{0.5f};
    const Elite::Vector2 textPos{m_CellWidth * fcIdx, m_CellHeight * (1 + frIdx) - offset};
    DEBUGRENDERER2D->DrawString(textPos, std::to_string(nrOfAgents).c_str());
}

/**
 * \brief Render the cells
 */
void CellSpace::RenderCells() const
{
    for (int rIdx{0}; rIdx < m_NrOfRows; ++rIdx)
    {
        const float frIdx{static_cast<float>(rIdx)};
        const Elite::Vector2 rowLine[]{
            Elite::Vector2{0.0f, m_CellHeight * frIdx}, Elite::Vector2{m_SpaceWidth, m_CellHeight * frIdx}
        };
        DEBUGRENDERER2D->DrawPolygon(rowLine, 2, {0.0f, 0.0f, 0.0f}, DEBUGRENDERER2D->NextDepthSlice());
        
        for (int cIdx{0}; cIdx < m_NrOfCols; ++cIdx)
        {
            const float fcIdx{static_cast<float>(cIdx)};
            const Elite::Vector2 colLine[]{
                Elite::Vector2{m_CellWidth * fcIdx, 0.0f}, Elite::Vector2{m_CellWidth * fcIdx, m_SpaceHeight}
            };
            DEBUGRENDERER2D->DrawPolygon(colLine, 2, {0.0f, 0.0f, 0.0f}, DEBUGRENDERER2D->NextDepthSlice());

            const size_t nrOfAgents{m_Cells[rIdx * m_NrOfCols + cIdx].agents.size()};
            DrawNumber(rIdx, cIdx, nrOfAgents);
            DrawCell(rIdx, cIdx, nrOfAgents);
        }
    }
}

/**
 * \brief Calculate the index of the cell based on the position
 * \param pos 
 * \return 
 */
int CellSpace::PositionToIndex(const Elite::Vector2 pos) const
{
    int row{static_cast<int>(pos.y / m_CellWidth)};
    int col{static_cast<int>(pos.x / m_CellHeight)};
    row = Elite::Clamp(row, 0, m_NrOfRows - 1);
    col = Elite::Clamp(col, 0, m_NrOfCols - 1);
    return row * m_NrOfCols + col;
}
