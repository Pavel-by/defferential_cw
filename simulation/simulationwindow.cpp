#include "ui_simulationwindow.h"
#include "simulationwindow.h"

#include <drawer.h>

SimulationWindow::SimulationWindow(Polyhedron* poly, QWidget *parent) : QMainWindow(parent), ui(new Ui::SimulationWindow)
{
    setWindowTitle("Simulation");
    drawer = new Drawer();

    ui->setupUi(static_cast<QMainWindow*>(this));
    ui->verticalLayout->addWidget(drawer, 1);

    _iceberg = new Iceberg(poly);
    _water = new Water();
    drawer->addFigure(_iceberg);
    drawer->addFigure(_water);
    QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(stepForward()));
    _iceberg->startTimer();
}

SimulationWindow::~SimulationWindow() {
    delete ui;
    delete _iceberg;
    delete _water;
    delete drawer;
}

void SimulationWindow::stepForward() {
    float step = ui->doubleSpinBox->value();
   _iceberg->stepForward(step);
}
