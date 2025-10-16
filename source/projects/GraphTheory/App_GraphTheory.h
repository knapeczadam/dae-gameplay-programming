#pragma once

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
namespace Elite
{
	class GraphRenderer;
	class GraphEditor;
	class GraphNode;
	class GraphConnection;
	class Graph;
}

class SteeringAgent;
class PathFollow;
//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_GraphTheory final : public IApp
{
public:
	//Constructor & Destructor
	App_GraphTheory() = default;
	virtual ~App_GraphTheory() final;

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:

	SteeringAgent* m_pAgent = nullptr;
	PathFollow* m_pPathFollowBehavior = nullptr;

	Elite::Graph* m_pGraph = nullptr;
	Elite::GraphEditor* m_pGraphEditor = nullptr;
	Elite::GraphRenderer* m_pGraphRenderer = nullptr;

	std::vector<Elite::GraphNode*> m_vPath;

	void UpdateAgentPath(const std::vector<Elite::GraphNode*>& trail);
	void UpdateNodeColors();

	//C++ make the class non-copyable
	App_GraphTheory(const App_GraphTheory&) = delete;
	App_GraphTheory& operator=(const App_GraphTheory&) = delete;
};