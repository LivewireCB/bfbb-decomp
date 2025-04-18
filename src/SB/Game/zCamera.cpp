#include "zCamera.h"

#include <types.h>

#include "zCameraTweak.h"
#include "zEntPlayer.h"
#include "zGlobals.h"
#include "zMusic.h"
#include "iMath.h"
#include "xMathInlines.h"
#include "xVec3Inlines.h"
#include "xScrFx.h"
#include "xstransvc.h"

F32 zcam_overrot_tmr;
S32 zcam_near;
S32 zcam_mode;
S32 zcam_bbounce;
S32 zcam_lbbounce;
S32 zcam_convers;
S32 zcam_lconvers;
S32 zcam_longbounce;
S32 zcam_highbounce;
S32 zcam_cutscene;
S32 zcam_reward;
xVec3* zcam_playervel;
S32 zcam_fly;
S32 zcam_flypaused;
void* zcam_flydata;
U32 zcam_flysize;
F32 zcam_flytime;
U32 zcam_flyasset_current;
xCamAsset* zcam_dest;
F32 zcam_tmr;
F32 zcam_ttm;
F32 zcam_fovcurr;
F32 zcam_fovdest;

xCamera zcam_backupcam;
xCamera zcam_backupconvers;
xQuat zcam_quat;

F32 zcam_pad_pyaw_scale = 0.18124573f;
F32 zcam_pad_pitch_scale = 0.01923077f;
F32 zcam_near_d = 3.0f;
F32 zcam_near_h = 1.8f;
F32 zcam_near_pitch = 0.17453294f;
F32 zcam_far_d = 5.0f;
F32 zcam_far_h = 3.0f;
F32 zcam_far_pitch = 0.2617994f;
F32 zcam_wall_d = 7.5f;
F32 zcam_wall_h = 2.0f;
F32 zcam_wall_pitch = 0.31415927f;
F32 zcam_above_d = 0.2f;
F32 zcam_above_h = 2.2f;
F32 zcam_above_pitch = 1.2217306f;
F32 zcam_below_d = 0.6f;
F32 zcam_below_h = 0.2f;
F32 zcam_below_pitch = -1.2217306f;
F32 zcam_highbounce_d = 0.2f;
F32 zcam_highbounce_h = 5.0f;
F32 zcam_highbounce_pitch = 1.553343f;
F32 zcam_overrot_min = 0.43633232f;
F32 zcam_overrot_mid = 1.5707964f;
F32 zcam_overrot_max = 2.9670596f;
F32 zcam_overrot_rate = 0.1f;
F32 zcam_overrot_tstart = 1.5f;
F32 zcam_overrot_tend = 2.5f;
F32 zcam_overrot_velmin = 3.0f;
F32 zcam_overrot_velmax = 5.0f;
F32 zcam_overrot_tmanual = 1.5f;
F32 zcam_mintgtheight = -9.9999997E37f;

namespace
{
    U32 stop_track;
    U8 lassocam_enabled;
    F32 lassocam_factor;
    WallJumpViewState wall_jump_enabled;
    F32 dMultiplier;
    F32 dOffset;
    F32 hMultiplier;
    F32 hOffset;

    bool input_enabled = true;
    xVec3 wall_jump_view;
} // namespace

static F32 rewardMove = 1.0f;
static F32 rewardMoveSpeed = 1.1f;
static F32 rewardZoomSpeed = 6.0f;
static F32 rewardZoomAmount = 100.0f;
static F32 rewardTiltTime = 1.5f;
static F32 rewardTiltAmount = -0.22f;

extern zGlobals globals;
extern const xVec3 g_O3;
extern F32 gSkipTimeFlythrough;

extern F32 zCamera_f_75_0; // 75.0
extern F32 zCamera_f_0_0; // 0.0
extern F32 zCamera_f_n1_0_e38; // -1e38 ~ neg infinity
extern F32 zCamera_f_1_0; // 1.0
extern F32 zCamera_f_0_5; // 0.5
extern F32 zCamera_f_2_0; // 2.0
extern F32 zCamera_f_1_5; // 1.5
extern F32 zCamera_f_30_0; // 30.0
extern F32 zCamera_f_114_592; // 114.592
extern F32 zCamera_f_0_033; // 0.0333333
extern F32 zCamera_f_0_1; // 0.1
extern F32 zCamera_f_3_5; // 3.5
extern F32 zCamera_f_2_4; // 2.4
extern F32 zCamera_f_0_523; // 0.5235988
extern F32 zCamera_f_0_698; // 0.6981317
extern F32 zCamera_f_3_141; // 3.141593 ~ pi
extern F32 zCamera_f_20_0; // 20.0
extern F32 zCamera_f_180_0; // 180.0

