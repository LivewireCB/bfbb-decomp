#include "zNPCTypeKingJelly.h"

#include "zNPCGoalCommon.h"
#include <types.h>
#include "string.h"

#define f1868 1.0f
#define f1869 0.0f
#define f2105 0.2f
#define f2106 0.1f

#define ANIM_Unknown 0 // 0x0
#define ANIM_Idle01 1 // 0x04
#define ANIM_Idle02 2 // 0x08      
#define ANIM_Idle03 3 // 0xC
#define ANIM_Fidget01 4 //
#define ANIM_Fidget02 5
#define ANIM_Fidget03 6
#define ANIM_Taunt01 7 // 0x1c
#define ANIM_Attack01 8 //0x20
#define ANIM_Damage01 9 //0x24
#define ANIM_Damage02 10 //0x28
#define ANIM_Death01 11 //0x2c
#define ANIM_AttackWindup01 12 //0x30
#define ANIM_AttackLoop01 13 //0x34
#define ANIM_AttackEnd01 14 //0x38
#define ANIM_SpawnKids01 15 //0x3C
#define ANIM_Attack02Windup01 16
#define ANIM_Attack02Loop01 17
#define ANIM_Attack02End01 18
#define ANIM_LassoGrab01 19

namespace
{
    S32 boss_cam()
    {
        return 0; //todo
    }

    S32 play_sound(int, const xVec3*)
    {
        return 0; //todo
    }

    S32 kill_sound(int)
    {
        return 0; //to do
    }

    void kill_sounds()
    {
        for (S32 i = 0; i < 11; i++)
        {
            kill_sound(i);
        }
    }

    void reset_model_color(xModelInstance* submodel) //25% matching. will need rewritten
    {
        while (submodel != NULL)
        {
            submodel = submodel->Next;
        }
    }

    void tweak()
    {
    }

} // namespace

void lightning_ring::create()
{
    // store 1 into 0x0
    active = 1;
    arcs_size = 0;

    //store 0 into 0x7c
}

void lightning_ring::destroy()
{
    for (S32 i = 0; i < arcs_size; i++)
    {
        zLightningKill(arcs[i]);
    }
    arcs_size = 0;
    active = 0;
}

