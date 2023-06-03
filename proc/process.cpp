#include "process.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <signal.h>
#include <string>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

using std::max;
using std::string;
using std::vector;

namespace fs = std::filesystem;

namespace Shared {

fs::path procPath, passwd_path;
long pageSize, clkTck, coreCount;
long SSmax = std::numeric_limits<std::streamsize>::max();

void init()
{
    // 初始化系统全局参数
    procPath = (fs::is_directory(fs::path("/proc")) and access("/proc", R_OK) != -1) ? "/proc" : "";
    if (procPath.empty())
        throw std::runtime_error("Proc filesystem not found or no permission to read from it!");

    coreCount = sysconf(_SC_NPROCESSORS_ONLN);
    if (coreCount < 1) {
        coreCount = sysconf(_SC_NPROCESSORS_CONF);
        if (coreCount < 1) {
            coreCount = 1;
            std::cerr << "[Warning]Could not determine number of cores, defaulting to 1."
                      << std::endl;
        }
    }

    pageSize = sysconf(_SC_PAGE_SIZE);
    if (pageSize <= 0) {
        pageSize = 4096;
        std::cerr << "[Warning]Could not get system page size. Defaulting to 4096, processes "
                     "memory usage might be "
                     "incorrect."
                  << std::endl;
    }

    clkTck = sysconf(_SC_CLK_TCK);
    if (clkTck <= 0) {
        clkTck = 100;
        std::cerr << "[Warning]Could not get system clock ticks per second. Defaulting to 100, "
                     "processes cpu usage "
                     "might be incorrect."
                  << std::endl;
    }
}

}

