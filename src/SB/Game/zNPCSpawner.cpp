#include "zNPCSpawner.h"

#include <types.h>

#include "zEvent.h"

extern SMDepot g_smdepot;
extern F32 _805_Spawner; // 5.0f

void zNPCSpawner_Startup()
{
}

void zNPCSpawner_Shutdown()
{
}

void zNPCSpawner_ScenePrepare()
{
    SMDepot* depot = &g_smdepot;
    XOrdInit(&depot->spawners, sizeof(g_smdepot), 0);
    for (S32 i = 0; i < 0x10; i++)
    {
        // FIXME: operator new call
        // zNPCSpawner* sm = RyzMemData::operator new((size_t)sizeof(zNPCSpawner), 'SPWN', NULL);
        // XOrdAppend(&depot->spawners, sm);
    }
}

void zNPCSpawner_SceneFinish()
{
    SMDepot* depot = &g_smdepot;
    for (S32 i = 0; i < depot->spawners.cnt; i++)
    {
        RyzMemData::operator delete(depot->spawners.list[i]);
    }
    XOrdDone(&depot->spawners, 0);
}

// Something weird with the conditions here.
zNPCSpawner* zNPCSpawner_GetInstance()
{
    SMDepot* depot = &g_smdepot;
    zNPCSpawner* sm = (zNPCSpawner*)depot->spawners.list;
    if (depot->spawners.cnt > 0)
    {
        for (S32 i = depot->spawners.cnt; i > 0; i--)
        {
            zNPCSpawner* sm_tmp = sm;
            if (!(sm_tmp->flg_spawner & 1))
            {
                sm_tmp->flg_spawner |= 1;
                return sm_tmp;
            }
            sm++;
        }
        return NULL;
    }
    else
    {
        return NULL;
    }
}

void zNPCSpawner::Subscribe(zNPCCommon* owner)
{
    this->npc_owner = owner;
    this->tym_delay = _805_Spawner;
    this->max_spawn = -1;
    this->wavestat = SM_STAT_BEGIN;
    XOrdInit(&this->pendlist, 0x10, 0);
    XOrdInit(&this->actvlist, 0x10, 0);
}

void zNPCSpawner::SetWaveMode(en_SM_WAVE_MODE mode, F32 delay, S32 lifemax)
{
    this->wavemode = mode;
    this->tym_delay = delay;
    this->max_spawn = lifemax;
}

S32 zNPCSpawner::AddSpawnPoint(zMovePoint* sp)
{
    S32 ack = 0;
    for (S32 i = 0; i < 0x10; i++)
    {
        SMSPStatus* sp_stat = &this->sppool[i];
        if (sp_stat->sp == NULL)
        {
            sp_stat->sp = sp;
            sp_stat->npc_prefer = NULL;
            ack = 1;
            break;
        }
    }
    return ack;
}

S32 zNPCSpawner::AddSpawnNPC(zNPCCommon* npc)
{
    S32 ack = 0;
    for (S32 i = 0; i < 0x10; i++)
    {
        SMNPCStatus* npc_stat = &this->npcpool[i];
        if (npc_stat->npc == NULL)
        {
            npc_stat->npc = npc;
            ack = 1;
            npc_stat->status = SM_NPC_READY;
            npc_stat->sp_prefer = NULL;
            break;
        }
    }
    // Need to figure out what it is calling here.
    npc->DBG_Name();
    return ack;
}

void zNPCSpawner::Reset()
{
    this->cnt_spawn = 0;
    this->wavestat = SM_STAT_BEGIN;
    this->cnt_cleanup = 0;
    this->flg_spawner &= 1;
    this->flg_spawner |= 8;
    this->ClearPending();
    this->ClearActive();
    this->MapPreferred();
}

void zNPCSpawner::MapPreferred()
{
    for (S32 i = 0; i < 16; i++)
    {
        SMNPCStatus* npc_stat = &this->npcpool[i];
        if (npc_stat->npc != NULL)
        {
            zMovePoint* sp = (zMovePoint*)npc_stat->npc->FirstAssigned();
            if (sp != NULL)
            {
                SMSPStatus* sp_stat = StatForSP(sp, 0);
                if (sp_stat != NULL)
                {
                    npc_stat->sp_prefer = sp;
                    sp_stat->npc_prefer = npc_stat->npc;
                }
            }
        }
    }
}

void zNPCSpawner::Timestep(F32 dt)
{
    if (flg_spawner & 0x8)
    {
        ChildHeartbeat(dt);
    }

    if (flg_spawner & 0x10)
    {
        ChildCleanup(dt);
    }

    if (!(flg_spawner & 0x4))
    {
        tmr_wave = -1.0f > tmr_wave - dt ? -1.0f : tmr_wave - dt;

        switch (wavemode)
        {
        case SM_WAVE_DISCREET:
            UpdateDiscreet(dt);
            break;
        case SM_WAVE_CONTINUOUS:
            UpdateContinuous(dt);
            break;
        }
    }
}

