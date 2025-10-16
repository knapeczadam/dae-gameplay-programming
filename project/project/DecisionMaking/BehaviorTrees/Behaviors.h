/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "../../DecisionMaking/SmartAgent.h"
#include "../../Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "../../Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"
#include "GlobalInterface.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------

namespace BT_Actions 
{
    //-----------------------------------------------------------------
    // TEMPLATE
    //-----------------------------------------------------------------
#pragma region Template
    inline Elite::BehaviorState Template(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget = nullptr;
        pBlackboard->GetData("Target", pTarget);
        
        float deltaTime = 0.f;
        pBlackboard->GetData("DeltaTime", deltaTime);
        
        float accuSearchTime = 0.f;
        pBlackboard->GetData("AccuSearchTime", accuSearchTime);
        
        accuSearchTime += deltaTime;
        pBlackboard->ChangeData("AccuSearchTime", accuSearchTime);

        Elite::DebugHelper* pDebugHelper = nullptr;
        pBlackboard->GetData("DebugHelper", pDebugHelper);
#pragma endregion
        
        return Elite::BehaviorState::Success;
    }
#pragma endregion

    //-----------------------------------------------------------------
    // TEST
    //-----------------------------------------------------------------
#pragma region Test
    inline Elite::BehaviorState Print(Elite::Blackboard* pBlackboard)
    {
        std::cout << __func__ << '\n';
        return Elite::BehaviorState::Success;
    }

    inline Elite::BehaviorState Success(Elite::Blackboard* pBlackboard)
    {
        std::cout << __func__ << '\n';
        return Elite::BehaviorState::Success;
    }

    inline Elite::BehaviorState Failure(Elite::Blackboard* pBlackboard)
    {
        std::cout << __func__ << '\n';
        return Elite::BehaviorState::Failure;
    }
#pragma endregion
    
    //-----------------------------------------------------------------
    // STEERING BEHAVIORS
    //-----------------------------------------------------------------
#pragma region Steering Behaviors
    // Seek
    inline Elite::BehaviorState Steering_Seek(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget = nullptr;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        // Orientation
        pPlayer->SetAutoOrient(true);

        // Target
        const auto seekBehavior = pPlayer->GetSeekBehavior(); 
        pPlayer->SetSteeringBehavior(seekBehavior);

        // Set target
        seekBehavior->SetTarget(*pTarget);
        
        if (not pPlayer->GetSteeringOutput().IsValid)
        {
            return Elite::BehaviorState::Success;
        }
        
        return Elite::BehaviorState::Running;
    }
    
    // Radar seek
    inline Elite::BehaviorState Steering_RadarSeek(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget = nullptr;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        // Orientation
        pPlayer->SetAutoOrient(false);

        // Target
        const auto radarSeekBehavior = pPlayer->GetRadarSeekBehavior(); 
        pPlayer->SetSteeringBehavior(radarSeekBehavior);

        // Set target
        radarSeekBehavior->SetTarget(*pTarget);

        if (not pPlayer->GetSteeringOutput().IsValid)
        {
            return Elite::BehaviorState::Success;
        }
        
        return Elite::BehaviorState::Running;
    }

    // Radar seek
    inline Elite::BehaviorState Steering_Radar(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget = nullptr;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        // Orientation
        pPlayer->SetAutoOrient(false);

        return Elite::BehaviorState::Success;
    }

    // Face
    inline Elite::BehaviorState Steering_Face(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);

        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        // Orientation
        pPlayer->SetAutoOrient(false);

        // Set steering
        const auto faceBehavior = pPlayer->GetFaceBehavior();
        pPlayer->SetSteeringBehavior(faceBehavior);

        // Set target
        faceBehavior->SetTarget(*pTarget);

        if (not pPlayer->GetSteeringOutput().IsValid)
        {
            return Elite::BehaviorState::Success;
        }

        return Elite::BehaviorState::Running;
    }

    // Wander
    inline Elite::BehaviorState Steering_Wander(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion

        // Orientation
        pPlayer->SetAutoOrient(true);

        // Set steering
        const auto wanderBehavior = pPlayer->GetWanderBehavior();
        pPlayer->SetSteeringBehavior(wanderBehavior);

        return Elite::BehaviorState::Success;
    }

    // Flee
    inline Elite::BehaviorState Steering_Flee(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        // Orientation
        pPlayer->SetAutoOrient(true);

        // Set steering
        const auto fleeBehavior = pPlayer->GetFleeBehavior();
        pPlayer->SetSteeringBehavior(fleeBehavior);

        // Set target
        fleeBehavior->SetTarget(*pTarget);

        return Elite::BehaviorState::Success;
    }

    // Evade
    inline Elite::BehaviorState Steering_Evade(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        // Orientation
        pPlayer->SetAutoOrient(true);

        // Set steering
        const auto evadeBehavior = pPlayer->GetEvadeBehavior();
        pPlayer->SetSteeringBehavior(evadeBehavior);

        // Set target
        evadeBehavior->SetTarget(*pTarget);

        if (not pPlayer->GetSteeringOutput().IsValid)
        {
            return Elite::BehaviorState::Success;
        }

        return Elite::BehaviorState::Running;
    }

    // Arrive
    inline Elite::BehaviorState Steering_Arrive(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        // Orientation
        pPlayer->SetAutoOrient(true);

        // Set steering
        const auto arriveBehavior = pPlayer->GetArriveBehavior();
        pPlayer->SetSteeringBehavior(arriveBehavior);

        // Set target
        arriveBehavior->SetTarget(*pTarget);

        if (not pPlayer->GetSteeringOutput().IsValid)
        {
            return Elite::BehaviorState::Success;
        }

        return Elite::BehaviorState::Running;
    }
