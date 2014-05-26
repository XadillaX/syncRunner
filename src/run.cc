#include <node.h>
#include <string>
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
    ThrowException(Exception::TypeError(String::New("Your OS is not supported yet.")));
    return "";
}
#endif
#endif
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

    return scope.Close(String::New(res.c_str()));
}

void Init(Handle<Object> exports)
{
    exports->Set(String::NewSymbol("exec"),
        FunctionTemplate::New(Exec)->GetFunction());
}

NODE_MODULE(exec_addon, Init)