F32 GetCurrentPitch();
F32 GetCurrentH();
F32 GetCurrentD();

void zCameraReset(xCamera* cam)
{
    // classic multiple float assign non match
    // float assigns should f0 everytime instead of f1, f2, ...
    zcam_mode = 0;
    zcam_bbounce = 0;
    zcam_lbbounce = 0;
    zcam_lconvers = 0;
    zcam_longbounce = 0;
    zcam_highbounce = 0;
    zcam_convers = 0;
    zcam_fly = 0;
    zcam_flypaused = 0;
    zcam_cutscene = 0;
    zcam_reward = 0;
    zcam_fovcurr = zCamera_f_75_0;
    zcam_overrot_tmr = zCamera_f_0_0;

    wall_jump_enabled = WJVS_DISABLED;
    lassocam_enabled = false;
    stop_track = 0;
    zcam_mintgtheight = zCamera_f_n1_0_e38;

    xCameraSetFOV(cam, zCamera_f_75_0);
    zCameraTweakGlobal_Update(zCamera_f_0_0);
    xCameraReset(cam, ::GetCurrentD(), ::GetCurrentH(), ::GetCurrentPitch());

    input_enabled = true;
    dMultiplier = zCamera_f_1_0;
    dOffset = zCamera_f_0_0;
    hMultiplier = zCamera_f_1_0;
    hOffset = zCamera_f_0_0;
}

namespace
{
    F32 _GetCurrentH();
    F32 _GetCurrentD();

    F32 GetCurrentPitch()
    {
        if (zcam_highbounce != 0)
        {
            return zcam_highbounce_pitch;
        }

        return zCameraTweakGlobal_GetPitch();
    }

    F32 GetCurrentH()
    {
        return dMultiplier * _GetCurrentH() + dOffset;
    }

    F32 _GetCurrentH()
    {
        if (zcam_highbounce != 0)
        {
            return zcam_highbounce_h;
        }

        if (wall_jump_enabled == WJVS_ENABLED)
        {
            return zcam_wall_h;
        }

        return zCameraTweakGlobal_GetH();
    }

    F32 GetCurrentD()
    {
        return dMultiplier * _GetCurrentD() + dOffset;
    }

    F32 _GetCurrentD()
    {
        if (zcam_highbounce != 0)
        {
            return zcam_highbounce_d;
        }

        if (wall_jump_enabled == WJVS_ENABLED)
        {
            return zcam_wall_d;
        }

        return zCameraTweakGlobal_GetD();
    }
} // namespace

static F32 EaseInOut(F32 param)
{
    // using shorter symbols the return expression below reads:
    // p (p (b - cp) + a) <=> ap + bp^2 - cp^3
    // to which wolframalpha associates the geometric figure 'line'
    return param * (param * (zCamera_f_2_0 - zCamera_f_1_5 * param) + zCamera_f_0_5);
}

static void zCameraConversUpdate(xCamera* cam, F32 dt)
{
    if (zcam_dest == NULL)
    {
        return;
    }

    if (zcam_tmr <= 0.0f)
    {
        zcam_tmr = 0.0f;
        return;
    }

    if ((dt / zcam_tmr) > 1.0f)
    {
        cam->mat.right = zcam_dest->right;
        cam->mat.up = zcam_dest->up;
        cam->mat.at = zcam_dest->at;
        cam->mat.pos = zcam_dest->pos;

        zcam_fovcurr = zcam_fovdest;
    }
    else
    {
        F32 ease1 = EaseInOut(1.0f - (zcam_tmr / zcam_ttm));
        F32 ease2 = EaseInOut(1.0f - (zcam_tmr - dt) / zcam_ttm);
        F32 t = (ease2 - ease1) / (1.0f - ease1);

        xQuat tOld;
        xQuat tNew;
        xQuatFromMat(&tOld, &cam->mat);
        xQuatSlerp(&tNew, &tOld, &zcam_quat, t);
        xQuatToMat(&tNew, &cam->mat);
        xVec3Lerp(&cam->mat.pos, &cam->mat.pos, &zcam_dest->pos, t);
        zcam_fovcurr = zcam_fovcurr * (zCamera_f_1_0 - t) + (zcam_fovdest * t);
    }

    zcam_tmr = zcam_tmr - dt;
}

