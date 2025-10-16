#pragma once
#pragma region MISC

// Standard includes
#include <string>
#include <unordered_map>
#include <vector>

struct SteeringPlugin_Output
{
	Elite::Vector2 LinearVelocity  = { 0.f,0.f };
	float          AngularVelocity = 0.f;
	bool           AutoOrient      = true;
	bool           RunMode         = false;
};

struct PluginInfo
{
	std::string BotName       = "NoName";
	std::string Student_Name  = "John     Cena";
	std::string Student_Class = "2DAE00";
	std::string LB_Password   = "ABC123";
};

struct GameDebugParams //Debuggin Purposes only (Ignored during release build)
{
	GameDebugParams(bool spawnEnemies = true, int enemyCount = 20, bool godMode = false, bool autoFollowCam = false)
	{
		SpawnEnemies  = spawnEnemies;
		GodMode       = godMode;
		AutoFollowCam = autoFollowCam;
		EnemyCount    = enemyCount;
	}

	bool        SpawnEnemies                 = true;             // Spawn enemies?
	int         EnemyCount                   = 20;               // Amount of enemies?
	int         ItemCount                    = 40;               // Amount of items?
	bool        GodMode                      = false;            // Use GodMode? (Invincible)
	bool        IgnoreEnergy                 = false;            // Ignore energy depletion
	bool        AutoFollowCam                = false;            // Auto follow the player
	bool        RenderUI                     = false;            // Render Player UI (Parameters)
	bool        AutoGrabClosestItem          = false;            // Auto Grab closest item (Item_Grab)
	std::string LevelFile                    = "GameLevel.gppl"; // Level to load?
	int         Seed                         = -1;               // Seed for random generator
	int         StartingDifficultyStage      = 0;                // Overwrites the          difficulty   stage
	bool        InfiniteStamina              = false;            // Agent has infinite stamina
	bool        SpawnDebugPistol             = false;            // Spawns pistol with 1000 ammo at start
	bool        SpawnDebugShotgun            = false;            // Spawns shotgun with 1000 ammo at start
	bool        SpawnPurgeZonesOnMiddleClick = false;            // Middle mouse clicks spawn purge zones
	bool        SpawnZombieOnRightClick      = false;            // Right mouse clicks spawn purge zones
	bool        PrintDebugMessages           = true;
	bool        ShowDebugItemNames           = true;
};
#pragma endregion

#pragma region ENTITIES
//Enumerations
//************
enum class eEntityType
{
	ITEM,
	ENEMY,
	PURGEZONE,

	//@END
	_LAST = PURGEZONE
};

enum class eItemType
{
	PISTOL,
	SHOTGUN,
	MEDKIT,
	FOOD,
	GARBAGE,
	RANDOM_DROP, //Internal Only
	RANDOM_DROP_WITH_CHANCE, //Internal Only

	//@END
	_LAST = GARBAGE
};

enum class eEnemyType
{
	DEFAULT,
	ZOMBIE_NORMAL,
	ZOMBIE_RUNNER,
	ZOMBIE_HEAVY,
	RANDOM_ENEMY, //Internal Only
	//...

	//@END
	_LAST = ZOMBIE_HEAVY
};


//Structures
//**********
struct StatisticsInfo
{
	int Score; //Current Score
	float Difficulty; //Current difficulty (0 > 1 > ... / Easy > Normal > Hard)
	float TimeSurvived; //Total time survived
	float KillCountdown; //Make sure to make a kill before this timer runs out

	int NumEnemiesKilled; //Amount of enemies killed
	int NumEnemiesHit; //Amount of enemy hits
	int NumItemsPickUp; //Amount of items picked up
	int NumMissedShots; //Shots missed after firing
	int NumChkpntsReached; //Amount of checkpoints reached

};

struct FOVStats
{
	int NumHouses     = 0;
	int NumEnemies    = 0;
	int NumItems      = 0;
	int NumPurgeZones = 0;
};

struct HouseInfo
{
	Elite::Vector2 Center;
	Elite::Vector2 Size;
};

struct CornerInfo
{
	bool Visited = false;
	Elite::Vector2 Location;

	int GetHash() const
	{
		return int(Location.x * 1000 + Location.y);
	}
};

struct DoorInfo
{
	Elite::Vector2 Location;

	int GetHash() const
	{
		return int(Location.x * 1000 + Location.y);
	}
};

struct HouseInfoEx : public HouseInfo
{
	bool  Visited     = false;
	bool DoorFound    = false;
	float TimeVisited = 0.0f;
	std::unordered_map<int, DoorInfo>   Doors;
	std::unordered_map<int, CornerInfo> Corners;

	int GetHash() const
	{
		return int(Center.x * 1000 + Center.y);
	}

