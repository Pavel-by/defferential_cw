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
    QObject::connect(ui->underwaterCheckBox, SIGNAL(clicked()), this, SLOT(underwaterClicked()));
    _iceberg->startTimer();
}

SimulationWindow::~SimulationWindow() {
    delete ui;
    delete _iceberg;
    delete _water;
    delete drawer;
}

void SimulationWindow::underwaterClicked() {
    _iceberg->setDrawRibs(ui->underwaterCheckBox->isChecked());
}
