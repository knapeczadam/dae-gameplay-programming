#include "stdafx.h"
#include "Flock.h"

#include "../SteeringAgent.h"
#include "../Steering/SteeringBehaviors.h"
#include "../CombinedSteering/CombinedSteeringBehaviors.h"
#include "../SpacePartitioning/SpacePartitioning.h"

using namespace Elite;
//Constructor & Destructor
Flock::Flock(
    int flockSize /*= 50*/,
    float worldSize /*= 100.f*/,
    SteeringAgent* pAgentToEvade /*= nullptr*/,
    bool trimWorld /*= false*/)

    : m_FlockSize{flockSize}
      , m_WorldSize{worldSize}
      , m_pAgentToEvade{pAgentToEvade}
      , m_TrimWorld{trimWorld}
{
    m_Agents.reserve(m_FlockSize);

    m_pSeparationBehavior = new Separation(this);
    m_pCohesionBehavior   = new Cohesion(this);
    m_pVelMatchBehavior   = new VelocityMatch(this);
    m_pSeekBehavior       = new Seek();
    m_pWanderBehavior     = new Wander();
    m_pEvadeBehavior      = new Evade();

    m_pBlendedSteering =
        new BlendedSteering({
                BlendedSteering::WeightedBehavior{m_pSeparationBehavior, 0.5f},
                BlendedSteering::WeightedBehavior{m_pCohesionBehavior, 0.2f},
                BlendedSteering::WeightedBehavior{m_pVelMatchBehavior, 0.3f},
                BlendedSteering::WeightedBehavior{m_pSeekBehavior, 0.2f},
                BlendedSteering::WeightedBehavior{m_pWanderBehavior, 0.1f}
            }
        );
    
    m_pPrioritySteering = new PrioritySteering( {m_pEvadeBehavior, m_pBlendedSteering});
    m_pCellSpace = new CellSpace(m_WorldSize, m_WorldSize, m_WorldRows, m_WorldCols, m_FlockSize);
    
    for (int idx{0}; idx < m_FlockSize; ++idx)
    {
        auto agent = new SteeringAgent();
        agent->SetSteeringBehavior(m_pPrioritySteering);
        agent->SetPosition({randomFloat(0.f, m_WorldSize), randomFloat(0.f, m_WorldSize)});
        agent->SetPreviousPosition(agent->GetPosition());
        agent->SetAutoOrient(true);
        agent->SetMaxLinearSpeed(50.0f);
        agent->SetMass(1.0f);
        
        m_Agents.emplace_back(agent);
        m_pCellSpace->AddAgent(agent);
    }
    m_Neighbors.resize(m_FlockSize);
    m_pCellSpace->SetFirstAgent(m_Agents[0]);
    std::cout << "Flock created" << std::endl;
}

Flock::~Flock()
{
    SAFE_DELETE(m_pSeparationBehavior);
    SAFE_DELETE(m_pCohesionBehavior);
    SAFE_DELETE(m_pVelMatchBehavior);
    SAFE_DELETE(m_pSeekBehavior);
    SAFE_DELETE(m_pWanderBehavior);
    SAFE_DELETE(m_pEvadeBehavior);
    SAFE_DELETE(m_pBlendedSteering);
    SAFE_DELETE(m_pPrioritySteering);
    
    for (auto& pAgent : m_Agents)
    {
        SAFE_DELETE(pAgent);
    }
    
    m_Agents.clear();
    m_Neighbors.clear();

    SAFE_DELETE(m_pCellSpace);
}

/**
 * \brief Update all the agents in the flock
 * register the neighbors for every agent
 * update every agent
 * trim every agent to the world
 */
void Flock::Update(float deltaT)
{
    for (const auto& agent : m_Agents)
    {
        RegisterNeighbors(agent);
        agent->Update(deltaT);
        if (m_TrimWorld)
        {
            agent->TrimToWorld(m_WorldSize);
        }
        m_pCellSpace->AgentPositionChanged(agent, agent->GetPreviousPosition());
        agent->SetPreviousPosition(agent->GetPosition());
    }
}

/**
 * \brief Render all the agents in the flock
 */
void Flock::Render(float deltaT)
{
    
    if (m_CanRenderNeighborhood)
    {
        RenderNeighborhood();
    }
    if (m_CanRenderNeighborhoodRadius)
    {
        DEBUGRENDERER2D->DrawCircle(m_Agents[0]->GetPosition(), m_NeighborhoodRadius, {0.f, 1.f, 1.f}, 0.4f);
    }
    
    m_Agents[0]->SetDebugRenderingEnabled(m_CanDebugRender);
    m_Agents[0]->SetBodyColor({1.0f, 0.0f, 1.0f});
    
    for (const auto& agent : m_Agents)
    {
        agent->Render(deltaT);
        agent->SetBodyColor({0.f, 1.f, 0.f});
    }

    if (m_CanRenderPartitions)
    {
        m_pCellSpace->RenderCells();
    }
}

