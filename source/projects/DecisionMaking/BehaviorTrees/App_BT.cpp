//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_BT.h"
#include "projects/Shared/NavigationColliderElement.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphRenderer.h"

#include "projects/Movement/SteeringBehaviors/SteeringAgent.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"

#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/ENavGraphPathfinding.h"
#include "framework/EliteAI/EliteGraphs/EliteNavGraph/ENavGraph.h"
#include "Behaviors.h"

using namespace Elite;

//Statics
bool App_BT::sShowPolygon              = false;
bool App_BT::sDrawPlayerPath           = false;
bool App_BT::sDrawPatrolPath           = true;
bool App_BT::sDrawDetectionRadius      = true;
bool App_BT::sDrawAgentGuardSegment    = true;
bool App_BT::sDrawGuardViewCone        = true;
bool App_BT::sDrawGuardForwardVelocity = true;
bool App_BT::sDrawPlayerLastSeenPos    = true;

//Destructor
App_BT::~App_BT()
{
	for (auto pNC : m_vNavigationColliders)
		SAFE_DELETE(pNC);
	m_vNavigationColliders.clear();

	SAFE_DELETE(m_pNavGraph);
	SAFE_DELETE(m_pPlayerPathFollow);
	SAFE_DELETE(m_pGuardPathFollow);
	SAFE_DELETE(m_pPlayer);
	SAFE_DELETE(m_pGuard);
}

//Functions
void App_BT::Start()
{
	//Initialization of your application. 
	//----------- CAMERA ------------
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(36.782f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(12.9361f, 0.2661f));

	//----------- WORLD ------------
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(15.f, 0.f), 3.0f, 15.0f));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-15.f, 0.f), 3.0f, 15.0f));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(0.f, 15.f), 15.0f, 3.0f));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(0.f, -15.f), 15.0f, 3.0f));

	//----------- NAVMESH  ------------

	const auto& shapes = PHYSICSWORLD->GetAllStaticShapesInWorld(PhysicsFlags::NavigationCollider);
	m_pNavGraph = new Elite::NavGraph(shapes, 120,60, m_AgentRadius);

	//----------- PLAYER AGENT ------------
	m_pPlayerPathFollow = new PathFollow();
	m_Target = TargetData(Elite::ZeroVector2);
	m_pPlayer = new SteeringAgent(m_AgentRadius);
	m_pPlayer->SetPosition(Elite::Vector2{ -20.f, -20.f });
	m_pPlayer->SetSteeringBehavior(m_pPlayerPathFollow);
	m_pPlayer->SetMaxLinearSpeed(m_PlayerSpeed);
	m_pPlayer->SetAutoOrient(true);
	m_pPlayer->SetMass(0.1f);

	//----------- PATROL PATH  ------------
	m_PatrolPath = { 
		Elite::Vector2(20.f, 20.f), 
		Elite::Vector2(-20.f, 20.f), 
		Elite::Vector2(-20.f, -20.f),
		Elite::Vector2(20.f, -20.f) 
	};

	//----------- GUARD (AI) AGENT ------------
	m_pGuardPathFollow = new PathFollow();
	m_pGuard = new SmartAgent(m_AgentRadius);
	m_pGuard->SetPosition(Elite::Vector2{ 20.f, 0.f });
	m_pGuard->SetSteeringBehavior(m_pGuardPathFollow);
	m_pGuard->SetMaxLinearSpeed(m_GuardSpeed);
	m_pGuard->SetAutoOrient(true);
	m_pGuard->SetMass(0.1f);

	//----------- AI SETUP ------------
	// BT TODO:
	//Create and add the necessary blackboard data
	//1. Create Blackboard
	m_pBlackboard = CreateBlackboard(m_pGuard);

	//2. Create BehaviorTree (make more conditions/actions and create a more advanced tree than the simple agents
	// Curious to see who can fully atomise the tree!
	// BehaviorTree* pBT = new BehaviorTree(pBlackboard, new BehaviorAction(BT_Actions::ChangeToPatrol));
	BehaviorTree* pBT = new BehaviorTree(m_pBlackboard, new BehaviorSelector({
		                                     new BehaviorSequence({
			                                     new BehaviorConditional(BT_Conditions::IsTargetVisible),
			                                     new BehaviorAction(BT_Actions::SavePlayerPosition),
			                                     new BehaviorAction(BT_Actions::ChangeToChase)
		                                     }),
		                                     new BehaviorSequence({
			                                     new BehaviorConditional(BT_Conditions::IsChasing),
			                                     new BehaviorConditional(BT_Conditions::ShouldSearch),
			                                     new BehaviorConditional(BT_Conditions::IsTargetReached),
			                                     new BehaviorAction(BT_Actions::ChangeToSearch)
		                                     }),
		                                     new BehaviorSequence({
			                                     new BehaviorConditional(BT_Conditions::IsSearching),
			                                     new BehaviorAction(BT_Actions::Search),
			                                     new BehaviorConditional(BT_Conditions::IsSearchingTooLong),
			                                     new BehaviorAction(BT_Actions::ResetSearchTime),
			                                     new BehaviorAction(BT_Actions::ChangeToPatrol)
		                                     }),
		                                     new BehaviorSequence({
			                                     new BehaviorConditional(BT_Conditions::IsNotSearching), // Inverter?
			                                     new BehaviorConditional(BT_Conditions::IsNotChasing), // Inverter?
			                                     new BehaviorAction(BT_Actions::ChangeToPatrol),
		                                     })
	                                     })
	);

	//3. Set the BehaviorTree active on the agent
	m_pGuard->SetDecisionMaking(pBT);
}

