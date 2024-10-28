#include "SDK.hpp"
#include "Utils/Offsets.hpp"
#include "Hacks/ESP.hpp"
#include "Hacks/Aimbot.hpp"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

FILE* fp;
HMODULE myModule;

bool running = true;
bool showImGuiWindow = true;
bool isInMatch = false;

bool GetObjects() {
    UnityObj::camObj = Unity::GameObject::Find("Camera");
    if (UnityObj::camObj) {
        UnityVars::cam = (Unity::CCamera*)UnityObj::camObj->GetComponent("Camera");
    }
    else { return false; }

    UnityObj::playerObj = Unity::GameObject::Find("Controll");
    if (UnityObj::playerObj) {
        UnityVars::player = UnityObj::playerObj;
    }
    else { return false; }

    UnityObj::coreObj = Unity::GameObject::Find("Core");
    if (UnityObj::coreObj) {
        UnityVars::PLH = UnityObj::coreObj->GetComponent("PLH");
    }
    else { return false; }

    UnityVars::entityList = (Unity::il2cppArray<uintptr_t>*)ReadChain(Addresses::GameAssembly, Offsets::entityListOffsets);
    vars::controll = ReadChain(Addresses::GameAssembly, Offsets::controllOffsets);

    if (!UnityVars::entityList || !vars::controll)
        return false;

    return true;
}

void UpdateFOV() {
    if (UnityVars::cam) {
        if (vars::isFovChanger) {
            UnityVars::cam->SetFieldOfView(vars::fov);
            vars::isFovReset = false;
        }
        else if (!vars::isFovReset) {
            UnityVars::cam->SetFieldOfView(65.0f);
        }
    }
}

void MainLoop() {
    if (GetObjects())
    {
        void* m_pThisThread = IL2CPP::Thread::Attach(IL2CPP::Domain::Get());
        Unity::Vector3 playerPos = UnityVars::player->GetTransform()->GetMemberValue<Unity::Vector3>("position");

        UpdateFOV();

        int pcount = UnityVars::PLH->GetMemberValue<int>("pcount");
        if (pcount <= 0) return;

        for (int i = 0; i < pcount; i++) 
        {
            uintptr_t playerData = UnityVars::entityList->At(i);
            if (!playerData) continue;

            int hp = *(int*)(playerData + Offsets::health);
            if (hp <= 0) continue;

            Unity::Vector3 entityPos;
            memcpy(&entityPos, reinterpret_cast<void*>(playerData + Offsets::pos), sizeof(Unity::Vector3));

            if (Distance3D(playerPos, entityPos) >= vars::maxDistance) continue;

            if (vars::isTeamCheck) {
                if (vars::localTeam == -1 && *(bool*)(playerData + Offsets::isMainPlayer)) {
                    vars::localTeam = *(int*)(playerData + Offsets::team);
                }

                int enemyTeam = *(int*)(playerData + Offsets::team);
                //std::cout << "Local Team: " << vars::localTeam << " | Enemy Team: " << enemyTeam << std::endl;
                if (vars::localTeam == enemyTeam) {
                    continue;
                }
            }

            uintptr_t* entityNamePtr = (uintptr_t*)(playerData + Offsets::name);
            Unity::System_String* UEntityName = (Unity::System_String*)*entityNamePtr;
            //std::cout << UEntityName->ToString() << std::endl;
            std::string entityName = UEntityName->ToString();

            entityPos.y -= 2.08;

            if (vars::isAimbot && GetAsyncKeyState(VK_LMENU))
            {
                float currentYaw = *(float*)(vars::controll + Offsets::yaw);
                float targetYaw = GetYaw(playerPos, entityPos);

                float currentPitch = *(float*)(vars::controll + Offsets::pitch);
                float targetPitch = GetPitch(playerPos, entityPos);

                float yawDiff = targetYaw - currentYaw;
                float pitchDiff = targetPitch - currentPitch;

                if (std::abs(yawDiff) < vars::aimbotFov && std::abs(pitchDiff) < vars::aimbotFov && Distance3D(playerPos, entityPos) < vars::maxAimbotDistance)
                {
                    float smoothYaw = Lerp(*(float*)(vars::controll + Offsets::yaw), GetYaw(playerPos, entityPos), vars::lerp);
                    float smoothPitch = Lerp(*(float*)(vars::controll + Offsets::pitch), GetPitch(playerPos, entityPos), vars::lerp);
                    *(float*)(vars::controll + Offsets::yaw) = smoothYaw;
                    *(float*)(vars::controll + Offsets::pitch) = smoothPitch;
                }
            }

            Unity::Vector2 screenPos;
            if (ESP::WorldToScreen(entityPos, screenPos)) {
                ESP::Draw(screenPos, entityPos, entityName);
            }
        }

        IL2CPP::Thread::Detach(m_pThisThread);
    }
}

