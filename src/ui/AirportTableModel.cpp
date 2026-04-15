#include "AirportTableModel.h"

AirportTableModel::AirportTableModel(QObject* parent)
    : QAbstractTableModel(parent) {}

void AirportTableModel::setController(AppController* contr) {
    beginResetModel();
    controller = contr;
    endResetModel();
}

int AirportTableModel::rowCount(const QModelIndex&) const {
    if (!controller) return 0;

    if (m_mode == Mode::AllAirports)
        return controller->getAirports().size();
    else if (m_mode == Mode::Route)
        return m_routeAirports.size();
    else
        return m_nearby.size();
}

int AirportTableModel::columnCount(const QModelIndex&) const {
    if (m_mode == Mode::AllAirports)
        return 3;
    else
        return 4;
}

QVariant AirportTableModel::data(const QModelIndex& index, int role) const {
    if (!controller || !index.isValid() || role != Qt::DisplayRole)
        return {};

    // if (!controller || !index.isValid())
    //     return {};
    // // 👉 СНАЧАЛА ОБРАБАТЫВАЕМ НЕ DisplayRole
    // if (role == Qt::TextAlignmentRole) {
    //     if (m_mode == Mode::Route && index.column() == 0)
    //         return Qt::AlignCenter;
    // }

    // // 👉 ПОТОМ DisplayRole
    // if (role != Qt::DisplayRole)
    //     return {};

    // ===== РЕЖИМ: ВСЕ АЭРОПОРТЫ =====
    if (m_mode == Mode::AllAirports) {
        const auto& airports = controller->getAirports();

        if (index.row() >= airports.size())
            return {};

        const Airport& a = airports[index.row()];

        switch (index.column()) {
            case 0: return a.code();
            case 1: return QString::number(a.latitude(), 'f', 10);
            case 2: return QString::number(a.longitude(), 'f', 10);
        }
    }

    // ===== РЕЖИМ: NEARBY =====
    else if (m_mode == Mode::Nearby) {
        if (index.row() >= m_nearby.size())
            return {};

        const auto& pair = m_nearby[index.row()];
        const Airport& a = pair.second;

        switch (index.column()) {
            case 0: return a.code();
            case 1: return QString::number(a.latitude(), 'f', 10);
            case 2: return QString::number(a.longitude(), 'f', 10);
            case 3:
                if (m_mode == Mode::Nearby)
                    return QString::number(pair.first, 'f', 2);
        }
    }

    else if (m_mode == Mode::Route) {
        const auto& a = m_routeAirports[index.row()];

        if (index.column() == 0) return index.row() + 1;
        if (index.column() == 1) return a.code();
        if (index.column() == 2) return QString::number(a.latitude(), 'f', 10);
        if (index.column() == 3) return QString::number(a.longitude(), 'f', 10);
    }

    return {};
}

QVariant AirportTableModel::headerData(int section,
                                       Qt::Orientation orientation,
                                       int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    if (m_mode == Mode::AllAirports) {
        switch (section) {
            case 0: return "Code";
            case 1: return "Latitude";
            case 2: return "Longitude";
        }
    }
    else if (m_mode == Mode::Nearby) {
        switch (section) {
            case 0: return "Code";
            case 1: return "Latitude";
            case 2: return "Longitude";
            case 3: return "Distance (km)";
        }
    }
    else if (m_mode == Mode::Route) {
        switch (section) {
            case 0: return "Step";
            case 1: return "Code";
            case 2: return "Latitude";
            case 3: return "Longitude";
        }
    }

    return {};
}

const Airport* AirportTableModel::airportAt(int row) const {
    if (!controller || row < 0)
        return nullptr;

    // ===== ALL =====
    if (m_mode == Mode::AllAirports) {
        const auto& airports = controller->getAirports();
        if (row >= airports.size()) return nullptr;
        return &airports[row];
    }

    // ===== NEARBY =====
    if (m_mode == Mode::Nearby) {
        if (row >= m_nearby.size()) return nullptr;
        return &m_nearby[row].second;
    }

    // ===== ROUTE =====
    if (m_mode == Mode::Route) {
        if (row >= m_routeAirports.size()) return nullptr;
        return &m_routeAirports[row];
    }

    return nullptr;
}
void AirportTableModel::setNearby(const std::vector<std::pair<double, Airport>>& data) {
    beginResetModel();
    m_mode = Mode::Nearby;
    m_nearby = data;
    endResetModel();
}

void AirportTableModel::setRoute(const std::vector<Airport>& route) {
    beginResetModel();
    m_mode = Mode::Route;
    m_routeAirports = route;
    endResetModel();
}

void AirportTableModel::showAll() {
    beginResetModel();
    m_mode = Mode::AllAirports;
    m_nearby.clear();
    endResetModel();
}


void AirportTableModel::refresh() {
    beginResetModel();
    endResetModel();
}