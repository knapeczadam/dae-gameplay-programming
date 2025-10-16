#pragma once
#include "IExamInterface.h"

#include <optional>
#include <cassert>
#include <ranges>


class IExtendedExamInterface : public IExamInterface
{
public:
	IExtendedExamInterface();
	~IExtendedExamInterface();

	[[deprecated]]
	virtual void PreserveInterfaceAbstract() const = 0;

	//-----------------------------------------------------------------
	// MEMORY
	//-----------------------------------------------------------------
#pragma region Memory
	//-----------------------------------------------------------------
	// GETTERS, SETTERS AND UPDATERS
	//-----------------------------------------------------------------
#pragma region Getters, setters, updaters
	// Get
	inline MemoryInfo* Memory_Get() const
	{
		return m_pMemory;
	}
	
	// Set
	inline void Memory_Set(MemoryInfo* pMemory)
	{
		m_pMemory = pMemory;
	}

	// Inventory
	inline void Memory_RefreshItem()
	{
		for (const auto& item : *m_pInventory)
		{
			// item was picked up, remove from memory
			if (m_pMemory->Items.contains(item.ItemHash))
			{
				m_pMemory->Items.erase(item.ItemHash);
			}
		}
	}

	inline void Memory_RefreshPathPoint(AgentInfoEx agentInfo, WorldInfo worldInfo)
	{
		// If not in world, remove all path points
		std::erase_if(m_pMemory->PathPoints, [worldInfo](const auto& point)
		{
			return point.second.Location.x <= worldInfo.Center.x - worldInfo.Dimensions.x / 2.0f or
				   point.second.Location.x >= worldInfo.Center.x + worldInfo.Dimensions.x / 2.0f or
				   point.second.Location.y <= worldInfo.Center.y - worldInfo.Dimensions.y / 2.0f or
				   point.second.Location.y >= worldInfo.Center.y + worldInfo.Dimensions.y / 2.0f;
		});
		
		for (auto& pathPoint : m_pMemory->PathPoints)
		{
			if (not pathPoint.second.Visited)
			{
				if (Elite::DistanceSquared(pathPoint.second.Location, agentInfo.Position) < agentInfo.GrabRange * agentInfo.GrabRange)
				{
					pathPoint.second.Visited = true;
				}
			}
			// check if pathpoint is within house corner range
			for (const auto& house : m_pMemory->Houses)
			{
				// copy unordered corners to vector
				std::vector<CornerInfo> corners;
				for (const auto& corner : house.second.Corners)
				{
					corners.push_back(corner.second);
				}

				const auto dimension = house.second.Size;
				auto bottomLeft  = Elite::Vector2(house.second.Center.x - dimension.x / 2.0f, house.second.Center.y - dimension.y / 2.0f);
				auto bottomRight = Elite::Vector2(house.second.Center.x + dimension.x / 2.0f, house.second.Center.y - dimension.y / 2.0f);
				auto topLeft     = Elite::Vector2(house.second.Center.x - dimension.x / 2.0f, house.second.Center.y + dimension.y / 2.0f);
				auto topRight    = Elite::Vector2(house.second.Center.x + dimension.x / 2.0f, house.second.Center.y + dimension.y / 2.0f);

				constexpr float outwardOffset = 5.5f;
				bottomLeft  += Elite::Vector2(-outwardOffset, -outwardOffset);
				bottomRight += Elite::Vector2( outwardOffset, -outwardOffset);
				topLeft     += Elite::Vector2(-outwardOffset,  outwardOffset);
				topRight    += Elite::Vector2( outwardOffset,  outwardOffset);
				
				// if x and y are within bounds, mark as visited
				if (pathPoint.second.Location.x > bottomLeft.x and pathPoint.second.Location.x < bottomRight.x and
					pathPoint.second.Location.y > bottomLeft.y and pathPoint.second.Location.y < topLeft.y)
				{
					pathPoint.second.Visited = true;
				}
			}
		}
	}
	
	inline void Memory_RefreshHouse(AgentInfoEx agentInfo)
	{
		// Path points
		const auto house = std::ranges::find_if(m_pMemory->Houses, [this, agentInfo](const auto& house) { return house.second.GetHash() == Memory_GetHouseHash(agentInfo.Position); });
		if (house != m_pMemory->Houses.end())
		{
			for (auto& corner : house->second.Corners)
			{
				if (not corner.second.Visited)
				{
					const float distanceSq = Elite::DistanceSquared(corner.second.Location, agentInfo.Position);
					const float grabRangeSq = agentInfo.GrabRange * agentInfo.GrabRange;
					if (distanceSq < grabRangeSq)
					{
						corner.second.Visited = true;
						continue;
					}
					
					// check if point is covered by the FOV
					const auto dir = corner.second.Location - agentInfo.Position;
					if (Elite::DistanceSquared(corner.second.Location, agentInfo.Position) < agentInfo.FOV_Range * agentInfo.FOV_Range)
					{
						const float angle = std::atan2f(dir.y, dir.x);
						const float angleDiff = std::abs(angle - agentInfo.Orientation);
						if (std::abs(angleDiff) < agentInfo.FOV_Angle / 2.0f)
						{
							corner.second.Visited = true;
						}
					}
				}
			}
			// If house has 4 visited corners, mark house as visited
			if (std::ranges::count_if(house->second.Corners, [](const auto& corner) { return corner.second.Visited; }) == 4)
			{
				house->second.Visited = true;
				house->second.TimeVisited = agentInfo.TimeSurvived;
			}
		}
	}

