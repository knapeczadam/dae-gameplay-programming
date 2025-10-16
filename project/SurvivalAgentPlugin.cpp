#include "stdafx.h"
#include "SurvivalAgentPlugin.h"

// Project includes
#include "IExtendedExamInterface.h"
#include "project/Movement/SteeringBehaviors/SteeringAgent.h"
#include "project/DecisionMaking/SmartAgent.h"
#include "project/DecisionMaking/BehaviorTrees/Behaviors.h"

// Standard includes
#include <map>

using namespace std;
using namespace Elite;

IExtendedExamInterface* g_pInterface = nullptr;

#pragma region Initalization
//--------------------------------------------------------------------------------------------------------
// INITIALIZATION
// Called only once
//--------------------------------------------------------------------------------------------------------
void SurvivalAgentPlugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//--------------------------------------------------------------------------------------------------------
	// Retrieving the INTERFACE
	// This interface gives you access to certain actions the AI_Framework can perform for you
	//--------------------------------------------------------------------------------------------------------
	m_pInterface = static_cast<IExtendedExamInterface*>(pInterface);
	g_pInterface = m_pInterface;

	
	//--------------------------------------------------------------------------------------------------------
	// LEADERBOARD SETTINGS
	//--------------------------------------------------------------------------------------------------------
	info.BotName       = "Zombrinator";
	info.Student_Name  = "ADAM KNAPECZ";       // No special characters allowed. Highscores won't work with special characters.
	info.Student_Class = "2DAE09";
	info.LB_Password   = ",N19&[R&Zns2F({>R$"; // Don't use a real password! This is only to prevent other students from overwriting your highscore!

	//--------------------------------------------------------------------------------------------------------
	// AGENT SETTINGS
	//--------------------------------------------------------------------------------------------------------
	m_pPlayer = new SmartAgent();
	m_pPlayer->SetDebugRenderingEnabled(true);

	//--------------------------------------------------------------------------------------------------------
	// AI SETUP
	//--------------------------------------------------------------------------------------------------------
	m_pBlackboard = CreateBlackboard(m_pPlayer);

	//--------------------------------------------------------------------------------------------------------
	// INVENTORY
	//--------------------------------------------------------------------------------------------------------
	EXAMINTERFACE->InventoryEx_Set(&m_Inventory);

	//--------------------------------------------------------------------------------------------------------
	// MEMORY
	//--------------------------------------------------------------------------------------------------------
	EXAMINTERFACE->Memory_Set(&m_Memory);

	//--------------------------------------------------------------------------------------------------------
	// WORLD
	//--------------------------------------------------------------------------------------------------------
	ScanWorld();

	//--------------------------------------------------------------------------------------------------------
	// BEHAVIOR TREE
	//--------------------------------------------------------------------------------------------------------
	// --- Level 6: Fallback ---
	const auto level6 = new BehaviorSelector({
		new BehaviorAction(BT_Actions::Steering_Wander),
	});
	
	// --- Level 5: Discover ---
	// Player run
	const auto level5PlayerRun = new BehaviorSelector({
		new BehaviorSequence({
			new BehaviorConditional(BT_Conditions::Player_CanRun),
			new BehaviorAction(BT_Actions::Player_Run)
		}),
		new BehaviorAction(BT_Actions::Success)
	});
	// Unvisited corner
	const auto level5UnvisitedCorner = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::Player_IsInHouse),
		new BehaviorConditional(BT_Conditions::Memory_ContainsUnvisitedCorner),
		new BehaviorAction(BT_Actions::Target_SetClosestUnvisitedCorner),
		new BehaviorAction(BT_Actions::Steering_RadarSeek),
	});
	// Add item
	const auto level5AddItem = new BehaviorSequence({
		new BehaviorConditionalInverter(BT_Conditions::Inventory_IsFull),
		new BehaviorConditional(BT_Conditions::Memory_ContainsItem),
		new BehaviorAction(BT_Actions::Target_SetClosestPriorityItem),
		new BehaviorAction(BT_Actions::Steering_RadarSeek),
		new BehaviorAction(BT_Actions::Inventory_AddNearestItem)
	});
	// Garbage
	const auto level5Garbage = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::Memory_ContainsGarbage),
		new BehaviorAction(BT_Actions::Target_SetClosestGarbage),
		new BehaviorAction(BT_Actions::Steering_RadarSeek),
		new BehaviorAction(BT_Actions::Inventory_DestroyNearestItem)
	});
	// Unvisited house
	const auto level5UnvisitedHouse = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::Memory_ContainsUnvisitedHouse),
		new BehaviorAction(BT_Actions::Target_SetClosestUnvisitedHouse),
		new BehaviorAction(BT_Actions::Steering_RadarSeek),
	});
	// Unvisited path point
	const auto level5UnvisitedPathPoint = new BehaviorSequence({
		new BehaviorConditionalInverter(BT_Conditions::Player_WorldDiscovered),
		new BehaviorConditional(BT_Conditions::Memory_ContainsUnvisitedPathPoint),
		new BehaviorAction(BT_Actions::Target_SetClosestUnvisitedPathPoint),
		new BehaviorAction(BT_Actions::Steering_RadarSeek),
	});
	// Player forget
	const auto level5PlayerForget = new BehaviorSequence({
		new BehaviorConditionalInverter(BT_Conditions::Memory_ContainsUnvisitedHouse),
		new BehaviorConditionalInverter(BT_Conditions::Memory_ContainsUnvisitedPathPoint),
		new BehaviorAction(BT_Actions::Memory_Forget),
		
	});
	
	const auto level5 = new BehaviorSelector({
		new BehaviorSequence({
			level5PlayerRun,
			new BehaviorSelector({
				level5AddItem,
				level5UnvisitedCorner,
				level5Garbage,
				level5UnvisitedHouse,
				level5UnvisitedPathPoint,
				level5PlayerForget,
			})
		}),
		level6
	});
	
	// --- Level 4: Purge zone ---
	const auto level4PurgeZone = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::FOV_ContainsPurgeZone),
		new BehaviorAction(BT_Actions::Memory_SetTime),
		new BehaviorAction(BT_Actions::Target_SetClosestSafeZone),
		new BehaviorAction(BT_Actions::Player_Run),
		new BehaviorAction(BT_Actions::Steering_RadarSeek),
	});
	// Player stay
	const auto level4PlayerStay = new BehaviorPartialSequence({
		new BehaviorConditional(BT_Conditions::Memory_TimeSet),
		new BehaviorAction(BT_Actions::Steering_RadarSeek),
		new BehaviorAction(BT_Actions::Player_Stay),
	});
	
	const auto level4 = new BehaviorSelector({
		level4PurgeZone,
		level4PlayerStay,
		level5
	});
	
	// --- Level 3: Enemy ---
	// Shoot enemy
	const auto level3PlayerShoot = new BehaviorPartialSequence({
		new BehaviorConditional(BT_Conditions::FOV_ContainsEnemy),
		new BehaviorConditional(BT_Conditions::Inventory_ContainsWeapon),
		new BehaviorConditional(BT_Conditions::FOV_IsInShootingRange),
		new BehaviorAction(BT_Actions::Target_SetNearestEnemy),
		new BehaviorAction(BT_Actions::Steering_Face),
		new BehaviorSelector({
			new BehaviorSequence({
				new BehaviorConditional(BT_Conditions::Inventory_ContainsPistol),
				new BehaviorAction(BT_Actions::Inventory_UsePistol),
			}),
			new BehaviorSequence({
				new BehaviorConditional(BT_Conditions::Inventory_ContainsShotgun),
				new BehaviorAction(BT_Actions::Inventory_UseShotgun),
			})
		})
	});
	// Avoid enemy
	const auto level3PlayerAvoid = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::FOV_ContainsEnemy),
		new BehaviorConditionalInverter(BT_Conditions::FOV_ContainsPurgeZone),
		new BehaviorConditionalInverter(BT_Conditions::Inventory_ContainsWeapon),
		// new BehaviorSelector({
		// 	new BehaviorAction(BT_Actions::Target_SetClosestHouseLocation),
		// 	new BehaviorAction(BT_Actions::Target_SetClosestPathPointLocation),
		// 	new BehaviorAction(BT_Actions::Target_SetPlayerBack),
		// }),
		new BehaviorAction(BT_Actions::Player_Run),
		new BehaviorAction(BT_Actions::Steering_RadarSeek),
	});
	// Player was bitten
	const auto level3PlayerRun = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::Player_WasBitten),
		new BehaviorAction(BT_Actions::Player_Run),
	});
	
	const auto level3 = new BehaviorSelector({
		level3PlayerShoot,
		level3PlayerAvoid,
		level3PlayerRun,
		level4
	});
	
	// --- Level 2: Basic needs ---
	// Player heal
	const auto level2PlayerHeal = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::Player_IsWounded),
		new BehaviorConditional(BT_Conditions::Inventory_ContainsMedkit),
		new BehaviorAction(BT_Actions::Player_Heal)
	});
	// Player emergency heal
	const auto level2PlayerEmergencyHeal = new BehaviorSequence({
		new BehaviorSelector({
			new BehaviorConditional(BT_Conditions::FOV_ContainsPurgeZone),
			new BehaviorConditional(BT_Conditions::Player_IsInPurgeZone),
		}),
		new BehaviorConditional(BT_Conditions::Player_IsCriticallyWounded),
		new BehaviorConditional(BT_Conditions::Inventory_ContainsMedkit),
		new BehaviorAction(BT_Actions::Player_EmergencyHeal)
	});
	// Player eat
	const auto level2PlayerEat = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::Player_IsHungry),
		new BehaviorConditional(BT_Conditions::Inventory_ContainsFood),
		new BehaviorAction(BT_Actions::Player_Eat)
	});
	// Player tired
	const auto level2PlayerWalk = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::Player_IsTired),
		new BehaviorAction(BT_Actions::Player_Walk)
	});
	// Player scan
	const auto level2PlayerScan = new BehaviorSequence({
		new BehaviorConditionalInverter(BT_Conditions::Player_WorldDiscovered),
		new BehaviorConditional(BT_Conditions::Player_CanScan),
		new BehaviorConditionalInverter(BT_Conditions::Player_IsInHouse),
		new BehaviorAction(BT_Actions::Player_Scan)
	});
	// Remember house
	const auto level2RememberHouse = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::FOV_ContainsHouse),
		new BehaviorAction(BT_Actions::Memory_RememberHouse),
	});
	// Remember first door
	const auto level2RememberFirstDoor = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::Player_IsInHouse),
		new BehaviorAction(BT_Actions::Memory_RememberFirstDoor),
	});
	// Remember second door
	const auto level2RememberSecondDoor = new BehaviorSequence({
		new BehaviorConditionalInverter(BT_Conditions::Player_IsInHouse),
		new BehaviorAction(BT_Actions::Memory_RememberSecondDoor),
	});
	// Remember item
	const auto level2RememberItem = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::FOV_ContainsItem),
		new BehaviorAction(BT_Actions::Memory_RememberItem)
	});
	// Clean inventory
	const auto level2CleanInventory = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::Inventory_ContainsEmptyItem),
		new BehaviorAction(BT_Actions::Inventory_RemoveEmptyItem)
	});
	// Remove weapon
	const auto level2RemoveWeapon = new BehaviorSequence({
		new BehaviorConditional(BT_Conditions::Inventory_IsWeaponFull),
		new BehaviorAction(BT_Actions::Inventory_RemoveWeapon)
	});
	
	const auto level2 = new BehaviorSelector({
		level2PlayerHeal,
		level2PlayerEmergencyHeal,
		level2PlayerEat,
		level2PlayerWalk,
		level2PlayerScan,
		level2RememberHouse,
		level2RememberFirstDoor,
		level2RememberSecondDoor,
		level2RememberItem,
		level2CleanInventory,
		level2RemoveWeapon,
		level3
	});
	
	// --- Level 1: Brain ---
	const auto level1 = new BehaviorSequence({
		new BehaviorAction(BT_Actions::Memory_Refresh),
		level2,
	});
	
	BehaviorTree* pBT = new BehaviorTree(m_pBlackboard, level1);

	m_pPlayer->SetDecisionMaking(pBT);
}

