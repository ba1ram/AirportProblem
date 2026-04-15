#include "MainWindow.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

// ─────────────────────────────────────────
//  Конструктор и инициализация
// ─────────────────────────────────────────

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    resize(kWindowWidth, kWindowHeight);
    setupUI();
}

void MainWindow::setupUI()
{
    setWindowTitle(tr("Airports"));
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* layout = new QGridLayout(central);
    layout->setContentsMargins(kLayoutMargin, kLayoutMargin,
                               kLayoutMargin, kLayoutMargin);
    layout->setRowStretch(0, 1);

    setupTable();
    layout->addWidget(m_table, 0, 0);

    auto* buttonLayout = new QHBoxLayout();
    setupButtons(buttonLayout);
    layout->addLayout(buttonLayout, 1, 0);
}

void MainWindow::setupTable()
{
    m_table = new QTableView(this);
    m_table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
}

void MainWindow::setupButtons(QLayout* parent)
{
    auto* hLayout = qobject_cast<QHBoxLayout*>(parent);

    // --- Загрузить файл ---
    m_fileLoad = new QPushButton(tr("Загрузить данные из файла"), this);
    m_fileLoad->setFlat(true);
    m_fileLoad->setCursor(Qt::PointingHandCursor);
    m_fileLoad->setStyleSheet(R"(
        QPushButton {
            border: none;
            background: transparent;
            color: #2677f1;
            text-decoration: underline;
        }
        QPushButton:hover { color: #1a5cc1; }
    )");

    // --- Back ---
    m_backBtn = new QPushButton(tr("Back"), this);
    m_backBtn->setFixedHeight(kButtonHeight);
    m_backBtn->setVisible(false);
    m_backBtn->setEnabled(false);

    // --- Near ---
    m_nearBtn = new QPushButton(tr("Near"), this);
    m_nearBtn->setFixedHeight(kButtonHeight);
    m_nearBtn->setEnabled(false);

    // --- Route ---
    m_routeBtn = new QPushButton(tr("Route"), this);
    m_routeBtn->setFixedHeight(kButtonHeight);
    m_routeBtn->setEnabled(false);

    hLayout->addWidget(m_fileLoad);
    hLayout->addStretch();
    hLayout->addWidget(m_routeBtn);
    hLayout->addWidget(m_nearBtn);
    hLayout->addWidget(m_backBtn);

    connect(m_fileLoad,  &QPushButton::clicked, this, &MainWindow::onLoadFileClicked);
    connect(m_backBtn,   &QPushButton::clicked, this, &MainWindow::onBackClicked);
    connect(m_nearBtn,   &QPushButton::clicked, this, &MainWindow::onFindNearClicked);
    connect(m_routeBtn,  &QPushButton::clicked, this, &MainWindow::onFindRouteClicked);
}

void MainWindow::connectSignals()
{
    connect(m_table->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &MainWindow::onSelectionChanged);
}

// ─────────────────────────────────────────
//  Внешние зависимости
// ─────────────────────────────────────────

void MainWindow::setModel(AirportTableModel* model)
{
    m_model = model;
    m_table->setModel(model);
    connectSignals();        
    updateUIState();
}

void MainWindow::setController(AppController* controller)
{
    m_controller = controller;
}

// ─────────────────────────────────────────
//  Обновление UI
// ─────────────────────────────────────────

void MainWindow::updateUIState()
{
    const bool hasSel = hasValidSelection();



    switch (m_routeState) {
    case RouteState::Idle:
        m_nearBtn->setEnabled(hasSel);
        m_routeBtn->setEnabled(hasSel);
        break;

    case RouteState::SelectingB:
        // кнопка Route активна только когда выбран аэропорт B ≠ A
        const bool readyToRoute = m_airportB
                               && m_airportA
                               && m_airportB->code() != m_airportA->code();
        m_routeBtn->setEnabled(readyToRoute);
        break;
    }
}

void MainWindow::applyNearbyResultsUI()
{
    m_fileLoad->setVisible(false);
    m_fileLoad->setEnabled(false);
    m_nearBtn->setVisible(false);
    m_nearBtn->setEnabled(false);
    m_routeBtn->setVisible(false);
    m_routeBtn->setEnabled(false);
    m_backBtn->setVisible(true);
    m_backBtn->setEnabled(true);
}

void MainWindow::applyRouteSelectingBUI()
{
    m_fileLoad->setVisible(false);
    m_fileLoad->setEnabled(false);
    m_nearBtn->setVisible(false);
    m_nearBtn->setEnabled(false);
    m_routeBtn->setEnabled(false);
    m_routeBtn->setText(tr("Ready"));
    m_backBtn->setVisible(true);
    m_backBtn->setEnabled(true);
    setWindowTitle(tr("Select arrival airport"));
}

void MainWindow::applyRouteResultUI()
{
    m_fileLoad->setVisible(false);
    m_fileLoad->setEnabled(false);
    m_nearBtn->setVisible(false);
    m_nearBtn->setEnabled(false);
    m_routeBtn->setVisible(false);
    m_routeBtn->setEnabled(false);
    m_backBtn->setVisible(true);
    m_backBtn->setEnabled(true);
}

void MainWindow::resetToIdleUI()
{
    m_routeState = RouteState::Idle;
    m_airportA   = nullptr;
    m_airportB   = nullptr;

    setWindowTitle(tr("Airports"));
    m_routeBtn->setText(tr("Route"));
    m_routeBtn->setVisible(true);
    m_nearBtn->setVisible(true);
    m_backBtn->setVisible(false);
    m_backBtn->setEnabled(false);
    m_fileLoad->setVisible(true);
    m_fileLoad->setEnabled(true);

    m_table->clearSelection();
    m_table->selectionModel()->clearSelection();
    m_table->setCurrentIndex(QModelIndex());

    updateUIState();
}

// ─────────────────────────────────────────
//  Слоты
// ─────────────────────────────────────────

void MainWindow::onLoadFileClicked()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Выберите CSV файл"),
        {},
        tr("CSV файлы (*.csv);;Все файлы (*.*)")
    );

    if (filePath.isEmpty())
        return;

    qDebug() << "Выбран файл:" << filePath;
    m_controller->loadFile(filePath);
    m_model->refresh();
}

