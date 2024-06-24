#pragma warning( disable : 4996)

#include "includes.h"

extern Hack* hack;

// filled rectangle
void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color) {
	D3DRECT rect = { x,y,x + w,y + h };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color) {

	if (!hack->LineL)
		D3DXCreateLine(pDevice, &hack->LineL);

	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(x1, y1);
	Line[1] = D3DXVECTOR2(x2, y2);
	hack->LineL->SetWidth(thickness);
	hack->LineL->Draw(Line, 2, color);
}

void DrawLine(Vec2 src, Vec2 dst, int thickness, D3DCOLOR color) {
	DrawLine(src.x, src.y, dst.x, dst.y, thickness, color);
}

void DrawEspBox2D(Vec2 top, Vec2 bot, int thickness, D3DCOLOR color) {
	int height = ABS(top.y - bot.y);
	Vec2 tl, tr;
	tl.x = top.x - height / 4;
	tr.x = top.x + height / 4;
	tl.y = tr.y = top.y;

	Vec2 bl, br;
	bl.x = bot.x - height / 4;
	br.x = bot.x + height / 4;
	bl.y = br.y = bot.y;

	DrawLine(tl, tr, thickness, color);
	DrawLine(bl, br, thickness, color);
	DrawLine(tl, bl, thickness, color);
	DrawLine(tr, br, thickness, color);
}

void drawBone(Vec3 Bone1, Vec3 Bone2, D3DCOLOR color) {
	Vec2 vScreenBone1;
	Vec2 vScreenBone2;
	if (hack->WorldToScreen(Bone1, vScreenBone1))
	{
		if (hack->WorldToScreen(Bone2, vScreenBone2))
		{
			DrawLine(vScreenBone1.x, vScreenBone1.y, vScreenBone2.x, vScreenBone2.y, 3, color);
			//Drawing::Line(vScreenBone1.x, vScreenBone1.y, vScreenBone2.x, vScreenBone2.y, D3DCOLOR_ARGB(BoneColorA, BoneColorR, BoneColorG, BoneColorB));
		}
	}
}

void DrawBoneEsp(Ent* ent, D3DCOLOR color) {
	//std::cout << "[!!!] Drawing ?!?!?!?!?!\n";
	//DrawString(Bonename, vScreenBone.x, vScreenBone.y, 255, 255, 0, 0, DirectX.espFont);
	/*for (int i = 8; i < 120; i++)
	{
		Vec3 vec3Bone = hack->GetBonePos(ent, i);
		Vec2 screenBone;
		if (hack->WorldToScreen(vec3Bone, screenBone))
		{
			char text[3];
			snprintf(text, sizeof(text), "%d", i);
			DrawText(text, screenBone.x, screenBone.y, D3DCOLOR_ARGB(255, 255, 0, 0));
		}
	}*/

	// head
	drawBone(hack->GetBonePos(ent, 8), hack->GetBonePos(ent, 10), color);
	drawBone(hack->GetBonePos(ent, 8), hack->GetBonePos(ent, 39), color);
	
	// left arm
	drawBone(hack->GetBonePos(ent, 10), hack->GetBonePos(ent, 11), color);
	drawBone(hack->GetBonePos(ent, 11), hack->GetBonePos(ent, 12), color);
	drawBone(hack->GetBonePos(ent, 12), hack->GetBonePos(ent, 13), color);
	
	// right arm
	drawBone(hack->GetBonePos(ent, 10), hack->GetBonePos(ent, 40), color);
	drawBone(hack->GetBonePos(ent, 40), hack->GetBonePos(ent, 41), color);
	drawBone(hack->GetBonePos(ent, 41), hack->GetBonePos(ent, 42), color);
	
	// torso
	drawBone(hack->GetBonePos(ent, 10), hack->GetBonePos(ent, 68), color);
	drawBone(hack->GetBonePos(ent, 68), hack->GetBonePos(ent, 81), color);
	
	// palvis
	drawBone(hack->GetBonePos(ent, 81), hack->GetBonePos(ent, 72), color);
	
	// right leg
	drawBone(hack->GetBonePos(ent, 81), hack->GetBonePos(ent, 86), color);
	drawBone(hack->GetBonePos(ent, 86), hack->GetBonePos(ent, 87), color);
	drawBone(hack->GetBonePos(ent, 87), hack->GetBonePos(ent, 82), color);
	drawBone(hack->GetBonePos(ent, 82), hack->GetBonePos(ent, 83), color);
	
	// left leg
	drawBone(hack->GetBonePos(ent, 72), hack->GetBonePos(ent, 77), color);
	drawBone(hack->GetBonePos(ent, 77), hack->GetBonePos(ent, 78), color);
	drawBone(hack->GetBonePos(ent, 78), hack->GetBonePos(ent, 73), color);
	drawBone(hack->GetBonePos(ent, 73), hack->GetBonePos(ent, 74), color);
}

void DrawText(const char* text, float x, float y, D3DCOLOR color) {
	RECT rect;

	if (!hack->FontF)
		D3DXCreateFont(pDevice, 14, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &hack->FontF);

	SetRect(&rect, x + 1, y + 1, x + 1, y + 1);
	hack->FontF->DrawTextA(NULL, text, -1, &rect, DT_CENTER | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));

	SetRect(&rect, x, y, x, y);
	hack->FontF->DrawTextA(NULL, text, -1, &rect, DT_CENTER | DT_NOCLIP, color);
}