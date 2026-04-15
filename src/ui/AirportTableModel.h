#pragma once

#include <QAbstractTableModel>
#include "../core/AirportDatabase.h"
#include "AppController.h"

class AirportTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit AirportTableModel(QObject* parent = nullptr);

    void setController(AppController* controller);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    const Airport* airportAt(int row) const;

    void setNearby(const std::vector<std::pair<double, Airport>>& data);
    void setRoute(const std::vector<Airport>& route);
    void showAll();

    void refresh();
private:
    enum class Mode {
        AllAirports,
        Nearby,
        Route
    };

    Mode m_mode = Mode::AllAirports;
    std::vector<std::pair<double, Airport>> m_nearby;
    std::vector<Airport> m_routeAirports;

    AppController* controller = nullptr;
};