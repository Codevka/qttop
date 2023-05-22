#include "CpuInfoHandler.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

CpuInfoHandler::CpuInfoHandler() {
    try {
        cpu_name = getCpuName();
    } catch (QString e) {
        cpu_name = e;
    }

    updateCpuHz();
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

    QString freq_path
        = "/sys/devices/system/cpu/cpufreq/policy0/scaling_cur_freq";
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
                cpu_hz_tmp
                    = QString(tmp_str.mid(pos, tmp_str.length() - prop_len));
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
