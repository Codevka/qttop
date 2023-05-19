#ifndef CPUINFOHANDLER_H
#define CPUINFOHANDLER_H

#include <QMap>
#include <QQueue>
#include <QString>
#include <QVarLengthArray>
#include <QVector>
#include <cstdint>
#include <memory>

class CpuInfoHandler {
public:
    static CpuInfoHandler &getInstance() {
        static std::unique_ptr<CpuInfoHandler> instance(new CpuInfoHandler());
        return *instance;
    }

public:
    QString getCpuName();
    QString cpu_name;

public:
    CpuInfoHandler(const CpuInfoHandler &)            = delete;
    CpuInfoHandler(CpuInfoHandler &&)                 = delete;
    CpuInfoHandler &operator=(const CpuInfoHandler &) = delete;
    CpuInfoHandler &operator=(CpuInfoHandler &&)      = delete;
    ~CpuInfoHandler()                                 = default;

private:
    CpuInfoHandler();
};

typedef struct CpuInfo {
    QMap<QString, QQueue<uint64_t>> cpu_percent
        = { { "total", {} },  { "user", {} },      { "nice", {} },
            { "system", {} }, { "idle", {} },      { "iowait", {} },
            { "irq", {} },    { "softirq", {} },   { "steal", {} },
            { "guest", {} },  { "guest_nice", {} } };
    QVector<QQueue<uint64_t>> core_percent;
    QVector<QQueue<uint64_t>> temperature;
    uint64_t                  max_temperature;
    QVarLengthArray<float, 3> load_avg_1_5_15;
} CpuInfo;

#endif // CPUINFOHANDLER_H
