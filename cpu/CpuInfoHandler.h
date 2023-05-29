#ifndef CPUINFOHANDLER_H
#define CPUINFOHANDLER_H

#include <QMap>
#include <QQueue>
#include <QString>
#include <QVarLengthArray>
#include <QVector>
#include <cstdint>
#include <memory>

#include <QDebug>

typedef struct CpuInfo {
    QQueue<uint64_t>          cpu_percent_total;
    QQueue<uint64_t>          cpu_percent_user;
    QQueue<uint64_t>          cpu_percent_system;
    QVector<QQueue<uint64_t>> core_percent;
    QVector<QQueue<uint64_t>> temperature;
    uint64_t                  max_temperature;
    uint64_t cpu_percent;
    QVarLengthArray<float, 3> load_avg_1_5_15;
} CpuInfo;

class CpuInfoHandler {
public:
    static std::shared_ptr<CpuInfoHandler> getInstance() {
        static std::shared_ptr<CpuInfoHandler> instance(new CpuInfoHandler());
        return instance;
    }

public:
    QString  getCpuName();
    QString  getCpuHz();
    void     updateCpuHz();
    CpuInfo &collectCpuInfo();

public:
    QString          cpu_name;
    QString          cpu_hz;
    uint32_t         core_num;
    QVector<QString> available_fields; // total, user, system
    CpuInfo          cur_cpu;

    uint64_t          old_cpu_total, old_cpu_idle, old_cpu_user, old_cpu_system;
    QVector<uint64_t> old_core_total, old_core_idle;

public:
    CpuInfoHandler(const CpuInfoHandler &)            = delete;
    CpuInfoHandler(CpuInfoHandler &&)                 = delete;
    CpuInfoHandler &operator=(const CpuInfoHandler &) = delete;
    CpuInfoHandler &operator=(CpuInfoHandler &&)      = delete;
    ~CpuInfoHandler()                                 = default;

private:
    CpuInfoHandler();
};


#endif // CPUINFOHANDLER_H
