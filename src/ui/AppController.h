#pragma once

#include "../core/AirportDatabase.h"
#include "../core/RouteCalculator.h"

class AppController {
public:
    AppController();

    bool loadFile(const QString& path);
    std::vector<std::pair<double, Airport>> findNear(const Airport& target, double radiusKm);
    RouteCalculator::PathResult findRoute(const QString& from, const QString& to, double maxRange);


    const QVector<Airport>& getAirports() const;
    const Airport* getAirportByCode(const QString& code) const;
private:
    AirportDatabase db;
    std::unique_ptr<RouteCalculator> calculator;
};