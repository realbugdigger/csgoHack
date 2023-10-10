#include "aimbot.h"

extern Hack* hack;

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
		if (currentDistance < closestDitance)
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
		Vec3 temp = hack->GetBonePos(closestEnemy, 8);
		hack->localEnt->AimAt( &temp );
	}
}