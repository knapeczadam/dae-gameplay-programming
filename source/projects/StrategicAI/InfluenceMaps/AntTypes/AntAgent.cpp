#include "stdafx.h"
#include "AntAgent.h"

#undef min

using namespace Elite;

AntAgent::AntAgent(InfluenceMap* pInfluenceMapFood, InfluenceMap* pInfluenceMapHome, float radius)
	:SteeringAgent(radius)
	, m_pInfluenceMap_Home{ pInfluenceMapHome }
	, m_pInfluenceMap_Food{ pInfluenceMapFood }
{
	m_pSeek = new Seek();
	m_pWander = new Wander();
	m_pBlendedSteering = new BlendedSteering({ { m_pWander , 0.f}, { m_pSeek, 1.f } });
	SteeringAgent::SetSteeringBehavior(m_pBlendedSteering);
	SetAutoOrient(true);
	SetMaxAngularSpeed(180.f);
	SetMaxLinearSpeed(20.f);
	SetMass(0.1f);
	SetBodyColor(Color{ 1.f, 0.5f, 0.f });
	SetRotation(Elite::randomFloat(float(M_PI)*2));

	//Don't let agents collide with each otherA
	BaseAgent::m_pRigidBody->RemoveAllShapes();
}

AntAgent::~AntAgent()
{
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pBlendedSteering);
}

void AntAgent::SetWanderAmount(float wanderPct)
{
	wanderPct = Clamp(wanderPct, 0.f, 1.f);
	auto& weightsRef = m_pBlendedSteering->GetWeightedBehaviorsRef();
	weightsRef[0].weight = wanderPct;
	weightsRef[1].weight = 1.f - wanderPct;
}

void AntAgent::SetHasFood(bool hasFood)
{
	if (m_HasFood == hasFood)return;
	m_HasFood = hasFood;

	SetBodyColor(m_HasFood ? Color{0.5f, 1.f, 0.f} : Color{1.f, 0.5f, 0.f});
}

void AntAgent::Update(float deltaTime)
{
	Vector2 pos = GetPosition();

	//1. Determine which influence map to write to (has food or not
	const auto pInfluenceMap = m_HasFood ? m_pInfluenceMap_Food : m_pInfluenceMap_Home;
	//   - Set influence at current position
	pInfluenceMap->SetInfluenceAtPosition(pos, m_influencePerSecond * deltaTime, true);

	//2. Update steering based on influence:
	//   - Determine which influence map to read from
	const auto pSampleMap = m_HasFood ? m_pInfluenceMap_Home : m_pInfluenceMap_Food;
	
	//   - Create 3 sample positions: one straight forward, two on either side
	//   - sample influence on all 3 positions
	Vector2 sampleLeft    = pos + OrientationToVector(GetRotation() + ToRadians(-m_sampleAngle)) * m_sampleDistance;
	Vector2 sampleRight   = pos + OrientationToVector(GetRotation() + ToRadians(m_sampleAngle))  * m_sampleDistance;
	Vector2 sampleForward = pos +                    GetDirection()                              * m_sampleDistance;
	
	std::map<float, Vector2, std::greater<>> influenceMap;
	influenceMap[pSampleMap->GetInfluenceAtPosition(sampleRight)]   = sampleRight;
	influenceMap[pSampleMap->GetInfluenceAtPosition(sampleLeft)]    = sampleLeft;
	influenceMap[pSampleMap->GetInfluenceAtPosition(sampleForward)] = sampleForward;
	
	const Vector2 bestSamplePos = influenceMap.begin()->second;
	m_pSeek->SetTarget(bestSamplePos);

	//Update steering
	SteeringAgent::Update(deltaTime);

	if (GetDebugRenderingEnabled())
	{
		//TODO: DEBUG RENDERING: show the sampled positions

		DEBUGRENDERER2D->DrawCircle(bestSamplePos, 1.0f, { 1.f, 1.f, 0.0f }, DEBUGRENDERER2D->NextDepthSlice());

	}
}