void Flock::UpdateAndRenderUI()
{
    //Setup
    constexpr  float menuWidth = 235.0f;
    const float width = static_cast<float>(DEBUGRENDERER2D->GetActiveCamera()->GetWidth());
    const float height = static_cast<float>(DEBUGRENDERER2D->GetActiveCamera()->GetHeight());
    bool windowActive = true;
    ImGui::SetNextWindowPos(ImVec2(width - menuWidth - 10, 10));
    ImGui::SetNextWindowSize(ImVec2(menuWidth, height - 20));
    ImGui::Begin("Gameplay Programming", &windowActive,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::PushAllowKeyboardFocus(false);

    //Elements
    ImGui::Text("CONTROLS");
    ImGui::Indent();
    ImGui::Text("LMB: place target");
    ImGui::Text("RMB: move cam.");
    ImGui::Text("Scrollwheel: zoom cam.");
    ImGui::Unindent();
    
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("CAMERA");
    ImGui::Indent();
    ImGui::Text("Zoom: %.2f", DEBUGRENDERER2D->GetActiveCamera()->GetZoom());
    ImGui::Unindent();
    ImGui::Checkbox("Focus on First Agent", &m_FocusOnFirstAgent);
    ImGui::Checkbox("Focus on Evaded Agent", &m_FocusOnAgentToEvade);
    if (m_FocusOnFirstAgent)
    {
        DEBUGRENDERER2D->GetActiveCamera()->SetCenter(m_Agents[0]->GetPosition());
    }
    else if (m_FocusOnAgentToEvade)
    {
        DEBUGRENDERER2D->GetActiveCamera()->SetCenter(m_pAgentToEvade->GetPosition());
    }

    ImGui::Spacing();
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
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("DEBUG ATTRIBUTES");
    ImGui::Indent();
    ImGui::Text("Number of boids: %d", m_FlockSize);
    ImGui::Text("World size: %.0f", m_WorldSize);
    ImGui::Text("Rows: %d", m_WorldRows);
    ImGui::Text("Cols: %d", m_WorldCols);
    ImGui::Text("Neighborhood radius: %.0f", m_NeighborhoodRadius);
    ImGui::Text("Number of neighbors: %d", GetNrOfNeighbors());
    ImGui::Unindent();

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::Text("DEBUG CELL SPACE");
    ImGui::Indent();
    ImGui::Text("Bottom-left idx: %d", m_pCellSpace->GetDebugProperties().bottomLeftIdx);
    ImGui::Text("Bottom-right idx: %d", m_pCellSpace->GetDebugProperties().bottomRightIdx);
    ImGui::Text("Top-left idx: %d", m_pCellSpace->GetDebugProperties().topLeftIdx);
    ImGui::Text("Top-right idx: %d", m_pCellSpace->GetDebugProperties().topRightIdx);
    ImGui::Text("Col diff: %d", m_pCellSpace->GetDebugProperties().colDiff);
    ImGui::Text("Agent in one cell: %s", m_pCellSpace->GetDebugProperties().agentInOneCell ? "true" : "false");
    ImGui::Unindent();
    
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("DEBUG RENDERING");

    ImGui::Checkbox("Render Steering behaviors", &m_CanDebugRender);
    ImGui::Checkbox("Render Neighborhood", &m_CanRenderNeighborhood);
    ImGui::Checkbox("Render Neighborhood radius", &m_CanRenderNeighborhoodRadius);
    if (m_CanRenderNeighborhoodRadius)
    {
        ImGui::SliderFloat("Neighborhood radius", &m_NeighborhoodRadius, 0.f, 100.f, "%1.");
    }
    ImGui::Checkbox("Render Partitions", &m_CanRenderPartitions);
    m_pCellSpace->SetCanRenderPartitions(m_CanRenderPartitions);
    
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("WORLD");
    ImGui::Checkbox("Use Space Partitioning", &m_UseSpacePartitioning);
    ImGui::Checkbox("Trim World", &m_TrimWorld);
    if (m_TrimWorld)
    {
        ImGui::SliderFloat("Trim Size", &m_WorldSize, 0.f, 500.f, "%1.");
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::Text("BEHAVIOR WEIGHTS");
    ImGui::SliderFloat("Separation", GetWeight(m_pSeparationBehavior), 0.f, 1.f, "%.2");
    ImGui::SliderFloat("Cohesion", GetWeight(m_pCohesionBehavior), 0.f, 1.f, "%.2");
    ImGui::SliderFloat("Velocity Math", GetWeight(m_pVelMatchBehavior), 0.f, 1.f, "%.2");
    ImGui::SliderFloat("Seek", GetWeight(m_pSeekBehavior), 0.f, 1.f, "%.2");
    ImGui::SliderFloat("Wander", GetWeight(m_pWanderBehavior), 0.f, 1.f, "%.2");
    
    //End
    ImGui::PopAllowKeyboardFocus();
    ImGui::End();
}

void Flock::UpdateWorldSize(float* size) const
{
    *size = m_WorldSize;
}

/**
 * \brief Render the neighborhood of the first agent in the flock
 */
void Flock::RenderNeighborhood()
{
    RegisterNeighbors(m_Agents[0]);
    for (int idx{0}; idx < GetNrOfNeighbors(); ++idx)
    {
        DEBUGRENDERER2D->DrawSegment(m_Agents[0]->GetPosition(), GetNeighbors()[idx]->GetPosition(), {1.f, 0.f, 0.f});
        GetNeighbors()[idx]->SetBodyColor({1.f, 0.f, 0.f});
    }
}

void Flock::RegisterNeighbors(const SteeringAgent* pAgent)
{
    if (m_UseSpacePartitioning)
    {
        m_pCellSpace->RegisterNeighbors(pAgent, m_NeighborhoodRadius);
        return;    
    }
    
    int nrOfNeighbors{0};
    for (const auto& agent : m_Agents)
    {
        if (agent != pAgent)
        {
            const float distanceSq{DistanceSquared(agent->GetPosition(), pAgent->GetPosition())};
            if (distanceSq < m_NeighborhoodRadius * m_NeighborhoodRadius)
            {
                m_Neighbors[nrOfNeighbors] = agent;
                ++nrOfNeighbors;
            }
        }
    }
    m_NrOfNeighbors = nrOfNeighbors;
}

int Flock::GetNrOfNeighbors() const
{
    if (m_UseSpacePartitioning)
    {
        return m_pCellSpace->GetNrOfNeighbors();    
    }
    return m_NrOfNeighbors;
}

const std::vector<SteeringAgent*>& Flock::GetNeighbors() const
{
    if (m_UseSpacePartitioning)
    {
        return m_pCellSpace->GetNeighbors();
    }
    return m_Neighbors; 
}

Vector2 Flock::GetAverageNeighborPos() const
{
    const int nrOfNeighbors{GetNrOfNeighbors()};
    Vector2 avgPosition = Elite::ZeroVector2;
    if (nrOfNeighbors == 0) return avgPosition;
    for (int idx{0}; idx < nrOfNeighbors; ++idx)
    {
        avgPosition += GetNeighbors()[idx]->GetPosition();
    }
    return avgPosition / static_cast<float>(nrOfNeighbors);
}

Vector2 Flock::GetAverageNeighborVelocity() const
{
    const int nrOfNeighbors{GetNrOfNeighbors()};
    Vector2 avgVelocity = Elite::ZeroVector2;
    if (nrOfNeighbors == 0) return avgVelocity;
    for (int idx{0}; idx < nrOfNeighbors; ++idx)
    {
        avgVelocity += GetNeighbors()[idx]->GetLinearVelocity();
    }
    avgVelocity = avgVelocity / static_cast<float>(nrOfNeighbors);
    return avgVelocity.GetNormalized() * m_Agents[0]->GetMaxLinearSpeed();
}

void Flock::SetTarget_Seek(const TargetData& target) const
{
    m_pSeekBehavior->SetTarget(target);
}

void Flock::SetTarget_Evade(const TargetData& target) const
{
    m_pEvadeBehavior->SetTarget(target);
}

float* Flock::GetWeight(ISteeringBehavior* pBehavior) const
{
    if (m_pBlendedSteering)
    {
        auto& weightedBehaviors = m_pBlendedSteering->GetWeightedBehaviorsRef();
        const auto it = std::ranges::find_if(weightedBehaviors,
                                             [pBehavior](BlendedSteering::WeightedBehavior el)
                                             {
                                                 return el.pBehavior == pBehavior;
                                             }
        );

        if (it != weightedBehaviors.end())
            return &it->weight;
    }
    return nullptr;
}
