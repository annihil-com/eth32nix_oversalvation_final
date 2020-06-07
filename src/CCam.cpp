// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2009 eth32 team
// www.nixcoders.org

#include "eth32.h"

#ifndef min
#define min( x, y ) ( ( ( x ) < ( y ) ) ? ( x ) : ( y ) )
#endif
#ifndef max
#define max( x, y ) ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )
#endif

CCam Cam;
bool drawingCam = false;
vector<refEntity_t> refEntities;

// buffalo: all rail colors are not adjustable yet until I finish the controls for it

const char *camTypeText[CAM_MAX] =
{
	"Mortar",
	"Mortar Follow",
};


void CCam::resetInfo(void)
{
	this->enemyNum = -1;
	VectorClear(Cam.gentityOrigin);
	VectorClear(Cam.dropAngles);
}

/***********
* Main Cam *
************/

void CCam::drawCam(float x, float y, float width, float height, refdef_t *refDef, qboolean crosshair)
{
	drawingCam = true;

	if (!eth32.cg.snap)
		return;

	for(int i=0; i < refEntities.size(); i++ )
	{
		refEntity_t* re = &refEntities.at(i);
		if(re->entityNum == eth32.cg.snap->ps.clientNum && eth32.cg.players[re->entityNum].infoValid && re->torsoFrameModel)
		{
			re->renderfx |= RF_DEPTHHACK | RF_NOSHADOW;
			re->renderfx &= ~RF_THIRD_PERSON;
		}
		Syscall.R_AddRefEntityToScene(&refEntities.at(i));
	}

	refDef->x = x;
	refDef->y = y;
	refDef->width = width;
	refDef->height = height;

	// Draw the spycam scene
	orig_syscall(CG_R_RENDERSCENE, refDef);

	// Draw border
	Draw.RawRect(x, y, width, height, GUI_FONTCOLOR1, 1);

	// Draw the crosshair
	#define CROSSHAIR_SIZE 4
	if (crosshair)
		Draw.RawRect(x + (width / 2) - (CROSSHAIR_SIZE / 2), y + (height / 2) - (CROSSHAIR_SIZE / 2) , CROSSHAIR_SIZE, CROSSHAIR_SIZE, colorRed, 1);
}

/*************
* Mortar Cam *
**************/


void CCam::mortarCam(camInfo_t *ci)
{
	if (eth32.cg.snap->ps.ammo == 0)
		return;

// Set mortar trajectory from current view
	vec3_t angles, forward;
	VectorCopy(eth32.cg.refdef->viewaxis[ROLL], forward);
	VectorCopy(eth32.cg.snap->ps.viewangles, angles);
	angles[PITCH] -= 60.f;
	AngleVectors(angles, forward, NULL, NULL);
	forward[0] *= 3000 * 1.1f;
	forward[1] *= 3000 * 1.1f;
	forward[2] *= 1500 * 1.1f;

	trajectory_t mortarTrajectory;
	mortarTrajectory.trType = TR_GRAVITY;
	mortarTrajectory.trTime = eth32.cg.time;
	VectorCopy(eth32.cg.muzzle, mortarTrajectory.trBase);
	VectorCopy(forward, mortarTrajectory.trDelta);

	// Calculate mortar impact
	int timeOffset = 0;
	trace_t mortarTrace;
	vec3_t mortarImpact;
	VectorCopy(mortarTrajectory.trBase, mortarImpact);
	#define TIME_STEP 20
	while (timeOffset < 10000) {
		vec3_t nextPos;
		timeOffset += TIME_STEP;
		BG_EvaluateTrajectory(&mortarTrajectory, eth32.cg.time + timeOffset, nextPos, qfalse, 0);
		orig_CG_Trace(&mortarTrace, mortarImpact, 0, 0, nextPos, eth32.cg.snap->ps.clientNum, MASK_MISSILESHOT);
		if ((mortarTrace.fraction != 1)
				// Stop if we hit sky
				&& !((mortarTrace.surfaceFlags & SURF_NODRAW) || (mortarTrace.surfaceFlags & SURF_NOIMPACT))
				&& (mortarTrace.contents != 0)) {
			break;
		}
		VectorCopy(nextPos, mortarImpact);
	}

	memcpy(&camRefDef, &eth32.cg.refdef, sizeof(refdef_t));

	// kobject: add some angles
	vec3_t	dpos;
	vec3_t	camOrg;

	dpos[0] = eth32.cg.refdef->vieworg[0]-mortarImpact[0];
	dpos[1] = eth32.cg.refdef->vieworg[1]-mortarImpact[1];
	dpos[2] = 0.0f;
	VectorNormalizeFast( dpos );
	VectorCopy( mortarImpact, camOrg );
	VectorMA( camOrg, ci->distance * sinf(ci->angle * M_PI/180.0), zAxis, camOrg );
	VectorMA( camOrg, ci->distance * cosf(ci->angle * M_PI/180.0), dpos, camOrg );

	int w = ci->x2 - ci->x1;
	int h = ci->y2 - ci->y1;

	camRefDef.fov_x = (w>h) ? ci->fov : ci->fov * w / h;
	camRefDef.fov_y = (h>w) ? ci->fov : ci->fov * h / w;

	VectorCopy(camOrg, camRefDef.vieworg);

	vec3_t camAngle;
	VectorCopy(eth32.cg.refdefViewAngles, camAngle);
	camAngle[PITCH] = ci->angle;

	AnglesToAxis(camAngle, camRefDef.viewaxis);

	drawCam(ci->x1, ci->y1, w, h, &camRefDef, qtrue);

	// Draw impact time
	sprintf(this->str, "^7Impact Time: ^b%.1f ^7seconds", (float)timeOffset / 1000.0f);
	Draw.Text(ci->x1 + (w / 2) - (TEXTWIDTH(this->str) / 2), ci->y1 + h - 22 , 0.24f, str, GUI_FONTCOLOR1, qfalse, qtrue, &eth32.cg.media.fontArial, true);

}

