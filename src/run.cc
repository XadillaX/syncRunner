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
    string line = *(String::Utf8Value(info[0]->ToString()));

    // current work directory & max million second...
    string cwd;
    if(info.Length() > 1 && info[1]->IsNumber())
    {
        max_million_second = (int)info[1]->NumberValue();
    }
    else
    if(info.Length() > 1)
    {
        if(info[1]->IsString())
        {
            cwd = *(String::Utf8Value(info[1]->ToString()));
        }

        if(info.Length() > 2 && info[2]->IsNumber())
        {
            max_million_second = (int)info[2]->NumberValue();
        }
    }

    string res = run(line.c_str(), cwd == "" ? NULL : cwd.c_str(), max_million_second);

    // delete the pointer...
    info.GetReturnValue().Set(Nan::New<String>(res.c_str()).ToLocalChecked());
}

void Init(Handle<Object> exports)
{
    exports->Set(Nan::New<String>("exec").ToLocalChecked(),
            Nan::New<FunctionTemplate>(Exec)->GetFunction());
}

NODE_MODULE(exec_addon, Init)
