#include "editwindow.h"

#include <simulation/simulationwindow.h>

EditWindow::EditWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::EditWindow)
{
    _drawer = new Drawer();

    ui->setupUi(static_cast<QMainWindow*>(this));
    ui->verticalLayout->addWidget(_drawer, 1);

    _iceberg = new Iceberg();
    _water = new Water();
    _drawer->addFigure(_iceberg);
    _drawer->addFigure(_water);
    _drawer->figureWrapper().figure = _iceberg;
    QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(launchSimulation()));
}

EditWindow::~EditWindow() {
    delete ui;
    delete _water;
    delete _iceberg;
    delete _drawer;
}

void EditWindow::launchSimulation() {
    _drawer->removeFigure(_iceberg);
    SimulationWindow * sim = new SimulationWindow(_iceberg);
    _iceberg = nullptr;
    sim->show();
    this->hide();
}
