#pragma once

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
bool noFog,noCoolDown,callBell,noClip,fastTask,alwayMove;
uintptr_t playerCollider,disableMovement;

#pragma pack(push, 4)

struct ACTkByte4 {
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;
    uint8_t b4;
};

struct ObscuredFloat {
    int32_t currentCryptoKey;       // 0x10
    int32_t hiddenValue;            // 0x14
    ACTkByte4 hiddenValueOldByte4;  // 0x18
    bool inited;                    // 0x1C
    float fakeValue;                // 0x20
    bool fakeValueActive;           // 0x24
};

struct ObscuredInt {
    int32_t currentCryptoKey;   // 0x10
    int32_t hiddenValue;        // 0x14
    bool inited;                // 0x18
    int32_t fakeValue;          // 0x1C
    bool fakeValueActive;       // 0x20
};

struct ObscuredBool {
    uint8_t currentCryptoKey;   // 0x10
    int32_t hiddenValue;        // 0x14
    bool inited;                // 0x18
    bool fakeValue;             // 0x19
    bool fakeValueActive;       // 0x1A
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

void NoFog(void *instance) {
    void (*_Disable)(void *) = (void (*)(void *))(
            GetMethodOffset(
                    oxorany("Assembly-CSharp.dll"),
                    oxorany("Handlers.GameHandlers.SpecialBehaviour"),
                    oxorany("FogOfWar"),
                    oxorany("Disable"),
                    0
            )
    );
    if(_Disable) {
        _Disable(instance);
    }
}

void AdjustCamera(void *instance, float value) {
    void (*_OverrideOrthographicSize)(void *, float) = (void (*)(void *, float))(
            GetMethodOffset(
                    oxorany("Assembly-CSharp.dll"),
                    oxorany("Handlers.GameHandlers.PlayerHandlers"),
                    oxorany("LocalPlayer"),
                    oxorany("OverrideOrthographicSize"),
                    1
            )
    );
    if(_OverrideOrthographicSize) {
        _OverrideOrthographicSize(instance, value);
    }
}

void (*_FogUpdate)(void *instance);
void FogUpdate(void *instance) {
    if(instance && noFog) {
        NoFog(instance);
    }
    _FogUpdate(instance);
}

void (*_PlayerUpdate)(void *instance);
void PlayerUpdate(void *instance) {
    if(instance) {
        if(zoom > 1) {
            AdjustCamera(instance,(float)zoom);
        }
        if(speed >= 5) {
            ObscuredFloat newSpeed = FloatToObscuredFloat(speed);
            // edit static field.
            Il2Cpp::SetStaticFieldValue(
                    "Assembly-CSharp.dll",
                    "Handlers.GameHandlers.PlayerHandlers",
                    "LocalPlayer",
                    "movementSpeed",
                    &newSpeed
            );
        }
        if(alwayMove) {
            *(bool *) ((uintptr_t) instance + disableMovement) = false;
        }
    }
    _PlayerUpdate(instance);
}

void (*_set_Cooldown)(void *instance, ObscuredFloat value);
void set_Cooldown(void *instance, ObscuredFloat value) {
    if(instance && noCoolDown) {
        return _set_Cooldown(instance, FloatToObscuredFloat(0.06));
    }
    _set_Cooldown(instance, value);
}

void DoFastTask(void *instance) {
    void (*_DoFastTask)(void *) = (void (*)(void *))(
            GetMethodOffset(
                    oxorany("Assembly-CSharp.dll"),
                    oxorany("Handlers.GameHandlers.TaskHandlers"),
                    oxorany("TaskPanelHandler"),
                    oxorany("CompleteTask"),
                    0
            )
    );
    if(_DoFastTask) {
        _DoFastTask(instance);
    }
    void (*_ClosePanel)(void *) = (void (*)(void *))(
            GetMethodOffset(
                    oxorany("Assembly-CSharp.dll"),
                    oxorany("Handlers.GameHandlers.TaskHandlers"),
                    oxorany("TaskPanelHandler"),
                    oxorany("ClosePanel"),
                    0
            )
    );
    if(_ClosePanel) {
        _ClosePanel(instance);
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
    void (*_CallEmergency)(void *) = (void (*)(void *))(
            GetMethodOffset(
                    oxorany("Assembly-CSharp.dll"),
                    oxorany("Handlers.GameHandlers.PlayerHandlers"),
                    oxorany("PlayerController"),
                    oxorany("CallEmergency"),
                    0
            )
    );
    if(_CallEmergency) {
        _CallEmergency(instance);
    }
}

void DoNoClip(void *instance, bool check) {
    void (*_Behavior_set)(void *, bool) = (void (*)(void *, bool))(
            GetMethodOffset(
                    oxorany("UnityEngine.CoreModule.dll"),
                    oxorany("UnityEngine"),
                    oxorany("Behaviour"),
                    oxorany("set_enabled"),
                    1
            )
    );
    if(_Behavior_set) {
        _Behavior_set(instance, check);
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
            void *adrr = *(void**)((uintptr_t) instance + playerCollider);
            if(adrr) {
                DoNoClip(adrr, false);
            }
        }
    }
    _CallUpdate(instance);
}

void DoNoRoof(void *instance, bool check) {
    void (*_DeactivateRoofs)(void *, bool) = (void (*)(void *, bool))(
            GetMethodOffset(
                    oxorany("Assembly-CSharp.dll"),
                    oxorany("Handlers.GameHandlers"),
                    oxorany("RoofHandler"),
                    oxorany("DeactivateRoofs"),
                    1
            )
    );
    if(_DeactivateRoofs) {
        _DeactivateRoofs(instance, check);
    }
}

void (*_RoomUpdate)(void *instance, void *room);
void RoomUpdate(void *instance, void *room) {
    if(instance) {
        if(NoFog) {
            DoNoRoof(instance, false);
        }
    }
    _RoomUpdate(instance, room);
}