	static HouseInfoEx CreateHouseInfoEx(const HouseInfo& houseInfo)
	{
		HouseInfoEx houseInfoEx {};
		houseInfoEx.Center = houseInfo.Center;
		houseInfoEx.Size   = houseInfo.Size;
		
		// create corners
		auto bottomLeft  = Elite::Vector2(houseInfo.Center.x - houseInfo.Size.x / 2, houseInfo.Center.y - houseInfo.Size.y / 2);
		auto bottomRight = Elite::Vector2(houseInfo.Center.x + houseInfo.Size.x / 2, houseInfo.Center.y - houseInfo.Size.y / 2);
		auto topLeft     = Elite::Vector2(houseInfo.Center.x - houseInfo.Size.x / 2, houseInfo.Center.y + houseInfo.Size.y / 2);
		auto topRight    = Elite::Vector2(houseInfo.Center.x + houseInfo.Size.x / 2, houseInfo.Center.y + houseInfo.Size.y / 2);
		
		constexpr float inwardOffset = 5.0f;
		bottomLeft  += Elite::Vector2( inwardOffset, inwardOffset);
		bottomRight += Elite::Vector2(-inwardOffset, inwardOffset);
		topLeft     += Elite::Vector2( inwardOffset, -inwardOffset);
		topRight    += Elite::Vector2(-inwardOffset, -inwardOffset);
		
		houseInfoEx.Corners.insert({ houseInfoEx.Corners.size(), CornerInfo{ false, bottomLeft } });
		houseInfoEx.Corners.insert({ houseInfoEx.Corners.size(), CornerInfo{ false, bottomRight } });
		houseInfoEx.Corners.insert({ houseInfoEx.Corners.size(), CornerInfo{ false, topLeft } });
		houseInfoEx.Corners.insert({ houseInfoEx.Corners.size(), CornerInfo{ false, topRight } });
		
		
		return houseInfoEx;
	}
};

struct EnemyInfo
{
	eEnemyType     Type;
	Elite::Vector2 Location;
	Elite::Vector2 LinearVelocity;

	int   EnemyHash = 0;
	float Size      = 0.0f;
	float Health    = 0.0f;
};

struct ItemInfo
{
	eItemType      Type;
	Elite::Vector2 Location;

	int ItemHash = 0;
	int Value    = 0;
};

struct PurgeZoneInfo
{
	Elite::Vector2 Center;
	float          Radius   = 0.0f;
	int            ZoneHash = 0;
};

struct EntityInfo
{
	eEntityType    Type;
	Elite::Vector2 Location;
	int            EntityHash = 0;
};

struct WorldInfo
{
	Elite::Vector2 Center;
	Elite::Vector2 Dimensions;
};

struct PathPointInfo
{
	Elite::Vector2 Location;
	bool           Visited = false;

	int GetHash() const
	{
		return int(Location.x * 1000 + Location.y);
	}
};

struct MemoryInfo
{
	std::unordered_map<int, ItemInfo>      Items;
	std::unordered_map<int, HouseInfoEx>   Houses;
	std::unordered_map<int, PathPointInfo> PathPoints;
	
	float TimeBitten      = -1000.0f;
	int TimeCanScan       = 2;
	float TimeStay        = -1000.0f;
	bool  TimeSet		  = false;
	float ForgetTimeHouse = 1000.0f;
	float ForgetTimeHurts = 1.0f;
	float ForgetTimeStay  = 12.0f;

	float CriticalHealth = 8.0f;

	// Min: 1, Max: 5
	int MaxHealthCount = 4;
	int MaxFoodCount   = 3;
	int MaxMedkitCount = 1;
	int MaxWeaponCount = 1;

	int MaxHouseCount = 20;

	float ItemRange = 150.0f;
};

struct AgentInfo
{
	float          Stamina;
	float          Health;
	float          Energy;
	bool           RunMode;
	bool           IsInHouse;
	bool           Bitten;    // agent was bitten by a zombie this frame (for internal use)
	bool           WasBitten; //agent was bitten by a zombie recently (0.5  seconds)
	bool           Death;

	float          FOV_Angle;
	float          FOV_Range;

	Elite::Vector2 LinearVelocity;
	float          AngularVelocity;
	float          CurrentLinearSpeed;
	Elite::Vector2 Position;
	float          Orientation;
	float          MaxLinearSpeed;
	float          MaxAngularSpeed;
	float          GrabRange;
	float          AgentSize;
};

struct AgentInfoEx : public AgentInfo
{
	Elite::Vector2 Position;
	Elite::Vector2 Direction;
	float TimeSurvived;

	static AgentInfoEx CreateAgentInfoEx(const AgentInfo& agentInfo)
	{
		AgentInfoEx agentInfoEx {};
		agentInfoEx.Stamina           = agentInfo.Stamina;
		agentInfoEx.Health            = agentInfo.Health;
		agentInfoEx.Energy            = agentInfo.Energy;
		agentInfoEx.RunMode           = agentInfo.RunMode;
		agentInfoEx.IsInHouse         = agentInfo.IsInHouse;
		agentInfoEx.Bitten            = agentInfo.Bitten;
		agentInfoEx.WasBitten         = agentInfo.WasBitten;
		agentInfoEx.Death             = agentInfo.Death;
		agentInfoEx.FOV_Angle         = agentInfo.FOV_Angle;
		agentInfoEx.FOV_Range         = agentInfo.FOV_Range;
		agentInfoEx.LinearVelocity    = agentInfo.LinearVelocity;
		agentInfoEx.AngularVelocity   = agentInfo.AngularVelocity;
		agentInfoEx.CurrentLinearSpeed = agentInfo.CurrentLinearSpeed;
		agentInfoEx.Position          = agentInfo.Position;
		agentInfoEx.Orientation       = agentInfo.Orientation;
		agentInfoEx.MaxLinearSpeed    = agentInfo.MaxLinearSpeed;
		agentInfoEx.MaxAngularSpeed   = agentInfo.MaxAngularSpeed;
		agentInfoEx.GrabRange         = agentInfo.GrabRange;
		agentInfoEx.AgentSize         = agentInfo.AgentSize;
		
		return agentInfoEx;
	}
};
#pragma endregion