#pragma once

#include <QMainWindow>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QFrame>

#include "AirportTableModel.h"
#include "AppController.h"


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    void setModel(AirportTableModel* model);
    void setController(AppController* controller);

private slots:
    void onLoadFileClicked();
    void onBackClicked();
    void onFindNearClicked();
    void onFindRouteClicked();
    void onSelectionChanged();

private:
    // --- Константы ---
    static constexpr int    kWindowWidth      = 900;
    static constexpr int    kWindowHeight     = 600;
    static constexpr int    kButtonHeight     = 50;
    static constexpr int    kLayoutMargin     = 20;
    static constexpr double kRadiusDefault    = 100.0;
    static constexpr double kRadiusMin        = 1.0;
    static constexpr double kRadiusMax        = 20000.0;
    static constexpr double kMaxDistDefault   = 1000.0;

    enum class RouteState { Idle, SelectingB };

    // --- Инициализация ---
    void setupUI();
    void setupTable();
    void setupButtons(QLayout* parent);
    void connectSignals();

    // --- Обновление UI ---
    void updateUIState();
    void applyNearbyResultsUI();
    void applyRouteSelectingBUI();
    void applyRouteResultUI();
    void resetToIdleUI();

    // --- Вспомогательное ---
    QModelIndex currentIndex() const;
    bool        hasValidSelection() const;

    // --- Виджеты ---
    QTableView* m_table   = nullptr;
    QPushButton* m_fileLoad = nullptr;
    QPushButton* m_backBtn  = nullptr;
    QPushButton* m_nearBtn  = nullptr;
    QPushButton* m_routeBtn = nullptr;

    // --- Зависимости ---
    AirportTableModel* m_model      = nullptr;
    AppController*     m_controller = nullptr;

    // --- Состояние ---
    RouteState    m_routeState = RouteState::Idle;
    const Airport* m_airportA  = nullptr;
    const Airport* m_airportB  = nullptr;
};