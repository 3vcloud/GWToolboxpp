#include "stdafx.h"

#include "TexmodWindow.h"
#include "logger.h"

#include <Modules\Resources.h>

#define BIG_BUFSIZE 1<<24
#define SMALL_BUFSIZE 1<<10

void TexmodWindow::Initialize() {
    ToolboxWindow::Initialize();
    LoadDll();
    //StartuModServer();
}
void TexmodWindow::Terminate() {
    StopuModServer();
}
void TexmodWindow::Update(float delta) {

}
void TexmodWindow::Draw(IDirect3DDevice9* pDevice) {

}
void TexmodWindow::LoadSettings(CSimpleIni* ini) {
    ToolboxWindow::LoadSettings(ini);
}
void TexmodWindow::SaveSettings(CSimpleIni* ini) {
    ToolboxWindow::SaveSettings(ini);
}
void TexmodWindow::DrawSettingInternal() {

}
void TexmodWindow::StopuModServer() {
    Log::Log("Stopping uMod Server\n");
    uMod_Server_NeedToStop = true;
    HANDLE pipe = CreateFileW(PIPE_Game2uMod,// pipe name
        GENERIC_WRITE,
        0,              // no sharing
        NULL,           // default security attributes
        OPEN_EXISTING,  // opens existing pipe
        0,              // default attributes
        NULL);          // no template file
    if (pipe != INVALID_HANDLE_VALUE) {
        const wchar_t* str = ABORT_SERVER;
        unsigned int len = 0u;
        while (str[len]) len++;
        len++; //to send also the zero
        unsigned long num;
        WriteFile(pipe, (const void*)str, len * sizeof(wchar_t), &num, NULL);
        CloseHandle(pipe);
    }    
    if (uMod_Server.joinable())
        uMod_Server.join();
    if (uMod_RemoveHook)
        uMod_RemoveHook();
    if (uMod_Injector.joinable())
        uMod_Injector.join();
    Log::Log("Stopped uMod Server\n");
}
bool TexmodWindow::LoadDll() {
    // Try to download and inject discord_game_sdk.dll for discord.
    std::wstring dll_location = L"C:\\Users\\Jon\\Dropbox\\Git Repositories\\Texmod\\uMod_DX9\\Debug\\uMod_DX9.dll";
    HINSTANCE hGetProcIDDLL = LoadLibraryW(dll_location.c_str());
    if (!hGetProcIDDLL) {
        Log::Error("Failed to find and load library for %ls", dll_location.c_str());
        return false;
    }
    uMod_InstallHook = (uMod_pt)GetProcAddress(hGetProcIDDLL, "InstallHook");
    if (!uMod_InstallHook) {
        Log::Error("Failed to find address for uMod_InstallHook");
        return false;
    }
    uMod_RemoveHook = (uMod_pt)GetProcAddress(hGetProcIDDLL, "RemoveHook");
    if (!uMod_RemoveHook) {
        Log::Error("Failed to find address for uMod_RemoveHook");
        return false;
    }
    Log::Log("%ls hooked!\n", dll_location.c_str());
    uMod_RemoveHook();
    uMod_InstallHook();
    return true;
}
void TexmodWindow::StartuModServer() {
    if (uMod_Server_Running)
        return;
    uMod_Server_Running = true;
    if (uMod_Server.joinable())
        uMod_Server.join();
    if (uMod_Injector.joinable())
        uMod_Injector.join();

    uMod_Server = std::thread([this]() {
        Log::Log("uMod Server Started\n");
        bool  fConnected = false;
        HANDLE pipe_in;
        HANDLE pipe_out;
        HANDLE CheckForSingleRun;
        char buffer[SMALL_BUFSIZE];
        std::wstring abort = ABORT_SERVER;
        CheckForSingleRun = CreateMutexW(NULL, true, L"Global\\uMod_CheckForSingleRun");
        if (ERROR_ALREADY_EXISTS == GetLastError()) {
            Log::Log("Failed to grab mutex for uMod - is it already running?");
            uMod_Server_Running = false;
            if(CheckForSingleRun) CloseHandle(CheckForSingleRun);
            return;
        }
        Log::Log("Injecting Dll...\n");
        if (!LoadDll()) {
            Log::Error("Failed to inject dll");
            uMod_Server_Running = false;
            if (CheckForSingleRun) CloseHandle(CheckForSingleRun);
            return;
        }
        
        // Remove the hook first, then wait 1 second for our server to start listening, then re-attach
        Log::Log("Removing hook\n");
        uMod_RemoveHook();
        uMod_Injector = std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::microseconds(500));
            Log::Log("Installing hook\n");
            if (uMod_InstallHook)
                uMod_InstallHook();
            });
        while (!uMod_Server_NeedToStop) {
            /*
                Beep(300,100);
                Beep(600,100);
            */
            Log::Log("Listening for pipe\n");
            pipe_in = CreateNamedPipeW(
                PIPE_Game2uMod,             // pipe name
                PIPE_ACCESS_INBOUND,       // read access
                PIPE_TYPE_BYTE |       // byte type pipe
                PIPE_WAIT,                // blocking mode
                PIPE_UNLIMITED_INSTANCES, // max. instances
                SMALL_BUFSIZE,                  // output buffer size
                SMALL_BUFSIZE,                  // input buffer size
                0,                        // client time-out
                NULL);                    // default security attribute
            if (pipe_in == INVALID_HANDLE_VALUE) {
                Log::Error("Failed to create pipe_in\n");
                break;
            }

            pipe_out = CreateNamedPipeW(
                PIPE_uMod2Game,             // pipe name
                PIPE_ACCESS_OUTBOUND,       // write access
                PIPE_TYPE_BYTE |       // byte type pipe
                PIPE_WAIT,                // blocking mode
                PIPE_UNLIMITED_INSTANCES, // max. instances
                BIG_BUFSIZE,                  // output buffer size
                BIG_BUFSIZE,                  // input buffer size
                0,                        // client time-out
                NULL);                    // default security attribute
            if (pipe_out == INVALID_HANDLE_VALUE) {
                Log::Error("Failed to create pipe_out\n");
                break;
            }

            Log::Log("Connect to pipe\n");
            // at first connect to the incoming pipe !!!
            fConnected = ConnectNamedPipe(pipe_in, NULL) ?
                true : (GetLastError() == ERROR_PIPE_CONNECTED);
            /*
            Beep(900,100);
            Beep(600,100);
            Beep(300,100);
            */
            if (!fConnected) {
                Log::Error("Failed to connect to pipe_in\n");
                break;
            }
            unsigned long num = 0;
            Log::Log("Read contents\n");
            //read the name of the game
            bool fSuccess = ReadFile(
                pipe_in,        // handle to pipe
                buffer,    // buffer to receive data
                SMALL_BUFSIZE, // size of buffer
                &num, // number of bytes read
                NULL);        // not overlapped I/O
            if (!fSuccess) {
                Log::Error("Failed to read from pipe_in\n");
                break;
            }
            if (num < 3) {
                Log::Error("pipe_in num < 3\n");
                break;
            }
            buffer[num] = 0;
            buffer[num - 1] = 0;
            std::wstring name((wchar_t*)buffer);

            if (name == abort) { // kill this server thread
                Log::Log("Server abort message received\n");
                break;
            }
            fConnected = ConnectNamedPipe(pipe_out, NULL) ?
                true : (GetLastError() == ERROR_PIPE_CONNECTED);

            if (!fConnected) { // kill this server thread
                Log::Log("Failed to connect to pipe_out\n");
                break;
            }
            Log::Log("Add game? %s\n", name);
        }
        if (pipe_in)            CloseHandle(pipe_in);
        if (pipe_out)           CloseHandle(pipe_out);
        if (CheckForSingleRun)  CloseHandle(CheckForSingleRun);
        Log::Log("uMod Server Stopped\n");
        uMod_Server_NeedToStop = false;
        uMod_Server_Running = false;
        });
}