namespace Proc {
std::string lower_string(const std::string &str)
{
    std::string lower{};
    for (auto ch : str) {
        if (ch <= 'Z' && ch >= 'A') {
            lower += ch - 'A' + 'a';
        } else {
            lower += ch;
        }
    }
    return lower;
}

std::function<bool(const ProcessInfo &, const ProcessInfo &)> sort_function(int column,
                                                                            Qt::SortOrder order)
{
    auto pid_ascend_func = [](const ProcessInfo &a, const ProcessInfo &b) { return a.pid > b.pid; };
    auto pid_descend_func = [](const ProcessInfo &a, const ProcessInfo &b) { return a.pid < b.pid; };
    auto name_ascend_func = [](const ProcessInfo &a, const ProcessInfo &b) {
        return a.name_lower > b.name_lower;
    };
    auto name_descend_func = [](const ProcessInfo &a, const ProcessInfo &b) {
        return a.name_lower < b.name_lower;
    };
    auto percent_ascend_func = [](const ProcessInfo &a, const ProcessInfo &b) {
        return a.cpu_p > b.cpu_p;
    };
    auto percent_descend_func = [](const ProcessInfo &a, const ProcessInfo &b) {
        return a.cpu_p < b.cpu_p;
    };
    auto memory_ascend_func = [](const ProcessInfo &a, const ProcessInfo &b) {
        return a.memory > b.memory;
    };
    auto memory_descend_func = [](const ProcessInfo &a, const ProcessInfo &b) {
        return a.memory < b.memory;
    };
    auto power_ascend_func = [](const ProcessInfo &a, const ProcessInfo &b) {
        return a.power > b.power;
    };
    auto power_descend_func = [](const ProcessInfo &a, const ProcessInfo &b) {
        return a.power < b.power;
    };
    auto default_func = percent_descend_func;

    switch (column) {
    case 0:
        return order == Qt::AscendingOrder ? pid_ascend_func : pid_descend_func;
    case 1:
        return order == Qt::AscendingOrder ? name_ascend_func : name_descend_func;
    case 2:
        return order == Qt::AscendingOrder ? percent_ascend_func : percent_descend_func;
    case 3:
        return order == Qt::AscendingOrder ? memory_ascend_func : memory_descend_func;
    case 4:
        return order == Qt::AscendingOrder ? power_ascend_func : power_descend_func;
    default:
        return default_func;
    }
}

vector<ProcessInfo> processes;
uint64_t old_cputimes{};
uint64_t ticks{};

vector<ProcessInfo> get_processes(int64_t duration, int column, Qt::SortOrder order)
{
    // 获取CPU时间
    std::ifstream stat_file("/proc/stat");
    uint64_t cpu_times = 0;
    if (stat_file.good()) {
        stat_file.ignore(Shared::SSmax, ' ');
        for (uint64_t times; stat_file >> times; cpu_times += times)
            ;
    } else
        throw std::runtime_error("Failure to read /proc/stat");
    stat_file.close();
    printf("CPU Time: %lu, OLD: %lu\n", cpu_times, old_cputimes);

    // 遍历系统内所有进程
    for (const auto &entry : fs::directory_iterator("/proc")) {
        if (!entry.is_directory())
            continue;
        int pid = 0;
        try {
            pid = std::stoi(entry.path().filename());
        } catch (const std::invalid_argument &) {
            continue;
        }

        // 检查该进程是否已经存在
        auto find_old = std::find_if(processes.begin(),
                                     processes.end(),
                                     [&](const ProcessInfo &proc) { return proc.pid == pid; });
        bool no_cache{}; // defaults to false
        if (find_old == processes.end()) {
            processes.push_back({pid: pid, ticks: ticks});
            find_old = processes.end() - 1;
            no_cache = true;
        }

        auto &proc = *find_old;

        // 获取进程名
        if (no_cache) {
            std::ifstream comm_file(entry.path() / "comm");
            std::getline(comm_file, proc.name);
            proc.name_lower = lower_string(proc.name);
            comm_file.close();
        }

        // 获取进程的CPU时间
        std::ifstream stat_file(entry.path() / "stat");
        string stat_line;
        std::getline(stat_file, stat_line);
        stat_file.close();
        size_t cpu_t = 0;
        size_t start_pos = 0;
        for (int i = 0; i < 13; i++) { // skip the next 13 fields
            start_pos = stat_line.find(' ', start_pos) + 1;
        }
        for (int i = 0; i < 2; i++) { // sum up the next two fields: utime and stime
            size_t end_pos = stat_line.find(' ', start_pos);
            try {
                cpu_t += std::stoull(stat_line.substr(start_pos, end_pos - start_pos));
            } catch (const std::invalid_argument &) {
                continue;
            }
            start_pos = end_pos + 1;
        }

        // 获取进程占用的内存
        std::ifstream statm_file(entry.path() / "statm");
        statm_file.ignore(Shared::SSmax, ' ');
        statm_file >> proc.memory;
        statm_file.close();
        proc.memory *= Shared::pageSize;

        // 计算进程的CPU使用率
        proc.cpu_p = std::clamp(100.0 * (cpu_t - proc.cpu_t)
                                    / max((uint64_t) 1, cpu_times - old_cputimes),
                                0.0,
                                100.0);

        /* 计算进程的功耗 */
        std::ifstream status_file(entry.path() / "status");
        std::string content((std::istreambuf_iterator<char>(status_file)),
                            std::istreambuf_iterator<char>());
        status_file.close();
        size_t pos1 = content.find("voluntary_ctxt_switches:");
        size_t pos2 = content.find("nonvoluntary_ctxt_switches:");
        pos1 = content.find_first_not_of(" \t", pos1 + 24);
        pos2 = content.find_first_not_of(" \t", pos2 + 27);
        uint64_t wakeups = 0;
        try {
            wakeups += std::stoull(content.substr(pos1, content.find("\n", pos1) - pos1));
            wakeups += std::stoull(content.substr(pos2, content.find("\n", pos2) - pos2));
        } catch (const std::invalid_argument &) {
            continue;
        } catch (const std::out_of_range &) {
            continue;
        }

        proc.power = (3.95 * (wakeups - proc.wakeups) + 15.0 * (cpu_t - proc.cpu_t)) / duration;

        // 更新进程信息
        proc.cpu_t = cpu_t;
        proc.wakeups = wakeups;
        proc.ticks++;
    }

    old_cputimes = cpu_times;

    // 调用计数
    ticks++;
    processes.erase(std::remove_if(processes.begin(),
                                   processes.end(),
                                   [](const ProcessInfo &proc) { return proc.ticks < ticks; }),
                    processes.end());

    // 进程按照指定的列排序
    std::sort(processes.begin(), processes.end(), sort_function(column, order));

    return processes;
}

int send_signal(int pid, int sig)
{
    return kill(pid, sig);
}

} // namespace Proc
