//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_GraphTheory.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphEditor.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphRenderer.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EEularianPath.h"
#include "framework/EliteAI/EliteGraphs/EliteGraph/EGraphConnection.h"
#include "projects/Movement/SteeringBehaviors/SteeringAgent.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"

#include <set>

using namespace Elite;
using namespace std;

//Destructor
App_GraphTheory::~App_GraphTheory()
{
    SAFE_DELETE(m_pAgent);
    SAFE_DELETE(m_pPathFollowBehavior);

    SAFE_DELETE(m_pGraph);
    SAFE_DELETE(m_pGraphEditor);
    SAFE_DELETE(m_pGraphRenderer);
}

//Functions
void App_GraphTheory::Start()
{
    // create graph
    m_pGraph = new Graph(false);
    int id1 = m_pGraph->AddNode(new GraphNode(Vector2(-10.0f, -10.0f)));
    int id2 = m_pGraph->AddNode(new GraphNode(Vector2(50.0f, 30.0f)));

    m_pGraph->AddConnection(new GraphConnection(id1, id2, 50.f));

    // create renderer & editor
    m_pGraphEditor = new GraphEditor();
    m_pGraphRenderer = new GraphRenderer();

    //Initialization of your application. If you want access to the physics world you will need to store it yourself.
    //----------- CAMERA ------------
    DEBUGRENDERER2D->GetActiveCamera()->SetZoom(80.f);
    DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(0, 0));
    DEBUGRENDERER2D->GetActiveCamera()->SetMoveLocked(false);
    DEBUGRENDERER2D->GetActiveCamera()->SetZoomLocked(false);

    m_pPathFollowBehavior = new PathFollow();
    m_pAgent = new SteeringAgent(2.f);
    m_pAgent->SetSteeringBehavior(m_pPathFollowBehavior);
    m_pAgent->SetMass(0.1f);
    m_pAgent->SetMaxLinearSpeed(20.f);
    m_pAgent->SetMaxAngularSpeed(90.f);
    m_pAgent->SetAutoOrient(true);
}

void App_GraphTheory::Update(float deltaTime)
{
    //------ INPUT ------
    if (m_pGraphEditor->UpdateGraph(m_pGraph))
    {
        m_pGraph->SetConnectionCostsToDistances();

        EulerianPath pathFinder{EulerianPath(m_pGraph)};
        Eulerianity eulerianity = pathFinder.IsEulerian();
        m_vPath = pathFinder.FindPath(eulerianity);
        UpdateAgentPath(m_vPath);
        UpdateNodeColors();
    }

    //------- UI --------
#ifdef PLATFORM_WINDOWS
#pragma region UI
    {
        //Setup
        int menuWidth = 250;
        int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
        int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
        bool windowActive = true;
        ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
        ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 90));
        ImGui::Begin("Gameplay Programming", &windowActive,
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
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

        ImGui::Text("Graph Theory");
        ImGui::Indent();
        ImGui::Text("Directional: %s", m_pGraph->IsDirectional() ? "true" : "false");
        ImGui::Text("Eulerianity: %s", Elite::ToString(EulerianPath{m_pGraph}.IsEulerian()).c_str());
        ImGui::Text("Number of active nodes: %i", m_pGraph->GetAmountOfNodes());
        ImGui::Text("Number of inactive nodes: %i", m_pGraph->GetAmountOfInactiveNodes());
        ImGui::Text("Number of connections: %i", m_pGraph->GetAmountOfConnections());
        std::stringstream ss;
        for (const auto m_v_path : m_vPath)
        {
            ss << m_v_path->GetId() << " ";
        }
        ImGui::Text("Path: %s", ss.str().c_str());
        ImGui::Spacing();
        ImGui::Spacing();

        //End
        ImGui::PopAllowKeyboardFocus();
        ImGui::End();
    }
#pragma endregion
#endif

    m_pAgent->Update(deltaTime);
}

void App_GraphTheory::Render(float deltaTime) const
{
    const GraphRenderingOptions options{true, true, true, true};
    m_pGraphRenderer->RenderGraph(m_pGraph, options);
    m_pAgent->Render(deltaTime);
}

void App_GraphTheory::UpdateAgentPath(const vector<Elite::GraphNode*>& trail)
{
    vector<Vector2> path{};

    //TODO: convert GraphNode vector to positions vector
    for (const auto& node : trail)
    {
        path.push_back(node->GetPosition());
    }

    m_pPathFollowBehavior->SetPath(path);
    if (path.size() > 0)
    {
        m_pAgent->SetPosition(path[0]);
    }
}

/**
 * \brief Update node colors by using Welsh-Powell algorithm
 */
void App_GraphTheory::UpdateNodeColors()
{
    // set to invalid label
    for (auto* node : m_pGraph->GetAllNodes())
    {
        node->SetLabel(invalid_label);
    }

    // sort nodes by degree
    auto nodes = m_pGraph->GetAllNodes();
    ranges::sort(nodes, [&](GraphNode* a, GraphNode* b)
    {
        return m_pGraph->GetConnectionsFromNode(a).size() > m_pGraph->GetConnectionsFromNode(b).size();
    });
    if (not nodes.empty())
    {
        int label{0};
        for (size_t i{0}; i < nodes.size(); ++i)
        {
            std::set<int> ids{};
            if (nodes[i]->GetLabel() != invalid_label)
                continue;
            for (size_t j{0}; j < nodes.size(); ++j)
            {
                if (m_pGraph->GetConnection(nodes[i]->GetId(), nodes[j]->GetId()) == nullptr)
                {
                    if (ids.empty())
                    {
                        ids.insert(nodes[j]->GetId());
                    }
                    for (int id : ids)
                    {
                        if (m_pGraph->GetConnection(nodes[j]->GetId(), id) == nullptr)
                        {
                            nodes[j]->SetLabel(label);
                            ids.insert(nodes[j]->GetId());
                            break;
                        }
                    }
                }
            }
            ++label;
        }
    }

    for (auto node : nodes)
    {
        switch (node->GetLabel())
        {
            case 0:
                node->SetColor({1, 0, 0});
                break;
            case 1:
                node->SetColor({0, 1, 0});
                break;
            case 2:
                node->SetColor({0, 0, 1});
                break;
            case 3:
                node->SetColor({1, 1, 0});
                break;
            case 4:
                node->SetColor({1, 0, 1});
                break;
            case 5:
                node->SetColor({0, 1, 1});
                break;
        }
    }
}