void App_BT::Update(float deltaTime)
{
	//Update target/path based on input
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
	{
		auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		Elite::Vector2 mouseTarget = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld(
			Elite::Vector2((float)mouseData.X, (float)mouseData.Y));
		m_vPath = NavMeshPathfinding::FindPath(m_pPlayer->GetPosition(), mouseTarget, m_pNavGraph);

		//Check if a path exist and move to the following point
		if (m_vPath.size() > 0)
		{
			m_pPlayerPathFollow->SetPath(m_vPath);
		}
	}
	
#pragma region GuardViewCone
	const float halfAngle{Elite::ToRadians(m_GuardViewAngle / 2.0f)};

	const Vector2 dir{m_pGuard->GetLinearVelocity().GetNormalized()};

	const float leftX{std::cos(halfAngle) * dir.x - std::sin(halfAngle) * dir.y};
	const float leftY{std::sin(halfAngle) * dir.x + std::cos(halfAngle) * dir.y};
	const float rightX{std::cos(-halfAngle) * dir.x - std::sin(-halfAngle) * dir.y};
	const float rightY{std::sin(-halfAngle) * dir.x + std::cos(-halfAngle) * dir.y};

	const Vector2 left{m_pGuard->GetPosition() + Vector2(leftX, leftY) * m_GuardViewDistance};
	const Vector2 right{m_pGuard->GetPosition() + Vector2(rightX, rightY) * m_GuardViewDistance};

	DebugHelper* pDebugHelper = nullptr;
	m_pBlackboard->GetData("DebugHelper", pDebugHelper);
	pDebugHelper->m_GuardViewConeLeft = left;
	pDebugHelper->m_GuardViewConeRight = right;
	m_pBlackboard->ChangeData("DebugHelper", pDebugHelper);
#pragma endregion

	// Update player's visibility
    pDebugHelper->m_IsPlayerBehindBlock = not m_pGuard->HasLineOfSight(m_pPlayer->GetPosition());
    m_pBlackboard->ChangeData("DebugHelper", pDebugHelper);
	
	// --Time--
	m_pBlackboard->ChangeData("DeltaTime", deltaTime);
	
	UpdateImGui();
	m_pPlayer->Update(deltaTime);
	m_pGuard->Update(deltaTime);
}