	inline void Memory_Forget()
	{
		for (auto& house : m_pMemory->Houses)
		{
			house.second.Visited = false;
			house.second.TimeVisited = 0;

			for (auto& corner : house.second.Corners)
			{
				corner.second.Visited = false;
			}
		}

		for (auto& pathPoint : m_pMemory->PathPoints)
		{
			pathPoint.second.Visited = false;
		}
	}

	inline void Memory_ForgetItem(int itemHash)
	{
		if (m_pMemory->Items.contains(itemHash))
		{
			m_pMemory->Items.erase(itemHash);
		}
	}
#pragma endregion

	//-----------------------------------------------------------------
	// PREDICATES
	//-----------------------------------------------------------------
#pragma region Predicates
	// Contains item
	inline bool Memory_ContainsItem() const
	{
		return m_pMemory->Items.size() > 0;
	}
#pragma endregion

	//-----------------------------------------------------------------
	// CLOSEST POSITION
	//-----------------------------------------------------------------
#pragma region Closest Position
	// Get closest item location
	inline std::optional<Elite::Vector2> Memory_GetClosestItemLocation(const Elite::Vector2& playerPos)
	{
		const auto it = std::ranges::min_element(m_pMemory->Items, [playerPos](const auto& item1, const auto& item2)
		{
			return Elite::DistanceSquared(item1.second.Location, playerPos) < Elite::DistanceSquared(item2.second.Location, playerPos);
		});
		if (it != m_pMemory->Items.end())
		{
			return it->second.Location;
		}
		return {};
	}
	
	// Get closest item location by type
	inline std::optional<Elite::Vector2> Memory_GetClosestItemLocation(const Elite::Vector2& playerPos, eItemType itemType)
	{
		auto filtered = std::ranges::filter_view(m_pMemory->Items, [itemType](const auto& item) { return item.second.Type == itemType; });
		const auto it = std::ranges::min_element(filtered, [playerPos, itemType](const auto& item1, const auto& item2)
		{
			return Elite::DistanceSquared(item1.second.Location, playerPos) < Elite::DistanceSquared(item2.second.Location, playerPos);
		});
		
		if (it != filtered.end())
		{
			return it->second.Location;
		}
		return {};
	}

	// Get closest item
	inline std::optional<ItemInfo> Memory_GetClosestItem(const Elite::Vector2& playerPos)
	{
		const auto it = std::ranges::min_element(m_pMemory->Items, [playerPos](const auto& item1, const auto& item2)
		{
			return Elite::DistanceSquared(item1.second.Location, playerPos) < Elite::DistanceSquared(item2.second.Location, playerPos);
		});
		if (it != m_pMemory->Items.end())
		{
			return it->second;
		}
		return {};
	}

	// Get closest priority location
	inline std::optional<Elite::Vector2> Memory_GetClosestPriorityItemLocation(const Elite::Vector2& playerPos)
	{
		const int pistolCount  = InventoryEx_GetPistolCount();
		const int shotgunCount = InventoryEx_GetShotgunCount();
		const int medkitCount  = InventoryEx_GetMedkitCount();
		const int foodCount    = InventoryEx_GetFoodCount();

		const int weaponCount = pistolCount + shotgunCount;

		if (weaponCount < m_pMemory->MaxWeaponCount and Memory_ContainsWeapon() and Memory_WeaponReachable(playerPos))
		{
			return Memory_GetClosesWeaponLocation(playerPos);
		}
		else if (foodCount < m_pMemory->MaxFoodCount and Memory_ContainsFood() and Memory_FoodReachable(playerPos))
		{
			return Memory_GetClosesFoodLocation(playerPos);
		}
		else if (medkitCount < m_pMemory->MaxMedkitCount and Memory_ContainsMedkit() and Memory_MedkitReachable(playerPos))
		{
			return Memory_GetClosesMedkitLocation(playerPos);
		}
		
		return {};
	}

