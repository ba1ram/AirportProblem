#include "AirportDatabase.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

bool AirportDatabase::loadFromFile(const QString& filename, bool hasHeader, QChar delimiter) {
    QFile file(filename);

    if (!file.exists()) {
        qDebug() << "File does not exist:" << filename;
        return false;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file:" << filename;
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    int lineNumber = 0;
    int skippedLines = 0;

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        lineNumber++;

        // Пропускаем пустые строки
        if (line.trimmed().isEmpty()) {
            continue;
        }

        // Пропускаем заголовок если есть
        if (hasHeader && lineNumber == 1) {
            continue;
        }

        // Разбиваем строку по разделителю ;
        QStringList parts = line.split(delimiter, Qt::KeepEmptyParts);

        // Ожидаем 3 колонки: код;широта;долгота
        if (parts.size() < 3) {
            qDebug() << "Skipping line" << lineNumber
                     << ": expected 3 columns, got" << parts.size();
            skippedLines++;
            continue;
        }

        QString code = parts[0].trimmed().toUpper();
        bool latOk = false, lonOk = false;
        double latitude  = parts[1].trimmed().toDouble(&latOk);
        double longitude = parts[2].trimmed().toDouble(&lonOk);

        if (code.isEmpty()) {
            qDebug() << "Skipping line" << lineNumber << ": empty airport code";
            skippedLines++;
            continue;
        }

        if (!latOk || !lonOk) {
            qDebug() << "Skipping line" << lineNumber
                     << ": invalid coordinates for" << code;
            skippedLines++;
            continue;
        }

        if (latitude < -90.0 || latitude > 90.0) {
            qDebug() << "Skipping line" << lineNumber
                     << ": latitude out of range for" << code;
            skippedLines++;
            continue;
        }

        if (longitude < -180.0 || longitude > 180.0) {
            qDebug() << "Skipping line" << lineNumber
                     << ": longitude out of range for" << code;
            skippedLines++;
            continue;
        }

        if (m_codeToIndex.contains(code)) {
            qDebug() << "Skipping line" << lineNumber
                     << ": duplicate airport code" << code;
            skippedLines++;
            continue;
        }

        m_airports.append(Airport(code, latitude, longitude));
        m_codeToIndex[code] = m_airports.size() - 1;
    }

    file.close();

    qDebug() << "Loaded" << m_airports.size() << "airports from" << filename;
    if (skippedLines > 0) {
        qDebug() << "Skipped" << skippedLines << "invalid lines";
    }

    return !m_airports.isEmpty();
}

const Airport* AirportDatabase::findAirport(const QString& code) const {
    auto it = m_codeToIndex.find(code.toUpper());
    if (it != m_codeToIndex.end()) {
        return &m_airports[it.value()];
    }
    return nullptr;
}

int AirportDatabase::getIndex(const QString& code) const {
    auto it = m_codeToIndex.find(code.toUpper());
    if (it != m_codeToIndex.end()) {
        return it.value();
    }
    return -1;
}

const QVector<Airport>& AirportDatabase::getAllAirports() const {
    return m_airports;
}

int AirportDatabase::size() const {
    return m_airports.size();
}

bool AirportDatabase::isEmpty() const {
    return m_airports.isEmpty();
}

const Airport& AirportDatabase::getByIndex(int index) const {
    return m_airports[index];
}