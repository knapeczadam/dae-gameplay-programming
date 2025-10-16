#pragma once
#include "../SteeringHelpers.h"
#include "FlockingSteeringBehaviors.h"

class ISteeringBehavior;
class SteeringAgent;
class BlendedSteering;
class PrioritySteering;
class CellSpace;

class Flock final
{
public:
    Flock(
        int flockSize = 50,
        float worldSize = 100.f,
        SteeringAgent* pAgentToEvade = nullptr,
        bool trimWorld = false);

    ~Flock();

    Flock(const Flock& other) = delete;
    Flock& operator=(const Flock& other) = delete;

    void Update(float deltaT);
    void UpdateAndRenderUI();
    void UpdateWorldSize(float* size) const;
    void Render(float deltaT);

    void RegisterNeighbors(const SteeringAgent* pAgent);
    int GetNrOfNeighbors() const;
    const std::vector<SteeringAgent*>& GetNeighbors() const;

    Elite::Vector2 GetAverageNeighborPos() const;
    Elite::Vector2 GetAverageNeighborVelocity() const;

    void SetTarget_Seek(const TargetData& target) const;
    void SetTarget_Evade(const TargetData& target) const;
    void SetWorldTrimSize(float size) { m_WorldSize = size; }
    bool TrimWorld() const { return m_TrimWorld; }

private:
    int   m_FlockSize          {0};
    float m_WorldSize          {0.0f};
    int   m_WorldRows          {25};
    int   m_WorldCols          {25};
    int   m_NrOfNeighbors      {0};
    float m_NeighborhoodRadius {5.0f};
    
    std::vector<SteeringAgent*> m_Agents    {};
    std::vector<SteeringAgent*> m_Neighbors {};
    
    SteeringAgent* m_pAgentToEvade {nullptr};
    CellSpace*     m_pCellSpace    {nullptr};
    
    // DEBUG
    bool m_CanDebugRender              {false};
    bool m_CanRenderNeighborhood       {false};
    bool m_CanRenderNeighborhoodRadius {false};
    bool m_CanRenderPartitions         {false};
    bool m_TrimWorld                   {false};
    bool m_UseSpacePartitioning        {false};
    bool m_FocusOnFirstAgent           {false};
    bool m_FocusOnAgentToEvade         {false};

    //Steering        Behaviors
    Separation*       m_pSeparationBehavior {nullptr};
    Cohesion*         m_pCohesionBehavior   {nullptr};
    VelocityMatch*    m_pVelMatchBehavior   {nullptr};
    Seek*             m_pSeekBehavior       {nullptr};
    Wander*           m_pWanderBehavior     {nullptr};
    Evade*            m_pEvadeBehavior      {nullptr};

    BlendedSteering*  m_pBlendedSteering    {nullptr};
    PrioritySteering* m_pPrioritySteering   {nullptr};

    Elite::Color m_NeighborhoodColor = {1.f, 0.f, 0.f};

    void RenderNeighborhood();
    float* GetWeight(ISteeringBehavior* pBehaviour) const;
};
