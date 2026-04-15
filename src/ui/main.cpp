#include <QApplication>
#include "MainWindow.h"
#include "AirportTableModel.h"
#include "../core/AirportDatabase.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    auto controller = std::make_unique<AppController>();

    AirportTableModel model;
    model.setController(controller.get());

    MainWindow w;
    w.setModel(&model);
    w.setController(controller.get());

    w.show();

    return app.exec();
}