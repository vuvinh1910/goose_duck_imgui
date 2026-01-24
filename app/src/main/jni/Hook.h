#pragma once

#include <vector>
#include <algorithm>
#include <mutex>
#include <unordered_set>
#include <atomic>
#include <float.h>
#include <math.h>
#include "TuanMeta/IL2CppSDKGenerator/Vector3.h"

#if defined(__aarch64__)
	auto RETURN = "CO 03 5F D6";
	auto NOP = "1F 20 03 D5";
	auto TRUE = "20 00 80 D2 CO 03 5F D6";
	auto FALSE = "00 00 80 D2 CO 03 5F D6";
	auto window_name = OBFUSCATE("Tuấn Meta | @ZTRChannel [X64]");
#else
	auto RETURN = "1E FF 2F E1";
	auto NOP = "00 F0 20 E3";
	auto TRUE = "01 00 A0 E3 1E FF 2F E1";
	auto FALSE = "00 00 A0 E3 1E FF 2F E1";
	auto window_name = OBFUSCATE("Tuấn Meta | @ZTRChannel [X32]");
#endif

int speed = 5, zoom = 1;
bool noFog, noCoolDown, callBell, noClip, fastTask, alwayMove, spectatorMode;
uintptr_t playerCollider, disableMovement, fogMesh;

// Use unordered_set for O(1) lookup instead of vector O(n)
std::unordered_set<void*> targetsSet;
std::vector<void*> targetsList; // For UI display only

std::mutex g_TargetMutex;

// Cached function pointers - avoid repeated GetMethodOffset calls
static void* cachedDisableFog = nullptr;
static void* cachedAdjustCamera = nullptr;
static void* cachedCompleteTask = nullptr;
static void* cachedClosePanel = nullptr;
static void* cachedCallEmergency = nullptr;
static void* cachedSetEnabled = nullptr;
static void* cachedDeactivateRoofs = nullptr;
static void* cachedGetNickname = nullptr;
static void* cachedIsLocal = nullptr;
static void* cachedGetTransform = nullptr;
static void* cachedGetPosition = nullptr;
static void* cachedSetPosition = nullptr;
static void* cachedSetCameraFollow = nullptr;


// Local player pointer
void* localPlayerPointer = nullptr;

// Teleport request - defer to main thread
std::atomic<int> pendingTeleportIndex{-1};
std::atomic<bool> shouldDoTeleport{false};

// Spectator request - defer to main thread
std::atomic<int> pendingSpectatorIndex{-1};
std::atomic<bool> shouldDoSpectator{false};

#pragma pack(push, 4)

struct ACTkByte4 {
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;
    uint8_t b4;
};

struct ObscuredFloat {
    int32_t currentCryptoKey;
    int32_t hiddenValue;
    ACTkByte4 hiddenValueOldByte4;
    bool inited;
    float fakeValue;
    bool fakeValueActive;
};

struct ObscuredInt {
    int32_t currentCryptoKey;
    int32_t hiddenValue;
    bool inited;
    int32_t fakeValue;
    bool fakeValueActive;
};

struct ObscuredBool {
    uint8_t currentCryptoKey;
    int32_t hiddenValue;
    bool inited;
    bool fakeValue;
    bool fakeValueActive;
};

#pragma pack(pop)

typedef ObscuredInt (*OpImplicit_t)(int value);
static OpImplicit_t ObscuredInt_OpImplicit;
ObscuredInt IntToObscuredInt(int value) {
    if(!ObscuredInt_OpImplicit) {
        const char* args[1] = { "System.Int32" };
        void* addr = Il2Cpp::GetMethodOffset(
                "ThirdPartyAssembly.dll",
                "CodeStage.AntiCheat.ObscuredTypes",
                "ObscuredInt",
                "op_Implicit",
                (char**)args,
                1
        );
        ObscuredInt_OpImplicit = (OpImplicit_t)addr;
    }
    return ObscuredInt_OpImplicit(value);
}