#pragma endregion

    //-----------------------------------------------------------------
    // TARGET
    //-----------------------------------------------------------------
#pragma region Target
    // Set nearest enemy as target
    inline Elite::BehaviorState Target_SetNearestEnemy(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion
        
        const auto nearestEnemies = EXAMINTERFACE->GetEnemiesInFOV();
        if (nearestEnemies.empty()) return Elite::BehaviorState::Failure;
        
        // find nearest item based on distance from player
        const auto nearestEnemyIt = std::ranges::min_element(nearestEnemies, [pPlayer](const EnemyInfo& enemy1, const EnemyInfo& enemy2)
        {
            const auto playerPos = pPlayer->GetPosition();
            return Elite::DistanceSquared(enemy1.Location, playerPos) < Elite::DistanceSquared(enemy2.Location, playerPos);
        });
        
        if (nearestEnemyIt != nearestEnemies.end())
        {
            pTarget->Position       = nearestEnemyIt->Location;
            pTarget->LinearVelocity = nearestEnemyIt->LinearVelocity;
            return Elite::BehaviorState::Success;
        }
        
        return Elite::BehaviorState::Failure;
    }

    // Set player back
    inline Elite::BehaviorState Target_SetPlayerFront(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        const auto dir = pPlayer->GetDirection();
        pTarget->Position = pPlayer->GetPosition() + dir * EXAMINTERFACE->Agent_GetInfo().FOV_Range;
        
        return Elite::BehaviorState::Success;
    }
    
    inline Elite::BehaviorState Target_SetPlayerBack(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        const auto enemies = EXAMINTERFACE->GetEnemiesInFOV();
        if (enemies.empty()) return Elite::BehaviorState::Failure;
        
        const auto nearestEnemyIt = std::ranges::min_element(enemies, [pPlayer](const EnemyInfo& enemy1, const EnemyInfo& enemy2)
        {
            const auto playerPos = pPlayer->GetPosition();
            return Elite::DistanceSquared(enemy1.Location, playerPos) < Elite::DistanceSquared(enemy2.Location, playerPos);
        });
        if (nearestEnemyIt != enemies.end())
        {
            const auto dir = (pPlayer->GetPosition() - nearestEnemyIt->Location).GetNormalized();
            pTarget->Position = pPlayer->GetPosition() + dir * EXAMINTERFACE->Agent_GetInfo().FOV_Range;
            
            return Elite::BehaviorState::Success;
        }
        
        return Elite::BehaviorState::Failure;
    }

    // Set purged zone as target
    inline Elite::BehaviorState Target_SetPurgeZone(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion
        
        const auto purgeZones = EXAMINTERFACE->GetPurgeZonesInFOV();
        if (purgeZones.empty()) return Elite::BehaviorState::Failure;
        
        const auto nearestPurgeZoneIt = std::ranges::min_element(purgeZones, [pPlayer](const PurgeZoneInfo& purgeZone1, const PurgeZoneInfo& purgeZone2)
        {
            const auto playerPos = pPlayer->GetPosition();
            return Elite::DistanceSquared(purgeZone1.Center, playerPos) < Elite::DistanceSquared(purgeZone2.Center, playerPos);
        });
        
        if (nearestPurgeZoneIt != purgeZones.end())
        {
            const float evadeRadius = nearestPurgeZoneIt->Radius + EXAMINTERFACE->Agent_GetInfo().FOV_Range;
            pPlayer->GetEvadeBehavior()->SetEvadeRadius(evadeRadius);
            
            pTarget->Position = nearestPurgeZoneIt->Center;
            return Elite::BehaviorState::Success;
        }
        
        return Elite::BehaviorState::Failure;
    }

    // Set forward direction
    inline Elite::BehaviorState Target_SetForwardDirection(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion
        
        const auto direction = pPlayer->GetDirection();
        const float distance = EXAMINTERFACE->Agent_GetInfo().FOV_Range;
        const auto targetPos = pPlayer->GetPosition() + direction * distance;
        pTarget->Position = targetPos;

        return Elite::BehaviorState::Success;
    }

    // Set closest safe zone
    inline Elite::BehaviorState Target_SetClosestSafeZone(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        const auto purgeZones = EXAMINTERFACE->GetPurgeZonesInFOV();
        if (purgeZones.empty()) return Elite::BehaviorState::Failure;
        
        const auto nearestPurgeZoneIt = std::ranges::find_if(purgeZones, [pPlayer](const PurgeZoneInfo& purgeZone)
        {
            const auto playerPos = pPlayer->GetPosition();
            return Elite::DistanceSquared(purgeZone.Center, playerPos) < purgeZone.Radius * purgeZone.Radius;
        });
        if (nearestPurgeZoneIt != purgeZones.end())
        {
            const auto dir = (pPlayer->GetPosition() - nearestPurgeZoneIt->Center).GetNormalized();
            const auto targetPos = nearestPurgeZoneIt->Center + dir * (nearestPurgeZoneIt->Radius + EXAMINTERFACE->Agent_GetInfo().GrabRange);
            pTarget->Position = targetPos;
            return Elite::BehaviorState::Success;
        }
        return Elite::BehaviorState::Failure;
    }

    // Set closest garbage location
    inline Elite::BehaviorState Target_SetClosestGarbage(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion
        
        const auto closest = EXAMINTERFACE->Memory_GetClosesGarbageLocation(pPlayer->GetPosition());
        if (closest.has_value())
        {
            pTarget->Position = closest.value();
            return Elite::BehaviorState::Success;
        }
        
        return Elite::BehaviorState::Failure;
    }

    // Set closest item location
    inline Elite::BehaviorState Target_SetClosestPriorityItem(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        const auto closest = EXAMINTERFACE->Memory_GetClosestPriorityItemLocation(pPlayer->GetPosition());
        if (closest.has_value())
        {
            pTarget->Position = closest.value();
            return Elite::BehaviorState::Success;
        }
        return Elite::BehaviorState::Failure;
    }

    // Set closest house location
    inline Elite::BehaviorState Target_SetClosestHouseLocation(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion
        
        const auto closest = EXAMINTERFACE->Memory_GetClosestHouse(pPlayer->GetPosition());
        if (closest.has_value())
        {
            pTarget->Position = closest.value();
            return Elite::BehaviorState::Success;
        }
        
        return Elite::BehaviorState::Failure;
    }

    // Set closest unvisited house location
    inline Elite::BehaviorState Target_SetClosestUnvisitedHouse(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion
        
        const auto closest = EXAMINTERFACE->Memory_GetClosestUnvisitedHouse(pPlayer->GetPosition());
        if (closest.has_value())
        {
            pTarget->Position = closest.value();
            return Elite::BehaviorState::Success;
        }
        
        return Elite::BehaviorState::Failure;
    }
    
    // Set closest unvisited corner
    inline Elite::BehaviorState Target_SetClosestUnvisitedCorner(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion
        
        const auto closest = EXAMINTERFACE->Memory_GetClosestUnvisitedCorner(pPlayer->GetPosition());
        if (closest.has_value())
        {
            pTarget->Position = closest.value();
            return Elite::BehaviorState::Success;
        }
        
        return Elite::BehaviorState::Failure;
    }

    // Set closest path point location
    inline Elite::BehaviorState Target_SetClosestPathPointLocation(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        const auto closest = EXAMINTERFACE->Memory_GetClosestPath(pPlayer->GetPosition(), EXAMINTERFACE->World_GetInfo());
        if (closest.has_value())
        {
            pTarget->Position = closest.value();
            return Elite::BehaviorState::Success;
        }
        
        return Elite::BehaviorState::Failure;
    }

    // Set closest unvisited path point location
    inline Elite::BehaviorState Target_SetClosestUnvisitedPathPoint(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        const auto closest = EXAMINTERFACE->Memory_GetClosestUnvisitedPathPoint(pPlayer->GetPosition(), EXAMINTERFACE->World_GetInfo());
        if (closest.has_value())
        {
            pTarget->Position = closest.value();
            return Elite::BehaviorState::Success;
        }
        
        return Elite::BehaviorState::Failure;
    }
