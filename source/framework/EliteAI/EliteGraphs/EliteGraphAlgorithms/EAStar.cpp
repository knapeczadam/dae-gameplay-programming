#include "stdafx.h"
#include "EAStar.h"

using namespace Elite;

AStar::AStar(Graph* pGraph, Heuristic hFunction)
    : m_pGraph(pGraph)
      , m_HeuristicFunction(hFunction)
{
}

std::vector<GraphNode*> AStar::FindPath(GraphNode* pStartNode, GraphNode* pGoalNode)
{
    return FindPathV2(pStartNode, pGoalNode);
    std::vector<GraphNode*> path{};
    std::vector<NodeRecord> openList{};
    std::vector<NodeRecord> closedList{};

    const NodeRecord startRecord{pStartNode, nullptr, 0.f, GetHeuristicCost(pStartNode, pGoalNode)};
    openList.push_back(startRecord);
    NodeRecord currentNodeRecord{};

    while (not openList.empty())
    {
        // get record from the open list with lowest F-score
        currentNodeRecord = *std::min_element(openList.begin(), openList.end());

        // check if that record refers to the end node
        if (currentNodeRecord.pNode == pGoalNode)
        {
            break;
        }

        // else get all the connections of the NodeRecord's node
        auto connections = m_pGraph->GetConnectionsFromNode(currentNodeRecord.pNode);

        // for each of the connections
        for (auto connection : connections)
        {
            GraphNode* pNextNode = m_pGraph->GetNode(connection->GetToNodeId());
            // total G-cost so far
            const float costSoFar = currentNodeRecord.costSoFar + connection->GetCost();
            //	check if the connection leads to a node that is already in the closed list

            // --- CLOSED LIST ---
            auto it = std::ranges::find_if(closedList, [&](const NodeRecord& nodeRecord)
            {
                return nodeRecord.pNode == pNextNode;
            });
            // check if the existing record is cheaper
            if (it != closedList.end())
            {
                if (it->costSoFar <= costSoFar)
                {
                    continue;
                }
                // remove it from the closed list
                closedList.erase(it);
            }

            // --- OPEN LIST ---
            // check if the connection leads to a node that is already in the open list
            it = std::ranges::find_if(openList, [&](const NodeRecord& nodeRecord)
            {
                return nodeRecord.pNode == pNextNode;
            });
            // check if the existing record is cheaper
            if (it != openList.end())
            {
                if (it->costSoFar <= costSoFar)
                {
                    continue;
                }
                // remove it from the closed list
                openList.erase(it);
            }

            // at this point any expensive connection should be removed (if it existed)
            // we create a new NodeRecord for the connection and add it to the open list
            NodeRecord newRecord{};
            newRecord.pNode = pNextNode;
            newRecord.pConnection = connection;
            newRecord.costSoFar = costSoFar;
            newRecord.estimatedTotalCost = costSoFar + GetHeuristicCost(newRecord.pNode, pGoalNode);
            openList.push_back(newRecord);
        }
        // remove the current node from the open list and add it to the closed list
        openList.erase(std::ranges::find(openList, currentNodeRecord));
        closedList.push_back(currentNodeRecord);
    }

    // recunstruct path from the last connection to start node
    while (currentNodeRecord.pNode != pStartNode)
    {
        path.push_back(currentNodeRecord.pNode);
        currentNodeRecord = *std::ranges::find_if(closedList, [&](const NodeRecord& nodeRecord)
        {
            return nodeRecord.pNode == m_pGraph->GetNode(currentNodeRecord.pConnection->GetFromNodeId());
        });
    }
    path.push_back(pStartNode);

    std::ranges::reverse(path);
    
    // check if path contains the end node
    // if not, agent stands still
    if (path.back() != pGoalNode)
    {
        path.clear();
        path.push_back(pStartNode);
    }
    
    return path;
}

/**
 * \brief Pseudo code: https://youtu.be/-L-WgKMFuhE?t=537
 * \param pStartNode 
 * \param pGoalNode 
 * \return 
 */
std::vector<GraphNode*> AStar::FindPathV2(GraphNode* pStartNode, GraphNode* pGoalNode)
{
    std::vector<GraphNode*> path{};
    std::vector<NodeRecord> openList{};
    std::vector<NodeRecord> closedList{};

    const NodeRecord startRecord{pStartNode, nullptr, 0.f, GetHeuristicCost(pStartNode, pGoalNode)};
    openList.push_back(startRecord);

    NodeRecord currentNodeRecord{};

    while (not openList.empty())
    {
        currentNodeRecord = *std::min_element(openList.begin(), openList.end());
        openList.erase(std::ranges::find(openList, currentNodeRecord));
        closedList.push_back(currentNodeRecord);

        if (currentNodeRecord.pNode == pGoalNode)
        {
            break;
        }

        auto connections = m_pGraph->GetConnectionsFromNode(currentNodeRecord.pNode);
        for (const auto connection : connections)
        {
            GraphNode* pNextNode{m_pGraph->GetNode(connection->GetToNodeId())};
            auto it{
                std::ranges::find_if(closedList, [&](const NodeRecord& nodeRecord)
                {
                    return nodeRecord.pNode == pNextNode;
                })
            };
            if (it != closedList.end())
            {
                continue;
            }
            // if new path to neighbour is shorter OR neighbour is not in open
            it = std::ranges::find_if(openList, [&](const NodeRecord& nodeRecord)
            {
                return nodeRecord.pNode == pNextNode;
            });
            if (it == openList.end())
            {
                NodeRecord newRecord{};
                newRecord.pNode = pNextNode;
                newRecord.pConnection = connection;
                newRecord.costSoFar = currentNodeRecord.costSoFar + connection->GetCost();
                newRecord.estimatedTotalCost = newRecord.costSoFar + GetHeuristicCost(newRecord.pNode, pGoalNode);
                openList.push_back(newRecord);
            }
            else if (currentNodeRecord.costSoFar + connection->GetCost() < it->costSoFar)
            {
                it->costSoFar = currentNodeRecord.costSoFar + connection->GetCost();
                it->pConnection = connection;
                it->estimatedTotalCost = it->costSoFar + GetHeuristicCost(it->pNode, pGoalNode);
            }
        }
    }

    while (currentNodeRecord.pNode != pStartNode)
    {
        path.push_back(currentNodeRecord.pNode);
        currentNodeRecord = *std::ranges::find_if(closedList, [&](const NodeRecord& nodeRecord)
        {
            return nodeRecord.pNode->GetId() == currentNodeRecord.pConnection->GetFromNodeId();
        });
    }
    path.push_back(pStartNode);
    std::ranges::reverse(path);
    
    // check if path contains the end node
    // if not, agent stands still
    if (path.back() != pGoalNode)
    {
        path.clear();
        path.push_back(pStartNode);
    }
    
    return path;
}


float AStar::GetHeuristicCost(GraphNode* pStartNode, GraphNode* pEndNode) const
{
    Vector2 toDestination = m_pGraph->GetNodePos(pEndNode->GetId()) - m_pGraph->GetNodePos(pStartNode->GetId());
    return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
}