	// Get closest weapon location
	inline std::optional<Elite::Vector2> Memory_GetClosesWeaponLocation(const Elite::Vector2& playerPos)
	{
		const auto pistolLocation  = Memory_GetClosesPistolLocation(playerPos);
		const auto shotgunLocation = Memory_GetClosesShotgunLocation(playerPos);

		if (pistolLocation.has_value() and shotgunLocation.has_value())
		{
			if (Elite::DistanceSquared(shotgunLocation.value(), playerPos) < Elite::DistanceSquared(pistolLocation.value(), playerPos))
			{
				return shotgunLocation;
			}
			return pistolLocation;
		}
		else if (shotgunLocation.has_value())
		{
			return shotgunLocation;
		}
		else if (pistolLocation.has_value())
		{
			return pistolLocation;
		}
		
		return {};
	}

	// Get closest pistol location
	inline std::optional<Elite::Vector2> Memory_GetClosesPistolLocation(const Elite::Vector2& playerPos)
	{
		return Memory_GetClosestItemLocation(playerPos, eItemType::PISTOL);
	}

	// Get closest shotgun location
	inline std::optional<Elite::Vector2> Memory_GetClosesShotgunLocation(const Elite::Vector2& playerPos)
	{
		return Memory_GetClosestItemLocation(playerPos, eItemType::SHOTGUN);
	}

	// Get closest medkit location
	inline std::optional<Elite::Vector2> Memory_GetClosesMedkitLocation(const Elite::Vector2& playerPos)
	{
		return Memory_GetClosestItemLocation(playerPos, eItemType::MEDKIT);
	}

	// Get closest food location
	inline std::optional<Elite::Vector2> Memory_GetClosesFoodLocation(const Elite::Vector2& playerPos)
	{
		return Memory_GetClosestItemLocation(playerPos, eItemType::FOOD);
	}

	// Get closest garbage location
	inline std::optional<Elite::Vector2> Memory_GetClosesGarbageLocation(const Elite::Vector2& playerPos)
	{
		return Memory_GetClosestItemLocation(playerPos, eItemType::GARBAGE);
	}

	// Get closest unvisited house
	inline std::optional<Elite::Vector2> Memory_GetClosestUnvisitedHouse(const Elite::Vector2& playerPos)
	{
		auto filtered = std::ranges::filter_view(m_pMemory->Houses, [](const auto& house) { return not house.second.Visited; });

		// First get closest door
		std::vector<DoorInfo> doors;
		for (const auto& house : filtered)
		{
			for (const auto& door : house.second.Doors)
			{
				doors.push_back(door.second);
			}
		}
		if (not doors.empty())
		{
			const auto it = std::ranges::min_element(doors, [playerPos](const auto& door1, const auto& door2)
			{
				return Elite::DistanceSquared(door1.Location, playerPos) < Elite::DistanceSquared(door2.Location, playerPos);
			});
			if (it != doors.end())
			{
				return it->Location;
			}
		}

		// If no doors, get closest house
		const auto it = std::ranges::min_element(filtered, [playerPos](const auto& house1, const auto& house2)
		{
			return Elite::DistanceSquared(house1.second.Center, playerPos) < Elite::DistanceSquared(house2.second.Center, playerPos);
		});
		if (it != filtered.end())
		{
			return it->second.Center;
		}
		return {};
	}

	// Get closest house
	inline std::optional<Elite::Vector2> Memory_GetClosestHouse(const Elite::Vector2& playerPos)
	{
		auto filtered = std::ranges::filter_view(m_pMemory->Houses, [this, playerPos](const auto& house) { return Memory_GetHouseHash(playerPos) != house.second.GetHash(); });
		
		// First get closest door
		std::vector<DoorInfo> doors;
		for (const auto& house : filtered)
		{
			for (const auto& door : house.second.Doors)
			{
				doors.push_back(door.second);
			}
		}
		if (not doors.empty())
		{
			const auto it = std::ranges::min_element(doors, [playerPos](const auto& door1, const auto& door2)
			{
				return Elite::DistanceSquared(door1.Location, playerPos) < Elite::DistanceSquared(door2.Location, playerPos);
			});
			if (it != doors.end())
			{
				return it->Location;
			}
		}
		const auto it = std::ranges::min_element(filtered, [playerPos](const auto& house1, const auto& house2)
		{
			return Elite::DistanceSquared(house1.second.Center, playerPos) < Elite::DistanceSquared(house2.second.Center, playerPos);
		});
		if (it != filtered.end())
		{
			return it->second.Center;
		}
		return {};
	}

