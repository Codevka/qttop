#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <thread>

using std::string;
using std::vector;
using std::max;

namespace fs = std::filesystem;

namespace Shared
{

    fs::path procPath, passwd_path;
    long pageSize, clkTck, coreCount;
    constexpr auto SSmax = std::numeric_limits<std::streamsize>::max();

    void init()
    {

        //? Shared global variables init
        procPath = (fs::is_directory(fs::path("/proc")) and access("/proc", R_OK) != -1) ? "/proc" : "";
        if (procPath.empty())
            throw std::runtime_error("Proc filesystem not found or no permission to read from it!");

        coreCount = sysconf(_SC_NPROCESSORS_ONLN);
        if (coreCount < 1)
        {
            coreCount = sysconf(_SC_NPROCESSORS_CONF);
            if (coreCount < 1)
            {
                coreCount = 1;
                std::cerr << "[Warning]Could not determine number of cores, defaulting to 1." << std::endl;
            }
        }

        pageSize = sysconf(_SC_PAGE_SIZE);
        if (pageSize <= 0)
        {
            pageSize = 4096;
            std::cerr << "[Warning]Could not get system page size. Defaulting to 4096, processes memory usage might be incorrect." << std::endl;
        }

        clkTck = sysconf(_SC_CLK_TCK);
        if (clkTck <= 0)
        {
            clkTck = 100;
            std::cerr << "[Warning]Could not get system clock ticks per second. Defaulting to 100, processes cpu usage might be incorrect." << std::endl;
        }
    }

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
        double power_consumption{};
    };

    vector<ProcessInfo> processes;
    uint64_t old_cputimes{};

    vector<ProcessInfo> getProcesses()
    {
        // Get the total CPU time
        std::ifstream stat_file("/proc/stat");
        uint64_t cpu_times = 0;
        if (stat_file.good())
        {
            stat_file.ignore(Shared::SSmax, ' ');
            for (uint64_t times; stat_file >> times; cpu_times += times)
                ;
        }
        else
            throw std::runtime_error("Failure to read /proc/stat");
        stat_file.close();
        printf("CPU Time: %lu, OLD: %lu\n", cpu_times, old_cputimes);

        // 遍历系统内所有进程
        for (const auto &entry : fs::directory_iterator("/proc"))
        {
            if (!entry.is_directory())
                continue;
            int pid = 0;
            try
            {
                pid = std::stoi(entry.path().filename());
            }
            catch (const std::invalid_argument &)
            {
                continue;
            }

            //? Check if pid already exists in current_procs
            auto find_old = std::find_if(processes.begin(), processes.end(), [&](const ProcessInfo &proc)
                                         { return proc.pid == pid; });
            bool no_cache{}; // defaults to false
            if (find_old == processes.end())
            {
                processes.push_back({pid : pid});
                find_old = processes.end() - 1;
                no_cache = true;
            }

            auto &proc = *find_old;

            // Get the process name
            if (no_cache)
            {
                std::ifstream comm_file(entry.path() / "comm");
                std::getline(comm_file, proc.name);
                comm_file.close();
            }

            // Get the process CPU time
            std::ifstream stat_file(entry.path() / "stat");
            string stat_line;
            std::getline(stat_file, stat_line);
            stat_file.close();
            size_t cpu_t = 0;
            size_t start_pos = 0;
            for (int i = 0; i < 13; i++)
            { // skip the next 13 fields
                start_pos = stat_line.find(' ', start_pos) + 1;
            }
            for (int i = 0; i < 2; i++)
            { // sum up the next two fields: utime and stime
                size_t end_pos = stat_line.find(' ', start_pos);
                try
                {
                    cpu_t += std::stoull(stat_line.substr(start_pos, end_pos - start_pos));
                }
                catch (const std::invalid_argument &)
                {
                    continue;
                }
                start_pos = end_pos + 1;
            }

            //? Get RSS memory from /proc/[pid]/statm if value from /proc/[pid]/stat looks wrong
            std::ifstream statm_file(entry.path() / "statm");
            statm_file.ignore(Shared::SSmax, ' ');
            statm_file >> proc.memory;
            statm_file.close();
            proc.memory *= Shared::pageSize;

            //? Process cpu usage since last update
            proc.cpu_p = std::clamp(100.0 * (cpu_t - proc.cpu_t) / max((uint64_t)1, cpu_times - old_cputimes), 0.0, 100.0);

            //? Update cached value with latest cpu times
            proc.cpu_t = cpu_t;

            // Get the process power consumption (not implemented)
            proc.power_consumption = 0.0;
        }

        old_cputimes = cpu_times;

        // Sort the processes by CPU usage
        std::sort(processes.begin(), processes.end(), [](const ProcessInfo &a, const ProcessInfo &b)
                  { return a.cpu_p > b.cpu_p; });

        return processes;
    }

}

int main()
{
    Shared::init();
    printf("Cores:%ld, Pagesize:%ld\n", Shared::coreCount, Shared::pageSize);
    for (;;)
    {
        auto processes = Proc::getProcesses();
        printf("-------------------------------------------------------------------\n");
        printf("CPU Time: %lu\n", Proc::old_cputimes);
        for (int i = 0; i < 5; i++)
        {
            const auto process = processes[i];
            std::cout << "Name: " << process.name << ", PID: " << process.pid << ", CPU Usage: " << process.cpu_p << "%"
                      << ", CPU_T: " << process.cpu_t
                      << ", Memory: " << (process.memory > 20) << " MB"
                      << ", Power Consumption: " << process.power_consumption << " mW" << '\n';
        }
        printf("-------------------------------------------------------------------\n\n");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}