/********************
* Mortar Follow Cam *
*********************/


void CCam::followCam(camInfo_t *ci)
{

	vec3_t		forward, right, up, ViewAngles;
	refdef_t	camRefDef;

	vec3_t		focusAngles, focusPoint, view;

	float		forwardScale, sideScale, focusDist;

	if(!IS_WEAPATTRIB(eth32.cg.snap->ps.weapon, WA_MORTAR))
		return;

	memcpy(&camRefDef, &eth32.cg.refdef, sizeof(refdef_t));

	VectorCopy( eth32.cg.refdefViewAngles, ViewAngles );
	VectorCopy( ViewAngles, camRefDef.vieworg );
	VectorCopy( ViewAngles, focusAngles );
	focusAngles[PITCH] = 45;

	AngleVectors( focusAngles, forward, NULL, NULL );

	VectorCopy( this->gentityOrigin, focusPoint );
	VectorCopy( this->gentityOrigin, view );
	view[2] += 8;

	ViewAngles[PITCH] *= 0.5;

	AngleVectors( ViewAngles, forward, right, up );

	forwardScale = cos( 0 / 180 * M_PI );
	sideScale = sin( 0 / 180 * M_PI );
	VectorMA( view, -ci->distance * forwardScale, forward, view );
	VectorMA( view, -ci->distance * sideScale, right, view );

	// select pitch to look at focus point from vieword
	VectorSubtract( focusPoint, view, focusPoint );
	focusDist = sqrt( focusPoint[0] * focusPoint[0] + focusPoint[1] * focusPoint[1] );

	if ( focusDist < 1 ) {
		focusDist = 1;	// should never happen
	}

	int w = ci->x2 - ci->x1;
	int h = ci->y2 - ci->y1;

	VectorCopy( view, camRefDef.vieworg );

	// fov
	camRefDef.fov_x = (w>h) ? ci->fov : ci->fov * w / h;
	camRefDef.fov_y = (h>w) ? ci->fov : ci->fov * h / w;

	//Adjusting the angle to look towards entity origin
	vec3_t temp;
	VectorCopy( ViewAngles, temp );

	temp[PITCH] = -180 / M_PI * atan2( focusPoint[2], focusDist );

	AnglesToAxis( temp, camRefDef.viewaxis );

	drawCam(ci->x1, ci->y1, w, h, &camRefDef, qfalse);
}