void InitImGui() {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

    const char* fontPath = "C:/Windows/Fonts/Arial.ttf";
    float fontSize = 16.0f;

    io.Fonts->AddFontFromFileTTF(fontPath, fontSize, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return true;
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    if (!init) {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice))) {
            pDevice->GetImmediateContext(&pContext);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            window = sd.OutputWindow;
            ID3D11Texture2D* pBackBuffer;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
            pBackBuffer->Release();
            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            InitImGui();
            init = true;
        }
        else {
            return oPresent(pSwapChain, SyncInterval, Flags);
        }
    }

    pContext->RSGetViewports(&vars::vps, &vars::viewport);
    vars::screenSize = { vars::viewport.Width, vars::viewport.Height };
    vars::screenCenter = { vars::viewport.Width / 2.0f, vars::viewport.Height / 2.0f };

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (showImGuiWindow) {
        ImGui::Begin("Blockpost cheat - Made by Einhar");
        ImGui::SetWindowSize(ImVec2(400, 300));

        if (ImGui::BeginTabBar("MyTabBar"))
        {
            if (ImGui::BeginTabItem("ESP"))
            {
                ImGui::SliderFloat("Max distance", &vars::maxDistance, 0.0f, 600.f);

                ImGui::Checkbox("Team Check", &vars::isTeamCheck);

                ImGui::Checkbox("Snaplines", &vars::isSnaplineEnabled);

                ImGui::Text("Snapline Color");
                ImGui::ColorEdit4("Snapline Color", (float*)&vars::snaplineColor);

                ImGui::Checkbox("Boxes", &vars::isEspEnabled);

                ImGui::Text("ESP Color");
                ImGui::ColorEdit4("ESP Color", (float*)&vars::espColor);

                ImGui::Checkbox("Names", &vars::isNames);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Aimbot"))
            {
                ImGui::Checkbox("Aimbot", &vars::isAimbot);
                ImGui::SliderFloat("Max distance", &vars::maxAimbotDistance, 1.0f, 600.0f);
                ImGui::SliderFloat("FOV", &vars::aimbotFov, 1.0f, 90.0f);
                //ImGui::SliderFloat("Lerp factor", &vars::lerp, 0.1f, 1.f);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Misc"))
            {
                ImGui::Checkbox("FOV changer", &vars::isFovChanger);
                ImGui::SliderFloat("FOV", &vars::fov, 20.0f, 180.0f);

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }        

        ImGui::End();
    }

    MainLoop();
    ImGui::Render();

    if (GetAsyncKeyState(VK_F2) & 1) {
        showImGuiWindow = !showImGuiWindow;
    }

    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (!running) {
        kiero::shutdown();
        pDevice->Release();
        pContext->Release();
        pSwapChain->Release();
        SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
        return 0;
    }

    return oPresent(pSwapChain, SyncInterval, Flags);
}

void Initialize() {
    AllocConsole();
    freopen_s(&fp, "CONOUT$", "w", stdout);

    IL2CPP::Initialize();
    vars::initil2cpp = true;

    Addresses::base = (uintptr_t)GetModuleHandle(nullptr);
    Addresses::GameAssembly = (uintptr_t)GetModuleHandleA("GameAssembly.dll");

    std::cout << "Blockpost cheat loaded - Made by Einhar" << std::endl;

    //Log(Addresses::base, "Base");
    //Log(Addresses::GameAssembly, "GameAssembly.dll");

    GetObjects();

    vars::snaplineColor = ImVec4(144 / 255.0f, 142 / 255.0f, 255 / 255.0f, 1.0f);
    vars::espColor = ImVec4(144 / 255.0f, 142 / 255.0f, 255 / 255.0f, 1.0f);
}

DWORD WINAPI UnloadThread(LPVOID lpParameter) {
    Sleep(100);
    FreeLibraryAndExitThread(myModule, 0);
    return 0;
}

DWORD WINAPI MainThread(LPVOID lpReserved) {
    bool init_hook = false;
    while (!init_hook) {
        if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
            Initialize();
            kiero::bind(8, (void**)&oPresent, hkPresent);
            init_hook = true;
        }
    }

    while (running) {
        if (GetAsyncKeyState(VK_F1) & 1) {
            fclose(fp);
            FreeConsole();
            running = false;

            Sleep(100);
            CreateThread(nullptr, 0, UnloadThread, myModule, 0, nullptr);
        }
        Sleep(50);
    }

    return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        myModule = hMod;
        DisableThreadLibraryCalls(hMod);
        CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        kiero::shutdown();
        break;
    }
    return TRUE;
}