typedef ObscuredFloat (*OpImplicitFloat_t)(float value);
static OpImplicitFloat_t ObscuredFloat_OpImplicit;
ObscuredFloat FloatToObscuredFloat(float value) {
    if (!ObscuredFloat_OpImplicit) {
        const char* args[1] = { "System.Single" };
        void* addr = Il2Cpp::GetMethodOffset(
                "ACTk.Runtime.dll",
                "CodeStage.AntiCheat.ObscuredTypes",
                "ObscuredFloat",
                "op_Implicit",
                (char**)args,
                1
        );
        ObscuredFloat_OpImplicit = (OpImplicitFloat_t)addr;
    }
    return ObscuredFloat_OpImplicit(value);
}

typedef ObscuredBool (*OpImplicitBool_t)(bool value);
static OpImplicitBool_t ObscuredBool_OpImplicit;
ObscuredBool BoolToObscuredBool(bool value) {
    if (!ObscuredBool_OpImplicit) {
        const char* args[1] = { "System.Boolean" };
        void* addr = Il2Cpp::GetMethodOffset(
                "ACTk.Runtime.dll",
                "CodeStage.AntiCheat.ObscuredTypes",
                "ObscuredBool",
                "op_Implicit",
                (char**)args,
                1
        );
        ObscuredBool_OpImplicit = (OpImplicitBool_t)addr;
    }
    return ObscuredBool_OpImplicit(value);
}

void AdjustCamera(void *instance, float value) {
    if(!cachedAdjustCamera) {
        cachedAdjustCamera = (void*)GetMethodOffset(
                oxorany("Assembly-CSharp.dll"),
                oxorany("Handlers.GameHandlers.PlayerHandlers"),
                oxorany("LocalPlayer"),
                oxorany("OverrideOrthographicSize"),
                1
        );
    }
    if(cachedAdjustCamera) {
        ((void (*)(void *, float))cachedAdjustCamera)(instance, value);
    }
}

void NoFog(void *instance) {
    if(!instance) return;
    *(void**)((uintptr_t)instance + fogMesh) = nullptr;
}

void (*_FogUpdate)(void *instance);
void FogUpdate(void *instance) {
    if(instance && noFog) {
        NoFog(instance);
    }
    _FogUpdate(instance);
}
void (*_set_Cooldown)(void *instance, ObscuredFloat value);
void set_Cooldown(void *instance, ObscuredFloat value) {
    if(instance && noCoolDown) {
        return _set_Cooldown(instance, FloatToObscuredFloat(0.06));
    }
    _set_Cooldown(instance, value);
}

void DoFastTask(void *instance) {
    if(!cachedCompleteTask) {
        cachedCompleteTask = (void*)GetMethodOffset(
                oxorany("Assembly-CSharp.dll"),
                oxorany("Handlers.GameHandlers.TaskHandlers"),
                oxorany("TaskPanelHandler"),
                oxorany("CompleteTask"),
                0
        );
    }
    if(!cachedClosePanel) {
        cachedClosePanel = (void*)GetMethodOffset(
                oxorany("Assembly-CSharp.dll"),
                oxorany("Handlers.GameHandlers.TaskHandlers"),
                oxorany("TaskPanelHandler"),
                oxorany("ClosePanel"),
                0
        );
    }
    if(cachedCompleteTask) {
        ((void (*)(void *))cachedCompleteTask)(instance);
    }
    if(cachedClosePanel) {
        ((void (*)(void *))cachedClosePanel)(instance);
    }
}

void (*_UpdatePanel)(void *instance);
void UpdatePanel(void *instance) {
    if(instance && fastTask) {
        DoFastTask(instance);
    }
    _UpdatePanel(instance);
}

void CallEmergency(void *instance) {
    if(!cachedCallEmergency) {
        cachedCallEmergency = (void*)GetMethodOffset(
                oxorany("Assembly-CSharp.dll"),
                oxorany("Handlers.GameHandlers.PlayerHandlers"),
                oxorany("PlayerController"),
                oxorany("CallEmergency"),
                0
        );
    }
    if(cachedCallEmergency) {
        ((void (*)(void *))cachedCallEmergency)(instance);
    }
}

