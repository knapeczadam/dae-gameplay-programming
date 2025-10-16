#include "stdafx.h"
#include "FoodSource.h"


using namespace Elite;
FoodSource::FoodSource(Vector2 pos, int amount): 
	m_position{pos},
	m_amount{amount}
{
}

FoodSource::~FoodSource()
{
}

void FoodSource::Render(float dt) const
{
	DEBUGRENDERER2D->DrawSolidCircle(m_position, 0.5f, { 1.f, 0.f }, { 0.f, 1.0f, 0.f }, DEBUGRENDERER2D->NextDepthSlice());
	std::stringstream stream;
	stream << m_amount;
	DEBUGRENDERER2D->DrawString(m_position, stream.str().c_str());
}
