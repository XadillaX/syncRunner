#include <node.h>
#include <iostream>
#include <windows.h>
#include "../util.h"
#define BUFSIZE 4096
using namespace v8;

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

    bool isUtf8 = IsUTF8(result.c_str(), result.length());
    if(!isUtf8)
    {
        result = ACPToUTF8(result.c_str());
    }

    return result;
}