	// Get closest unvisited corner
	inline std::optional<Elite::Vector2> Memory_GetClosestUnvisitedCorner(const Elite::Vector2& playerPos)
	{
		const auto house = std::ranges::find_if(m_pMemory->Houses, [this, playerPos](const auto& house) { return house.second.GetHash() == Memory_GetHouseHash(playerPos); });
		
		auto filtered = std::ranges::filter_view(house->second.Corners, [](const auto& corner) { return not corner.second.Visited; });
		const auto it = std::ranges::min_element(filtered, [playerPos](const auto& corner1, const auto& corner2)
		{
			return Elite::DistanceSquared(corner1.second.Location, playerPos) < Elite::DistanceSquared(corner2.second.Location, playerPos);
		});

		if (it != filtered.end())
		{
			return it->second.Location;
		}
		
		return {};
	}

	// Get closest unvisited path point
	inline std::optional<Elite::Vector2> Memory_GetClosestUnvisitedPathPoint(const Elite::Vector2& playerPos, WorldInfo worldInfo)
	{
		// In world and unvisited
		auto filtered = std::ranges::filter_view(m_pMemory->PathPoints, [worldInfo](const auto& point)
		{
			return point.second.Location.x > worldInfo.Center.x - worldInfo.Dimensions.x / 2.0f and
				   point.second.Location.x < worldInfo.Center.x + worldInfo.Dimensions.x / 2.0f and
				   point.second.Location.y > worldInfo.Center.y - worldInfo.Dimensions.y / 2.0f and
				   point.second.Location.y < worldInfo.Center.y + worldInfo.Dimensions.y / 2.0f and
			       not point.second.Visited;
		});
		const auto it = std::ranges::min_element(filtered, [playerPos](const auto& point1, const auto& point2)
		{
			return Elite::DistanceSquared(point1.second.Location, playerPos) < Elite::DistanceSquared(point2.second.Location, playerPos);
		});
		if (it != filtered.end())
		{
			return it->second.Location;
		}
		return {};
	}

	// Get closest path point
	inline std::optional<Elite::Vector2> Memory_GetClosestPath(const Elite::Vector2& playerPos, WorldInfo worldInfo)
	{
		// In world
		auto filtered = std::ranges::filter_view(m_pMemory->PathPoints, [worldInfo](const auto& point)
		{
			return point.second.Location.x > worldInfo.Center.x - worldInfo.Dimensions.x / 2.0f and
				   point.second.Location.x < worldInfo.Center.x + worldInfo.Dimensions.x / 2.0f and
				   point.second.Location.y > worldInfo.Center.y - worldInfo.Dimensions.y / 2.0f and
				   point.second.Location.y < worldInfo.Center.y + worldInfo.Dimensions.y / 2.0f;
		});
		const auto it = std::ranges::min_element(filtered, [playerPos](const auto& point1, const auto& point2)
		{
			return Elite::DistanceSquared(point1.second.Location, playerPos) < Elite::DistanceSquared(point2.second.Location, playerPos);
		});
		if (it != filtered.end())
		{
			return it->second.Location;
		}
		return {};
	}

	// Get closest safe location
	inline std::optional<Elite::Vector2> Memory_GetClosestSafeZone(const Elite::Vector2& playerPos, PurgeZoneInfo purgeZone)
	{
		// get a path point that is outside the purge zone
		auto filtered = std::ranges::filter_view(m_pMemory->PathPoints, [purgeZone](const auto& point)
		{
			return Elite::DistanceSquared(point.second.Location, purgeZone.Center) > ((purgeZone.Radius * 1.5f) * (purgeZone.Radius * 1.5f));
		});
		const auto it = std::ranges::min_element(filtered, [playerPos](const auto& point1, const auto& point2)
		{
			return Elite::DistanceSquared(point1.second.Location, playerPos) < Elite::DistanceSquared(point2.second.Location, playerPos);
		});
		if (it != filtered.end())
		{
			return it->second.Location;
		}
		return {};
	}
#pragma endregion

	//-----------------------------------------------------------------
	// HELPER FUNCTIONS
	//-----------------------------------------------------------------
#pragma region Helper functions
private:
	inline int Memory_GetHouseHash(const Elite::Vector2& playerPos) const
	{
		// player is within bounds
		auto filtered = std::ranges::filter_view(m_pMemory->Houses, [playerPos](const auto& house)
		{
			return playerPos.x > house.second.Center.x - house.second.Size.x / 2.0f and
				   playerPos.x < house.second.Center.x + house.second.Size.x / 2.0f and
				   playerPos.y > house.second.Center.y - house.second.Size.y / 2.0f and
				   playerPos.y < house.second.Center.y + house.second.Size.y / 2.0f;
		});
		if (filtered.begin() != filtered.end())
		{
			return filtered.begin()->second.GetHash();
		}
		return 0;
	}
public:
	std::optional<HouseInfoEx> Memory_GetHouse(const Elite::Vector2& playerPos) const
	{
		const auto house = std::ranges::find_if(m_pMemory->Houses, [this, playerPos](const auto& house) { return house.second.GetHash() == Memory_GetHouseHash(playerPos); });
		if (house != m_pMemory->Houses.end())
		{
			return house->second;
		}
		return {};
	}
#pragma endregion

