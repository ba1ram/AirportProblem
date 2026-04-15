#include "AppController.h"


AppController::AppController() {
    calculator = std::make_unique<RouteCalculator>(db, 1000.0);
}

bool AppController::loadFile(const QString& path){
    bool ok = db.loadFromFile(path, true, '\t');
    return ok;
}

std::vector<std::pair<double, Airport>> AppController::findNear(const Airport& target, double radiusKm){
    return calculator->findNearby(target, radiusKm);
}


RouteCalculator::PathResult
AppController::findRoute(const QString& from, const QString& to, double maxRange)
{
    if (!calculator) {
        return {{}, 0.0, false};
    }

    calculator->setMaxRange(maxRange);

    return calculator->findShortestPath(from, to);
}

const QVector<Airport>& AppController::getAirports() const {
    return db.getAllAirports();
}
const Airport* AppController::getAirportByCode(const QString& code) const {
    for (const auto& a : db.getAllAirports()) {
        if (a.code() == code)
            return &a;
    }
    return nullptr;
}