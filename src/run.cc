#include <node.h>
#include <string>
#include "util.h"
using namespace v8;
using namespace std;

#ifdef _WIN32
// Windows platform OS
#include <iostream>
#include <windows.h>
#define BUFSIZE 4096

// Create STDIO pipe
bool redirect_stdio(HANDLE* pChildStdoutRd, HANDLE* pChildStdoutWr)
{
    SECURITY_ATTRIBUTES secattr; 
    ZeroMemory(&secattr, sizeof(secattr));
    secattr.nLength = sizeof(secattr);
    secattr.lpSecurityDescriptor = NULL;
    secattr.bInheritHandle = TRUE;

    if(!CreatePipe(pChildStdoutRd, pChildStdoutWr, &secattr, 0))
    {
        return false;
    }

    return true;
}

string GetLastError(DWORD dw)
{
    LPTSTR lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPTSTR) &lpMsgBuf,
        0, NULL);

    int len = strlen(lpMsgBuf);
    while(len && (lpMsgBuf[len - 1] == '\r' || lpMsgBuf[len - 1] == '\n'))
    {
        lpMsgBuf[len - 1] = 0;
        len--;
    }

    string msg = lpMsgBuf;
    LocalFree(lpMsgBuf);

    return msg;
}

// run the command line...
string run(char* line, char* cwd, int max_million_second)
{
    HANDLE hChildStdoutRd, hChildStdoutWr;
    if(!redirect_stdio(&hChildStdoutRd, &hChildStdoutWr))
    {
        DWORD dw = GetLastError();
        string str = GetLastError(dw); 
        char err[256];
        sprintf(err, "Failed while creating process. [%d. %s]", dw, str.c_str());

        ThrowException(Exception::Error(String::New("Failed while creating pipe.")));
        return "";
    }

    PROCESS_INFORMATION pi;
    STARTUPINFO si = { sizeof(si) };
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = hChildStdoutWr;
    si.hStdError = hChildStdoutWr;
    si.wShowWindow = SW_HIDE;    

    BOOL bSuccess = CreateProcess(NULL, line, NULL, NULL,  
        TRUE, 0, NULL, cwd, &si, &pi);

    if(!bSuccess)
    {
        CloseHandle(hChildStdoutRd);
        CloseHandle(hChildStdoutWr);

        DWORD dw = GetLastError();
        string str = GetLastError(dw); 
        char err[256];
        sprintf(err, "Failed while creating process. [%d. %s]", dw, str.c_str());

        ThrowException(Exception::Error(String::New(err)));
        return "";
    }
    CloseHandle(hChildStdoutWr);

    if(!max_million_second)
    {
        max_million_second = INFINITE;
    }
    WaitForSingleObject(pi.hProcess, max_million_second);

    // Terminate the process.
    TerminateProcess(pi.hProcess, 4);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Read from rd pipe.
    string result = "";
    DWORD dwRead;
    CHAR chBuf[BUFSIZE + 1];
    bSuccess = FALSE;
    for(;;)
    {
        bSuccess = ReadFile(hChildStdoutRd, chBuf, BUFSIZE, &dwRead, NULL);
        if(!bSuccess || dwRead == 0) break;

        chBuf[dwRead] = 0;
        result += chBuf;
    }
    CloseHandle(hChildStdoutRd);

    return result;
}
#else
// Other platform OS...

string run(char* line, char* cwd, int max_million_second)
{
    ThrowException(Exception::TypeError(String::New("Your OS is not supported yet.")));
    return "";
}
#endif

Handle<Value> Exec(const Arguments& args)
{
    HandleScope scope;

    int max_million_second = 0;

    if(args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments.")));
        return scope.Close(String::New(""));
    }

    // cmd line...
    char* line = new char[args[0]->ToString()->Length() + 1];
    Local<String>::Cast(args[0])->WriteAscii(line);

    // current work directory & max million second...
    char* cwd = NULL;
    if(args.Length() > 1 && args[1]->IsNumber())
    {
        max_million_second = (int)args[1]->NumberValue();
    }
    else
    if(args.Length() > 1)
    {
        if(args[1]->IsString())
        {
            cwd = new char[args[1]->ToString()->Length() + 1];
            Local<String>::Cast(args[1])->WriteAscii(cwd);
        }

        if(args.Length() > 2 && args[2]->IsNumber())
        {
            max_million_second = (int)args[2]->NumberValue();
        }
    }

    string res = run(line, cwd, max_million_second);

    // delete the pointer...
    if(line)
    {
        delete []line;
    }
    if(cwd)
    {
        delete []cwd;
    }

    bool bUTF8 = IsUTF8(res.c_str(), res.length());
    if(bUTF8)
    {
        return scope.Close(String::New(res.c_str()));
    }
    else
    {
        res = ACPToUTF8(res.c_str());
        return scope.Close(String::New(res.c_str()));
    }
}

void Init(Handle<Object> exports)
{
    exports->Set(String::NewSymbol("exec"),
        FunctionTemplate::New(Exec)->GetFunction());
}

NODE_MODULE(exec_addon, Init)
