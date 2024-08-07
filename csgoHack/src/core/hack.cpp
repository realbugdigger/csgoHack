#include "includes.h"

Hack::~Hack() {
	this->FontF->Release();
	this->LineL->Release();
}

void Hack::Init() {
	client = (uintptr_t)GetModuleHandle(L"client.dll");
	server = (uintptr_t)GetModuleHandle(L"server.dll");
	engine = (uintptr_t)GetModuleHandle(L"engine.dll");

	entList = (EntList*)(client + dwEntityList);
	localEnt = entList->ents[0].ent;
}

void Hack::Update() {
	memcpy(&viewMatrix, (PBYTE*)(client + dwViewMatrix), sizeof(viewMatrix));

	this->CheckButtons();
}

void Hack::CheckButtons() {
	if (GetAsyncKeyState(button.showMenuBtn) & 1)
		settings.showMenu = !settings.showMenu;
	if (GetAsyncKeyState(button.snaplinesBtn) & 1)
		settings.snaplines = !settings.snaplines;
	if (GetAsyncKeyState(button.box2DBtn) & 1)
		settings.box2D = !settings.box2D;
	if (GetAsyncKeyState(button.statusBtn) & 1)
		settings.status = !settings.status;
	if (GetAsyncKeyState(button.statusTextBtn) & 1)
		settings.statusText = !settings.statusText;
	if (GetAsyncKeyState(button.rcsCrosshairBtn) & 1)
		settings.rcsCrosshair = !settings.rcsCrosshair;
	if (GetAsyncKeyState(button.aimbotBtn) & 1)
		settings.aimbot = !settings.aimbot;
	if (GetAsyncKeyState(button.triggerbotBtn) & 1)
		settings.triggerbot = !settings.triggerbot;
	if (GetAsyncKeyState(button.boneEspBtn) & 1)
		settings.boneEsp = !settings.boneEsp;
	if (GetAsyncKeyState(button.nadePredictBtn) & 1)
		settings.nadePrediction = !settings.nadePrediction;
}

bool Hack::CheckValidEnt(Ent* ent) {
	if (ent == nullptr)
		return false;
	if (ent == localEnt)
		return false;
	if (ent->iHealth <= 0)
		return false;
	if (ent->isDormant)
		return false;
	return true;
}

bool Hack::WorldToScreen(Vec3 pos, Vec2& screen) {
	Vec4 clipCoords;
	clipCoords.x = pos.x * viewMatrix[0] + pos.y * viewMatrix[1] + pos.z * viewMatrix[2] + viewMatrix[3];
	clipCoords.y = pos.x * viewMatrix[4] + pos.y * viewMatrix[5] + pos.z * viewMatrix[6] + viewMatrix[7];
	clipCoords.z = pos.x * viewMatrix[8] + pos.y * viewMatrix[9] + pos.z * viewMatrix[10] + viewMatrix[11];
	clipCoords.w = pos.x * viewMatrix[12] + pos.y * viewMatrix[13] + pos.z * viewMatrix[14] + viewMatrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	Vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

Vec3 Hack::GetBonePos(Ent* ent, int bone) {
	uintptr_t bonePtr = ent->boneMatrix;
	Vec3 bonePos;
	bonePos.x = *(float*)(bonePtr + 0x30 * bone + 0x0C);
	bonePos.y = *(float*)(bonePtr + 0x30 * bone + 0x1C);
	bonePos.z = *(float*)(bonePtr + 0x30 * bone + 0x2C);
	return bonePos;
}