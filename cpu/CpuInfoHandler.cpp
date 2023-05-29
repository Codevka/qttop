#include "CpuInfoHandler.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <string>
#include <unistd.h>

CpuInfoHandler::CpuInfoHandler() {
    cur_cpu.load_avg_1_5_15.resize(3);
    try {
        cpu_name = getCpuName();
    } catch (QString e) {
        cpu_name = e;
    }

    core_num = sysconf(_SC_NPROCESSORS_ONLN);
    if (core_num < 1) {
        core_num = sysconf(_SC_NPROCESSORS_CONF);
        if (core_num < 1) {
            core_num = 1;
        }
    }

    for (uint32_t i = 0; i < core_num; i++) {
        cur_cpu.core_percent.push_back({});
        old_core_total.push_back(0);
        old_core_idle.push_back(0);
    }

    updateCpuHz();
    collectCpuInfo();
}

QString CpuInfoHandler::getCpuName() {
    QFile file("/proc/cpuinfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw "无法获取CPU信息";
    }

    QTextStream ins(&file);
    do {
        auto tmp_str  = ins.readLine();
        auto pos      = tmp_str.indexOf("model name");
        auto prop_len = QString("model name\\t:").length();
        if (pos != -1) {
            pos += prop_len;
            return QString(tmp_str.mid(pos, tmp_str.length() - prop_len));
        }
    } while (!ins.atEnd());
    file.close();
    return QString("无法获取CPU名称");
}

void CpuInfoHandler::updateCpuHz() {
    try {
        cpu_hz = getCpuHz();
    } catch (QString e) {
        cpu_hz = e;
    }
}

QString CpuInfoHandler::getCpuHz() {
    static int failed = 0;
    double     hz     = 0.0;

    if (failed > 4)
        return QString("");

    QString freq_path = "/sys/devices/system/cpu/cpufreq/policy0/scaling_cur_freq";
    QFile file(freq_path);
    QString cpuinfo_path = "/proc/cpuinfo";
    QFile   file2(cpuinfo_path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString cpu_hz_tmp = file.readLine();
        hz                 = cpu_hz_tmp.toDouble() / 1000.0;
        file.close();
    } else if (file2.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream ins(&file2);
        QString     cpu_hz_tmp;
        do {
            auto tmp_str  = ins.readLine();
            auto pos      = tmp_str.indexOf("cpu MHz");
            auto prop_len = QString("cpu MHz\\t:").length();
            if (pos != -1) {
                pos += prop_len;
                cpu_hz_tmp = QString(tmp_str.mid(pos, tmp_str.length() - prop_len));
            }
        } while (!ins.atEnd());
        hz = cpu_hz_tmp.toDouble();
        file2.close();
    } else {
        throw QString("");
    }
    if (hz > 1000.0) {
        QString cpu_ghz = QString::number(hz / 1000.0, 'f', 2) + " GHz";
        return cpu_ghz;
    }
    QString cpu_mhz = QString::number(hz, 'f', 0) + " MHz";
    return cpu_mhz;
}

CpuInfo &CpuInfoHandler::collectCpuInfo() {
    constexpr auto SSmax         = std::numeric_limits<std::streamsize>::max();
    std::string   load_avg_path = "/proc/loadavg";
    std::ifstream ifs;

    ifs.open(load_avg_path);
    if (ifs.good()) {
        ifs >> cur_cpu.load_avg_1_5_15[0] >> cur_cpu.load_avg_1_5_15[1] >> cur_cpu.load_avg_1_5_15[2];
    }
    ifs.close();
    qDebug() << cur_cpu.load_avg_1_5_15[0] << cur_cpu.load_avg_1_5_15[1] << cur_cpu.load_avg_1_5_15[2];

    std::string stat_path = "/proc/stat";
    ifs.open(stat_path);

    {
        ifs.ignore(SSmax, ' ');
        QVector<uint64_t> times;
        uint64_t          time_sum = 0;
        for (uint64_t val; ifs >> val; time_sum += val) {
            times.push_back(val);
        }
        ifs.clear();

        uint64_t total = std::max(
            (uint64_t)0, time_sum - (times.size() > 8 ? std::accumulate(times.begin() + 8, times.end(), 0) : 0));
        uint64_t idle       = std::max((uint64_t)0, times[3] + (times.size() > 4 ? times[4] : 0));
        uint64_t calc_total = std::max((uint64_t)1, total - old_cpu_total);
        uint64_t calc_idle  = std::max((uint64_t)1, idle - old_cpu_idle);
        old_cpu_total       = total;
        old_cpu_idle        = idle;
        qDebug() << "calc_total: " << calc_total;

        cur_cpu.cpu_percent = std::clamp((long long) round((double) (calc_total - calc_idle) * 1000
                                                           / calc_total),
                                         0ll,
                                         1000ll);
        cur_cpu.cpu_percent_total.push_back(cur_cpu.cpu_percent);
        qDebug() << "cpu_percent_total: " << cur_cpu.cpu_percent;

        cur_cpu.cpu_percent_user.push_back(
            std::clamp((long long)round((double)(times[0] - old_cpu_user) * 1000 / calc_total), 0ll, 1000ll));
        qDebug() << "cpu_percent_user: "
                 << std::clamp((long long)round((double)(times[0] - old_cpu_user) * 100 / calc_total), 0ll, 1000ll);
        old_cpu_user = times[0];

        cur_cpu.cpu_percent_system.push_back(
            std::clamp((long long)round((double)(times[2] - old_cpu_system) * 1000 / calc_total), 0ll, 1000ll));
        qDebug() << "cpu_percent_system: "
                 << std::clamp((long long)round((double)(times[2] - old_cpu_system) * 1000 / calc_total), 0ll, 1000ll);
        old_cpu_system = times[2];
    }

    uint32_t    i = 0;
    std::string core_name;
    while (i < core_num || (ifs.good() && ifs.peek() == 'c')) {
        ifs >> core_name;
        QVector<uint64_t> times;
        uint64_t          time_sum = 0;
        for (uint64_t val; ifs >> val; time_sum += val) {
            times.push_back(val);
        }
        ifs.clear();

        uint64_t total = std::max(
            (uint64_t)0, time_sum - (times.size() > 8 ? std::accumulate(times.begin() + 8, times.end(), 0) : 0));
        uint64_t idle       = std::max((uint64_t)0, times[3] + (times.size() > 4 ? times[4] : 0));
        uint64_t calc_total = std::max((uint64_t)0, total - old_core_total[i]);
        uint64_t calc_idle  = std::max((uint64_t)0, idle - old_core_idle[i]);
        old_core_total[i]   = total;
        old_core_idle[i]    = idle;
        cur_cpu.core_percent[i].push_back(
            std::clamp((long long)round((double)(calc_total - calc_idle) * 100 / calc_total), 0ll, 100ll));

        QString tmp("");
        for (auto j : cur_cpu.core_percent[i]) {
            tmp += QString::number(j) + " ";
        }
        qDebug() << "core" << i << ": " << tmp;

        i++;
    }
    for (auto &c : cur_cpu.core_percent) {
        if (c.size() >= 40)
            c.pop_front();
    }

    return cur_cpu;
}