void zNPCSpawner::UpdateDiscreet(F32 dt)
{
    SMSPStatus* spstat = NULL;

    switch (wavestat)
    {
    case SM_STAT_BEGIN:
        FillPending();

        if (pendlist.cnt > 0)
        {
            wavestat = SM_STAT_INPROG;
            zEntEvent(npc_owner, eEventDuploWaveBegin);

            flg_spawner &= ~0x20;
        }

        tmr_wave = tym_delay;

        break;
    case SM_STAT_INPROG:
        if (pendlist.cnt < 1)
        {
            wavestat = SM_STAT_MARKTIME;
        }
        else if (max_spawn > 0 && !(cnt_spawn < max_spawn))
        {
            wavestat = SM_STAT_ABORT;
        }
        else if (tmr_wave < 0.0f)
        {
            SMNPCStatus* npcstat = NextPendingNPC(0);
            if (npcstat != NULL)
            {
                spstat = SelectSP(npcstat);
            }

            if (!(spstat == NULL))
            {
                SpawnBeastie(npcstat, spstat);
            }

            tmr_wave = tym_delay * (0.25f * (xurand() - 0.5f)) + tym_delay;
        }

        break;
    case SM_STAT_MARKTIME:
        if (actvlist.cnt < 1)
        {
            wavestat = SM_STAT_DONE;
        }
        else if (max_spawn > 0 && !(cnt_spawn < max_spawn))
        {
            wavestat = SM_STAT_ABORT;
        }

        break;
    case SM_STAT_ABORT:
        if (actvlist.cnt < 1)
        {
            wavestat = SM_STAT_DONE;
        }

        ClearPending();

        break;
    case SM_STAT_DONE:
        if (!(flg_spawner & 0x20))
        {
            zEntEvent(npc_owner, eEventDuploWaveComplete);
        }

        if (max_spawn > 0 && !(cnt_spawn < max_spawn))
        {
            if (!(flg_spawner & 0x20))
            {
                flg_spawner |= 0x20;

                zEntEvent(npc_owner, eEventDuploExpiredMaxNPC);
                zEntEvent(npc_owner, eEventDuploSuperDuperDone);

                if (flg_spawner & 0x2)
                {
                    zEntEvent(npc_owner, eEventDuploDuperIsDoner);
                }
            }
        }
        else
        {
            if (flg_spawner & 0x2)
            {
                if (!(flg_spawner & 0x20))
                {
                    flg_spawner |= 0x20;

                    zEntEvent(npc_owner, eEventDuploSuperDuperDone);
                    zEntEvent(npc_owner, eEventDuploDuperIsDoner);
                }
            }
            else
            {
                wavestat = SM_STAT_BEGIN;
            }
        }

        break;
    default:
        break;
    }
}

void zNPCSpawner::UpdateContinuous(F32 dt)
{
    SMSPStatus* spstat = NULL;

    switch (wavestat)
    {
    case SM_STAT_BEGIN:
        FillPending();

        if (pendlist.cnt > 0)
        {
            wavestat = SM_STAT_INPROG;
            zEntEvent(npc_owner, eEventDuploWaveBegin);

            flg_spawner &= ~0x20;
        }

        tmr_wave = tym_delay;

        break;
    case SM_STAT_INPROG:
        if (pendlist.cnt < 1)
        {
            wavestat = SM_STAT_MARKTIME;
        }
        else if (max_spawn > 0 && !(cnt_spawn < max_spawn))
        {
            wavestat = SM_STAT_ABORT;
        }
        else if (tmr_wave < 0.0f)
        {
            SMNPCStatus* npcstat = NextPendingNPC(0);
            if (npcstat != NULL)
            {
                spstat = SelectSP(npcstat);
            }

            if (!(spstat == NULL))
            {
                SpawnBeastie(npcstat, spstat);
            }

            tmr_wave = tym_delay * (0.25f * (xurand() - 0.5f)) + tym_delay;
        }

        break;
    // MarkTime case is the significantly different one between this function and UpdateDiscreet
    case SM_STAT_MARKTIME:
        if (flg_spawner & 0x2)
        {
            wavestat = SM_STAT_ABORT;
        }
        else if (max_spawn > 0 && !(cnt_spawn < max_spawn))
        {
            wavestat = SM_STAT_ABORT;
        }
        else
        {
            if (tmr_wave < 0.0f)
            {
                tmr_wave = tym_delay * (0.25f * (xurand() - 0.5f)) + tym_delay;

                ReFillPending();

                if (pendlist.cnt > 0)
                {
                    wavestat = SM_STAT_INPROG;
                }
            }
            else if (pendlist.cnt > 0)
            {
                wavestat = SM_STAT_INPROG;
            }
        }

        break;
    case SM_STAT_ABORT:
        if (actvlist.cnt < 1)
        {
            wavestat = SM_STAT_DONE;
        }

        ClearPending();

        break;
    case SM_STAT_DONE:
        if (!(flg_spawner & 0x20))
        {
            zEntEvent(npc_owner, eEventDuploWaveComplete);
        }

        if (max_spawn > 0 && !(cnt_spawn < max_spawn))
        {
            if (!(flg_spawner & 0x20))
            {
                flg_spawner |= 0x20;

                zEntEvent(npc_owner, eEventDuploExpiredMaxNPC);
                zEntEvent(npc_owner, eEventDuploSuperDuperDone);

                if (flg_spawner & 0x2)
                {
                    zEntEvent(npc_owner, eEventDuploDuperIsDoner);
                }
            }
        }
        else
        {
            if (flg_spawner & 0x2)
            {
                if (!(flg_spawner & 0x20))
                {
                    flg_spawner |= 0x20;

                    zEntEvent(npc_owner, eEventDuploSuperDuperDone);
                    zEntEvent(npc_owner, eEventDuploDuperIsDoner);
                }
            }
        }

        break;
    default:
        break;
    }
}