	//-----------------------------------------------------------------
	// PREDICATES
	//-----------------------------------------------------------------
#pragma region Predicates
public:
	// Item reachable
	inline bool Memory_ItemReachable(const Elite::Vector2& playerPos, eItemType itemType) const
	{
		auto filtered = std::ranges::filter_view(m_pMemory->Items, [itemType](const auto& item) { return item.second.Type == itemType; });
		return std::ranges::any_of(filtered, [playerPos](const auto& item) { return Elite::DistanceSquared(item.second.Location, playerPos) < m_pMemory->ItemRange * m_pMemory->ItemRange; });
	}

	// Weapon reachable
	inline bool Memory_WeaponReachable(const Elite::Vector2& playerPos) const
	{
		return Memory_PistolReachable(playerPos) or Memory_ShotgunReachable(playerPos);
	}

	// Pistol reachable
	inline bool Memory_PistolReachable(const Elite::Vector2& playerPos) const
	{
		return Memory_ItemReachable(playerPos, eItemType::PISTOL);
	}

	// Shotgun reachable
	inline bool Memory_ShotgunReachable(const Elite::Vector2& playerPos) const
	{
		return Memory_ItemReachable(playerPos, eItemType::SHOTGUN);
	}

	// Medkit reachable
	inline bool Memory_MedkitReachable(const Elite::Vector2& playerPos) const
	{
		return Memory_ItemReachable(playerPos, eItemType::MEDKIT);
	}

	// Food reachable
	inline bool Memory_FoodReachable(const Elite::Vector2& playerPos) const
	{
		return Memory_ItemReachable(playerPos, eItemType::FOOD);
	}
	
	// Contains weapon
	inline bool Memory_ContainsWeapon() const
	{
		return std::ranges::any_of(m_pMemory->Items, [](const auto& item) { return item.second.Type == eItemType::PISTOL or item.second.Type == eItemType::SHOTGUN; });
	}
	
	// Contains pistol
	inline bool Memory_ContainsPistol() const
	{
		return std::ranges::any_of(m_pMemory->Items, [](const auto& item) { return item.second.Type == eItemType::PISTOL; });
	}

	// Contains shotgun
	inline bool Memory_ContainsShotgun() const
	{
		return std::ranges::any_of(m_pMemory->Items, [](const auto& item) { return item.second.Type == eItemType::SHOTGUN; });
	}

	// Contains medkit
	inline bool Memory_ContainsMedkit() const
	{
		return std::ranges::any_of(m_pMemory->Items, [](const auto& item) { return item.second.Type == eItemType::MEDKIT; });
	}

	// Contains food
	inline bool Memory_ContainsFood() const
	{
		return std::ranges::any_of(m_pMemory->Items, [](const auto& item) { return item.second.Type == eItemType::FOOD; });
	}

	// Contains garbage
	inline bool Memory_ContainsGarbage() const
	{
		return std::ranges::any_of(m_pMemory->Items, [](const auto& item) { return item.second.Type == eItemType::GARBAGE; });
	}

	// Contains house
	inline bool Memory_ContainsHouse() const
	{
		return m_pMemory->Houses.size() > 0;
	}

	// Contains path point
	inline bool Memory_ContainsPathPoint() const
	{
		return m_pMemory->PathPoints.size() > 0;
	}

	// Contains unvisited house
	inline bool Memory_ContainsUnvisitedHouse() const
	{
		return std::ranges::any_of(m_pMemory->Houses, [](const auto& house) { return not house.second.Visited; });
	}

	// Contains unvisited corner
	inline bool Memory_ContainsUnvisitedCorner(const Elite::Vector2& playerPos) const
	{
		const auto house = std::ranges::find_if(m_pMemory->Houses, [this, playerPos](const auto& house) { return house.second.GetHash() == Memory_GetHouseHash(playerPos); });
		if (house != m_pMemory->Houses.end())
		{
			return std::ranges::any_of(house->second.Corners, [](const auto& corner) { return not corner.second.Visited; });
		}
		return false;
	}

	// Contains unvisited path point
	inline bool Memory_ContainsUnvisitedPathPoint() const
	{
		return std::ranges::any_of(m_pMemory->PathPoints, [](const auto& point) { return not point.second.Visited; });
	}

	// Hurts
	inline bool Memory_Hurts(float timeSurvived) const
	{
		const float forgetTime = m_pMemory->ForgetTimeHurts;
		return m_pMemory->TimeBitten > timeSurvived - forgetTime;
	}

