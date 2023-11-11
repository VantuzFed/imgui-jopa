#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <cmath>
#include "guidev.h"
#include "Theme/Theme.h"
#include <windows.h>
#include "imgui/imgui_impl_win32.h"
#include <tlhelp32.h>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

bool reset_size = true;
bool ncmenu = true;
bool DebugMod = false;
static bool previousDebugMod = false;

bool TerminateProcessByName(const WCHAR* processName) {
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);

    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes) {
        if (wcscmp(pEntry.szExeFile, processName) == 0) {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
            if (hProcess != NULL) {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
                CloseHandle(hSnapShot);
                return true;
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
    return false;
}

int tabs = 1;
Theme theme;
std::vector<std::string> logs;

void AddLog(const std::string& log) {
    // Получение текущего времени
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    // Безопасное преобразование в местное время
    std::tm localTime;
    localtime_s(&localTime, &now_c);

    // Форматирование времени
    std::ostringstream timeStream;
    timeStream << std::put_time(&localTime, "[%H:%M:%S] ");

    // Добавление временной метки к логу
    logs.push_back(timeStream.str() + log);
}



int main()
{


    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(console, 1);

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("NEVERPASTE"), NULL };
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, _T("NEVERPASTE"), WS_OVERLAPPEDWINDOW, 0, 0, 50, 50, NULL, NULL, wc.hInstance, NULL);
    


    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    

    ShowWindow(GetConsoleWindow(), SW_HIDE);
    UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    theme.Register(io);

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    bool done = true;

    std::string str_window_title = "NEVERPASTE - imgui eto jopa";
    const char* window_title = str_window_title.c_str();
    ImVec2 window_size{ 690, 450 };

    DWORD window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    while (done)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = false;
        }
        if (!done)
            break;

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (ncmenu)
        {
            if (reset_size)
            {
                ImGui::SetNextWindowSize(window_size);
                reset_size = false;
            }

            



            ImGui::PushFont(theme.font);
            ImGui::Begin(window_title, &ncmenu, window_flags);

            

            static int selectedTab = 0;
            ImGui::BeginChild("Tabs", ImVec2(150, 0), true);
            if (ImGui::Selectable("Main", selectedTab == 0)) selectedTab = 0;
            if (ImGui::Selectable("Misc", selectedTab == 1)) selectedTab = 1;
            if (ImGui::Selectable("Steam", selectedTab == 4)) selectedTab = 4;
            if (DebugMod) {
                if (ImGui::Selectable("Debug", selectedTab == 2)) selectedTab = 2;
            }
            if (ImGui::Selectable("Logs", selectedTab == 3)) selectedTab = 3;
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginGroup();
            switch (selectedTab)
            {
            case 0:
                ImGui::Text("browser");
                ImGui::Separator();
                if (ImGui::Button("firefox")) {
                    AddLog("try to open firefox");
                    system("\"C:\\Program Files\\Mozilla Firefox\\firefox.exe\"");
                }
                ImGui::SameLine();
                if (ImGui::Button("chatgpt")) {
                    AddLog("chat gpt started");
                    system("\"C:\\Program Files\\Mozilla Firefox\\firefox.exe\" https://chat.openai.com/");
                }
                ImGui::SameLine();

                if (ImGui::Button("repositories")) {
                    AddLog("github repos");
                    system("\"C:\\Program Files\\Mozilla Firefox\\firefox.exe\" https://github.com/nkplka?tab=repositories");
                }
                ImGui::Separator();
                ImGui::Text("discord");
                ImGui::Separator();
                if (ImGui::Button("start")) {
                    AddLog("discord started");
                    system("\"C:\\Users\\nkpl\\AppData\\Local\\Discord\\app-1.0.9023\\Discord.exe");
                }
                ImGui::SameLine();
                
                if (ImGui::Button("close")) {
                    TerminateProcessByName(L"discord.exe");
                    AddLog("discord closed");
                }
                ImGui::SameLine();
                if (ImGui::Button("restart")) {
                    TerminateProcessByName(L"discord.exe");
                    system("\"C:\\Users\\nkpl\\AppData\\Local\\Discord\\app-1.0.9023\\Discord.exe");
                    AddLog("discord restarted");
                }
                ImGui::Separator();
                break;
            case 1:
                ImGui::Checkbox("Debug Mod", &DebugMod);
                if (DebugMod != previousDebugMod) {
                    if (DebugMod) {
                        AddLog("DebugMod enabled");
                    }
                    else {
                        AddLog("DebugMod disabled");
                    }
                    previousDebugMod = DebugMod;
                }
                break;
            case 2:
                if (ImGui::Button("source code")) {
                    AddLog("github repos");
                    system("\"C:\\Program Files\\Mozilla Firefox\\firefox.exe\" https://github.com/nkplka/imgui-jopa");
                }
                break;
            case 3:
                ImGui::Text("Logs");
                ImGui::Separator();
                for (const auto& log : logs) {
                    ImGui::TextUnformatted(log.c_str());
                }
                break;

            case 4:
                if (ImGui::Button("close")) {
                    if (TerminateProcessByName(L"steam.exe")) {
                        AddLog("Steam process terminated successfully.");
                    }
                    else {
                        AddLog("Failed to terminate Steam process.");
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("strart")) {
                        ShellExecute(NULL, L"open", L"C:\\Program Files (x86)\\Steam\\steam.exe", NULL, NULL, SW_SHOWNORMAL);
                        AddLog("steam started");
                    }

                
                ImGui::SameLine();
                if (ImGui::Button("restrart")) {
                    if (TerminateProcessByName(L"steam.exe")) {
                        ShellExecute(NULL, L"open", L"C:\\Program Files (x86)\\Steam\\steam.exe", NULL, NULL, SW_SHOWNORMAL);
                        AddLog("steam restarted");
                    }
                    
                }
                
                break;
                break;

            }

           
            ImGui::PopFont();
            ImGui::End();
        }
        else
        {
            exit(0);
        }

        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, NULL, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}