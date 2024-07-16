#include "includes.h"
#include "../valve/serverCom.h"
#include "../valve/weapons.h"
//#include "concurency.h"
#include <sstream>
#include <string.h>
#include <iomanip>
#include <cstdint>

// data
void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;
extern LPDIRECT3DDEVICE9 pDevice = nullptr;
Hack* hack;

// capture an individual interface by name & module
template <typename Interface>
Interface* Capture(const char* moduleName, const char* interfaceName) noexcept
{
	const HINSTANCE handle = GetModuleHandleA(moduleName);

	if (!handle)
		return nullptr;

	// get the exported Createinterface function
	using CreateInterfaceFn = Interface * (__cdecl*)(const char*, int*);
	const CreateInterfaceFn createInterface =
		reinterpret_cast<CreateInterfaceFn>(GetProcAddress(handle, "CreateInterface"));

	// return the interface pointer by calling the function
	return createInterface(interfaceName, nullptr);
}

// hook function
void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice) {
	if (!pDevice)
		pDevice = o_pDevice;

	// drawing stuff

	int menuOffX = windowWidth / 2;
	int menuOffY = 100;
	D3DCOLOR enabled = D3DCOLOR_ARGB(255, 0, 255, 0);
	D3DCOLOR disabled = D3DCOLOR_ARGB(255, 255, 0, 0);

	if (!hack->settings.showMenu) {
		DrawText("Show Menu			(INS)",		 menuOffX, menuOffY, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	else {
		DrawText("Snaplines			[NUMPAD 1]", menuOffX, menuOffY + 1 * 12, hack->settings.snaplines ? enabled : disabled);
		DrawText("2D Box			[NUMPAD 2]", menuOffX, menuOffY + 2 * 12, hack->settings.box2D ? enabled : disabled);
		DrawText("Statusbars		[NUMPAD 3]", menuOffX, menuOffY + 3 * 12, hack->settings.status ? enabled : disabled);
		DrawText("Status Text		[NUMPAD 4]", menuOffX, menuOffY + 4 * 12, hack->settings.statusText ? enabled : disabled);
		DrawText("Recoil crosshair	[NUMPAD 5]", menuOffX, menuOffY + 5 * 12, hack->settings.rcsCrosshair ? enabled : disabled);
		DrawText("Aimbot			[NUMPAD 6]", menuOffX, menuOffY + 6 * 12, hack->settings.aimbot ? enabled : disabled);
		DrawText("Triggerbot		[NUMPAD 7]", menuOffX, menuOffY + 7 * 12, hack->settings.triggerbot ? enabled : disabled);
		DrawText("Bone Esp			[NUMPAD 8]", menuOffX, menuOffY + 8 * 12, hack->settings.boneEsp ? enabled : disabled);
		DrawText("Hide Menu			(INS)",		 menuOffX, menuOffY + 9 * 12, D3DCOLOR_ARGB(255, 255, 255, 255));
	}


	for (int i = 1; i < 32; i++) {
		Ent* curEnt = hack->entList->ents[i].ent;
		if (!hack->CheckValidEnt(curEnt))
			continue;


		D3DCOLOR espColor, snaplineColor, velocityColor, headlineColor;
		if (curEnt->iTeamNum == hack->localEnt->iTeamNum) {
			espColor = hack->color.team.esp;
			snaplineColor = hack->color.team.snapline;
		}
		else {
			espColor = hack->color.enemy.esp;
			snaplineColor = hack->color.enemy.snapline;
		}

		Vec3 entHead3D = hack->GetBonePos(curEnt, 8);
		entHead3D.z += 8;
		Vec2 entPos2D, entHead2D;
		
		if (hack->WorldToScreen(curEnt->vecOrigin, entPos2D)) {
			
			if (hack->settings.snaplines) {
				// snapline
				DrawLine(entPos2D.x, entPos2D.y, windowWidth / 2, windowHeight, 2, snaplineColor);
			}

			if (hack->WorldToScreen(entHead3D, entHead2D)) {

				if (hack->settings.box2D) {
					DrawEspBox2D(entPos2D, entHead2D, 2, espColor);
				}

				if (hack->settings.status) {
					int height = ABS(entPos2D.y - entHead2D.y);
					int dX = (entPos2D.x - entHead2D.x);

					float healthPerc = curEnt->iHealth / 100.f;
					float armorPerc = curEnt->ArmorValue / 100.f;

					Vec2 botHealth, topHealth, botArmor, topArmor;
					int healthHeight = height * healthPerc;
					int armorHeight = height * armorPerc;

					botHealth.y = botArmor.y = entPos2D.y;

					botHealth.x = entPos2D.x - (height / 4) - 2;
					botArmor.x = entPos2D.x + (height / 4) + 2;

					topHealth.y = entHead2D.y + height - healthHeight;
					topArmor.y = entHead2D.y + height - armorHeight;

					topHealth.x = entPos2D.x - (height / 4) - 2 - (dX * healthPerc);
					topArmor.x = entPos2D.x + (height / 4) + 2 - (dX * armorPerc);

					DrawLine(botHealth, topHealth, 4, hack->color.health);
					DrawLine(botArmor, topArmor, 4, hack->color.armor);
				}

				if (hack->settings.statusText) {
					std::string t1 = "RandomNameForNow";
					char* name = (char*)t1.c_str();

					DrawText(name, entPos2D.x, entPos2D.y, D3DCOLOR_ARGB(255, 255, 255, 255));
				}

				if (hack->settings.boneEsp) {
					DrawBoneEsp(curEnt, espColor);
				}
			}
		}


		//IPlayerInfoManager* playerinfomanager = Capture<IPlayerInfoManager>("server.dll", "PlayerInfoManager002");
		//std::cout << "\t player info manager: " << playerinfomanager << "\n\n";
		//IVEngineServer* engine = Capture<IVEngineServer>("engine.dll", "VEngineServer023");
		//std::cout << "\t VEngineServer023: " << engine << "\n\n";
		//engine->IsInternalBuild();
		//if (playerinfomanager)
		//{
		//	std::cout << "\t TESTING... \n\n";
		//	//engine->GetGameDir();
		//	std::cout << "Entity count in game : " << engine->GetEntityCount() << std::endl;
		//	edict_t* edict = engine->PEntityOfEntIndex(i);
		//	edict_t* edict4 = engine->PEntityOfEntIndex(3);
		//	edict_t* edict7 = engine->PEntityOfEntIndex(6);
		//	edict_t* edict9 = engine->PEntityOfEntIndex(8);
		//	std::cout << "\t edict: " << edict << "\n\n";
		//	std::cout << "\t edict: " << edict4 << "\n\n";
		//	std::cout << "\t edict: " << edict7 << "\n\n";
		//	std::cout << "\t edict: " << edict9 << "\n\n";
		//	std::cout << "\t TESTING 2... \n\n";
		//	IPlayerInfo* player = playerinfomanager->GetPlayerInfo(edict); // For each player, retrieve player object
		//	std::cout << "\t TESTING 3... \n\n";

		//	// Access other data in loop as desired
		//	std::cout << "[*] Player name: " << player->GetName() << std::endl;
		//	std::cout << "[*] Used weapon: " << player->GetWeaponName() << std::endl;
		//}

		// Read the name this way !!!
		/*uintptr_t clientState = *(uintptr_t*)(hack->engine + 0x59F19C);
		uintptr_t user_info_table = *(uintptr_t*)(clientState + 0x52C0);
		uintptr_t x = *(uintptr_t*)((*(uintptr_t*)(user_info_table + 0x40))+0xC);
		player_info_t p = *(player_info_t*)(*(uintptr_t*)(x + 0x28 + 0x34 * (i - 1)));
		std::cout << p.name << std::endl;*/



		// Player current weapon
		uintptr_t currWeaponAddress = (uintptr_t)curEnt + 0x2F08;
		unsigned int weaponId = *(int*)currWeaponAddress;
		unsigned int currWpnId = weaponId & 0xfff;
		uintptr_t iBase = *(uintptr_t*)(hack->client + 0x4E051DC + (currWpnId - 1) * 0x10);
		uintptr_t weaponIdLocation = iBase + 0x2FBA;
		int id = *(short*)(iBase + 0x2FBA);
		std::cout << "Current weapon is " << getWeapon(id) << std::endl;
	}

	// crosshair
	if (hack->settings.rcsCrosshair) {
		hack->crosshair2D.x = windowWidth / 2;
		hack->crosshair2D.y = windowHeight / 2;

		Vec2 l, r, t, b;
		l = r = t = b = hack->crosshair2D;
		l.x -= hack->crosshairSize;
		r.x += hack->crosshairSize;
		b.y += hack->crosshairSize;
		t.y -= hack->crosshairSize;

		DrawLine(l, r, 2, hack->color.crosshair);
		DrawLine(t, b, 2, hack->color.crosshair);
	}

	if (hack->settings.aimbot) {
		Aimbot();
	}

	if (hack->settings.triggerbot) {
		Triggerbot();
	}

	// call og function
	oEndScene(pDevice);
}

DWORD WINAPI HackThread(HMODULE hModule) {
	globalPosPtr = std::make_shared<Vec3>();

	//Create Debugging Console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	std::cout << "[*] Debugging Console Started\n";

	// hook
	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {
		memcpy(EndSceneBytes, (char*)d3d9Device[42], 7);

		oEndScene = (tEndScene)TrampHook((char*)d3d9Device[42], (char*)hkEndScene, 7);
	}

	hack = new Hack();
	hack->Init();

	// hack loop
	while (!GetAsyncKeyState(VK_END)) {
		hack->Update();

		Vec3 pAng = hack->localEnt->aimPunchAngle;
		hack->crosshair2D.x = windowWidth / 2 - (windowWidth / 90 * pAng.y);
		hack->crosshair2D.y = windowHeight / 2 - (windowHeight / 90 * pAng.x);
	}

	// unhook
	Patch((BYTE*)d3d9Device[42], EndSceneBytes, 7);

	fclose(f);
	FreeConsole();

	// uninject
	FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpr) {
	if (reason == DLL_PROCESS_ATTACH)
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0));
	return TRUE;
}