void DoNoClip(void *instance, bool check) {
    if(!cachedSetEnabled) {
        cachedSetEnabled = (void*)GetMethodOffset(
                oxorany("UnityEngine.CoreModule.dll"),
                oxorany("UnityEngine"),
                oxorany("Behaviour"),
                oxorany("set_enabled"),
                1
        );
    }
    if(cachedSetEnabled) {
        ((void (*)(void *, bool))cachedSetEnabled)(instance, check);
    }
}

void (*_CallUpdate)(void *instance);
void CallUpdate(void *instance) {
    if(instance) {
        if(callBell) {
            CallEmergency(instance);
            callBell = false;
        }
        if(noClip) {
            void *addr = *(void**)((uintptr_t)instance + playerCollider);
            if(addr) {
                DoNoClip(addr, false);
            }
        }
    }
    _CallUpdate(instance);
}

void DoNoRoof(void *instance, bool check) {
    if(!cachedDeactivateRoofs) {
        cachedDeactivateRoofs = (void*)GetMethodOffset(
                oxorany("Assembly-CSharp.dll"),
                oxorany("Handlers.GameHandlers"),
                oxorany("RoofHandler"),
                oxorany("DeactivateRoofs"),
                1
        );
    }
    if(cachedDeactivateRoofs) {
        ((void (*)(void *, bool))cachedDeactivateRoofs)(instance, check);
    }
}

void (*_RoomUpdate)(void *instance, void *room);
void RoomUpdate(void *instance, void *room) {
    if(instance) {
        if(noFog) {
            DoNoRoof(instance, true);
        }
    }
    _RoomUpdate(instance, room);
}

std::string GetNickname(void *entity) {
    if(!entity) return "Unknown";
    
    if(!cachedGetNickname) {
        cachedGetNickname = (void*)GetMethodOffset(
                oxorany("Assembly-CSharp.dll"),
                oxorany("Handlers.GameHandlers.PlayerHandlers"),
                oxorany("PlayableEntity"),
                oxorany("get_Nickname"),
                0
        );
    }
    
    if(cachedGetNickname) {
        void* monoString = ((void* (*)(void *))cachedGetNickname)(entity);
        if(monoString) {
            auto chars = (uint16_t*)((uintptr_t)monoString + 0x14);
            int len = *(int*)((uintptr_t)monoString + 0x10);
            
            // Pre-reserve to avoid reallocations
            std::string result;
            result.reserve(len * 3);
            
            for(int i = 0; i < len; i++) {
                uint16_t ch = chars[i];
                if(ch < 0x80) {
                    result += (char)ch;
                } 
                else if(ch < 0x800) {
                    result += (char)(0xC0 | (ch >> 6));
                    result += (char)(0x80 | (ch & 0x3F));
                } 
                else {
                    result += (char)(0xE0 | (ch >> 12));
                    result += (char)(0x80 | ((ch >> 6) & 0x3F));
                    result += (char)(0x80 | (ch & 0x3F));
                }
            }
            return result;
        }
    }
    return "Unknown";
}

bool IsLocalPlayer(void* entity) {
    if(!entity) return false;
    
    if(!cachedIsLocal) {
        cachedIsLocal = (void*)GetMethodOffset(
                oxorany("Assembly-CSharp.dll"),
                oxorany("Handlers.GameHandlers.PlayerHandlers"),
                oxorany("PlayableEntity"),
                oxorany("get_IsLocal"),
                0
        );
    }
    if(cachedIsLocal) {
        return ((bool (*)(void*))cachedIsLocal)(entity);
    }
    return false;
}

void* GetTransform(void* component) {
    if(!component) return nullptr;
    
    if(!cachedGetTransform) {
        cachedGetTransform = (void*)GetMethodOffset(
                oxorany("UnityEngine.CoreModule.dll"),
                oxorany("UnityEngine"),
                oxorany("Component"),
                oxorany("get_transform"),
                0
        );
    }
    if(cachedGetTransform) {
        return ((void* (*)(void*))cachedGetTransform)(component);
    }
    return nullptr;
}

