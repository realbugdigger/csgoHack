#include <thread>

#include "aimbot.h"
//#include "concurency.h"

#include "../valve/csgosdk.h"
#include "../valve/csgoVector.h"
#include "../valve/csgotrace.h"

extern Hack* hack;

uintptr_t clientdll = (uintptr_t)GetModuleHandle(L"client.dll");
uintptr_t enginedll = (uintptr_t)GetModuleHandle(L"engine.dll");

tTraceRay TraceRay;

typedef void* (__cdecl* tCreateInterface)(const char* name, int* pReturnCode);

void* GetInterface(tCreateInterface fn, const char* name)
{
	return (fn)(name, 0);
}

tCreateInterface CreateInterface = (tCreateInterface)GetProcAddress((HMODULE)enginedll, "CreateInterface");

IEngineTrace* EngineTrace = (IEngineTrace*)GetInterface(CreateInterface, "EngineTraceClient004");

static SDKVec3 Vec2SDKVec(Vector3 vec3) {
	return SDKVec3(vec3.x, vec3.y, vec3.z);
}

static bool isVisible(Ent* player) {
	SDKVec3 eyepos = Vec2SDKVec(hack->localEnt->vecOrigin + hack->localEnt->m_vecViewOffset);
	SDKVec3 targeteyepos = Vec2SDKVec(player->vecOrigin + player->m_vecViewOffset);

	CGameTrace trace;
	Ray_t ray;
	CTraceFilter tracefilter;
	tracefilter.pSkip = (void*)hack->localEnt;

	ray.Init(eyepos, targeteyepos);

	EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &tracefilter, &trace);

	if (player == trace.hit_entity)
	{
		return true;
	}
	return false;
}

static Ent* GetClosestEnemy()
{
	float closestDitance = 1000000;
	int closesDistanceIndex = -1;

	Ent* currentPlayer = NULL;

	for (int i = 1; i < 32; i++)
	{
		currentPlayer = hack->entList->ents[i].ent;

		if (!currentPlayer || currentPlayer == hack->localEnt)
		{
			continue;
		}

		if (currentPlayer->iTeamNum == hack->localEnt->iTeamNum)
		{
			continue;
		}

		if (currentPlayer->iHealth < 1 || hack->localEnt->iHealth < 1)
		{
			continue;
		}

		float currentDistance = hack->localEnt->GetDistance(currentPlayer->GetOrigin());
		if (currentDistance < closestDitance && isVisible(currentPlayer))
		{
			closestDitance = currentDistance;
			closesDistanceIndex = i;
		}
	}

	if (closesDistanceIndex == -1)
	{
		return NULL;
	}
	return hack->entList->ents[closesDistanceIndex].ent;
}

void Aimbot() {
	Ent* closestEnemy = GetClosestEnemy();

	if (closestEnemy)
	{
		enemyInSight.store(true);
		//const auto temp = std::make_shared<Vec3>(hack->GetBonePos(closestEnemy, 8));
		Vec3 temp = hack->GetBonePos(closestEnemy, 8);
		{
			std::lock_guard<std::mutex> lock(mtx);
			*globalPosPtr = temp;
		}
		hack->localEnt->AimAt();
	}
	else {
		enemyInSight.store(false);
	}
}

void Triggerbot() {
	uintptr_t* localEntPtr = (uintptr_t*)(clientdll + 0xDEF97C);
	uintptr_t localEnt = *localEntPtr;
	int crosshairId = *(int*)(localEnt + 0x11838);

	if (crosshairId <= 64 && crosshairId != 0)
	{
		uintptr_t crossHairEnt = *(uintptr_t*)(clientdll + 0x4E051DC + (crosshairId - 1) * 0x10);

		if (crossHairEnt)
		{
			int crosshairTeam = *(int*)(crossHairEnt + 0xF4);
			int crosshairLifeState = *(int*)(crossHairEnt + 0x25F);

			if ((hack->localEnt->iTeamNum != crosshairTeam) && (crosshairLifeState == 0))
			{
				*(int*)(clientdll + 0x3233024) = 5;
				std::this_thread::sleep_for(std::chrono::milliseconds(30));
				*(int*)(clientdll + 0x3233024) = 4;
			}
			else *(int*)(clientdll + 0x3233024) = 4;
		}
	}
}