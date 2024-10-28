#include "includes.h"

namespace UnityObj {
	Unity::CGameObject* camObj;
	Unity::CGameObject* playerObj;
	Unity::CGameObject* coreObj;
};

namespace UnityVars {
	Unity::CCamera* cam;
	Unity::CGameObject* player;
	Unity::CComponent* PLH;

	Unity::il2cppArray<uintptr_t>* entityList;
};

namespace Addresses {
	uintptr_t base;
	uintptr_t GameAssembly;
};

namespace vars {
	static UINT vps = 1;
	D3D11_VIEWPORT viewport;
	Unity::Vector2 screenSize;
	Unity::Vector2 screenCenter;
	uintptr_t controll;

	bool initil2cpp = false;

	float maxDistance = 50;

	int playerIndex = 0;

	bool isEspEnabled = true;
	bool isSnaplineEnabled = true;
	ImVec4 snaplineColor;
	ImVec4 espColor;

	bool isFovChanger = false;
	bool isFovReset = false;
	float fov = 65.f;

	bool isTeamCheck = true;

	int localTeam = -1;
	float entityHeight = 2.58f;

	float aimbotFov = 10.f;
	bool isAimbot = true;
	float maxAimbotDistance = 60.f;
	float lerp = 1.f;

	bool isNames = true;
};