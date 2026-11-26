#ifndef MORSELIB_H
#define MORSELIB_H

#include <QString>
#include <QStringList>
#include <map>

namespace morse {

extern const std::map<QString, QString> CHAR2CODE;   // буква -> код
extern const std::map<QString, QString> CODE2CHAR;   // код -> буква

QString encodeText(const QString &text);
QString decodeClean(const QString &code);

struct CorruptOptions {
    double   pPanic = 0.30;
    double   pPause = 0.30;
    unsigned seed   = 0;
};
QString corruptCode(const QString &code, const CorruptOptions &opt);

QStringList recoverVariants(const QString &corrupted, int maxCycles, int maxVariants,
                            bool *truncated = nullptr);
QStringList recoverAuto(const QString &corrupted, int maxVariants,
                        int *usedK = nullptr, bool *truncated = nullptr);
bool canProduce(const QString &origLine, const QString &corrLine, int maxCycles);

} // namespace morse
#endif