void App_BT::Render(float deltaTime) const
{
	if (sShowPolygon)
	{
		DEBUGRENDERER2D->DrawPolygon(m_pNavGraph->GetNavMeshPolygon(),
			Color(0.1f, 0.1f, 0.1f));
		DEBUGRENDERER2D->DrawSolidPolygon(m_pNavGraph->GetNavMeshPolygon(),
			Color(0.0f, 0.5f, 0.1f, 0.05f), DEBUGRENDERER2D->NextDepthSlice());
	}

	if (sDrawPlayerPath && m_vPath.size() > 0)
	{
		for (auto pathPoint : m_vPath)
			DEBUGRENDERER2D->DrawCircle(pathPoint, 2.0f, Color(1.f, 0.f, 0.f), -0.2f);
	
		//DEBUGRENDERER2D->DrawSegment(m_pAgent->GetPosition(), m_vPath[0], Color(1.f, 0.0f, 0.0f), -0.2f);
		for (size_t i = 0; i < m_vPath.size() - 1; i++)
		{
			float g = float(i) / m_vPath.size();
			DEBUGRENDERER2D->DrawSegment(m_vPath[i], m_vPath[i+1], Color(1.f, g, g), -0.2f);
		}	
	}

	if (sDrawPatrolPath && !m_PatrolPath.empty())
	{
		for (auto pathPoint : m_PatrolPath)
			DEBUGRENDERER2D->DrawCircle(pathPoint, 2.0f, Color(0.f, 0.f, 1.f), -0.2f);

		//DEBUGRENDERER2D->DrawSegment(m_pAgent->GetPosition(), m_vPath[0], Color(1.f, 0.0f, 0.0f), -0.2f);
		for (size_t i = 0; i < m_PatrolPath.size(); i++)
		{
			DEBUGRENDERER2D->DrawSegment(m_PatrolPath[i], m_PatrolPath[(i + 1) % m_PatrolPath.size()], Color(0.f, 0.f, 1.f), -0.2f);
		}
	}

	if (sDrawDetectionRadius)
	{
		DEBUGRENDERER2D->DrawCircle(m_pGuard->GetPosition(), m_DetectionRadius, Color(0.f, 0.f, 1.f), -0.2f);
	}
	
	if (sDrawAgentGuardSegment)
	{
		if (m_DebugHelper.m_IsPlayerBehindBlock)
		{
			DEBUGRENDERER2D->DrawSegment(m_pPlayer->GetPosition(), m_pGuard->GetPosition(), Color(1.f, 0.f, 0.f), -0.2f);
		}
		else
		{
			DEBUGRENDERER2D->DrawSegment(m_pPlayer->GetPosition(), m_pGuard->GetPosition(), Color(0.f, 1.f, 0.f), -0.2f);
		}
	}

	if (sDrawGuardViewCone)
	{
		DEBUGRENDERER2D->DrawSegment(m_pGuard->GetPosition(), m_DebugHelper.m_GuardViewConeLeft, Color(1.f, 0.f, 0.f), -0.2f);
		DEBUGRENDERER2D->DrawSegment(m_pGuard->GetPosition(), m_DebugHelper.m_GuardViewConeRight, Color(1.f, 0.f, 0.f), -0.2f);
		DEBUGRENDERER2D->DrawSegment(m_DebugHelper.m_GuardViewConeLeft, m_DebugHelper.m_GuardViewConeRight, Color(1.f, 0.f, 0.f), -0.2f);
	}

	if (sDrawGuardForwardVelocity)
	{
		DEBUGRENDERER2D->DrawSegment(m_pGuard->GetPosition(), m_pGuard->GetPosition() + m_pGuard->GetLinearVelocity(), Color(1.f, 0.f, 0.f), -0.3f);
	}

	if (sDrawPlayerLastSeenPos)
	{
		TargetData* lastSeenPos = nullptr;
		m_pBlackboard->GetData("Target", lastSeenPos);
		DEBUGRENDERER2D->DrawCircle(lastSeenPos->Position, 1.0f, Color(0.f, 0.f, 0.5f), -0.2f);
	}

	m_pPlayer->Render(deltaTime);
	m_pGuard->Render(deltaTime);
}