#pragma endregion
    
    //-----------------------------------------------------------------
    // INVENTORY
    //-----------------------------------------------------------------
#pragma region Inventory
    // Add nearest item
    inline Elite::BehaviorState Inventory_AddNearestItem(Elite::Blackboard* pBlackboard)
    {
		ItemInfo item;
        if (EXAMINTERFACE->GrabNearestItem(item))
        {
            const auto firstEmptySlot = EXAMINTERFACE->InventoryEx_GetFirstEmptySlot();
            if (firstEmptySlot.has_value())
            {
                if (EXAMINTERFACE->InventoryEx_AddItem(firstEmptySlot.value(), item))
                {
                    return Elite::BehaviorState::Success;
                }
            }
        }
        return Elite::BehaviorState::Failure;
    }

    // Destroy nearest item
    inline Elite::BehaviorState Inventory_DestroyNearestItem(Elite::Blackboard* pBlackboard)
    {
        const auto items = EXAMINTERFACE->GetItemsInFOV();
        bool success = false;
        for (const auto& item : items)
        {
            if (EXAMINTERFACE->DestroyItem(item))
            {
                success = true;
                EXAMINTERFACE->Memory_ForgetItem(item.ItemHash);
            }
        }
        return success ? Elite::BehaviorState::Success : Elite::BehaviorState::Failure;
    }

    // --- USE ITEM ---
