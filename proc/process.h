#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <thread>
#include <sys/types.h>
#include <signal.h>

using std::max;
using std::string;
using std::vector;

namespace fs = std::filesystem;

namespace Shared
{

    extern fs::path procPath, passwd_path;
    extern long pageSize, clkTck, coreCount;
    extern long SSmax;

    void init();
}

namespace Proc
{
    struct ProcessInfo
    {
        string name{};
        int pid{};
        uint64_t cpu_t{};
        double cpu_p{};
        uint64_t memory{};
        uint64_t wakeups{};
        double power{};
        uint64_t ticks{};
    };

    extern vector<ProcessInfo> processes;
    extern uint64_t old_cputimes;
    extern uint64_t ticks;

    vector<ProcessInfo> get_processes(int64_t duration);
    int send_signal(int pid, int sig);
}

#endif // PROCESS_H