Elite::Blackboard* App_BT::CreateBlackboard(SmartAgent* smartAgent)
{
	// using namespace std::chrono; // Hint hint

	Blackboard* pBlackboard = new Blackboard();
	
	// --Time--
	pBlackboard->AddData("DeltaTime", 0.0f);
	
	// --Agents--
	pBlackboard->AddData("Player", m_pPlayer);
	pBlackboard->AddData("Guard", m_pGuard);
	pBlackboard->AddData("PlayerPathFollow", m_pPlayerPathFollow);
	pBlackboard->AddData("GuardPathFollow", m_pGuardPathFollow);
	pBlackboard->AddData("Target", &m_Target);
	pBlackboard->AddData("AgentRadius", m_AgentRadius);
	pBlackboard->AddData("PlayerSpeed", m_PlayerSpeed);
	pBlackboard->AddData("GuardSpeed", m_GuardSpeed);
	pBlackboard->AddData("DetectionRadius", m_DetectionRadius);
	pBlackboard->AddData("MaxSearchTime", m_MaxSearchTime);
	pBlackboard->AddData("AccuSearchTime", m_AccuSearchTime);
	pBlackboard->AddData("GuardViewAngle", m_GuardViewAngle);
	pBlackboard->AddData("GuardViewDistance", m_GuardViewDistance);
	pBlackboard->AddData("DebugHelper", &m_DebugHelper);

	// --Level--
	pBlackboard->AddData("NavigationColliders", m_vNavigationColliders);

	// --Pathfinder--
	pBlackboard->AddData("PlayerPath", m_vPath);
	pBlackboard->AddData("PatrolPath", m_PatrolPath);

	// --Graph--
	pBlackboard->AddData("NavGraph", m_pNavGraph);

	return pBlackboard;
}

void App_BT::UpdateImGui()
{
	//------- UI --------
#ifdef PLATFORM_WINDOWS
#pragma region UI
	{
		//Setup
		int menuWidth = 150;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 90));
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::PushAllowKeyboardFocus(false);
		ImGui::SetWindowFocus();
		ImGui::PushItemWidth(70);
		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
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

		ImGui::Checkbox("Show Polygon", &sShowPolygon);
		ImGui::Checkbox("Show Player Path", &sDrawPlayerPath);
		ImGui::Checkbox("Show Patrol Path", &sDrawPatrolPath);
		ImGui::Checkbox("Show Detection Radius", &sDrawDetectionRadius);
		ImGui::Checkbox("Show Agent-Guard Segment", &sDrawAgentGuardSegment);
		ImGui::Checkbox("Show Guard View Cone", &sDrawGuardViewCone);
		ImGui::Checkbox("Show Guard Forward Velocity", &sDrawGuardForwardVelocity);
		ImGui::Checkbox("Show Player Last Seen Position", &sDrawPlayerLastSeenPos);
		ImGui::Separator();
		ImGui::Checkbox("Enable Guard View Cone", &m_DebugHelper.m_EnableGuardViewCone);
		ImGui::Checkbox("Can player hide", &m_DebugHelper.m_CanPlayerHide);
		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::SliderFloat("PlayerSpeed", &m_PlayerSpeed, 0.0f, 22.0f))
		{
			m_pPlayer->SetMaxLinearSpeed(m_PlayerSpeed);
		}

		if (ImGui::SliderFloat("GuardSpeed", &m_GuardSpeed, 0.0f, 22.0f))
		{
			m_pGuard->SetMaxLinearSpeed(m_GuardSpeed);
		}
		
		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif
}