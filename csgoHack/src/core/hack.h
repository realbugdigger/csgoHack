#pragma once
#include <random>
#include <thread>
#include <iostream>

#include "includes.h"

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

private:
	// p0 - start view angles
	// p1 - middle point or random point
	// p2 - final view angles
	POINT CalculateBezierPoint(const float t, const POINT p0, const POINT p1, const POINT p2)
	{
		const float u{ 1 - t };
		const float tt{ t * t };
		const float uu{ u * u };

		POINT p{ 0, 0 };
		p.x = static_cast<float>(uu * p0.x + 2 * u * t * p1.x + tt * p2.x);
		p.y = static_cast<float>(uu * p0.y + 2 * u * t * p1.y + tt * p2.y);

		return p;
	}

	//void DoWork(std::shared_ptr<Vec3> target) {
	void DoWork() {
		std::cout << "[!!!] New Thread Started\n";

		std::shared_ptr<Vec3> target;
		{
			std::lock_guard<std::mutex> lock(mtx);
			target = globalPosPtr;
		}

		static uintptr_t engineModule = (uintptr_t)GetModuleHandle(L"engine.dll");
		//static Vec3* viewAngles = (Vec3*)(*(uint32_t*)(engineModule + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_ViewAngles);
		static Vec3* viewAngles = (Vec3*)(*(uintptr_t*)(engineModule + 0x59F19C) + 0x4D90);

		Vec3 origin = *GetOrigin();
		Vec3 viewOffset = *GetViewOffset();
		Vec3 tmp = origin + viewOffset;
		Vec3* myPos = &(tmp);

		// Calculate the distance from our head position to enemy head position (The Hypotenuse !!!!)
		Vec3 deltaVec = { target->x - myPos->x, target->y - myPos->y, target->z - myPos->z };
		float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);

		std::cout << "\t[INFO] Local player view angle: x,y,z = " << viewAngles->x << ", " << viewAngles->y << ", " << viewAngles->z << "\n";
		std::cout << "\t[INFO] Enemy player head pos  : x,y,z = " << target->x << ", " << target->y << ", " << target->z << "\n";

		// when we have degrees we have to multiply the value by (180 / PI) to get radians
		// when we have radians we have to multiply the value by (PI / 180) to get degrees

		// deltaVec.z - the height difference between our head and enemy head
		float pitch = -asin(deltaVec.z / deltaVecLength) * (180 / PI);
		float yaw = atan2f(deltaVec.y, deltaVec.x) * (180 / PI);

		std::cout << "\t\t[***] Pitch: " << pitch << "\n";
		std::cout << "\t\t[***] Yaw: " << yaw << "\n";

		std::mt19937 gen{ std::random_device{}() };
		std::uniform_int_distribution<int> dist{ -4, 4 };
		int random_number = 0;
		do {
			random_number = dist(gen);
		} while (random_number == 0);


		/*POINT cursorsLoc;
		GetCursorPos(&cursorsLoc);
		Vec2 deltaVecDistance = { pitch - cursorsLoc.x, yaw - cursorsLoc.y};
		float deltaVecDistanceLength = sqrt(deltaVecDistance.x * deltaVecDistance.x + deltaVecDistance.y * deltaVecDistance.y);
		std::cout << "Distance to enemy head: " << deltaVecDistanceLength << "\n";*/

		// maybe just check the distance and not the yaw and pitch for LERP ???
		if (pitch >= -25 && pitch <= 25 && yaw >= -40 && yaw <= 40) {
			std::cout << "\t\t[LERP] Linear interprotation !!!!!\n";

			constexpr int steps{ 50 }; // Number of steps for interpolation
			constexpr int delayMs{ 10 }; // Delay in milliseconds between steps

			for (int i{ 0 }; i <= steps; i++)
			{
				// 't' represents the interpolation parameter ranging from 0 to 1.
				// It indicates the relative position between the start and end points:
				// - At t = 0, the position is at the start.
				// - At t = 1, the position is at the end.
				// - At 0 < t < 1, the position is proportionally between the start and end.
				const float t{ static_cast<float>(i) / static_cast<float>(steps) };
				
				const float newPitch{ viewAngles->x + static_cast<float>((pitch - viewAngles->x) * t) };
				const float newYaw{ viewAngles->y + static_cast<float>((yaw - viewAngles->y) * t) };
				viewAngles->x = newPitch;
				viewAngles->y = newYaw;

				Sleep(delayMs);
			}
		}
		else if (pitch >= -89 && pitch <= 89 && yaw >= -180 && yaw <= 180)
		{
			constexpr int steps{ 100 }; // Number of steps for interpolation
			constexpr int delayMs{ 3 }; // Delay in milliseconds between steps

			const POINT startingPoint{viewAngles->x, viewAngles->y};
			const POINT destinationPoint{ pitch, yaw };
			const POINT midPoint{ startingPoint.x + (destinationPoint.x - startingPoint.x) / abs(random_number), destinationPoint.y + 3 * random_number };
			
			for (int i{ 0 }; i <= steps /*&& enemyInSight.load()*/; i++)
			{
				// 't' represents the interpolation parameter ranging from 0 to 1.
				// It indicates the relative position between the start and end points:
				// - At t = 0, the position is at the start.
				// - At t = 1, the position is at the end.
				// - At 0 < t < 1, the position is proportionally between the start and end.
				float t{ static_cast<float>(i) / static_cast<float>(steps) };
				
				const POINT bezierPoint{ CalculateBezierPoint(t, startingPoint, midPoint, destinationPoint) };

				viewAngles->x = bezierPoint.x;
				viewAngles->y = bezierPoint.y;
				
				Sleep(delayMs);
			}

			/*viewAngles->x = pitch;
			viewAngles->y = yaw;*/
		}
		std::cout << "\t[*] One Thread finished execution\n";
		existsWorkingThread.store(false);
	}

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

	void AimAt() {
		//DoWork(target);
		//while (existsWorkingThread.load()) {}
		if (existsWorkingThread.load() == false)
		{
			existsWorkingThread.store(true);
			std::thread worker([this] { DoWork(); });
			worker.detach(); // Optionally detach the new threaded task if you're sure it will finish safely
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