void DoSpectator(void* localPlayerInstance, void* targetPlayer) {
    if(!localPlayerInstance || !targetPlayer) return;
    
    if(!cachedSetCameraFollow) {
        cachedSetCameraFollow = (void*)GetMethodOffset(
                oxorany("Assembly-CSharp.dll"),
                oxorany("Handlers.GameHandlers.PlayerHandlers"),
                oxorany("LocalPlayer"),
                oxorany("SetCameraFollow"),
                1
        );
    }
    
    if(cachedSetCameraFollow) {
        void* targetTransform = GetTransform(targetPlayer);
        if(targetTransform) {
            ((void (*)(void*, void*))cachedSetCameraFollow)(localPlayerInstance, targetTransform);
        }
    }
}

void (*_PlayerUpdate)(void *instance);
void PlayerUpdate(void *instance) {
    if(instance) {
        // Handle spectator request
        if(shouldDoSpectator.load() && pendingSpectatorIndex.load() >= 0) {
            std::vector<void*> playersCopy;
            {
                std::lock_guard<std::mutex> lock(g_TargetMutex);
                playersCopy = targetsList;
            }
            int idx = pendingSpectatorIndex.load();
            if(idx < (int)playersCopy.size() && playersCopy[idx]) {
                DoSpectator(instance, playersCopy[idx]);
            }
            shouldDoSpectator.store(false);
            pendingSpectatorIndex.store(-1);
        }
        
        if(zoom > 1) {
            AdjustCamera(instance, (float)zoom);
        }
        if(speed >= 5) {
            ObscuredFloat newSpeed = FloatToObscuredFloat(speed);
            Il2Cpp::SetStaticFieldValue(
                    "Assembly-CSharp.dll",
                    "Handlers.GameHandlers.PlayerHandlers",
                    "LocalPlayer",
                    "movementSpeed",
                    &newSpeed
            );
        }
        if(alwayMove) {
            *(bool *)((uintptr_t)instance + disableMovement) = false;
        }
    }
    _PlayerUpdate(instance);
}

Vector3 GetPosition(void* transform) {
    Vector3 pos = {0, 0, 0};
    if(!transform) return pos;
    
    if(!cachedGetPosition) {
        cachedGetPosition = (void*)GetMethodOffset(
                oxorany("UnityEngine.CoreModule.dll"),
                oxorany("UnityEngine"),
                oxorany("Transform"),
                oxorany("get_position"),
                0
        );
    }
    if(cachedGetPosition) {
        pos = ((Vector3 (*)(void*))cachedGetPosition)(transform);
    }
    return pos;
}

void SetPosition(void* transform, Vector3 pos) {
    if(!transform) return;
    
    if(!cachedSetPosition) {
        cachedSetPosition = (void*)GetMethodOffset(
                oxorany("UnityEngine.CoreModule.dll"),
                oxorany("UnityEngine"),
                oxorany("Transform"),
                oxorany("set_position"),
                1
        );
    }
    if(cachedSetPosition) {
        ((void (*)(void*, Vector3))cachedSetPosition)(transform, pos);
    }
}

void DoTeleport(void* localPlayer, void* targetPlayer) {
    if(!localPlayer || !targetPlayer) return;
    
    void* targetTransform = GetTransform(targetPlayer);
    void* localTransform = GetTransform(localPlayer);
    
    if(targetTransform && localTransform) {
        Vector3 targetPos = GetPosition(targetTransform);
        SetPosition(localTransform, targetPos);
    }
}

void (*_OnDestroyEntity)(void *instance);
void OnDestroyEntity(void *instance) {
    if(instance) {
        std::lock_guard<std::mutex> lock(g_TargetMutex);
        targetsSet.clear();
        targetsList.clear();
        
        // Reset pointers when game ends (entities destroyed)
        localPlayerPointer = nullptr;
    }
    _OnDestroyEntity(instance);
}

