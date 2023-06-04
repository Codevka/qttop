#ifndef PROCESS_H
#define PROCESS_H

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <string>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include <Qt>

using std::max;
using std::string;
using std::vector;

namespace fs = std::filesystem;

namespace Shared {

extern fs::path procPath, passwd_path;
extern long pageSize, clkTck, coreCount;
extern long SSmax;

void init();
} // namespace Shared

namespace Proc {
struct ProcessInfo
{
    string name{};
    string name_lower{};
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

vector<ProcessInfo> get_processes(int64_t duration, int column, Qt::SortOrder order);
int send_signal(int pid, int sig);
} // namespace Proc

#endif // PROCESS_H