#pragma region Use Item
    // Use pistol
    inline Elite::BehaviorState Inventory_UsePistol(Elite::Blackboard* pBlackboard)
    {
        if (EXAMINTERFACE->InventoryEx_UsePistol())
        {
            return Elite::BehaviorState::Success;
        }
        return Elite::BehaviorState::Failure;
    }

    // Use shotgun
    inline Elite::BehaviorState Inventory_UseShotgun(Elite::Blackboard* pBlackboard)
    {
        if (EXAMINTERFACE->InventoryEx_UseShotgun())
        {
            return Elite::BehaviorState::Success;
        }
        return Elite::BehaviorState::Failure;
    }
#pragma endregion

    // --- REMOVE ITEM ---
#pragma region Remove item
    // Remove empty item
    inline Elite::BehaviorState Inventory_RemoveEmptyItem(Elite::Blackboard* pBlackboard)
    {
        if (EXAMINTERFACE->InventoryEx_RemoveEmptyItem())
        {
            return Elite::BehaviorState::Success;
        }
        return Elite::BehaviorState::Failure;
    }

    // Remove weapon
    inline Elite::BehaviorState Inventory_RemoveWeapon(Elite::Blackboard* pBlackboard)
    {
        if (EXAMINTERFACE->InventoryEx_RemoveWeapon())
        {
            return Elite::BehaviorState::Success;
        }
        return Elite::BehaviorState::Failure;
    }
#pragma endregion
#pragma endregion

    //-----------------------------------------------------------------
    // ENEMY
    //-----------------------------------------------------------------
#pragma region Enemy
#pragma endregion

    //-----------------------------------------------------------------
    // MEMORY
    //-----------------------------------------------------------------
#pragma region Memory
    // Remember item
    inline Elite::BehaviorState Memory_RememberItem(Elite::Blackboard* pBlackboard)
    {
        const auto items = EXAMINTERFACE->GetItemsInFOV();
        bool success = false;
        for (const auto& item : items)
        {
            if (not EXAMINTERFACE->Memory_Get()->Items.contains(item.ItemHash))
            {
                EXAMINTERFACE->Memory_Get()->Items[item.ItemHash] = item;
                success = true;
            }
        }
        return success ? Elite::BehaviorState::Success : Elite::BehaviorState::Failure;
    }

    // Remember house
    inline Elite::BehaviorState Memory_RememberHouse(Elite::Blackboard* pBlackboard)
    {
        const auto houses = EXAMINTERFACE->GetHousesInFOV();
        bool success = false;
        for (const auto& house : houses)
        {
            HouseInfoEx houseInfo  = HouseInfoEx::CreateHouseInfoEx(house);
            if (not EXAMINTERFACE->Memory_Get()->Houses.contains(houseInfo.GetHash()))
            {
                EXAMINTERFACE->Memory_Get()->Houses[houseInfo.GetHash()] = houseInfo;
                success = true;
            }
        }
        
        return success ? Elite::BehaviorState::Success : Elite::BehaviorState::Failure;
    }

    // Remember first door
    inline Elite::BehaviorState Memory_RememberFirstDoor(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion
        
        bool success = false;
        const auto house  = EXAMINTERFACE->Memory_GetHouse(pPlayer->GetPosition());
        if (house.has_value())
        {
            HouseInfoEx houseInfo = house.value();
            if (not houseInfo.DoorFound)
            {
                const DoorInfo doorInfo {pPlayer->GetPosition() + pPlayer->GetDirection() * EXAMINTERFACE->Agent_GetInfo().GrabRange};
                if (not EXAMINTERFACE->Memory_Get()->Houses[houseInfo.GetHash()].Doors.contains(doorInfo.GetHash()))
                {
                    EXAMINTERFACE->Memory_Get()->Houses[houseInfo.GetHash()].Doors[doorInfo.GetHash()] = doorInfo;
                    EXAMINTERFACE->Memory_Get()->Houses[houseInfo.GetHash()].DoorFound = true;
                    success = true;
                }
            }
        }
        
        return success ? Elite::BehaviorState::Success : Elite::BehaviorState::Failure;
    }
    
    inline Elite::BehaviorState Memory_RememberSecondDoor(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion
        
        bool success = false;
        const auto playerBack = pPlayer->GetPosition() - pPlayer->GetDirection() * EXAMINTERFACE->Agent_GetInfo().AgentSize;
        const auto house  = EXAMINTERFACE->Memory_GetHouse(playerBack);
        if (house.has_value())
        {
            HouseInfoEx houseInfo = house.value();
            if (houseInfo.DoorFound and houseInfo.Doors.size() == 1)
            {
                const DoorInfo doorInfo {pPlayer->GetPosition() - pPlayer->GetDirection() * EXAMINTERFACE->Agent_GetInfo().GrabRange};
                
                const auto distance = Elite::DistanceSquared(houseInfo.Doors.begin()->second.Location, doorInfo.Location);
                const float minDistance = EXAMINTERFACE->Agent_GetInfo().FOV_Range / 2.0f;
                if (distance > minDistance * minDistance)
                {
                    if (not EXAMINTERFACE->Memory_Get()->Houses[houseInfo.GetHash()].Doors.contains(doorInfo.GetHash()))
                    {
                        EXAMINTERFACE->Memory_Get()->Houses[houseInfo.GetHash()].Doors[doorInfo.GetHash()] = doorInfo;
                        success = true;
                    }
                }
            }
        }
        
        return success ? Elite::BehaviorState::Success : Elite::BehaviorState::Failure;
    }

    // Memory work
    inline Elite::BehaviorState Memory_Refresh(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion
        
        AgentInfoEx agentInfo = AgentInfoEx::CreateAgentInfoEx(EXAMINTERFACE->Agent_GetInfo());
        agentInfo.Position     = pPlayer->GetPosition();
        agentInfo.Direction    = pPlayer->GetDirection();
        agentInfo.TimeSurvived = EXAMINTERFACE->World_GetStats().TimeSurvived;

        // Refresh memory: update internal inventory, remember visited house, path point
        EXAMINTERFACE->Memory_RefreshItem();
        EXAMINTERFACE->Memory_RefreshPathPoint(agentInfo, EXAMINTERFACE->World_GetInfo());
        EXAMINTERFACE->Memory_RefreshHouse(agentInfo);

        // // Manage run
        // if (EXAMINTERFACE->Agent_GetInfo().Stamina > 9.9f)
        // {
        //     pPlayer->CanRun(true);
        // }
        // else if (EXAMINTERFACE->Agent_GetInfo().Stamina < 0.1f)
        // {
        //     pPlayer->CanRun(false);
        // }
        
        return Elite::BehaviorState::Success;
    }

    // Memory forget
    inline Elite::BehaviorState Memory_Forget(Elite::Blackboard* pBlackboard)
    {
        EXAMINTERFACE->Memory_Forget();
        return Elite::BehaviorState::Success;
    }

    // Set time bitten
    inline Elite::BehaviorState Memory_SetTimeBitten(Elite::Blackboard* pBlackboard)
    {
        EXAMINTERFACE->Memory_Get()->TimeBitten = EXAMINTERFACE->World_GetStats().TimeSurvived;
        
        return Elite::BehaviorState::Success;
    }
    
    inline Elite::BehaviorState Memory_SetTime(Elite::Blackboard* pBlackboard)
    {
        
        if (not EXAMINTERFACE->Memory_Get()->TimeSet)
        {
            EXAMINTERFACE->Memory_Get()->TimeSet =  true;
            EXAMINTERFACE->Memory_Get()->TimeStay = EXAMINTERFACE->World_GetStats().TimeSurvived;
        }
        return Elite::BehaviorState::Success;
    }