Blackboard* SurvivalAgentPlugin::CreateBlackboard(SmartAgent* smartAgent)
{
	Blackboard* pBlackboard = new Blackboard();
	
	// --Time--
	pBlackboard->AddData("DeltaTime", 0.0f);
	
	// --Agents--
	pBlackboard->AddData("Target", &m_Target);
	pBlackboard->AddData("Player", m_pPlayer);
	pBlackboard->AddData("MaxSearchTime", m_MaxSearchTime);
	pBlackboard->AddData("AccuSearchTime", m_AccuSearchTime);
	pBlackboard->AddData("TargetShootingRange", m_TargetShootingRange);
	
	pBlackboard->AddData("DebugHelper", &m_DebugHelper);

	return pBlackboard;
}

void SurvivalAgentPlugin::ScanWorld()
{
	// fill angles with radians
	std::vector<float> angles(720);
	std::ranges::generate(angles, [i = 0]() mutable { return Elite::ToRadians(static_cast<float>(i++) / 2.0f); });

	std::vector<Elite::Vector2> hits;

	const auto distance = EXAMINTERFACE->World_GetInfo().Dimensions.x;
	for (int i = 0; i < angles.size(); i++)
	{
		const auto rayDir = Elite::Vector2{std::cos(angles[i]), std::sin(angles[i])};
		const auto rayEnd = m_pPlayer->GetPosition() + rayDir * distance;
		const auto hit = EXAMINTERFACE->NavMesh_GetClosestPathPoint(rayEnd);
		hits.push_back(hit);
	}

	// sort hits by distance to player
	std::ranges::sort(hits, [this](const Elite::Vector2& hit1, const Elite::Vector2& hit2)
	{
		const auto playerPos = m_pPlayer->GetPosition();
		return Elite::DistanceSquared(hit1, playerPos) < Elite::DistanceSquared(hit2, playerPos);
	});

	// keep only unique hits
	hits.erase(std::unique(hits.begin(), hits.end(), [](const Elite::Vector2& hit1, const Elite::Vector2& hit2)
	{
		return Elite::DistanceSquared(hit1, hit2) < 1.f;
	}), hits.end());

	for (const auto& hit : hits)
	{
		// check if hit is in range
		if (hit.MagnitudeSquared() >= distance * distance) continue;
		
		PathPointInfo pathPoint{hit, false};
		EXAMINTERFACE->Memory_Get()->PathPoints.insert({pathPoint.GetHash(), pathPoint});
	}
}
#pragma endregion

