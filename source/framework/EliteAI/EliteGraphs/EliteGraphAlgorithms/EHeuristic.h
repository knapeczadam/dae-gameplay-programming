#pragma once
/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ENavigation.h: General Navigation header that includes all navigation pathfinders and utilities
/*=============================================================================*/


/* --- TYPE DEFINES --- */
//Function pointer type to heuristic function
namespace Elite
{
	typedef float(*Heuristic)(float, float);
}

/* --- UTILITIES --- */
//Utilities
#include "EHeuristicFunctions.h"