void MainWindow::onBackClicked()
{
    m_model->showAll();
    resetToIdleUI();
}

void MainWindow::onFindNearClicked()
{
    if (!m_table || !m_model || !m_controller)
        return;

    const Airport* airport = m_model->airportAt(currentIndex().row());
    if (!airport)
        return;

    bool ok = false;
    const double radiusKm = QInputDialog::getDouble(
        this,
        tr("Поиск аэропортов"),
        tr("Введите радиус (км):"),
        kRadiusDefault, kRadiusMin, kRadiusMax,
        /*decimals=*/1,
        &ok
    );

    if (!ok)
        return;

    const auto results = m_controller->findNear(*airport, radiusKm);
    m_model->setNearby(results);
    setWindowTitle(
        tr("Nearby airports: %1 km around %2")
            .arg(radiusKm, 0, 'f', 1)
            .arg(airport->code())
    );
    applyNearbyResultsUI();
}

void MainWindow::onFindRouteClicked()
{
    // ── Фаза 1: начало выбора маршрута ───────────────────────────────────
    if (m_routeState == RouteState::Idle) {
        m_airportA = m_model->airportAt(currentIndex().row());
        if (!m_airportA)
            return;

        m_routeState = RouteState::SelectingB;
        applyRouteSelectingBUI();
        return;
    }

    // ── Фаза 2: точки A и B выбраны — строим маршрут ─────────────────────
    if (m_routeState == RouteState::SelectingB) {
        if (!m_airportA || !m_airportB)
            return;

        bool ok = false;
        const double maxDist = QInputDialog::getDouble(
            this,
            tr("Max distance"),
            tr("Введите M (км):"),
            kMaxDistDefault, kRadiusMin, kRadiusMax,
            /*decimals=*/1,
            &ok
        );

        if (!ok)
            return;

        const auto result = m_controller->findRoute(
            m_airportA->code(),
            m_airportB->code(),
            maxDist
        );

        if (!result.reachable) {
            QMessageBox::warning(this, tr("Route"), tr("Маршрут невозможен"));
            return;
        }

        // Собираем вектор Airport из кодов
        std::vector<Airport> route;
        route.reserve(result.airports.size());
        for (const auto& code : result.airports) {
            if (const Airport* a = m_controller->getAirportByCode(code))
                route.push_back(*a);
        }

        m_model->setRoute(route);
        setWindowTitle(
            tr("Route: %1 → %2 (%3 km)")
                .arg(m_airportA->code())
                .arg(m_airportB->code())
                .arg(result.totalDistance, 0, 'f', 2)
        );
        applyRouteResultUI();

        // Сбрасываем внутреннее состояние (UI уже установлен выше)
        m_routeState = RouteState::Idle;
        m_airportA   = nullptr;
        m_airportB   = nullptr;
    }
}

void MainWindow::onSelectionChanged()
{
    if (!m_table || !m_model)
        return;

    if (m_routeState == RouteState::SelectingB)
        m_airportB = m_model->airportAt(currentIndex().row());

    updateUIState();
}

// ─────────────────────────────────────────
//  Вспомогательные методы
// ─────────────────────────────────────────

QModelIndex MainWindow::currentIndex() const
{
    return m_table->currentIndex();
}

bool MainWindow::hasValidSelection() const
{
    return currentIndex().isValid();
}