xAnimTable* ZNPC_AnimTable_KingJelly()
{
    // clang-format off
    S32 ourAnims[11] = {
        ANIM_Idle01,
        ANIM_Idle02,
        ANIM_Idle03,
        ANIM_Taunt01,
        ANIM_Attack01,
        ANIM_AttackWindup01,        
        ANIM_AttackLoop01,
        ANIM_AttackEnd01,
        ANIM_Damage01,
        ANIM_SpawnKids01,
        ANIM_Unknown,
        
    };
    // clang-format on
    xAnimTable* table = xAnimTableNew("zNPCKingJelly", NULL, 0);

    xAnimTableNewState(table, g_strz_subbanim[ANIM_Idle01], 0x10, 0, f1868, NULL, NULL, f1869, NULL,
                       NULL, xAnimDefaultBeforeEnter, NULL, NULL);
    xAnimTableNewState(table, g_strz_subbanim[ANIM_Idle02], 0x20, 0, f1868, NULL, NULL, f1869, NULL,
                       NULL, xAnimDefaultBeforeEnter, NULL, NULL);
    xAnimTableNewState(table, g_strz_subbanim[ANIM_Idle03], 0x20, 0, f1868, NULL, NULL, f1869, NULL,
                       NULL, xAnimDefaultBeforeEnter, NULL, NULL);
    xAnimTableNewState(table, g_strz_subbanim[ANIM_Taunt01], 0x20, 0, f1868, NULL, NULL, f1869,
                       NULL, NULL, xAnimDefaultBeforeEnter, NULL, NULL);
    xAnimTableNewState(table, g_strz_subbanim[ANIM_Attack01], 0x10, 0, f1868, NULL, NULL, f1869,
                       NULL, NULL, xAnimDefaultBeforeEnter, NULL, NULL);
    xAnimTableNewState(table, g_strz_subbanim[ANIM_AttackWindup01], 0x20, 0, f1868, NULL, NULL,
                       f1869, NULL, NULL, xAnimDefaultBeforeEnter, NULL, NULL);
    xAnimTableNewState(table, g_strz_subbanim[ANIM_AttackLoop01], 0x10, 0, f1868, NULL, NULL, f1869,
                       NULL, NULL, xAnimDefaultBeforeEnter, NULL, NULL);
    xAnimTableNewState(table, g_strz_subbanim[ANIM_AttackEnd01], 0x20, 0, f1868, NULL, NULL, f1869,
                       NULL, NULL, xAnimDefaultBeforeEnter, NULL, NULL);
    xAnimTableNewState(table, g_strz_subbanim[ANIM_Damage01], 0x20, 0, f1868, NULL, NULL, f1869,
                       NULL, NULL, xAnimDefaultBeforeEnter, NULL, NULL);
    xAnimTableNewState(table, g_strz_subbanim[ANIM_SpawnKids01], 0x10, 0, f1868, NULL, NULL, f1869,
                       NULL, NULL, xAnimDefaultBeforeEnter, NULL, NULL);

    NPCC_BuildStandardAnimTran(table, g_strz_subbanim, ourAnims, 1, f2105);

    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_AttackWindup01],
                            g_strz_subbanim[ANIM_Attack01], 0, 0, 0x10, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_AttackLoop01],
                            g_strz_subbanim[ANIM_Attack01], 0, 0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_Attack01],
                            g_strz_subbanim[ANIM_AttackLoop01], 0, 0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_AttackLoop01],
                            g_strz_subbanim[ANIM_AttackEnd01], 0, 0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_Idle02], g_strz_subbanim[ANIM_Damage01], 0,
                            0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_Idle03], g_strz_subbanim[ANIM_Damage01], 0,
                            0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_Taunt01], g_strz_subbanim[ANIM_Damage01], 0,
                            0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_AttackWindup01],
                            g_strz_subbanim[ANIM_Damage01], 0, 0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_AttackLoop01],
                            g_strz_subbanim[ANIM_Damage01], 0, 0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_Attack01], g_strz_subbanim[ANIM_Damage01],
                            0, 0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_AttackEnd01],
                            g_strz_subbanim[ANIM_Damage01], 0, 0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_SpawnKids01],
                            g_strz_subbanim[ANIM_Damage01], 0, 0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_Idle02], g_strz_subbanim[ANIM_Taunt01], 0,
                            0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_Idle03], g_strz_subbanim[ANIM_Taunt01], 0,
                            0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_AttackWindup01],
                            g_strz_subbanim[ANIM_Taunt01], 0, 0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_AttackLoop01],
                            g_strz_subbanim[ANIM_Taunt01], 0, 0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_Attack01], g_strz_subbanim[ANIM_Taunt01], 0,
                            0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_AttackEnd01], g_strz_subbanim[ANIM_Taunt01],
                            0, 0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_SpawnKids01], g_strz_subbanim[ANIM_Taunt01],
                            0, 0, 0, 0, 0, 0, 0, 0, f2106, 0);
    xAnimTableNewTransition(table, g_strz_subbanim[ANIM_Damage01],
                            g_strz_subbanim[ANIM_SpawnKids01], 0, 0, 0, 0, 0, 0, 0, 0, f2106, 0);

    return table;
}

void zNPCKingJelly::Init(xEntAsset* asset)
{
    zNPCCommon::Init(asset);
    memset(&flag.fighting, 0, 5);
    this->bossCam.init();
}

xVec3* zNPCKingJelly::get_bottom()
{
    return (xVec3*)&this->model->Mat->pos;
}

void zNPCKingJelly::Setup()
{
    this->children_size = 0; //0x88C
    load_model();
    load_curtain_model();
    zNPCSubBoss::Setup();
}

void zNPCKingJelly::Destroy()
{
    decompose();
    post_decompose();
    zNPCCommon::Destroy();
}

void zNPCKingJelly::BUpdate(xVec3* pos)
{
    // Original stack variables:
    //xVec3& subloc;
    //xVec3 loc;

    // Something like this...
    // Some vec is being added to another on the stack (probably)
    // (xVec3&)this->model->Mat->pos = (xVec3&)this->model->Mat->pos + *pos;

    zNPCCommon::BUpdate(pos);
}

void zNPCKingJelly::RenderExtra()
{
    zNPCKingJelly::render_debug();
}

void zNPCKingJelly::ParseINI()
{
    zNPCCommon::ParseINI();
    cfg_npc->snd_traxShare = &g_sndTrax_KingJelly;
    NPCS_SndTablePrepare((NPCSndTrax*)&g_sndTrax_KingJelly);
    cfg_npc->snd_trax = &g_sndTrax_KingJelly;
    NPCS_SndTablePrepare((NPCSndTrax*)&g_sndTrax_KingJelly);
}