	// Must stay
	inline bool Memory_MustStay(float timeSurvived) const
	{
		const float forgetTime = m_pMemory->ForgetTimeStay;
		return m_pMemory->TimeStay > timeSurvived - forgetTime;
	}
#pragma endregion
#pragma endregion

	//-----------------------------------------------------------------
	// INVENTORY
	//-----------------------------------------------------------------
#pragma region Inventory
	//-----------------------------------------------------------------
	// GETTERS, SETTERS AND UPDATERS
	//-----------------------------------------------------------------
#pragma region Getters, setters, updaters
	// Get
	inline std::array<ItemInfo, 5>* InventoryEx_Get() const
	{
		return m_pInventory;
	}

	// Set
	inline void InventoryEx_Set(std::array<ItemInfo, 5>* pInventory)
	{
		m_pInventory = pInventory;
	}

	// Update
	inline void InventoryEx_UpdateItem(UINT slotId, ItemInfo item)
	{
		(*m_pInventory)[slotId] = item;
	}

	// Update all
	inline void InventoryEx_UpdateItems()
	{
		for (UINT i = 0; i < Inventory_GetCapacity(); ++i)
		{
			ItemInfo item;
			if (Inventory_GetItem(i, item))
			{
				(*m_pInventory)[i] = item;
			}
		}
	}
#pragma endregion

	//-----------------------------------------------------------------
	// EXTENDED INVENTORY
	//-----------------------------------------------------------------
#pragma region Extended Inventory
	inline bool InventoryEx_AddItem(UINT slotId, ItemInfo item)
	{
		if (Inventory_AddItem(slotId, item))
		{
			(*m_pInventory)[slotId] = item;
			return true;
		}
		return false;
	}

	inline UINT InventoryEx_GetCapacity() const
	{
		const auto capacity = Inventory_GetCapacity();
		assert(capacity == 5 and "Inventory capacity is not 5!");
		return capacity;
	}
	
	inline bool InventoryEx_RemoveItem(UINT slotId)
	{
		if (Inventory_RemoveItem(slotId))
		{
			(*m_pInventory)[slotId] = {};
			return true;
		}
		return false;
	}
	
	inline bool InventoryEx_UseItem(UINT slotId)
	{
		if (Inventory_UseItem(slotId))
		{
			// TODO: Check if this is needed
			// (*m_pInventory)[slotId].Value = 0;
			return true;
		}
		return false;
	}
#pragma endregion

	//-----------------------------------------------------------------
	// ACTIONS
	//-----------------------------------------------------------------
#pragma region Actions
	// --- USE ITEM FUNCTIONS ---
#pragma region Use item
	// Use pistol
	inline bool InventoryEx_UsePistol()
	{
		const auto slot = InventoryEx_GetLowestValueSlot(eItemType::PISTOL);
		if (slot.has_value())
		{
			return InventoryEx_UseItem(slot.value());
		}
		return false;
	}

	// Use shotgun
	inline bool InventoryEx_UseShotgun()
	{
		const auto slot = InventoryEx_GetLowestValueSlot(eItemType::SHOTGUN);
		if (slot.has_value())
		{
			return InventoryEx_UseItem(slot.value());
		}
		return false;
	}

	// Use medkit
	inline bool InventoryEx_UseMedkit()
	{
		const auto slot = InventoryEx_GetLowestValueSlot(eItemType::MEDKIT);
		if (slot.has_value())
		{
			return InventoryEx_UseItem(slot.value());
		}
		return false;
	}
#pragma endregion

	// --- REMOVE ITEM FUNCTIONS ---
#pragma region Remove item
	// Remove all empty items
	inline bool InventoryEx_RemoveEmptyItem()
	{
		bool removed = false;
		for (UINT i = 0; i < 5; ++i)
		{
			if ((*m_pInventory)[i].ItemHash != 0 and (*m_pInventory)[i].Value == 0)
			{
				removed = InventoryEx_RemoveItem(i);
			}
		}
		return removed;
	}

	// Remove pistol
	inline bool InventoryEx_RemovePistol()
	{
		const auto slot = InventoryEx_GetLowestValueSlot(eItemType::PISTOL);
		if (slot.has_value())
		{
			return InventoryEx_RemoveItem(slot.value());
		}
		return false;
	}

	// Remove shotgun
	inline bool InventoryEx_RemoveShotgun()
	{
		const auto slot = InventoryEx_GetLowestValueSlot(eItemType::SHOTGUN);
		if (slot.has_value())
		{
			return InventoryEx_RemoveItem(slot.value());
		}
		return false;
	}

	// Remove Weapon
	inline bool InventoryEx_RemoveWeapon()
	{
		if (InventoryEx_ContainsPistol())
		{
			return InventoryEx_RemovePistol();
		}
		else if (InventoryEx_ContainsShotgun())
		{
			return InventoryEx_RemoveShotgun();
		}
		return false;
	}
#pragma endregion

#pragma endregion

