// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2009 eth32 team
// www.nixcoders.org

#pragma once

#define ANG_CLIP(ang) if(ang>180.0f)ang-=360.0f;else if(ang<-180.0f)ang+=360.0f
#define AIM_FOV_VALUE eth32.settings.fov

typedef enum {
    SORT_OFF,
    SORT_CROSSHAIR,
    SORT_DISTANCE,
	SORT_ATTACKER,
    SORT_ACCURACY,
	SORT_THREAT,
	SORT_KDRATIO,
    SORT_MAX
} sorttype_t;

extern const char *sortTypeText[];
extern const char *humanAimTypeText[];
extern const char *aimModeText[];
extern const char *aimprotectText[];

/* enum and struct for advanced trace */
typedef enum {
	HEAD_CENTER,			// trace center point only
	HEAD_STATIC,			// generate static points  
	HEAD_XTRACE,
	HEAD_MAX
} headBoxtrace_t;

typedef enum {
	BODY_CENTER,			// trace center point only
	BODY_CONTOUR,			// trace periphery and plane centers
	BODY_STATIC,			// generate static points  
	BODY_XTRACE,
 	BODY_RANDOM_VOLUME,		// generate random points within box volume
   	BODY_RANDOM_SURFACE,		// generate random points on the side(s) of the box that is(are) facing us
	BODY_CAPSULE_VOLUME,		// fill a capsule with random points
	BODY_CAPSULE_SURFACE,		// fill the surface of a capsule with random points
	BODY_MAX
} bodyBoxtrace_t;

extern const char *bodyTraceTypeText[BODY_MAX];
extern const char *headTraceTypeText[HEAD_MAX];

typedef struct {
	vec3_t pt;
	float  d;
} trace_point;

/* mdx box offsets vary on stance, movement */
typedef struct {
	vec3_t stand_offset;
	vec3_t crouch_offset;
	vec3_t prone_offset;
	vec3_t stand_offset_moving;
	vec3_t crouch_offset_moving;
	vec3_t size;
} hitbox_t;

typedef enum {
	HITBOX_OFF,
 	HITBOX_ETPRO,
 	HITBOX_CUSTOM,
	HITBOX_MAX
} hitbox_type_t;

extern const char *hitboxText[];

/* these define the head hitboxes. format is { fwd, right, up }, relative to the tag's orientation
	Matched them as good as possibly with server hitboxes using modded servercode
	Hitboxes don't rotate in world space, but the offset rotates with tag_head.

	etpub/etmain boxes are really based on centity position, not on tag_head position, so need
	different aimbot code to handle those mods... for now use everything tag_head based (tag_mouth sucks anyway)

	always use correct hitbox mode.. wrong vecs == bad
*/

static hitbox_t head_hitboxes[] =
{
    // stand               crouch         prone              stand moving      crouch moving    hitbox size (x,y,z)
 { { 0.0, 0.0, 0.0}, { 0.0, 0.0, 0.0}, { 0.0,  0.0, 0.0}, { 0.0,  0.0, 0.0}, { 0.0,  0.0, 0.0}, { 0.0, 0.0, 0.0} },	// OFF
 { { 0.3, 0.3, 7.0}, {-0.3, 0.8, 7.0}, { 0.0,  0.3, 6.9}, { 0.0,  0.0, 6.5}, { 0.0, -0.7, 7.0}, {11.0,11.0,12.0} },	// ETPRO b_realhead 1

};

// master aim mode
typedef enum
{
	AIMMODE_OFF,
	AIMMODE_NORMAL,
	AIMMODE_HUMAN,
	AIMMODE_MAX
} aimMode_t;

typedef enum
{
	HUMAN_AIM_LUCKY,
	HUMAN_AIM_FULL,
	HUMAN_AIM_MAX
} humanaimtype_t;

typedef enum
{
	AIM_OFF,
	AIM_ON_FIRE,
	AIM_ON_BUTTON,
	AIM_ALWAYS,
	AIM_TRIGGER,
	AIM_MAX
} aimtype_t;

extern const char *aimTypeText[];

typedef enum {
	BODY_ONLY,
	HEAD_ONLY,
	BODY_HEAD,
	HEAD_BODY,
	HEAD_PRIORITY,
	AP_MAX
} headBody_t;

extern const char *priorityTypeText[];

#define MAX_BULLETS 100

class CAimbot
{
public:
	CAimbot(void);
	void SetSelf( int clientNum );
	void PreFrame(void);
	void PostFrame(void);
	void Respawn(void);
	void AddTarget(player_t *player);
	void LockSensitivity(bool lock);
	bool SensitivityLocked();
	void Autofire(bool enable);
	void Autocrouch(bool enable, bool force);
	void SetUserAttackStatus(bool atkPressed);
	void SetUserCrouchStatus(bool crouched);
	void SetAimkeyStatus(bool state) { aimkeyPressed = state; }
	void DrawHeadBox( int clientNum, bool axes );
	void DrawBodyBox( int clientNum );
	void DrawGrenadelauncherTrace();
	void DrawGrenadeTrace();
	void SetAttackButton( void *ptr );
	void SetCrouchButton( void *ptr );
	float CrosshairDistance(const player_t *player);
	void PredictPlayer(player_t *player, float time, vec3_t pos, int type);

	hitbox_t customHitbox;	// user defined head hitbox. public for now so others can modify
	float spreadd;

	bool userCrouching;			// are we crouching
	bool userProning;			// are we proning

	// for cams
	vec3_t	lastImpact;
	float	flyTime;
	float	Lastpitch;

private:
	void Point(vec3_t vieworg);
	inline int CheckFov(vec3_t origin);
	inline void applySelfPrediction();
	inline void applyTargPrediction();

	bool traceHeadBox( vec3_t boxOrigin, float dist, vec3_t size, vec3_t trOrigin, int skipEnt, player_t *player, headBoxtrace_t trType,  vec3_t visible, int maxTraces );
	bool traceBodyBox( vec3_t boxOrigin, float dist, vec3_t size, vec3_t trOrigin, int skipEnt, player_t *player, bodyBoxtrace_t trType,  vec3_t visible, int maxTraces );

	bool traceHead( orientation_t *head, vec3_t hitPoint, int clientNum );
	bool traceHead( orientation_t *head, vec3_t hitPoint, vec3_t start, int clientNum );
	bool traceBody( vec3_t hitPoint, int clientNum );
	bool ballisticTrajectoryValid(vec3_t start, vec3_t end, float pitch, float flytime, float v);

	void DoBulletBot(void);

	player_t* frameTargets[MAX_CLIENTS];
	int numFrameTargets;

	bool lockMouse;
	bool moving;
	bool stopAutoTargets;	// block auto targets if we manually intervene
	bool senslocked;		// mousemovent blocked
	bool firing;			// used for autofire purposes
	bool attackPressed;		// if fire button is down == true
	bool aimkeyPressed;		// if bound aim key is pressed
	bool validFrameTarget;	// used to check if we should allow +attack to pass
	bool autoMode;			// bot controls fire
	bool Trigger;			// trigger aim
	bool acFirstFrame;		// this first AC frame
	kbutton_t *atkBtn;		// ET.exe's attackbutton
	kbutton_t *crhBtn;		// ET.exe's crouchbutton

	//   else set target to NULL
	player_t *target;
	player_t *lastTarget;	// for use by lockTarget

	bool lastTargetValid;
	player_t *self;			// us :)
};

extern CAimbot Aimbot;
