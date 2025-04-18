#ifndef ZNPCTYPEVILLAGER_H
#define ZNPCTYPEVILLAGER_H

#include "zNPCTypeCommon.h"
#include "zNPCSupport.h"
#include "zTaskBox.h"
#include "zPlatform.h"
#include "xShadow.h"

struct HiThere : ztaskbox::callback
{
    zNPCCommon* npc;

    virtual void on_talk_start();
    virtual void on_talk_stop();
};

struct zNPCVillager : zNPCCommon
{
    HiThere hithere;
    ztaskbox* converse; //0x2a8
    S32 current_talk_anim;

    zNPCVillager(S32 myType);

    void FindMyConverse();
    U8 ColPenByFlags() const;
    U8 ColChkByFlags() const;
    U8 ColPenFlags() const;
    U8 PhysicsFlags() const;
    void Init(xEntAsset*);
    void Reset();
    void ParseINI();
    void ParseNonRandTalk();
    void Process(xScene* xscn, float dt);
    void SelfSetup();
    void SpeakBegin();
    void SpeakEnd();
    void TossMyConverse();
    S32 PlayerIsStaring();
    void ChkCheatSize();
    F32 GenShadCacheRad();
};

struct zNPCFish : zNPCVillager
{
    NPCTarget tgt_robonear;
    F32 tmr_robonear;
    F32 tmr_checkagain;
    xEntDrive raw_drvdata;

    zNPCFish(S32 myType);
    void Init(xEntAsset*);
    void ParseINI();
    void FishSoundTables();
    void CheckDoChat();
    void Reset();
    void SelfSetup();
};

struct zNPCBubbleBuddy : zNPCFish
{
    zNPCBubbleBuddy(S32 myType);

    void Reset();
};

enum en_BBOY_PLATANIM
{
    BBOY_PLATANIM_MOVE,
    BBOY_PLATANIM_HIT,
    BBOY_PLATANIM_NOMORE,
    BBOY_PLATANIM_FORCE = 0x7FFFFFFF,
};

struct zNPCBalloonBoy : zNPCFish
{
    S32 specialBalloon;
    zPlatform* plat_balloons;
    xShadowCache* shadCache;
    static RwRaster* rast_shadBalloon;

    zNPCBalloonBoy(S32 myType);

    void Init(xEntAsset* asset);
    void SelfSetup();
    void Render();
    void PlatShadRend();
    void AddBallooning(xPsyche* psy);
    void PlatAnimSet(en_BBOY_PLATANIM anim);
    void PlatAnimSync();
};

struct zNPCSandyBikini : zNPCVillager
{
    F32 tmr_leakCycle; //0xac

    zNPCSandyBikini(S32 myType);
    void Reset();
    void Process(xScene* xscn, float dt);
    void VFXLeakyFaucet(float dt);
};

struct zNPCMerManChair : zNPCVillager
{
    S32 flg_mermanchair;

    zNPCMerManChair(S32 myType);
    void Init(xEntAsset*);
    U8 PhysicsFlags() const;
    U8 ColPenFlags() const;
    U8 ColChkFlags() const;
};

struct zNPCNewsFish : zNPCVillager
{
    struct say_data
    {
        S32 total;
        S32 prev_total;
    };

    say_data said[71];
    U8 was_reset;
    U32 soundHandle; //0x4ec
    U32 currSoundID; //0x4f0
    U32 nextSoundID; //0x4f4
    F32 jawTime; //0x4f8
    void* jawData;
    U32 newsfishFlags; //0x500
    xVec2 onScreenCoords;
    xVec2 offScreenCoords;
    xVec2 screenCoords;
    F32 screenSize;
    F32 screenRot;
    F32 appearSpeed;
    F32 disappearSpeed;
    F32 screenLerp;
    S32 IsTalking();
    void reset_said();

    zNPCNewsFish(S32 myType);