#pragma region DLL
//--------------------------------------------------------------------------------------------------------
// Called only once
//--------------------------------------------------------------------------------------------------------
void SurvivalAgentPlugin::DllInit()
{
	// Called when the plugin is loaded
}

//--------------------------------------------------------------------------------------------------------
// Called only once
//--------------------------------------------------------------------------------------------------------
void SurvivalAgentPlugin::DllShutdown()
{
	//Called when the plugin gets unloaded
	SAFE_DELETE(m_pPlayer);
}
#pragma endregion

#pragma region Debug
//--------------------------------------------------------------------------------------------------------
// DEBUG
// Called only once, during initialization. 
//--------------------------------------------------------------------------------------------------------
void SurvivalAgentPlugin::InitGameDebugParams(GameDebugParams& params)
{
	params.LevelFile                    = "GameLevel.gppl"; //Level to load?
	
	params.StartingDifficultyStage      = 1;    // Overwrites the difficulty stage (Default = 0)
	params.EnemyCount                   = 20;   // How many enemies? (Default = 20)
	params.ItemCount					= 40;   // How many items? (Default = 40)
	
	params.GodMode                      = true; // GodMode > You can't die, can be useful to inspect certain behaviors (Default = false)
	params.IgnoreEnergy                 = false; // Ignore energy depletion (Default = false)
	params.AutoFollowCam                = true; // Automatically follow the AI? (Default = false)            
	params.AutoGrabClosestItem          = false; // A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored) (Default = false)
	params.InfiniteStamina              = false; // Agent has infinite stamina (Default = false)
	
	params.SpawnEnemies                 = true; // Do you want to spawn enemies? (Default = true)            
	params.SpawnDebugPistol             = false; // Spawns pistol with 1000 ammo at start (Default = false)
	params.SpawnDebugShotgun            = false; // Spawns shotgun with 1000 ammo at start (Default = false)
	params.SpawnPurgeZonesOnMiddleClick = true; // Middle mouse clicks spawn purge zones (Default = false)
	params.SpawnZombieOnRightClick	    = true; // Right mouse clicks spawn purge zones (Default = false)
	
	params.RenderUI                     = true; // Render the IMGUI Panel? (Default = false)            
	params.PrintDebugMessages           = false; // (Default = true)
	params.ShowDebugItemNames           = true; // (Default = true)

	params.Seed                         = 666;                  // -1 = don't set seed. Any other number = fixed seed 
	// params.Seed                         = int(time(nullptr)); // Pure randomness
}

