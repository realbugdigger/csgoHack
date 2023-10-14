#pragma once

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define ABS(x) ((x < 0) ? (-x) : (x))

#define W2S(x,y) hack->WorldToScreen(x, y)

static double PI = 3.14159265358;

struct Vec2 {
	float x, y;
};

struct Vector3 {
	float x, y, z;

	Vector3() {};
	Vector3(const float x, const float y, const float z) : x(x), y(y), z(z) {}
	Vector3 operator + (const Vector3& rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
	Vector3 operator - (const Vector3& rhs) const { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
	Vector3 operator * (const float& rhs) const { return Vector3(x * rhs, y * rhs, z * rhs); }
	Vector3 operator / (const float& rhs) const { return Vector3(x / rhs, y / rhs, z / rhs); }
	Vector3& operator += (const Vector3& rhs) { return *this = *this + rhs; }
	Vector3& operator -= (const Vector3& rhs) { return *this = *this - rhs; }
	Vector3& operator *= (const float& rhs) { return *this = *this * rhs; }
	Vector3& operator /= (const float& rhs) { return *this = *this / rhs; }
	float Length() const { return sqrt(x * x + y * y + z * z); }
	Vector3 Normalize() const { return *this * (1 / Length()); }
	float Distance(const Vector3& rhs) const { return (*this - rhs).Length(); }
};

struct Vec4 {
	float x, y, z, w;
};

using Vec3 = Vector3;
using vec3 = Vector3;
using Vec = Vector3;
using vec = Vector3;


class Ent {
public:
	union {
		// isDormant
		DEFINE_MEMBER_N(bool, isDormant, 0xED);
		// iTeamNum
		DEFINE_MEMBER_N(int, iTeamNum, 0xF4);
		// iHealth
		DEFINE_MEMBER_N(int, iHealth, 0x100);
		// m_vecViewOffset
		DEFINE_MEMBER_N(Vec3, m_vecViewOffset, 0x108);
		// m_fFlags
		DEFINE_MEMBER_N(int, m_fFlags, 0x104);
		// vecOrigin
		DEFINE_MEMBER_N(Vec3, vecOrigin, 0x138);
		// m_bSpotted
		DEFINE_MEMBER_N(bool, m_bSpotted, 0x93D);
		// boneMatrix
		DEFINE_MEMBER_N(int, boneMatrix, 0x26A8);
		// aimPunchAngle
		DEFINE_MEMBER_N(Vec3, aimPunchAngle, 0x303C);
		// armorValue
		DEFINE_MEMBER_N(int, ArmorValue, 0x117CC);
	};

public:
	Vec3* GetOrigin() {
		//return (Vec3*)(*(uintptr_t*)this + hazedumper::netvars::m_vecOrigin);
		//return (Vec3*)(*(uintptr_t*)this + 0x138);
		return &vecOrigin;
	}

	Vec3* GetViewOffset() {
		//return (Vec3*)(*(uintptr_t*)this + hazedumper::netvars::m_vecViewOffset);
		//return (Vec3*)(*(uintptr_t*)this + 0x12C); // sta je view offset
		return &m_vecViewOffset;
	}

	void AimAt(Vec3* target) {
		static uintptr_t engineModule = (uintptr_t)GetModuleHandle(L"engine.dll");
		//static Vec3* viewAngles = (Vec3*)(*(uint32_t*)(engineModule + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_ViewAngles);
		static Vec3* viewAngles = (Vec3*)(*(uintptr_t*)(engineModule + 0x59F19C) + 0x4D90);

		Vec3 origin = *GetOrigin();
		Vec3 viewOffset = *GetViewOffset();
		Vec3 tmp = origin + viewOffset;
		Vec3* myPos = &(tmp);

		Vec3 deltaVec = { target->x - myPos->x, target->y - myPos->y, target->z - myPos->z };
		float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);

		float pitch = -asin(deltaVec.z / deltaVecLength) * (180 / PI);
		float yaw = atan2(deltaVec.y, deltaVec.x) * (180 / PI);

		if (pitch >= -89 && pitch <= 89 && yaw >= -180 && yaw <= 180)
		{
			viewAngles->x = pitch;
			viewAngles->y = yaw;
		}
	}

	float GetDistance(Vec3* otherEnt) {
		Vec3* myPos = GetOrigin();
		Vec3 delta;
		delta.x = otherEnt->x - myPos->x;
		delta.y = otherEnt->y - myPos->y;
		delta.z = otherEnt->z - myPos->z;

		return sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
	}
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
	uintptr_t dwEntityList = 0x4E051DC;
	uintptr_t dwViewMatrix = 0x4DF6024;

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
		bool aimbot = false;
		bool triggerbot = false;
	} settings;

	struct Buttons {
		DWORD showMenuBtn = VK_INSERT;
		DWORD snaplinesBtn = VK_NUMPAD1;
		DWORD box2DBtn = VK_NUMPAD2;
		DWORD statusBtn = VK_NUMPAD3;
		DWORD statusTextBtn = VK_NUMPAD4;
		DWORD rcsCrosshairBtn = VK_NUMPAD5;
		DWORD aimbotBtn = VK_NUMPAD6;
		DWORD triggerbotBtn = VK_NUMPAD7;
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