#pragma endregion
    
    //-----------------------------------------------------------------
    // PLAYER
    //-----------------------------------------------------------------
#pragma region Player
    // Eat
    inline Elite::BehaviorState Player_Eat(Elite::Blackboard* pBlackboard)
    {
        const float energy = EXAMINTERFACE->Agent_GetInfo().Energy;
        if (EXAMINTERFACE->PlayerEx_Eat(energy))
        {
            return Elite::BehaviorState::Success;
        }
        return Elite::BehaviorState::Failure;
    }

    // Heal
    inline Elite::BehaviorState Player_Heal(Elite::Blackboard* pBlackboard)
    {
        const float health = EXAMINTERFACE->Agent_GetInfo().Health;
        if (EXAMINTERFACE->PlayerEx_Heal(health))
        {
            return Elite::BehaviorState::Success;
        }
        return Elite::BehaviorState::Failure;
    }

    // Emergency
    inline Elite::BehaviorState Player_EmergencyHeal(Elite::Blackboard* pBlackboard)
    {
        if (EXAMINTERFACE->InventoryEx_UseMedkit())
        {
            return Elite::BehaviorState::Success;
        }
        return Elite::BehaviorState::Failure;
    }

    // Run
    inline Elite::BehaviorState Player_Run(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion

        pPlayer->CanRun(true);
        
        return Elite::BehaviorState::Success;
    }
    // Toggle run
    inline Elite::BehaviorState Player_ToggleRun(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion

        const bool canRun = pPlayer->CanRun();
        pPlayer->CanRun(not canRun);
        
        const float timeSurvived = EXAMINTERFACE->World_GetStats().TimeSurvived;
        
        return Elite::BehaviorState::Success;
    }

    // Walk
    inline Elite::BehaviorState Player_Walk(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion

        pPlayer->CanRun(false);
        
        return Elite::BehaviorState::Success;
    }

    // Stay
    inline Elite::BehaviorState Player_Stay(Elite::Blackboard* pBlackboard)
    {
        if (EXAMINTERFACE->Memory_MustStay(EXAMINTERFACE->World_GetStats().TimeSurvived))
        {
            return Elite::BehaviorState::Running;
        }
        
        EXAMINTERFACE->Memory_Get()->TimeSet = false;
        return Elite::BehaviorState::Success;
    }

    // Scan
    inline Elite::BehaviorState Player_Scan(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion
        
        // cast 8 rays around the player
        const auto playerPos = pPlayer->GetPosition();
        const float distance = EXAMINTERFACE->World_GetInfo().Dimensions.x;
        
        static constexpr auto angles = {
            0.f,
            Elite::ToRadians(45.f),
            Elite::ToRadians(90.f),
            Elite::ToRadians(135.f),
            Elite::ToRadians(180.f),
            Elite::ToRadians(225.f),
            Elite::ToRadians(270.f),
            Elite::ToRadians(315.f)
        };
        
        for (const auto angle : angles)
        {
            const auto rayDir = Elite::Vector2{ cos(angle), sin(angle) };
            const auto rayEnd = playerPos + rayDir * distance;
            const auto hit = EXAMINTERFACE->NavMesh_GetClosestPathPoint(rayEnd);
            EXAMINTERFACE->Draw_Direction(playerPos, rayEnd, 0.5f, { 1.f, 0.f, 0.f });
        	if (hit.MagnitudeSquared() >= distance * distance) continue;
            {
                PathPointInfo pathPoint{ hit, false };
                if ( not EXAMINTERFACE->Memory_Get()->PathPoints.contains(pathPoint.GetHash()))
                {
                    EXAMINTERFACE->Memory_Get()->PathPoints.insert({ pathPoint.GetHash(), pathPoint });
                }
            }
        }
        
        return Elite::BehaviorState::Success;
    }