void zNPCKingJelly::SelfSetup()
{
    xBehaveMgr* bmgr;
    xPsyche* psy;

    bmgr = xBehaveMgr_GetSelf();
    psy_instinct = bmgr->Subscribe(this, 0);
    psy = psy_instinct;
    psy->BrainBegin();
    psy->AddGoal(NPC_GOAL_KJIDLE, NULL);
    psy->AddGoal(NPC_GOAL_KJBORED, NULL);
    psy->AddGoal(NPC_GOAL_KJSPAWNKIDS, NULL);
    psy->AddGoal(NPC_GOAL_KJTAUNT, NULL);
    psy->AddGoal(NPC_GOAL_KJSHOCKGROUND, NULL);
    psy->AddGoal(NPC_GOAL_KJDAMAGE, NULL);
    psy->AddGoal(NPC_GOAL_KJDEATH, NULL);
    psy->AddGoal(NPC_GOAL_LIMBO, NULL);
    psy->BrainEnd();
    psy->SetSafety(NPC_GOAL_KJIDLE);
}

S32 zNPCKingJelly::max_strikes() const
{
    return round + 1;
}

void zNPCKingJelly::init_child(zNPCKingJelly::child_data& child, zNPCCommon& npc, int wave)
{
    child.npc = &npc;
    child.wave = wave;
    child.active = 1;
    child.callback.eventFunc = npc.eventFunc;
    child.callback.update = npc.update;
    child.callback.bupdate = npc.bupdate;
    child.callback.move = npc.move;
    child.callback.render = npc.render;
    child.callback.transl = npc.transl;
}

void zNPCKingJelly::disable_child(zNPCKingJelly::child_data& child)
{
    if (child.active)
    {
        ((zNPCJelly*)child.npc)->JellyKill();
        child.active = false;
    }
}

void zNPCKingJelly::enable_child(zNPCKingJelly::child_data& child)
{
    if (child.active == false)
    {
        child.active = true;
    }
}

S32 zNPCKingJelly::max_strikes()
{
    return round + 1;
}

void zNPCKingJelly::on_change_ambient_ring(const tweak_info&)
{
}

void zNPCKingJelly::on_change_fade_obstructions(const tweak_info&)
{
}

void zNPCKingJelly::render_debug()
{
}

void zNPCKingJelly::decompose()
{
}

void zNPCKingJelly::post_decompose()
{
}

void zNPCKingJelly::vanish()
{
    old.moreFlags = moreFlags;
    pflags = 0;
    moreFlags = 0;
    flags2.flg_colCheck = 0;
    flags2.flg_penCheck = 0;
    chkby = 0;
    penby = 0;
    xEntHide(this);
}

void zNPCKingJelly::reappear()
{
    moreFlags = old.moreFlags;
    this->RestoreColFlags();
    xEntShow(this);
}

void zNPCKingJelly::create_tentacle_lightning()
{
}

void zNPCKingJelly::destroy_tentacle_lightning()
{
    for (S32 i = 0; i < 7; i++)
    {
        if (tentacle_lightning[i])
        {
            zLightningKill(tentacle_lightning[i]);
            tentacle_lightning[i] = NULL;
        }
    }
}

void zNPCKingJelly::refresh_tentacle_points()
{
    S32 tempvar = 0;
    do
    {
        refresh_tentacle_points(tempvar);
        tempvar = tempvar + 1;
    } while (tempvar < 7);
}

void zNPCKingJelly::destroy_ambient_rings()
{
    for (S32 i = 0; i < 3; i++)
    {
        ambient_rings[i].destroy();
    }
}

void zNPCKingJelly::generate_spawn_particles()
{
}

void zNPCKingJelly::load_model()
{
}

void zNPCKingJelly::load_curtain_model()
{
}

void zNPCKingJelly::show_shower_model()
{
}

void zNPCKingJelly::reset_curtain()
{
}

S32 zNPCGoalKJIdle::Exit(float dt, void* updCtxt)
{
    zNPCKingJelly& kj = *(zNPCKingJelly*)this->psyche->clt_owner;
    kill_sound(6);
    kj.flag.stop_moving = 1;
    return xGoal::Exit(dt, updCtxt);
}

