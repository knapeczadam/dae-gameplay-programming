//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_CombinedSteering.h"
#include "../SteeringAgent.h"
#include "CombinedSteeringBehaviors.h"
#include "projects\Movement\SteeringBehaviors\Obstacle.h"

using namespace Elite;
App_CombinedSteering::~App_CombinedSteering()
{	
	SAFE_DELETE(m_pAgent1);
	SAFE_DELETE(m_pAgent1Seek);
	SAFE_DELETE(m_pAgent1Wander);
	SAFE_DELETE(m_pAgent1Steering);

	SAFE_DELETE(m_pAgent2);
	SAFE_DELETE(m_pAgent2Evade);
	SAFE_DELETE(m_pAgent2Wander);
	SAFE_DELETE(m_pAgent2Steering);
}

void App_CombinedSteering::Start()
{
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(55.0f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(m_TrimWorldSize / 1.5f, m_TrimWorldSize / 2));

	//Agent 1
	m_pAgent1Wander = new Wander();
	m_pAgent1Seek = new Seek();
	// drunk seek
	m_pAgent1 = new SteeringAgent();
	m_pAgent1->SetMaxLinearSpeed(15.f);
	m_pAgent1->SetAutoOrient(true);
	m_pAgent1->SetMass(1.f);

	//Agent 2
	m_pAgent2Wander = new Wander();
	m_pAgent2Evade = new Evade();
	m_pAgent1Flee = new Flee();
	// m_pAgent2Steering = new PrioritySteering({ m_pAgent2Evade, m_pAgent2Wander });

	m_pAgent1Steering = new BlendedSteering({
		BlendedSteering::WeightedBehavior{m_pAgent1Seek, 0.5f},
		BlendedSteering::WeightedBehavior{m_pAgent1Flee, 0.5f},
	});
	m_pAgent1->SetSteeringBehavior(m_pAgent1Steering);
	// m_pAgent2 = new SteeringAgent();
	// m_pAgent2->SetSteeringBehavior(m_pAgent2Steering);
	// m_pAgent2->SetMaxLinearSpeed(15.f);
	// m_pAgent2->SetAutoOrient(true);
	// m_pAgent2->SetMass(1.f);
	// m_pAgent2->SetBodyColor({0.f, 1.f, 0.f});

}

void App_CombinedSteering::Update(float deltaTime)
{
	//INPUT
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft) && m_VisualizeMouseTarget)
	{
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eLeft);
		m_MouseTarget.Position = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) });
	}

#ifdef PLATFORM_WINDOWS
	#pragma region UI
	//UI
	{
		//Setup
		int const menuWidth = 235;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::PushAllowKeyboardFocus(false);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Checkbox("Debug Rendering", &m_CanDebugRender);
		ImGui::Checkbox("Trim World", &m_TrimWorld);
		if (m_TrimWorld)
		{
			ImGui::SliderFloat("Trim Size", &m_TrimWorldSize, 0.f, 500.f, "%1.");
		}
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Behavior Weights");
		ImGui::Spacing();
		
		ImGui::SliderFloat("Wander", &m_pAgent1Steering->GetWeightedBehaviorsRef()[0].weight, 0.f, 1.f, "%.2");
		ImGui::SliderFloat("Seek", &m_pAgent1Steering->GetWeightedBehaviorsRef()[1].weight, 0.f, 1.f, "%.2");

		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
	#pragma endregion
#endif
	//Agent 1
	m_pAgent1Seek->SetTarget(m_MouseTarget);
	m_pAgent1->SetDebugRenderingEnabled(m_CanDebugRender);
	m_pAgent1->Update(deltaTime);
	m_pAgent1->TrimToWorld(m_TrimWorldSize);

	//Agent 2
	TargetData target{};
	target.Position = m_pAgent1->GetPosition();
	target.LinearVelocity = m_pAgent1->GetLinearVelocity();

	m_pAgent2Evade->SetTarget(target);
	m_pAgent1Flee->SetTarget(target);
	// m_pAgent2->SetDebugRenderingEnabled(m_CanDebugRender);
	// m_pAgent2->Update(deltaTime);
	// m_pAgent2->TrimToWorld(m_TrimWorldSize);
}

void App_CombinedSteering::Render(float deltaTime) const
{

	if (m_TrimWorld)
	{
		RenderWorldBounds(m_TrimWorldSize);
	}

	m_pAgent1->Render(deltaTime);
	// m_pAgent2->Render(deltaTime);

	//Render Target
	if(m_VisualizeMouseTarget)
		DEBUGRENDERER2D->DrawSolidCircle(m_MouseTarget.Position, 0.3f, { 0.f,0.f }, { 1.f,0.f,0.f },-0.8f);
}