#pragma endregion

    //-----------------------------------------------------------------
    // WORLD
    //-----------------------------------------------------------------
#pragma region World
#pragma endregion
}




namespace BT_Conditions
{
    //-----------------------------------------------------------------
    // TEMPLATE
    //-----------------------------------------------------------------
#pragma region Template
    inline bool Template(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
        
        float deltaTime = 0.f;
        pBlackboard->GetData("DeltaTime", deltaTime);
        
        float accuSearchTime = 0.f;
        pBlackboard->GetData("AccuSearchTime", accuSearchTime);
        
        accuSearchTime += deltaTime;
        pBlackboard->ChangeData("AccuSearchTime", accuSearchTime);
        
        Elite::DebugHelper* pDebugHelper = nullptr;
        pBlackboard->GetData("DebugHelper", pDebugHelper);
#pragma endregion

        return true;
    }
#pragma endregion

    //-----------------------------------------------------------------
    // TEST
    //-----------------------------------------------------------------
#pragma region Test
    inline bool True(Elite::Blackboard* pBlackboard)
    {
        return true;
    }

    inline bool False(Elite::Blackboard* pBlackboard)
    {
        return false;
    }
#pragma endregion

    //-----------------------------------------------------------------
    // TARGET
    //-----------------------------------------------------------------
#pragma region Target
    // Is nearest item reached
    inline bool Target_IsReached(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion

        return Elite::DistanceSquared(pPlayer->GetPosition(), pTarget->Position) < 1.0f;
    }

    // Is enemy in shooting range
    inline bool FOV_IsInShootingRange(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);

        float shootingRange = 0.f;
        pBlackboard->GetData("TargetShootingRange", shootingRange);
#pragma endregion
        
        auto enemies = EXAMINTERFACE->GetEnemiesInFOV();
        if (enemies.empty()) return false;
        
        const auto nearestEnemyIt = std::ranges::min_element(enemies, [pPlayer](const EnemyInfo& enemy1, const EnemyInfo& enemy2)
        {
            const auto playerPos = pPlayer->GetPosition();
            return Elite::DistanceSquared(enemy1.Location, playerPos) < Elite::DistanceSquared(enemy2.Location, playerPos);
        });
        if (nearestEnemyIt != enemies.end())
        {
            return Elite::DistanceSquared(nearestEnemyIt->Location, pPlayer->GetPosition()) < shootingRange * shootingRange;
        }
        return false;
    }
#pragma endregion

    //-----------------------------------------------------------------
    // INVENTORY
    //-----------------------------------------------------------------
#pragma region Intentory
    // Is full
    inline bool Inventory_IsFull(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->InventoryEx_IsFull();
    }

    // Is weapon full
    inline bool Inventory_IsWeaponFull(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->InventoryEx_IsWeaponFull();
    }
    // --- FOV ---
#pragma region FOV
    // --- IS ITEM IN FOV ---
#pragma region Is Item
    // Is item in FOV
    inline bool FOV_ContainsItem(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->FOV_GetStats().NumItems > 0;
    }

    // Is only one item in FOV
    inline bool FOV_ContainsOneItem(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->FOV_GetStats().NumItems == 1;
    }
    
    // Is pistol in FOV
    inline bool FOV_ContainsPistol(Elite::Blackboard* pBlackboard)
    {
        return std::ranges::any_of(EXAMINTERFACE->GetItemsInFOV(), [](const ItemInfo& item) { return item.Type == eItemType::PISTOL; });
    }

    // Is shotgun in FOV
    inline bool FOV_ContainsShotgun(Elite::Blackboard* pBlackboard)
    {
        return std::ranges::any_of(EXAMINTERFACE->GetItemsInFOV(), [](const ItemInfo& item) { return item.Type == eItemType::SHOTGUN; });
    }

    // Is medkit in FOV
    inline bool FOV_ContainsMedkit(Elite::Blackboard* pBlackboard)
    {
        return std::ranges::any_of(EXAMINTERFACE->GetItemsInFOV(), [](const ItemInfo& item) { return item.Type == eItemType::MEDKIT; });
    }

    // Is food in FOV
    inline bool FOV_ContainsFood(Elite::Blackboard* pBlackboard)
    {
        return std::ranges::any_of(EXAMINTERFACE->GetItemsInFOV(), [](const ItemInfo& item) { return item.Type == eItemType::FOOD; });
    }
    
    // Is only garbage in FOV
    inline bool FOV_ContainsOnlyGarbage(Elite::Blackboard* pBlackboard)
    {
        return std::ranges::all_of(EXAMINTERFACE->GetItemsInFOV(), [](const ItemInfo& item) { return item.Type == eItemType::GARBAGE; });
    }

    // Nearest item in grab range
    inline bool FOV_NearestItemInGrabRange(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion
        const auto items = EXAMINTERFACE->GetItemsInFOV();
        if (items.empty()) return false;
        
        const auto nearestItemIt = std::ranges::min_element(items, [pPlayer](const ItemInfo& item1, const ItemInfo& item2)
        {
            const auto playerPos = pPlayer->GetPosition();
            return Elite::DistanceSquared(item1.Location, playerPos) < Elite::DistanceSquared(item2.Location, playerPos);
        });
        
        if (nearestItemIt != items.end())
        {
            const float grabRange = EXAMINTERFACE->Agent_GetInfo().GrabRange;
            return Elite::DistanceSquared(nearestItemIt->Location, pPlayer->GetPosition()) < grabRange * grabRange;
        }
        return false;
    }
