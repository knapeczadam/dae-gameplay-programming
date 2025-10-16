#pragma once
#include "stdafx.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphNode.h"

namespace Elite
{
	class ConnectionCostCalculator
	{
	public:
		ConnectionCostCalculator() = default;
		virtual ~ConnectionCostCalculator() = default;

		virtual float CalculateConnectionCost(const Graph* pGraph, int fromNodeId, int toNodeId) const = 0;
	};

}