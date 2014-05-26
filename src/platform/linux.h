#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include "popen_plus/popen_plus.h"
#define LLD long long

void child_process(popen_plus_process* process, int p[])
{
    close(p[0]);
    char buf[512] = { 0 };
    while(fgets(buf, sizeof(buf) - 1, process->read_fp) != NULL)
    {
        write(p[1], buf, sizeof(buf));
    }
    close(p[1]);

    // close process...
    //printf("%d\n", popen_plus_close(process));
}

// Wait for child process...
bool parent_process(popen_plus_process* process, int max_million_second, int pid, int p[])
{
    timeval t_start,t_end;
    gettimeofday(&t_start, NULL);
    bool tle = false;
    while(1)
    {
        int result = waitpid(-1, NULL, WNOHANG);
        if(result == -1)
        {
            // kill process.
            kill(pid, 9);
            popen_plus_kill(process);
            popen_plus_close(process);

            // error occurred.
            write(p[1], "\0", 1);
            close(p[1]);
            
            return false;
        }
        else
        if(result == pid)
        {
            write(p[1], "\0", 1);
            close(p[1]);

            //int code = popen_plus_close(process);
            //printf("进程：%d\n状态码：%d\n", pid, code);

            return true;
        }

        gettimeofday(&t_end, NULL);

        LLD duaring = (t_end.tv_sec - t_start.tv_sec) * 1000 + (t_end.tv_usec - t_start.tv_usec) / 1000;
        if(duaring > max_million_second && max_million_second != 0)
        {
            // kill process.
            kill(pid, 9);
            popen_plus_kill(process);
            popen_plus_close(process);

            write(p[1], "\0", 1);
            close(p[1]);

            return false;
        }

        usleep(1000);
    }
}

string run(char* line, char* cwd, int max_million_second)
{
    int p[2];
    if(pipe(p) < 0)
    {
        ThrowException(Exception::Error(String::New("Failed while creating pipe.")));
        return "";
    }

    // run the cmd...
    if(cwd)
    {
        chdir(cwd);
    }
    popen_plus_process* process = popen_plus(line);
    if(NULL == process)
    {
        ThrowException(Exception::Error(String::New("Failed while running command.")));
        return "";
    }

    // fork a child...
    int pid = fork();
    if(pid == -1)
    {
        ThrowException(Exception::Error(String::New("Failed while forking.")));
    }
    else
    if(pid == 0)
    {
        child_process(process, p);
        exit(0);
    }
    else
    if(pid > 0)
    {
        parent_process(process, max_million_second, pid, p);

        int ird = 0;
        char buf[512] = { 0 };
        string result = "";
        while(1)
        {
            read(p[0], buf, sizeof(buf));
            if(buf[0] == 0) break;
            result += buf;
        }
        close(p[0]);

        return result;
    }
}