#pragma endregion
#pragma endregion

    // --- CONTAINS ---
#pragma region Contains
    // Contains only one empty slot
    inline bool Inventory_ContainsOnlyOneEmptySlot(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->InventoryEx_ContainsOnlyOneEmptySlot();
    }

    // Contains empty item
    inline bool Inventory_ContainsEmptyItem(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->InventoryEx_ContainsEmptyItem();
    }
    
    // Has weapon in inventory
    inline bool Inventory_ContainsWeapon(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->InventoryEx_ContainsWeapon();
    }
    
    // Has pistol in inventory
    inline bool Inventory_ContainsPistol(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->InventoryEx_ContainsPistol();
    }

    // Has shotgun in inventory
    inline bool Inventory_ContainsShotgun(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->InventoryEx_ContainsShotgun();
    }

    // Has medkit in inventory
    inline bool Inventory_ContainsMedkit(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->InventoryEx_ContainsMedkit();
    }

    // Has food in inventory
    inline bool Inventory_ContainsFood(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->InventoryEx_ContainsFood();
    }
#pragma endregion
#pragma endregion

    //-----------------------------------------------------------------
    // ENEMY
    //-----------------------------------------------------------------
#pragma region Enemy
    // Is enemy in FOV
    inline bool FOV_ContainsEnemy(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->FOV_GetStats().NumEnemies > 0;
    }

    // Contains only one enemy
    inline bool FOV_ContainsOneEnemy(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->FOV_GetStats().NumEnemies == 1;
    }

    // Contains normal zombie
    inline bool FOV_ContainsNormalZombie(Elite::Blackboard* pBlackboard)
    {
        return std::ranges::any_of(EXAMINTERFACE->GetEnemiesInFOV(), [](const EnemyInfo& enemy) { return enemy.Type == eEnemyType::ZOMBIE_NORMAL; });
    }

    // Contains runner zombie
    inline bool FOV_ContainsRunnerZombie(Elite::Blackboard* pBlackboard)
    {
        return std::ranges::any_of(EXAMINTERFACE->GetEnemiesInFOV(), [](const EnemyInfo& enemy) { return enemy.Type == eEnemyType::ZOMBIE_RUNNER; });
    }

    // Contains heavy zombie
    inline bool FOV_ContainsHeavyZombie(Elite::Blackboard* pBlackboard)
    {
        return std::ranges::any_of(EXAMINTERFACE->GetEnemiesInFOV(), [](const EnemyInfo& enemy) { return enemy.Type == eEnemyType::ZOMBIE_HEAVY; });
    }
#pragma endregion

    //-----------------------------------------------------------------
    // MEMORY
    //-----------------------------------------------------------------
#pragma region Memory
    // Contains pistol
    inline bool Memory_ContainsPistol(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Memory_ContainsPistol();
    }

    // Contains shotgun
    inline bool Memory_ContainsShotgun(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Memory_ContainsShotgun();
    }

    // Contains medkit
    inline bool Memory_ContainsMedkit(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Memory_ContainsMedkit();
    }

    // Contains food
    inline bool Memory_ContainsFood(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Memory_ContainsFood();
    }

    // Contains garbage
    inline bool Memory_ContainsGarbage(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Memory_ContainsGarbage();
    }

    // Contains house
    inline bool Memory_ContainsHouse(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Memory_ContainsHouse();
    }

    // Contains item
    inline bool Memory_ContainsItem(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Memory_ContainsItem();
    }

    // Contains unvisited house
    inline bool Memory_ContainsUnvisitedHouse(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Memory_ContainsUnvisitedHouse();
    }

    // Contains unvisited corner
    inline bool Memory_ContainsUnvisitedCorner(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion
        return EXAMINTERFACE->Memory_ContainsUnvisitedCorner(pPlayer->GetPosition());
    }

    // Hurts
    inline bool Memory_Hurts(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Memory_Hurts(EXAMINTERFACE->World_GetStats().TimeSurvived);
    }

    // Contains path point
    inline bool Memory_ContainsPathPoint(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Memory_ContainsPathPoint();
    }

    // Contains unvisited path point
    inline bool Memory_ContainsUnvisitedPathPoint(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Memory_ContainsUnvisitedPathPoint();
    }

    // Time set
    inline bool Memory_TimeSet(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Memory_Get()->TimeSet;
    }