//--------------------------------------------------------------------------------------------------------
// DEBUG
// (=Use only for Debug Purposes)
//--------------------------------------------------------------------------------------------------------
void SurvivalAgentPlugin::Update_Debug(float dt)
{
	// Demo Event Code
	// In the end your Agent should be able to walk around without external input
	
	//--------------------------------------------------------------------------------------------------------
	// MOUSE INPUTS
	//--------------------------------------------------------------------------------------------------------
	// Button Up
	if (EXAMINTERFACE->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		// Update_Debug target based on input
		Elite::MouseData mouseData = EXAMINTERFACE->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos   = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		m_Target.Position          = EXAMINTERFACE->Debug_ConvertScreenToWorld(pos);
	}

	//--------------------------------------------------------------------------------------------------------
	// KEYBOARD INPUTS
	//--------------------------------------------------------------------------------------------------------
	std::map<Elite::InputScancode, std::function<void()>> actions = {
		{Elite::eScancode_Left,             [&]() { m_AngSpeed -= Elite::ToRadians(10);          }},
		{Elite::eScancode_Right,            [&]() { m_AngSpeed += Elite::ToRadians(10);          }},
		{Elite::eScancode_Space,            [&]() { m_CanRun    = true;                          }},
		{Elite::eScancode_Delete,           [&]() { EXAMINTERFACE->RequestShutdown();             }},
		{Elite::eScancode_KP_Minus,         [&]() { if (m_InventorySlot > 0) --m_InventorySlot;  }},
		{Elite::eScancode_KP_Plus,          [&]() { if (m_InventorySlot < 4) ++m_InventorySlot;  }},
		{Elite::eScancode_G,                [&]() { m_GrabItem          = true;                  }},
		{Elite::eScancode_U,                [&]() { m_UseItem           = true;                  }},
		{Elite::eScancode_R,                [&]() { m_RemoveItem        = true;                  }},
		{Elite::eScancode_X,                [&]() { m_DestroyItemsInFOV = true;                  }},
		{Elite::eScancode_Q,                [&]() {
			ItemInfo info = {};
			EXAMINTERFACE->Inventory_GetItem(m_InventorySlot, info);
			std::cout << (int)info.Type << '\n';
		}}
	};
	
	// Key Down
	for (auto& action : actions)
	{
		if (EXAMINTERFACE->Input_IsKeyboardKeyDown(action.first))
		{
			action.second();
		}
	}

	// Key Up
	if (EXAMINTERFACE->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	{
		m_CanRun = false;
	}
}

void SurvivalAgentPlugin::DebugFOV()
{
	// FOV stats = CHEAP! info about the FOV
	FOVStats stats = EXAMINTERFACE->FOV_GetStats(); // first always ask this

	// FOV data (snapshot of the FOV of the current frame) = EXPENSIVE! returns a new vector for every call
	auto vPurgezonesInFOV = EXAMINTERFACE->GetPurgeZonesInFOV();
	auto vEnemiesInFOV    = EXAMINTERFACE->GetEnemiesInFOV();
	auto vItemsInFOV      = EXAMINTERFACE->GetItemsInFOV();
	auto vHousesInFOV     = EXAMINTERFACE->GetHousesInFOV();
	
	for (auto& zoneInfo : vPurgezonesInFOV)
	{
		std::cout << "Purge Zone in FOV:" << zoneInfo.PurgeZoneInfo::Center.x << ", "<< zoneInfo.PurgeZoneInfo::Center.y << "---Radius: "<< zoneInfo.PurgeZoneInfo::Radius << '\n';
	}
	
	for (auto& enemyInfo : vEnemiesInFOV)
	{
		std::cout << "Enemy in FOV:" << enemyInfo.EnemyInfo::Location.x << ", " << enemyInfo.EnemyInfo::Location.y << "---Health: " << enemyInfo.EnemyInfo::Health << '\n';
	}
	
	for (auto& item : vItemsInFOV)
	{
		std::cout << "Item in FOV:" << item.ItemInfo::Location.x << ", " << item.ItemInfo::Location.y << "---Value: " << item.ItemInfo::Value << '\n';	
	}

	for (auto& house : vHousesInFOV)
	{
		std::cout << "House in FOV:" << house.HouseInfo::Center.x << ", " << house.HouseInfo::Center.y << "---Size: " << house.HouseInfo::Size.x << ", " << house.HouseInfo::Size.y << '\n';
	}
}

void SurvivalAgentPlugin::DemoInventory()
{
	// FOV stats = CHEAP! info about the FOV
	FOVStats stats = EXAMINTERFACE->FOV_GetStats(); // first always ask this
	
	auto vItemsInFOV= EXAMINTERFACE->GetItemsInFOV();

	if (m_GrabItem)
	{
		ItemInfo item;
		// Item_Grab > When DebugParams.AutoGrabClosestItem is TRUE, the Item_Grab function returns the closest item in range
		// Keep in mind that DebugParams are only used for debugging purposes, by default this flag is FALSE
		// Otherwise, use GetEntitiesInFOV() to retrieve a vector of all entities in the FOV (EntityInfo)
		// Item_Grab gives you the ItemInfo back, based on the passed EntityHash (retrieved by GetEntitiesInFOV)
		if (EXAMINTERFACE->GrabNearestItem(item)) 
		{
			// Once grabbed, you can add it to a specific inventory slot
			// Slot must be empty
			EXAMINTERFACE->Inventory_AddItem(m_InventorySlot, item);
		}
	}

	if (m_UseItem)
	{
		// Use an item (make sure there is an item at the given inventory slot)
		EXAMINTERFACE->Inventory_UseItem(m_InventorySlot);
	}

	if (m_RemoveItem)
	{
		// Remove an item from a inventory slot
		EXAMINTERFACE->Inventory_RemoveItem(m_InventorySlot);
	}

	if (m_DestroyItemsInFOV)
	{
		for (auto& item : vItemsInFOV)
		{
			EXAMINTERFACE->DestroyItem(item);
		}
	}
}
#pragma endregion

#pragma region Game Loop
//--------------------------------------------------------------------------------------------------------
// This function calculates the new SteeringOutput, called once per frame
//--------------------------------------------------------------------------------------------------------
SteeringPlugin_Output SurvivalAgentPlugin::UpdateSteering(float dt)
{
	//--------------------------------------------------------------------------------------------------------
	// CLOSEST PATH POINT
	//--------------------------------------------------------------------------------------------------------
	// const auto nextTargetPos = EXAMINTERFACE->NavMesh_GetClosestPathPoint(m_Target.Position);
	// m_Target.Position = nextTargetPos;



	
	//--------------------------------------------------------------------------------------------------------
	// BLACKBOARD UPDATE
	//--------------------------------------------------------------------------------------------------------
	// --Time--
	m_pBlackboard->ChangeData("DeltaTime", dt);

	// --Debug Helper--
	DebugHelper* pDebugHelper = nullptr;
	m_pBlackboard->GetData("DebugHelper", pDebugHelper);
	// ...
	m_pBlackboard->ChangeData("DebugHelper", pDebugHelper);

	// --Target--
	m_pBlackboard->ChangeData("Target", &m_Target);



	
	//--------------------------------------------------------------------------------------------------------
	// DEBUG FOV
	//--------------------------------------------------------------------------------------------------------
	// DebugFOV();



	
	//--------------------------------------------------------------------------------------------------------
	// INVENTORY
	//--------------------------------------------------------------------------------------------------------
	DemoInventory();
	EXAMINTERFACE->InventoryEx_UpdateItems();



	
	//--------------------------------------------------------------------------------------------------------
	// RESET STATES
	//--------------------------------------------------------------------------------------------------------
	m_GrabItem          = false;
	m_UseItem           = false;
	m_RemoveItem        = false;
	m_DestroyItemsInFOV = false;



	
	//--------------------------------------------------------------------------------------------------------
	// UPDATE AGENT
	//--------------------------------------------------------------------------------------------------------
	m_pPlayer->Update(dt);



	
	//--------------------------------------------------------------------------------------------------------
	// UPDATE STEERING OUTPUT
	//--------------------------------------------------------------------------------------------------------
	auto steering       = m_pPlayer->GetSteeringOutput().AsSteeringPluginOutput(); // SteeringPlugin_Output is works the exact same way a SteeringBehaviour output
	steering.AutoOrient = m_pPlayer->IsAutoOrienting();                            // Setting AutoOrient to true overrides the AngularVelocity
	steering.RunMode    = m_pPlayer->CanRun();                                     // If RunMode is True > MaxLinearSpeed is increased for a limited time (until your stamina runs out)
	// steering.RunMode    = m_CanRun;
	
	return steering;
}
#pragma endregion

#pragma region Rendering
//--------------------------------------------------------------------------------------------------------
// RENDERING
// This function should only be used for rendering debug elements
//--------------------------------------------------------------------------------------------------------
void SurvivalAgentPlugin::Render(float dt) const
{
	// This Render function should only contain calls to Interface->Draw_... functions
	EXAMINTERFACE->Draw_SolidCircle(m_Target.Position, 3.0f, { 0,0 }, { 1, 0, 0 }, EXAMINTERFACE->NextDepthSlice());

	// World center
	EXAMINTERFACE->Draw_SolidCircle(EXAMINTERFACE->World_GetInfo().Center, 5.0f, { 0,0 }, { 1, 1, 1 }, EXAMINTERFACE->NextDepthSlice());
	// World bounds
	static const auto dimension   = EXAMINTERFACE->World_GetInfo().Dimensions;
	static const auto bottomLeft  = Elite::Vector2{ -dimension.x / 2.f, -dimension.y / 2.f };
	static const auto bottomRight = Elite::Vector2{  dimension.x / 2.f, -dimension.y / 2.f };
	static const auto topLeft     = Elite::Vector2{ -dimension.x / 2.f,  dimension.y / 2.f };
	static const auto topRight    = Elite::Vector2{  dimension.x / 2.f,  dimension.y / 2.f };
	
	EXAMINTERFACE->Draw_Segment(bottomLeft,  bottomRight, { 1, 1, 1 }, EXAMINTERFACE->NextDepthSlice());
	EXAMINTERFACE->Draw_Segment(bottomRight, topRight,    { 1, 1, 1 }, EXAMINTERFACE->NextDepthSlice());
	EXAMINTERFACE->Draw_Segment(topRight,    topLeft,     { 1, 1, 1 }, EXAMINTERFACE->NextDepthSlice());
	EXAMINTERFACE->Draw_Segment(topLeft,     bottomLeft,  { 1, 1, 1 }, EXAMINTERFACE->NextDepthSlice());

	// Path points
	for (const auto& pathPoint : EXAMINTERFACE->Memory_Get()->PathPoints | views::values)
	{
		// if point outside world
		if (pathPoint.Location.x <= -dimension.x / 2.f or pathPoint.Location.x >= dimension.x / 2.f or 
			pathPoint.Location.y <= -dimension.y / 2.f or pathPoint.Location.y >= dimension.y / 2.f)
		{
			EXAMINTERFACE->Draw_SolidCircle(pathPoint.Location, 1.5f, { 0,0 }, { 0.2f, 0.2f, 0.2f }, EXAMINTERFACE->NextDepthSlice());
		}
		else if (pathPoint.Visited)
		{
			EXAMINTERFACE->Draw_SolidCircle(pathPoint.Location, 1.5f, { 0,0 }, { 0, 0, 1 }, EXAMINTERFACE->NextDepthSlice());
		}
		else
		{
			EXAMINTERFACE->Draw_SolidCircle(pathPoint.Location, 1.5f, { 0,0 }, { 0, 1, 0 }, EXAMINTERFACE->NextDepthSlice());
		}
	}

	// Houses
	for (const auto& house : EXAMINTERFACE->Memory_Get()->Houses | views::values)
	{
		if (house.Visited)
		{
			EXAMINTERFACE->Draw_SolidCircle(house.Center, 1.5f, { 0,0 }, { 0, 0, 1 }, EXAMINTERFACE->NextDepthSlice());
		}
		else
		{
			EXAMINTERFACE->Draw_SolidCircle(house.Center, 1.5f, { 0,0 }, { 0, 1, 0 }, EXAMINTERFACE->NextDepthSlice());
		}

		// Door
		for (const auto& door : house.Doors | views::values)
		{
			EXAMINTERFACE->Draw_SolidCircle(door.Location, 1.5f, { 0,0 }, { 0, 1, 1 }, EXAMINTERFACE->NextDepthSlice());
		}

		// Corners
		for (const auto& corner : house.Corners | views::values)
		{
			if (corner.Visited)
			{
				EXAMINTERFACE->Draw_SolidCircle(corner.Location, 0.5f, { 0,0 }, { 0, 0, 1 }, EXAMINTERFACE->NextDepthSlice());
			}
			else
			{
				EXAMINTERFACE->Draw_SolidCircle(corner.Location, 0.5f, { 0,0 }, { 0, 1, 0 }, EXAMINTERFACE->NextDepthSlice());
			}
			EXAMINTERFACE->Draw_Circle(corner.Location, 3, { 1, 1, 1 }, EXAMINTERFACE->NextDepthSlice());
		}
	}

	// Items
	for (const auto& item : EXAMINTERFACE->Memory_Get()->Items | views::values)
	{
		if (item.Type == eItemType::GARBAGE)
		{
			EXAMINTERFACE->Draw_SolidCircle(item.Location, 1.5f, { 0,0 }, { 0, 0, 0 }, EXAMINTERFACE->NextDepthSlice());
		}
		else
		{
			EXAMINTERFACE->Draw_SolidCircle(item.Location, 1.5f, { 0,0 }, { 1, 0, 1 }, EXAMINTERFACE->NextDepthSlice());
		}
	}

	EXAMINTERFACE->Draw_Direction(m_pPlayer->GetPosition(), m_pPlayer->GetDirection(), EXAMINTERFACE->Memory_Get()->ItemRange, { 1, 1, 1 }, EXAMINTERFACE->NextDepthSlice());
	EXAMINTERFACE->Draw_Circle(m_pPlayer->GetPosition(), EXAMINTERFACE->Memory_Get()->ItemRange, { 1, 1, 1 }, EXAMINTERFACE->NextDepthSlice());
	EXAMINTERFACE->Draw_Segment(m_pPlayer->GetPosition(), m_Target.Position, { 1, 0, 0 }, EXAMINTERFACE->NextDepthSlice());
}
#pragma endregion