S32 zNPCGoalKJBored::Enter(float dt, void* updCtxt)
{
    zNPCKingJelly& kj = *(zNPCKingJelly*)this->psyche->clt_owner;
    //play_sound(int, const xVec3*);
    play_sound(3, kj.model->anim_coll.verts); // kj.model is correct? dont know the xVec3*
    play_sound(3, kj.model->anim_coll.verts); // same as above
    return zNPCGoalCommon::Enter(dt, updCtxt);
}

S32 zNPCGoalKJBored::Exit(float dt, void* updCtxt)
{
    return xGoal::Exit(dt, updCtxt);
}

S32 zNPCGoalKJSpawnKids::Enter(float dt, void* updCtxt)
{
    zNPCKingJelly& kj = *(zNPCKingJelly*)this->psyche->clt_owner;
    count_children(kj.round);
    return zNPCGoalCommon::Enter(dt, updCtxt);
}

S32 zNPCGoalKJSpawnKids::Exit(float dt, void* updCtxt)
{
    zNPCKingJelly& kj = *(zNPCKingJelly*)this->psyche->clt_owner;
    if (spawn_count < child_count) //0x58 child_count
    {
        kj.generate_spawn_particles();
        kj.spawn_children(kj.round, child_count - spawn_count);
    }
    return zNPCGoalCommon::Exit(dt, updCtxt);
}

S32 zNPCGoalKJTaunt::Enter(float dt, void* updCtxt)
{
    zNPCKingJelly& kj = *(zNPCKingJelly*)this->psyche->clt_owner;
    //play_sound(int, const xVec3*);
    play_sound(9, kj.model->anim_coll.verts); // kj.model is correct? dont know the xVec3*
    play_sound(9, kj.model->anim_coll.verts); // same as above
    return zNPCGoalCommon::Enter(dt, updCtxt);
}

S32 zNPCGoalKJTaunt::Exit(float dt, void* updCtxt)
{
    return xGoal::Exit(dt, updCtxt);
}

// void zNPCKingJelly::start_blink()
// {
//     blink.active = 1;
//     blink.delay = 0;
//     blink.count = 0;
//     // 0x24 model
//     // 0x44 render
// }

S32 zNPCGoalKJDamage::Process(en_trantype* trantype, F32 dt, void* updCtxt, xScene* xscn)
{
    // TODO
    return 0;
}

S32 zNPCGoalKJShockGround::Exit(F32 dt, void* updCtxt)
{
    zNPCKingJelly& kj = *(zNPCKingJelly*)this->psyche->clt_owner;
    if (kj.flag.charging != 0)
    {
        kj.end_charge();
    }
    kj.create_ambient_rings();
    kj.disable_tentacle_damage = 0;
    return xGoal::Exit(dt, updCtxt);
}

S32 zNPCGoalKJDamage::Enter(F32 dt, void* updCtxt)
{
    zNPCKingJelly& kj = *(zNPCKingJelly*)this->psyche->clt_owner;
    //play_sound(int, const xVec3*);
    play_sound(4, kj.model->anim_coll.verts); // kj.model is correct? dont know the xVec3*
    play_sound(4, kj.model->anim_coll.verts); // same as above
    kj.disable_tentacle_damage = 1;
    return zNPCGoalCommon::Enter(dt, updCtxt);
}

S32 zNPCGoalKJDamage::Exit(F32 dt, void* updCtxt)
{
    // Needs to be a reference, casting as a pointer doesn't work.
    // Would never have gotten this if not for DWARF data.
    zNPCKingJelly& kj = *(zNPCKingJelly*)this->psyche->clt_owner;

    kj.update_round();
    kj.disable_tentacle_damage = false;

    return xGoal::Exit(dt, updCtxt);
}

void zNPCKingJelly::update_round()
{
}

S32 zNPCGoalKJDeath::Enter(float dt, void* updCtxt)
{
    zNPCKingJelly& kj = *(zNPCKingJelly*)this->psyche->clt_owner;
    kj.decompose();
    kj.post_decompose();
    return zNPCGoalCommon::Enter(dt, updCtxt);
}

S32 zNPCGoalKJDeath::Exit(float dt, void* updCtxt)
{
    return xGoal::Exit(dt, updCtxt);
}

S32 zNPCGoalKJDeath::Process(en_trantype* trantype, float dt, void* updCtxt, xScene* xscn)
{
    return xGoal::Process(trantype, dt, updCtxt, xscn);
}
