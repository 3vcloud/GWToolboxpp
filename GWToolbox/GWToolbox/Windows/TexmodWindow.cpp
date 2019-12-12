#include "TexmodWindow.h"
#include "logger.h"

#define BIG_BUFSIZE 1<<24
#define SMALL_BUFSIZE 1<<10

void TexmodWindow::Initialize() {
    ToolboxWindow::Initialize();
    StartuModServer();
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
    Log::Log("Stopped uMod Server\n");
}
void TexmodWindow::OnServerMessage() {

}
void TexmodWindow::OnAddGame(std::wstring name, HANDLE pipe_in, HANDLE pipe_out)
{
    if (NumberOfGames >= MaxNumberOfGames)
    {
        if (GetMoreMemory(Clients, MaxNumberOfGames, MaxNumberOfGames + 10))
        {
            wxMessageBox(Language->Error_Memory, "ERROR", wxOK | wxICON_ERROR);
            return;
        }
        MaxNumberOfGames += 10;
    }

    wxString name = ((uMod_Event&)event).GetName();
    PipeStruct pipe;

    pipe.In = ((uMod_Event&)event).GetPipeIn();
    pipe.Out = ((uMod_Event&)event).GetPipeOut();

    uMod_Client* client = new uMod_Client(pipe, this);
    client->Create();
    client->Run();

    wxString save_file;
    int num = SaveFile_Exe.GetCount();
    for (int i = 0; i < num; i++) if (name == SaveFile_Exe[i])
    {
        save_file = SaveFile_Name[i];
        break;
    }

    uMod_GamePage* page = new uMod_GamePage(Notebook, name, save_file, client->Pipe);
    if (page->LastError.Len() > 0)
    {
        wxMessageBox(page->LastError, "ERROR", wxOK | wxICON_ERROR);
        delete page;
        return;
    }
    name = name.AfterLast('\\');
    name = name.AfterLast('/');
    name = name.BeforeLast('.');
    Notebook->AddPage(page, name, true);

    Clients[NumberOfGames] = client;
    NumberOfGames++;
    if (NumberOfGames == 1) ActivateGamesControl();
}
void TexmodWindow::StartuModServer() {
    if (uMod_Server_Running)
        return;
    uMod_Server_Running = true;
    if (uMod_Server.joinable())
        uMod_Server.join();
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
            uMod_Server_NeedToStop = true;
        }
        while (!uMod_Server_NeedToStop) {
            /*
                Beep(300,100);
                Beep(600,100);
            */
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
            OnAddGame(name, pipe_in, pipe_out);
        }
        if (pipe_in)            CloseHandle(pipe_in);
        if (pipe_out)           CloseHandle(pipe_out);
        if (CheckForSingleRun)  CloseHandle(CheckForSingleRun);
        Log::Log("uMod Server Stopped\n");
        uMod_Server_NeedToStop = false;
        uMod_Server_Running = false;
        });
}