static F32 TranSpeed(zFlyKey keys[])
{
    return zCamera_f_30_0 * xVec3Dist((xVec3*)&keys[0].matrix[9], (xVec3*)&keys[1].matrix[9]);
}

static F32 MatrixSpeed(zFlyKey keys[])
{
    F32 dot1 = iabs(xVec3Dot((xVec3*)&keys[0].matrix[0], (xVec3*)&keys[1].matrix[0]));
    F32 dot2 = iabs(xVec3Dot((xVec3*)&keys[0].matrix[3], (xVec3*)&keys[1].matrix[3]));

    // dot1 = max(dot1, dot2)
    if (dot2 > dot1)
    {
        dot1 = iabs(xVec3Dot((xVec3*)&keys[0].matrix[3], (xVec3*)&keys[1].matrix[3]));
    }

    dot2 = iabs(xVec3Dot((xVec3*)&keys[0].matrix[6], (xVec3*)&keys[1].matrix[6]));

    // dot1 = max(dot1, dot2)
    if (dot2 > dot1)
    {
        dot1 = iabs(xVec3Dot((xVec3*)&keys[0].matrix[6], (xVec3*)&keys[1].matrix[6]));
    }

    // m = max(1.0, dot1)
    F32 m = zCamera_f_1_0 < dot1 ? zCamera_f_1_0 : dot1;
    return xacos(m) * zCamera_f_114_592 * zCamera_f_30_0;
}

void zCameraFlyStart(U32 assetID)
{
    st_PKR_ASSET_TOCINFO info;
    if (xSTGetAssetInfo(assetID, &info) == 0)
    {
        return;
    }

    // weird register use
    zcam_fly = 1;
    zcam_flypaused = 0;
    zcam_flydata = info.mempos;
    zcam_flysize = info.size;
    // until here
    zcam_flytime = zCamera_f_0_033;
    zcam_flyasset_current = assetID;

    zEntPlayerControlOff(CONTROL_OWNER_FLY_CAM);
    xScrFxLetterbox(1);

    zcam_backupcam = globals.camera;

    if (zCamera_FlyOnly() == 0)
    {
        zMusicSetVolume(zCamera_f_0_5, zCamera_f_0_1);
    }
}

void zCameraFreeLookSetGoals(xCamera* cam, F32 pitch_s, F32& dgoal, F32& hgoal, F32& pitch_goal,
                             F32& lktm, F32 dt)
{
    if (zcam_bbounce != 0)
    {
        if (zcam_highbounce != 0)
        {
            dgoal = ::GetCurrentD();
            hgoal = ::GetCurrentH();
            pitch_goal = ::GetCurrentPitch();
            return;
        }

        dgoal = zcam_near != 0 ? zCamera_f_3_5 : ::GetCurrentD();
        hgoal = zcam_near != 0 ? zCamera_f_2_4 : ::GetCurrentH();

        F32 newPitchGoal;
        if (zcam_longbounce != 0)
        {
            F32 len = xsqrt(zcam_playervel->x * zcam_playervel->x +
                            zcam_playervel->y * zcam_playervel->y +
                            zcam_playervel->z * zcam_playervel->z);

            bool lenValid = false;
            if (zcam_playervel != NULL)
            {
                if (len != zCamera_f_0_0)
                {
                    lenValid = true;
                }
            }

            if (lenValid)
            {
                newPitchGoal =
                    (cam->mat.at.x * zcam_playervel->x + cam->mat.at.y * zcam_playervel->y +
                     cam->mat.at.z * zcam_playervel->z) /
                    len;
                newPitchGoal = zCamera_f_0_0 < newPitchGoal ? zCamera_f_0_0 : newPitchGoal;
                newPitchGoal = -newPitchGoal;
            }
            else
            {
                newPitchGoal = zCamera_f_0_0;
            }

            if (zcam_near != 0)
            {
                newPitchGoal = zCamera_f_3_141 * (zCamera_f_20_0 * newPitchGoal + zCamera_f_20_0) /
                               zCamera_f_180_0;
            }
            else
            {
                newPitchGoal = zCamera_f_0_523;
            }

            pitch_goal = newPitchGoal;
        }
        else
        {
            if (zcam_near != 0)
            {
                newPitchGoal = zCamera_f_0_698;
            }
            else
            {
                newPitchGoal = zCamera_f_0_523;
            }

            pitch_goal = newPitchGoal;
        }
        return;
    }

    F32 d = ::GetCurrentD();
    F32 h = ::GetCurrentH();
    F32 p = ::GetCurrentPitch();

    if (lassocam_enabled && stop_track == 0)
    {
        dgoal = lassocam_factor * (d - zcam_near_d) + zcam_near_d;
        hgoal = lassocam_factor * (h - zcam_near_h) + zcam_near_h;
        pitch_goal = lassocam_factor * (p - zcam_near_pitch) + zcam_near_pitch;
        return;
    }

    if (pitch_s > zCamera_f_0_0)
    {
        dgoal = pitch_s * (zcam_below_d - d) + d;
        hgoal = pitch_s * (zcam_below_h - h) + h;
        pitch_goal = pitch_s * (pitch_s * (pitch_s * (zcam_below_pitch - p))) + p;
    }
    else
    {
        dgoal = -pitch_s * (zcam_above_d - d) + d;
        hgoal = -pitch_s * (zcam_above_h - h) + h;
        pitch_goal = -pitch_s * (zcam_above_pitch - p) + p;
    }

    if (lktm > 0.1f)
    {
        lktm -= dt;
        if (lktm < 0.1f)
        {
            lktm = 0.1f;
        }
        return;
    }

    lktm = 0.1f;
}