    // Vtable Information
    // 0x00000000; // 0x0
    // 0x00000000; // 0x4
    void Init(xEntAsset*); // 0x8 zNPCNewsFish
    void PostInit(); // 0xC xNPCBasic
    void Setup(); // 0x10 zNPCCommon
    void PostSetup(); // 0x14 zNPCNewsFish
    void Reset(); // 0x18 zNPCNewsFish
    void Process(xScene*, F32); // 0x1C zNPCNewsFish
    void BUpdate(xVec3*); // 0x20 zNPCCommon
    void NewTime(xScene*, F32); // 0x24 zNPCCommon
    void Move(xScene*, F32, xEntFrame*); // 0x28 zNPCCommon
    S32 SysEvent(xBase*, xBase*, U32, const F32*, xBase*, S32*); // 0x2C zNPCCommon
    void Render(); // 0x30 zNPCNewsFish
    void Save(xSerial*) const; // 0x34 xNPCBasic
    void Load(xSerial*); // 0x38 xNPCBasic
    void CollideReview(); // 0x3C zNPCVillager
    U8 ColChkFlags() const; // 0x40 zNPCVillager
    U8 ColPenFlags() const; // 0x44 zNPCVillager
    U8 ColChkByFlags() const; // 0x48 zNPCVillager
    U8 ColPenByFlags() const; // 0x4C zNPCVillager
    U8 PhysicsFlags() const; // 0x50 zNPCVillager
    void Destroy(); // 0x54 zNPCCommon
    S32 NPCMessage(NPCMsg*); // 0x58 zNPCVillager
    void RenderExtra(); // 0x5C zNPCCommon
    void RenderExtraPostParticles(); // 0x60 zNPCCommon
    void ParseINI(); // 0x64 zNPCVillager
    void ParseLinks(); // 0x68 zNPCCommon
    void ParseProps(); // 0x6C zNPCCommon
    void SelfSetup(); // 0x70 zNPCNewsFish
    void SelfDestroy(); // 0x74 zNPCCommon
    S32 IsHealthy(); // 0x78 zNPCCommon
    S32 IsAlive(); // 0x7C zNPCCommon
    void Damage(en_NPC_DAMAGE_TYPE, xBase*, const xVec3*); // 0x80 zNPCCommon
    S32 Respawn(const xVec3*, zMovePoint*, zMovePoint*); // 0x84 zNPCCommon
    void DuploOwner(zNPCCommon*); // 0x88 zNPCCommon
    void DuploNotice(en_SM_NOTICES, void*); // 0x8C zNPCCommon
    S32 CanRope(); // 0x90 zNPCCommon
    void LassoNotify(en_LASSO_EVENT); // 0x94 zNPCCommon
    S32 SetCarryState(en_NPC_CARRY_STATE); // 0x98 zNPCCommon
    void Stun(F32); // 0x9C zNPCCommon
    void SpeakBegin(); // 0xA0 zNPCVillager
    void SpeakEnd(); // 0xA4 zNPCVillager
    void SpeakStart(U32 sndid, U32 sndhandle, S32 anim); // 0xA8 zNPCNewsFish
    void SpeakStop(); // 0xAC zNPCNewsFish
    U32 AnimPick(S32, en_NPC_GOAL_SPOT, xGoal*); // 0xB0 zNPCVillager
    void GetParm(en_npcparm, void*); // 0xB4 zNPCCommon
    void GetParmDefault(en_npcparm, void*); // 0xB8 zNPCCommon
    F32 GenShadCacheRad(); // 0xBC zNPCVillager
    xEntDrive* PRIV_GetDriverData(); // 0xC0 zNPCCommon
    zNPCLassoInfo* PRIV_GetLassoData(); // 0xC4 zNPCCommon
    S32 LassoSetup(); // 0xC8 zNPCCommon
    void FolkHandleMail(NPCMsg*); // 0xCC zNPCVillager

    void TalkOnScreen(S32 talkOnScreen);
};

struct zNPCNewsFishTV : zNPCVillager
{
    zNPCNewsFishTV(S32 myType);
    U8 PhysicsFlags() const;
    U8 ColPenByFlags() const;
    U8 ColChkByFlags() const;
    U8 ColPenFlags() const;
    U8 ColChkFlags() const;
};

xAnimTable* ZNPC_AnimTable_Villager();
xAnimTable* ZNPC_AnimTable_Villager(xAnimTable* callerTable);
void zNPCBubbleBuddy_AlphaUpdate(F32 dt);
xAnimTable* ZNPC_AnimTable_BalloonBoy();
xAnimTable* ZNPC_AnimTable_BalloonBoy(xAnimTable* callerTable);
xAnimTable* ZNPC_AnimTable_SuperFriend();
xAnimTable* ZNPC_AnimTable_SuperFriend(xAnimTable* callerTable);
S32 FOLK_grul_goAlert(xGoal*, void*, en_trantype*, float, void*);
void FOLK_KillEffects();
void FOLK_InitEffects();
void zNPCVillager_ScenePostInit();
void zNPCVillager_ScenePrepare();
void zNPCVillager_SceneReset();
void ZNPC_Villager_Startup();
void ZNPC_Villager_Shutdown();
xFactoryInst* ZNPC_Create_Villager(S32 who, RyzMemGrow* growCtxt, void*);
void ZNPC_Destroy_Villager(xFactoryInst* inst);
S32 zParamGetFloatList(xModelAssetParam* parmdata, U32 pdatsize, const char* str32, S32 found,
                       F32* non_choices, F32 len_mvptspline);
void zNPCVillager_SceneFinish();
void zNPCVillager_SceneTimestep(xScene* xscn, F32 dt);

extern NPCSndTrax g_sndTrax_Villager;
extern NPCSndTrax g_sndTrax_VillagerMale;
extern NPCSndTrax g_sndTrax_VillagerFemale;
extern NPCSndTrax g_sndTrax_VillagerElder;
extern NPCSndTrax g_sndTrax_VillagerEldess;
extern NPCSndTrax g_sndTrax_VillagerBoy;
extern NPCSndTrax g_sndTrax_VillagerGirl;
extern NPCSndTrax g_sndTrax_VillagerCoStar;
extern NPCSndTrax g_sndTrax_Squidward;

#endif
