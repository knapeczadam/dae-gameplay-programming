#include "stdafx.h"

#include "ENavGraphPathfinding.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EAStar.h"
#include "framework/EliteAI/EliteGraphs/EliteNavGraph/ENavGraph.h"

using namespace Elite;

std::vector<Vector2> NavMeshPathfinding::FindPath(Vector2 startPos, Vector2 endPos, NavGraph* pNavGraph, std::vector<Vector2>& debugNodePositions, std::vector<Portal>& debugPortals)
{
    //Create the path to return
    std::vector<Vector2> finalPath{};

    //Get the startTriangle and endTriangle
    const Triangle* startTriangle{pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(startPos)};
    const Triangle* endTriangle{pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(endPos)};

    //If we don't have a valid startTriangle or endTriangle -> return empty path
    if (not startTriangle or not endTriangle) return finalPath;

    //If the startTriangle and endTriangle are the same -> return straight line path
    if (startTriangle == endTriangle)
    {
        finalPath.push_back(startPos);
        finalPath.push_back(endPos);
        return finalPath;
    }

    //=> Start looking for a path
    //Clone the graph (returns shared_ptr!)
    const auto pNavGraphClone{pNavGraph->Clone()};

    auto createConnection = [&](const Triangle* triangle, NavGraphNode* node)
    {
        for (const int lineIdx : triangle->metaData.IndexLines)
        {
            const int nodeId{pNavGraphClone->GetNodeIdFromLineIndex(lineIdx)};
            if (nodeId != invalid_node_id)
            {
                const float distance{Distance(node->GetPosition(), pNavGraphClone->GetNode(nodeId)->GetPosition())};
                const auto connection{new GraphConnection(node->GetId(), nodeId, distance)};
                pNavGraphClone->AddConnection(connection);
            }
        }
    };

    // --- Start node ---
    //Create extra node for the Start Node (Agent's position) and add it to the graph.
    const auto startNode{new NavGraphNode(invalid_node_id, startPos)};
    pNavGraphClone->AddNode(startNode);
    
    //Make connections between the Start Node and the startTriangle nodes.
    createConnection(startTriangle, startNode);

    // --- End node ---
    //Create extra node for the End Node (endpos) and add it to the graph.
    const auto endNode{new NavGraphNode(invalid_node_id, endPos)};
    pNavGraphClone->AddNode(endNode);
    
    //Make connections between the End Node and the endTriangle nodes.
    createConnection(endTriangle, endNode);

    //Run AStar on the new graph
    // Tip: shared_ptr::get() returns the stored pointer
    AStar pathfinder{pNavGraphClone.get(), HeuristicFunctions::Euclidean};
    auto path{pathfinder.FindPath(startNode, endNode)};

    // Save the positions in debugNodePositions for debug rendering!
    debugNodePositions.clear();
    for (const auto node : path)
    {
        // finalPath.push_back(node->GetPosition());
        debugNodePositions.push_back(node->GetPosition());
    }

    //Run optimiser on new graph, MAKE SURE the AStar path is working properly before starting the following section:
    const auto m_Portals{SSFA::FindPortals(path, pNavGraph->GetNavMeshPolygon())};
    debugPortals = m_Portals;
    finalPath = SSFA::OptimizePortals(m_Portals);

    return finalPath;
}

std::vector<Vector2> NavMeshPathfinding::FindPath(Vector2 startPos, Vector2 endPos, NavGraph* pNavGraph)
{
    std::vector<Vector2> debugNodePositions{};
    std::vector<Portal> debugPortals{};
    return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}