	//-----------------------------------------------------------------
	// PREDICATES
	//-----------------------------------------------------------------
#pragma region Predicates
	// --- IS FUNCTIONS ---
#pragma region Is
	// Is full
	inline bool InventoryEx_IsFull() const
	{
		return InventoryEx_GetItemCount() == InventoryEx_GetCapacity();
	}

	// Is weapon full
	inline bool InventoryEx_IsWeaponFull() const
	{
		return InventoryEx_GetWeaponCount() > m_pMemory->MaxWeaponCount;
	}
#pragma endregion
	
	// --- CONTAINS ITEM FUNCTIONS ---
#pragma region Contains item
	// Contains only one empty slot
	inline bool InventoryEx_ContainsOnlyOneEmptySlot() const
	{
		return InventoryEx_GetItemCount() == 4;
	}

	// Contains empty item
	inline bool InventoryEx_ContainsEmptyItem() const
	{
		auto filtered = std::ranges::filter_view(*m_pInventory, [](const ItemInfo& item) { return item.ItemHash != 0; });
		return std::ranges::any_of(filtered, [](const ItemInfo& item) { return item.Value == 0; });
	}

private:
	// Contains item by type
	inline bool InventoryEx_ContainsItem(eItemType itemType) const
	{
		auto filtered = std::ranges::filter_view(*m_pInventory, [itemType](const ItemInfo& item) { return item.ItemHash != 0; });
		return std::ranges::any_of(filtered, [itemType](const ItemInfo& item) { return item.Type == itemType; });
	}
	
public:

	// Contains weapon
	inline bool InventoryEx_ContainsWeapon() const
	{
		return InventoryEx_ContainsItem(eItemType::PISTOL) or InventoryEx_ContainsItem(eItemType::SHOTGUN);
	}

	// Contains pistol
	inline bool InventoryEx_ContainsPistol() const
	{
		return InventoryEx_ContainsItem(eItemType::PISTOL);
	}

	// Contains shotgun
	inline bool InventoryEx_ContainsShotgun() const
	{
		return InventoryEx_ContainsItem(eItemType::SHOTGUN);
	}

	// Contains medkit
	inline bool InventoryEx_ContainsMedkit() const
	{
		return InventoryEx_ContainsItem(eItemType::MEDKIT);
	}

	// Contains food
	inline bool InventoryEx_ContainsFood() const
	{
		return InventoryEx_ContainsItem(eItemType::FOOD);
	}
#pragma endregion

#pragma endregion

	//-----------------------------------------------------------------
	// NUMBERS 
	//-----------------------------------------------------------------
#pragma region Numbers
	// --- GET COUNT ---
#pragma region Get item count
private:
	// Get item count
	inline int InventoryEx_GetItemCount() const
	{
		return std::ranges::count_if(*m_pInventory, [](const ItemInfo& item) { return item.ItemHash != 0; });
	}
	
	// Get item count by type
	inline int InventoryEx_GetItemCount(eItemType itemType) const
	{
		auto filtered = std::ranges::filter_view(*m_pInventory, [itemType](const ItemInfo& item) { return item.ItemHash != 0; });
		return std::ranges::count_if(filtered, [itemType](const ItemInfo& item) { return item.Type == itemType; });
	}

	// Get weapon count
	inline int InventoryEx_GetWeaponCount() const
	{
		return InventoryEx_GetItemCount(eItemType::PISTOL) + InventoryEx_GetItemCount(eItemType::SHOTGUN);
	}

	// Get pistol count
	inline int InventoryEx_GetPistolCount() const
	{
		return InventoryEx_GetItemCount(eItemType::PISTOL);
	}

	// Get shotgun count
	inline int InventoryEx_GetShotgunCount() const
	{
		return InventoryEx_GetItemCount(eItemType::SHOTGUN);
	}

	// Get medkit count
	inline int InventoryEx_GetMedkitCount() const
	{
		return InventoryEx_GetItemCount(eItemType::MEDKIT);
	}

	// Get food count
	inline int InventoryEx_GetFoodCount() const
	{
		return InventoryEx_GetItemCount(eItemType::FOOD);
	}
#pragma endregion

