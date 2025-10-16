//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_Flocking.h"
#include "../SteeringAgent.h"
#include "Flock.h"

using namespace Elite;

//Destructor
App_Flocking::~App_Flocking()
{	
	SAFE_DELETE(m_pFlock);
	SAFE_DELETE(m_pAgentToEvade);
	SAFE_DELETE(m_pWander);
}

//Functions
void App_Flocking::Start()
{
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(55.0f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(m_WorldSize / 1.5f, m_WorldSize / 2));

	m_pWander = new Wander();
	m_pAgentToEvade = new SteeringAgent();
	m_pAgentToEvade->SetSteeringBehavior(m_pWander);
	m_pAgentToEvade->SetPosition({ m_WorldSize / 2, m_WorldSize / 2 });
	m_pAgentToEvade->SetSteeringBehavior(m_pWander);
	m_pAgentToEvade->SetMaxLinearSpeed(5.f);
	m_pAgentToEvade->SetAutoOrient(true);
	m_pAgentToEvade->SetMass(1.0f);
	
	m_pFlock = new Flock(m_FlockSize, m_WorldSize, m_pAgentToEvade, true);
}

void App_Flocking::Update(float deltaTime)
{
	//INPUT
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft) && m_VisualizeMouseTarget)
	{
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eLeft);
		m_MouseTarget.Position = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) });
	}
	m_pAgentToEvade->Update(deltaTime);
	m_pAgentToEvade->TrimToWorld(m_WorldSize);
	
	m_pFlock->UpdateAndRenderUI();
	m_pFlock->UpdateWorldSize(&m_WorldSize);
	m_pFlock->Update(deltaTime);
	
	if (m_UseMouseTarget)
		m_pFlock->SetTarget_Seek(m_MouseTarget);
	if (m_pAgentToEvade)
		m_pFlock->SetTarget_Evade(m_pAgentToEvade->GetPosition());

}

void App_Flocking::Render(float deltaTime) const
{
	if (m_pFlock->TrimWorld())
	{
		RenderWorldBounds(m_WorldSize);
	}

	m_pAgentToEvade->Render(deltaTime);
	m_pFlock->Render(deltaTime);
	
	//Render Target
	if(m_VisualizeMouseTarget)
		DEBUGRENDERER2D->DrawSolidCircle(m_MouseTarget.Position, 0.3f, { 0.f,0.f }, { 1.f,0.f,0.f },-0.8f);
}