void zCameraSetBbounce(S32 bbouncing)
{
    zcam_bbounce = bbouncing;
}

void zCameraSetLongbounce(S32 lbounce)
{
    if (zcam_highbounce != 0 || zcam_longbounce != lbounce)
    {
        zcam_lbbounce = 0;
    }

    zcam_longbounce = lbounce;
    // li r0 happens too early
    zcam_highbounce = 0;
}

void zCameraSetHighbounce(S32 lbounce)
{
    if (zcam_longbounce != 0 || zcam_highbounce != lbounce)
    {
        zcam_lbbounce = 0;
    }

    zcam_highbounce = lbounce;
    // li r0 happens too early
    zcam_longbounce = 0;
}

void zCameraSetPlayerVel(xVec3* vel)
{
    zcam_playervel = vel;
}

void zCameraDisableTracking(camera_owner_enum owner)
{
    stop_track = stop_track | owner;
}

void zCameraEnableTracking(camera_owner_enum owner)
{
    stop_track = stop_track & ~owner;
}

U32 zCameraIsTrackingDisabled()
{
    return stop_track;
}

void zCameraDisableInput()
{
    input_enabled = 0;
}

void zCameraEnableInput()
{
    input_enabled = 1;
}

void zCameraDisableLassoCam()
{
    lassocam_enabled = 0;
}

void zCameraEnableLassoCam()
{
    lassocam_enabled = 1;
}

void zCameraSetLassoCamFactor(F32 new_factor)
{
    lassocam_factor = new_factor;
}

F32 zCameraGetLassoCamFactor()
{
    return lassocam_factor;
}

S32 zCameraGetConvers()
{
    return zcam_convers;
}

void zCameraTranslate(xCamera* cam, F32 x, F32 y, F32 z)
{
    cam->mat.pos.x += x;
    cam->mat.pos.y += y;
    cam->mat.pos.z += z;
    cam->tran_accum.x += x;
    cam->tran_accum.y += y;
    cam->tran_accum.z += z;
}

void zCameraDisableWallJump(xCamera* cam)
{
    if (wall_jump_enabled != WJVS_DISABLED)
    {
        wall_jump_enabled = WJVS_DISABLING;
    }
}

void zCameraSetReward(S32 reward)
{
    if (zCameraIsTrackingDisabled() != 0)
    {
        zcam_reward = 0;
        return;
    }
    zcam_reward = reward;
}

void zCameraMinTargetHeightSet(F32 min_height)
{
    zcam_mintgtheight = min_height;
}

void zCameraMinTargetHeightClear()
{
    zcam_mintgtheight = zCamera_f_n1_0_e38;
}

F32 xVec3Dist2(const xVec3* vecA, const xVec3* vecB)
{
    float dx = vecA->x - vecB->x;
    float dy = vecA->y - vecB->y;
    float dz = vecA->z - vecB->z;
    return dx * dx + dy * dy + dz * dz;
}

U32 zCamera_FlyOnly()
{
    switch (globals.sceneCur->sceneID)
    {
    case 'PG12':
    case 'KF05':
    case 'DB02':
    case 'SM02':
    case 'SM03':
    case 'SM04':
        return 1;
    default:
        return 0;
    }
}