#pragma endregion

    //-----------------------------------------------------------------
    // WORLD
    //-----------------------------------------------------------------
#pragma region World
#pragma endregion

    //-----------------------------------------------------------------
    // HOUSES
    //-----------------------------------------------------------------
#pragma region House
    // Is house in FOV
    inline bool FOV_ContainsHouse(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->FOV_GetStats().NumHouses > 0;
    }
#pragma endregion

    //-----------------------------------------------------------------
    // PURGE ZONES
    //-----------------------------------------------------------------
#pragma region PurgeZone
    // Is purge zone in FOV
    inline bool FOV_ContainsPurgeZone(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->FOV_GetStats().NumPurgeZones > 0;
    }
#pragma endregion

    //-----------------------------------------------------------------
    // PLAYER
    //-----------------------------------------------------------------
#pragma region Player
    // Was bitten
    inline bool Player_WasBitten(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Agent_GetInfo().WasBitten;
    }

    // Is in house
    inline bool Player_IsInHouse(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Agent_GetInfo().IsInHouse;
    }

    // Is hungry
    inline bool Player_IsHungry(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Agent_GetInfo().Energy < 10.0f;
    }

    // Is wounded
    inline bool Player_IsWounded(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Agent_GetInfo().Health < 10.0f;
    }
    
    // Is wounded
    inline bool Player_IsCriticallyWounded(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Agent_GetInfo().Health <= EXAMINTERFACE->Memory_Get()->CriticalHealth;
    }

    // Has stamina
    inline bool Player_HasStamina(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Agent_GetInfo().Stamina > 0.f;
    }

    // Is in purge zone
    inline bool Player_IsInPurgeZone(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion
        
        const auto purgeZones = EXAMINTERFACE->GetPurgeZonesInFOV();

        return std::ranges::any_of(purgeZones, [pPlayer](const PurgeZoneInfo& purgeZone)
        {
            return Elite::DistanceSquared(pPlayer->GetPosition(), purgeZone.Center) < purgeZone.Radius * purgeZone.Radius;
        });
    }

    // Is running
    inline bool Player_IsRunning(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion

        return pPlayer->CanRun();
    }

    // Can scan
    inline bool Player_CanScan(Elite::Blackboard* pBlackboard)
    {
        return static_cast<int>(EXAMINTERFACE->World_GetStats().TimeSurvived) % EXAMINTERFACE->Memory_Get()->TimeCanScan == 0;
    }
    
    // Can shake
    inline bool Player_CanShake(Elite::Blackboard* pBlackboard)
    {
        return static_cast<int>(EXAMINTERFACE->World_GetStats().TimeSurvived) % 13 == 0;
    }

    // Can run
    inline bool Player_CanRun(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Agent_GetInfo().Stamina == 10.0f;
    }

    // Can shoot
    inline bool Player_CanShoot(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion
        
        auto enemies = EXAMINTERFACE->GetEnemiesInFOV();
        if (enemies.empty()) return false;

        auto nearestEnemyIt = std::ranges::min_element(enemies, [](const EnemyInfo& enemy1, const EnemyInfo& enemy2)
        {
            return enemy1.Location < enemy2.Location;
        });

        if (nearestEnemyIt != enemies.end())
        {
            float orientation = EXAMINTERFACE->Agent_GetInfo().Orientation;
            auto dir = Elite::Vector2{ cos(orientation), sin(orientation) };
            auto playerPos = pPlayer->GetPosition();
            auto enemyPos = nearestEnemyIt->Location;
            auto enemyDir = enemyPos - playerPos;
            auto angle = Elite::Dot(dir, enemyDir);
            return angle > 0.99f;
        }
        return false;
    }

    // Is tired
    inline bool Player_IsTired(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Agent_GetInfo().Stamina == 0.0f;
    }

    // World discovered
    inline bool Player_WorldDiscovered(Elite::Blackboard* pBlackboard)
    {
        return EXAMINTERFACE->Memory_Get()->Houses.size() == EXAMINTERFACE->Memory_Get()->MaxHouseCount;
    }

    // Is idle
    inline bool Player_IsIdle(Elite::Blackboard* pBlackboard)
    {
        const auto linearVelocity = EXAMINTERFACE->Agent_GetInfo().LinearVelocity;
        const auto angularVelocity = EXAMINTERFACE->Agent_GetInfo().AngularVelocity;
        const auto currentLinearSpeed = EXAMINTERFACE->Agent_GetInfo().CurrentLinearSpeed;
        // std::cout << "\033[1;31m";
        // std::cout << "Linear velocity:\t" << linearVelocity << '\n';
        // std::cout << "Current linear speed:\t" << currentLinearSpeed << '\n';
        // std::cout << "Angular velocity:\t" << angularVelocity << '\n';
        // std::cout << "\033[0m";
        return linearVelocity.Magnitude() < 0.1f and angularVelocity < 0.1f;
    }
#pragma endregion
}

#endif