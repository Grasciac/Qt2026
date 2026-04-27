#ifndef POI_H
#define POI_H

#include <QString>
#include <QJsonObject>
#include <QFile>
#include <QIODevice>
#include <QTextStream>

class PointOfInterest {
public:
    QString name;           // название
    QString description;    // описание
    int difficulty;         // сложность (1-10)
    double x;               // координата X
    double y;               // координата Y

    PointOfInterest() : difficulty(0), x(0.0), y(0.0) {}

    PointOfInterest(const QString& n, const QString& desc, int diff, double coordX, double coordY)
        : name(n), description(desc), difficulty(diff), x(coordX), y(coordY) {}

    // Конвертация в QJsonObject
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["description"] = description;
        obj["difficulty"] = difficulty;
        obj["x"] = x;
        obj["y"] = y;
        return obj;
    }

    // Создание из QJsonObject
    static PointOfInterest fromJson(const QJsonObject& obj) {
        PointOfInterest poi;
        poi.name = obj["name"].toString();
        poi.description = obj["description"].toString();
        poi.difficulty = obj["difficulty"].toInt();
        poi.x = obj["x"].toDouble();
        poi.y = obj["y"].toDouble();
        return poi;
    }

    // Проверка корректности объекта
    bool isValid() const {
        if (name.isEmpty() || description.isEmpty())
            return false;
        if (difficulty < 1 || difficulty > 10)
            return false;
        return true;
    }

    // Парсинг из текстового файла
    static PointOfInterest parseFromTxt(const QString& filePath) {
        PointOfInterest poi;
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (line.contains("=")) {
                    QStringList parts = line.split("=");
                    if (parts.size() >= 2) {
                        QString key = parts[0].trimmed();
                        QString value = parts[1].trimmed();

                        if (key == "name") poi.name = value;
                        else if (key == "description") poi.description = value;
                        else if (key == "difficulty") poi.difficulty = value.toInt();
                        else if (key == "x") poi.x = value.toDouble();
                        else if (key == "y") poi.y = value.toDouble();
                    }
                }
            }
            file.close();
        }
        return poi;
    }
};

#endif // POI_H
