#pragma once

#include "Airport.h"
#include <QVector>
#include <QHash>
#include <QString>

class AirportDatabase {
private:
    QVector<Airport> m_airports;
    QHash<QString, int> m_codeToIndex;

public:
    bool loadFromFile(const QString& filename, bool hasHeader = false, QChar delimiter=';');
    
    const Airport* findAirport(const QString& code) const;
    int getIndex(const QString& code) const;
    
    const QVector<Airport>& getAllAirports() const;
    int size() const;
    bool isEmpty() const;
    const Airport& getByIndex(int index) const;
};