	// --- GET LOWEST VALUE FUNCTIONS ---
#pragma region Get lowest value
public:
	// Get lowest value item
	inline int InventoryEx_GetLowestValue() const
	{
		auto filtered = std::ranges::filter_view(*m_pInventory, [](const ItemInfo& item) { return item.ItemHash != 0; });
		const auto it = std::ranges::min_element(filtered, [](const ItemInfo& item1, const ItemInfo& item2) { return item1.Value < item2.Value; });
		if (it != filtered.end())
		{
			return it->Value;
		}
		return 0;
	}

private:
	// Get lowest value item by type
	inline int InventoryEx_GetLowestValue(eItemType itemType) const
	{
		auto filtered = std::ranges::filter_view(*m_pInventory, [itemType](const ItemInfo& item) { return item.ItemHash != 0 and item.Type == itemType; });
		const auto it = std::ranges::min_element(filtered, [itemType](const ItemInfo& item1, const ItemInfo& item2) { return item1.Value < item2.Value; });
		if (it != filtered.end())
		{
			return it->Value;
		}
		return 0;
	}
#pragma endregion
	
	// --- GET SLOT FUNCTIONS ---
#pragma region Get slot
	// Get first empty slot
public:
	inline std::optional<UINT> InventoryEx_GetFirstEmptySlot() const
	{
		const auto it = std::ranges::find_if(*m_pInventory, [](const ItemInfo& item) { return item.ItemHash == 0; });
		if (it != m_pInventory->end())
		{
			return static_cast<UINT>(std::distance(m_pInventory->begin(), it));
		}
		return {};
	}

private:
	// Get lowest value item slot by type
	inline std::optional<UINT> InventoryEx_GetLowestValueSlot(eItemType itemType) const
	{
		auto filtered = std::ranges::filter_view(*m_pInventory, [itemType](const ItemInfo& item) { return item.ItemHash != 0 and item.Type == itemType; });
		auto it = std::ranges::min_element(filtered, [itemType](const ItemInfo& item1, const ItemInfo& item2) { return item1.Value < item2.Value; });
		if (it != filtered.end())
		{
			const auto idx = std::ranges::find_if(*m_pInventory, [it](const ItemInfo& item) { return item.ItemHash == it->ItemHash; });
			return static_cast<UINT>(std::distance(m_pInventory->begin(), idx));
		}
		return {};
	}
#pragma endregion
#pragma endregion
#pragma endregion

	//-----------------------------------------------------------------
	// PLAYER
	//-----------------------------------------------------------------
#pragma region Player
	// --- LIFE FUNCTIONS ---
#pragma region Life
	// Is hungry
	inline bool PlayerEx_IsHungry(float energy)
	{
		constexpr float maxEnergy = 10.0f;
		const float diff = maxEnergy - energy;
		auto filtered = std::ranges::filter_view(*m_pInventory, [diff](const ItemInfo& item) { return item.ItemHash != 0; });
		return std::ranges::any_of(filtered, [diff](const ItemInfo& item) { return item.Type == eItemType::FOOD and item.Value <= diff; });
	}

	// Is wounded
	inline bool PlayerEx_IsWounded(float health)
	{
		constexpr float maxHealth = 10.0f;
		const float diff = maxHealth - health;
		auto filtered = std::ranges::filter_view(*m_pInventory, [diff](const ItemInfo& item) { return item.ItemHash != 0; });
		return std::ranges::any_of(filtered, [diff](const ItemInfo& item) { return item.Type == eItemType::MEDKIT and item.Value <= diff; });
	}
	
	// Eat
public:
	inline bool PlayerEx_Eat(float energy)
	{
		constexpr float maxEnergy = 10.0f;
		const float diff = maxEnergy - energy;
		auto filtered = std::ranges::filter_view(*m_pInventory, [diff](const ItemInfo& item) { return item.ItemHash != 0; });
		const auto it = std::ranges::find_if(filtered, [diff](const ItemInfo& item) { return item.Type == eItemType::FOOD and item.Value <= diff; });
		if (it != filtered.end())
		{
			const auto idx = std::ranges::find_if(*m_pInventory, [it](const ItemInfo& item) { return item.ItemHash == it->ItemHash; });
			return InventoryEx_UseItem(static_cast<UINT>(std::distance(m_pInventory->begin(), idx)));
		}
		return false;
	}

	// Heal
	inline bool PlayerEx_Heal(float health)
	{
		constexpr float maxHealth = 10.0f;
		const float diff = maxHealth - health;
		auto filtered = std::ranges::filter_view(*m_pInventory, [diff](const ItemInfo& item) { return item.ItemHash != 0; });
		const auto it = std::ranges::find_if(filtered, [diff](const ItemInfo& item) { return item.Type == eItemType::MEDKIT and item.Value <= diff; });
		if (it != filtered.end())
		{
			const auto idx = std::ranges::find_if(*m_pInventory, [it](const ItemInfo& item) { return item.ItemHash == it->ItemHash; });
			return InventoryEx_UseItem(static_cast<UINT>(std::distance(m_pInventory->begin(), idx)));
		}
		return false;
	}
#pragma endregion
#pragma endregion

private:
	inline static std::array<ItemInfo, 5>* m_pInventory = nullptr;
	inline static MemoryInfo*              m_pMemory = nullptr;
};