void (*_UpdateEntity)(void *instance);
void UpdateEntity(void *instance) {
    if(instance) {
        std::lock_guard<std::mutex> lock(g_TargetMutex);
        
        // Capture local player pointer
        if(!localPlayerPointer && IsLocalPlayer(instance)) {
            localPlayerPointer = instance;
        }
        
        // Execute pending teleport on main thread
        if(shouldDoTeleport.load() && localPlayerPointer && pendingTeleportIndex.load() >= 0) {
            int idx = pendingTeleportIndex.load();
            if(idx < (int)targetsList.size() && targetsList[idx]) {
                DoTeleport(localPlayerPointer, targetsList[idx]);
            }
            shouldDoTeleport.store(false);
            pendingTeleportIndex.store(-1);
        }
        
        // O(1) lookup with unordered_set
        if(targetsSet.find(instance) == targetsSet.end()) {
            targetsSet.insert(instance);
            targetsList.push_back(instance);
        }
    }
    _UpdateEntity(instance);
}

std::vector<void*> GetTargetsCopy() {
    std::lock_guard<std::mutex> lock(g_TargetMutex);
    return targetsList;
}

// Use squared distance to avoid expensive sqrtf - for comparison only
inline float CalculateDistanceSquared(Vector3 a, Vector3 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

// Real distance when needed for display
inline float CalculateDistance(Vector3 a, Vector3 b) {
    return sqrtf(CalculateDistanceSquared(a, b));
}

struct NearbyPlayerInfo {
    void* player;
    float distance;
    std::string name;
};

// Find all players within specified range
std::vector<NearbyPlayerInfo> FindPlayersInRange(void* deadEntity, Vector3 deadPos, float maxDistance) {
    std::vector<NearbyPlayerInfo> result;
    float maxDistSq = maxDistance * maxDistance;
    
    // Quick copy under lock - minimize lock time
    std::vector<void*> playersCopy;
    {
        std::lock_guard<std::mutex> lock(g_TargetMutex);
        playersCopy = targetsList;
    }
    
    // Now iterate without holding lock - IL2CPP calls are slow
    for(void* entity : playersCopy) {
        if(entity == deadEntity || entity == nullptr) continue;
        
        void* transform = GetTransform(entity);
        if(!transform) continue;
        
        Vector3 entityPos = GetPosition(transform);
        float distSq = CalculateDistanceSquared(deadPos, entityPos);
        
        // Check if within range
        if(distSq <= maxDistSq) {
            NearbyPlayerInfo info;
            info.player = entity;
            info.distance = sqrtf(distSq);
            info.name = GetNickname(entity);
            result.push_back(info);
        }
    }
    
    // Sort by distance (closest first)
    std::sort(result.begin(), result.end(), [](const NearbyPlayerInfo& a, const NearbyPlayerInfo& b) {
        return a.distance < b.distance;
    });
    
    return result;
}



void (*_TurnIntoGhost)(void *instance, int deathReason);
void TurnIntoGhost(void *instance, int deathReason) {
    if(instance) {
        std::string deadPlayerName = GetNickname(instance);
        
        void* deadTransform = GetTransform(instance);
        if(deadTransform) {
            Vector3 deadPos = GetPosition(deadTransform);
            
            // Find all players within 3.0 distance
            std::vector<NearbyPlayerInfo> nearbyPlayers = FindPlayersInRange(instance, deadPos, 3.0f);
            
            if(!nearbyPlayers.empty()) {
                // Build list of nearby players
                std::string playersList;
                for(size_t i = 0; i < nearbyPlayers.size(); i++) {
                    if(i > 0) playersList += ", ";
                    playersList += nearbyPlayers[i].name;
                    char distBuf[16];
                    snprintf(distBuf, sizeof(distBuf), " (%.1f)", nearbyPlayers[i].distance);
                    playersList += distBuf;
                }
                
                AddDebugLog("[DEATH] %s died! Nearby: %s", 
                    deadPlayerName.c_str(), playersList.c_str());
            } else {
                AddDebugLog("[DEATH] %s died! No players within range.", 
                    deadPlayerName.c_str());
            }
        } else {
            AddDebugLog("[DEATH] %s died! (Could not get position)", 
                deadPlayerName.c_str());
        }
    }
    _TurnIntoGhost(instance, deathReason);
}
