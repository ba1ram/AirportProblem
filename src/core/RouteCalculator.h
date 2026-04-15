#pragma once

#include "AirportDatabase.h"
#include <vector>
#include <QString>

class RouteCalculator {
public:

    struct PathResult {
        std::vector<QString> airports;
        double totalDistance;
        bool reachable;
    };

    RouteCalculator(const AirportDatabase& db, double maxRange);
    
    void setMaxRange(double maxRange);
    
    // Пункт 2 задания — аэродромы ближе radiusKm к target
    std::vector<std::pair<double, Airport>> findNearby(const Airport& target, double radiusKm) const;
    
    // Пункт 3 задания — кратчайший путь с ограничением дальности m_maxRangeKm
    PathResult findShortestPath(const QString& from, const QString& to) const;

private:
    std::vector<std::vector<std::pair<int, double>>> m_graph;

    const AirportDatabase& m_db;
    double m_maxRangeKm;
    
    
    void buildGraph();
};