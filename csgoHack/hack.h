#pragma once

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define ABS(x) ((x < 0) ? (-x) : (x))

#define W2S(x,y) hack->WorldToScreen(x, y)

struct Vec2 {
	float x, y;
};
struct Vec3 {
	float x, y, z;
};
struct Vec4 {
	float x, y, z, w;
};

class Ent {
public:
	union {
		// isDormant
		DEFINE_MEMBER_N(bool, isDormant, 0xED);
		// iHealth
		DEFINE_MEMBER_N(int, iHealth, 0x100);
		// vecOrigin
		DEFINE_MEMBER_N(Vec3, vecOrigin, 0x138);
		// iTeamNum
		DEFINE_MEMBER_N(int, iTeamNum, 0xF4);
		// boneMatrix
		DEFINE_MEMBER_N(int, boneMatrix, 0x26A8);
		// armorValue
		DEFINE_MEMBER_N(int, ArmorValue, 0x117CC);
		// aimPunchAngle
		DEFINE_MEMBER_N(Vec3, aimPunchAngle, 0x303C);
	};
};

class EntListObj {
public:
	Ent* ent;
	char padding[12];
};

class EntList {
public:
	EntListObj ents[32];
};

class Hack {
public:
	uintptr_t dwEntityList = 0x4E0102C;
	uintptr_t dwViewMatrix = 0x4DF1E74;

	uintptr_t engine;
	uintptr_t client;
	Ent* localEnt;
	EntList* entList;
	float viewMatrix[16];

	ID3DXLine* LineL;
	ID3DXFont* FontF;

	Vec2 crosshair2D;
	int crosshairSize = 4;

	~Hack();

	void Init();
	void Update();

	void CheckButtons();

	bool CheckValidEnt(Ent* ent);
	bool WorldToScreen(Vec3 pos, Vec2& screen);
	Vec3 GetBonePos(Ent* ent, int bone);

	struct Settings {
		bool showMenu = true;
		bool snaplines = false;
		bool box2D = false;
		bool status = false;
		bool statusText = false;
		bool rcsCrosshair = false;
	} settings;

	struct Buttons {
		DWORD showMenuBtn = VK_INSERT;
		DWORD snaplinesBtn = VK_NUMPAD1;
		DWORD box2DBtn = VK_NUMPAD2;
		DWORD statusBtn = VK_NUMPAD3;
		DWORD statusTextBtn = VK_NUMPAD4;
		DWORD rcsCrosshairBtn = VK_NUMPAD5;
	} button;

	struct Colors {
		D3DCOLOR health = D3DCOLOR_ARGB(255, 46, 139, 87);
		D3DCOLOR armor = D3DCOLOR_ARGB(255, 30, 144, 255);
		D3DCOLOR crosshair = D3DCOLOR_ARGB(255, 255, 255, 255);
		struct Team {
			D3DCOLOR esp = D3DCOLOR_ARGB(255, 0, 255, 0);
			D3DCOLOR snapline = D3DCOLOR_ARGB(255, 0, 255, 0);
		} team;
		struct Enemy {
			D3DCOLOR esp = D3DCOLOR_ARGB(255, 255, 0, 0);
			D3DCOLOR snapline = D3DCOLOR_ARGB(255, 255, 0, 0);
		} enemy;
	} color;
};