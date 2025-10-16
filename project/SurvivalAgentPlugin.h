#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
#include "project/Movement/SteeringBehaviors/SteeringHelpers.h"

// Forward Declarations
class IBaseInterface;
class IExamInterface;
class IExtendedExamInterface;

class SteeringAgent;
class SmartAgent;

class SurvivalAgentPlugin :public IExamPlugin
{
public:
	SurvivalAgentPlugin()                   = default;
	virtual ~SurvivalAgentPlugin() override = default;

	void Initialize(IBaseInterface* pInterface, PluginInfo& info) override;
	void DllInit()                                                override;
	void DllShutdown()                                            override;

	void InitGameDebugParams(GameDebugParams& params) override;
	void Update_Debug(float dt)                       override;

	SteeringPlugin_Output UpdateSteering(float dt) override;
	void Render(float dt)                    const override;
	
	void DebugFOV();
	void DemoInventory();
	
private:
	Elite::Blackboard* CreateBlackboard(SmartAgent* smartAgent);
	void ScanWorld();

private:
	//Interface, used to request data from/perform actions with the AI Framework
	IExtendedExamInterface* m_pInterface        = nullptr;

	TargetData      m_Target            = {};
	bool            m_CanRun            = false;   //Demo purpose
	bool            m_GrabItem          = false;   //Demo purpose
	bool            m_UseItem           = false;   //Demo purpose
	bool            m_RemoveItem        = false;   //Demo purpose
	bool            m_DestroyItemsInFOV = false;
	float           m_AngSpeed          = 0.f;     //Demo purpose

	UINT            m_InventorySlot     = 0;

	//-- Agent --//
	SmartAgent*    m_pPlayer = nullptr;
	
	// -- Blackboard -- //
	Elite::Blackboard* m_pBlackboard = nullptr;

	float                     m_AccuSearchTime        = 0.0f;
	float                     m_MaxSearchTime         = 5.0f;
	Elite::DebugHelper        m_DebugHelper           = {};
	std::array<ItemInfo, 5>   m_Inventory             = {};
	MemoryInfo                m_Memory                = {};
	float                     m_TargetShootingRange   = 10.0f;
	
};

//ENTRY
//This is the first function that is called by the host program
//The plugin returned by this function is also the plugin used by the host program
extern "C"
{
	inline __declspec (dllexport) IPluginBase* Register()
	{
		return new SurvivalAgentPlugin();
	}
}