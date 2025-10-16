#pragma once
/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EHeuristicFunctions.h: Supported Heuristic functions in the engine.
// Dependencies: math.h and algorithm
/*=============================================================================*/


namespace Elite
{
	namespace HeuristicFunctions
	{
		//Manhattan distance
		static float Manhattan(float x, float y)
		{
			return float(x + y);
		}

		//Euclidean distance
		static float Euclidean(float x, float y)
		{
			return float(sqrtf(x*x + y*y));
		}

		//Sqrt Euclidean distance
		static float SqEuclidean(float x, float y)
		{
			return float(x*x + y*y);
		}

		//Octile distance
		static float Octile(float x, float y)
		{
			auto f = 0.414213562373095048801f; // == sqrt(2) - 1;
			return float((x < y) ? f * x + y : f * y + x);
		}

		//Chebyshev distance
		static float Chebyshev(float x, float y)
		{
			return max(x, y);
		}
	};
}