void zNPCSpawner::SetNPCStatus(zNPCCommon* npc, en_SM_NPC_STATUS status)
{
    SMNPCStatus* stat = this->StatForNPC(npc);
    if (stat != NULL)
    {
        stat->status = status;
    }
}

SMNPCStatus* zNPCSpawner::ToastedBeastie(zNPCCommon* npc)
{
    SMNPCStatus* ret = this->StatForNPC(npc);
    XOrdRemove(&this->actvlist, ret, -1);
    zEntEvent((xBase*)this->npc_owner, eEventDuploNPCKilled);
    return ret;
}

void zNPCSpawner::ChildHeartbeat(F32 dt)
{
}

U8 zMovePoint::IsOn()
{
    return this->on;
}

st_XORDEREDARRAY* zNPCSpawner::FillPending()
{
    ClearPending();
    ReFillPending();
    return (st_XORDEREDARRAY*)this->pendlist.cnt;
}

st_XORDEREDARRAY* zNPCSpawner::ReFillPending()
{
    s32 var_r28;
    zNPCCommon* temp_r29;
    zNPCSpawner* var_r30;

    var_r28 = 0;
    var_r30 = this;
    do
    {
        temp_r29 = var_r30->npc_owner;
        if (((zNPCCommon*)var_r30->npc_owner != NULL) && ((s32)temp_r29->flg_vuln == 1))
        {
            XOrdAppend(&this->pendlist, (void*)temp_r29);
            temp_r29->flg_vuln = 2;
        }
        var_r28 += 1;
        var_r30 += 0xC;
    } while (var_r28 < 0x10);
    return &this->actvlist;
}

// void zNPCSpawner::ClearActive()
// {
//     s32 var_r6;
//     s32 var_r7;
//     void *temp_r5;

//     var_r7 = 0;
//     var_r6 = 0;
// loop_4:
//     if (var_r7 < (s32) this->cnt_cleanup) {
//         temp_r5 = *(this->pendlist->list + var_r6);
//         if (temp_r5 != NULL) {
//             (u32) temp_r5[1] = 1;
//         }
//         var_r6 += 4;
//         var_r7 += 1;
//         goto loop_4;
//     }
//     XOrdReset__FP16st_XORDEREDARRAY(&this->unk1B0);
// }

SMNPCStatus* zNPCSpawner::NextPendingNPC(S32 arg0)
{
    S32 temp_r4;
    const F32* temp_ptr = NULL;

    temp_r4 = this->pendlist.cnt;
    if (temp_r4 < 1)
    {
        return NULL;
    }
    return xUtil_select<SMNPCStatus>((SMNPCStatus**)this->pendlist.list, temp_r4, temp_ptr);
}

/* zNPCSpawner::StatForNPC (zNPCCommon *) */
SMNPCStatus* zNPCSpawner::StatForNPC(zNPCCommon* npc)
{
    s32 var_ctr;
    SMNPCStatus* var_r6;
    zNPCCommon* temp_r0;
    zNPCCommon* temp_r0_2;
    zNPCCommon* temp_r0_3;
    zNPCCommon* temp_r0_4;
    zNPCCommon* temp_r0_5;
    zNPCCommon* temp_r0_6;
    zNPCCommon* temp_r0_7;
    zNPCCommon* temp_r0_8;

    var_r6 = NULL;
    var_ctr = 2;

    for (var_ctr = 0; var_ctr < 16; var_ctr++)
    {
        temp_r0 = this->npcpool[var_ctr].npc;
        if ((temp_r0 != NULL) && (temp_r0 == npc))
        {
            var_r6 = &this->npcpool[var_ctr];
        }
    }

    return var_r6;
}
