#include <node.h>
#include <string>
#include <nan.h>
using namespace v8;
using namespace std;

#ifdef _WIN32
// Windows platform OS
#include "platform/windows.h"
#else
#ifdef __linux
// Linux OS
#include "platform/linux.h"
#else
#ifdef __APPLE__
#include "platform/linux.h"
#else
// Other platform OS...
string run(char* line, char* cwd, int max_million_second)
{
    NanThrowError("Your OS is not supported yet.");
    return "";
}
#endif
#endif
#endif

NAN_METHOD(Exec)
{
    int max_million_second = 0;

    if(info.Length() < 1)
    {
        return Nan::ThrowError("Wrong number of arguments.");
    }

    // cmd line...
    string line = *(Nan::Utf8String(info[0]));

    // current work directory & max million second...
    string cwd;
    if(info.Length() > 1 && info[1]->IsNumber())
    {
        max_million_second = Nan::To<int32_t>(info[1]).FromJust();
    }
    else
    if(info.Length() > 1)
    {
        if(info[1]->IsString())
        {
            cwd = *(Nan::Utf8String(info[1]));
        }

        if(info.Length() > 2 && info[2]->IsNumber())
        {
            max_million_second = Nan::To<int32_t>(info[2]).FromJust();
        }
    }

    string res = run(line.c_str(), cwd == "" ? NULL : cwd.c_str(), max_million_second);

    // delete the pointer...
    info.GetReturnValue().Set(Nan::New<String>(res.c_str()).ToLocalChecked());
}

NAN_MODULE_INIT(Init)
{
    Nan::Export(target, "exec", Exec);
}

NODE_MODULE(exec_addon, Init)
