#include "ui_simulationwindow.h"
#include "simulationwindow.h"

#include <drawer.h>

SimulationWindow::SimulationWindow(Polyhedron* poly, QWidget *parent) : QMainWindow(parent), ui(new Ui::SimulationWindow)
{
    ui->setupUi(static_cast<QMainWindow*>(this));
    setWindowTitle("Simulation");
    drawer = new Drawer();

    ui->verticalLayout->addWidget(drawer, 1);

    _iceberg = new Iceberg(poly);
    _water = new Water();
    _underwater = new Underwater();
    _underwater->attachIceberg(_iceberg);
    drawer->addFigure(_iceberg);
    drawer->addFigure(_underwater);
    drawer->addFigure(_water);

    QObject::connect(ui->underwaterCheckBox, SIGNAL(clicked()), this, SLOT(underwaterClicked()));
    QObject::connect(ui->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(speedChanged(int)));
    _iceberg->startTimer();

    underwaterClicked();
    speedChanged(ui->speedSlider->value());
}

SimulationWindow::~SimulationWindow() {
    delete ui;
    delete _iceberg;
    delete _water;
    delete drawer;
}

void SimulationWindow::underwaterClicked() {
    bool underwaterEnabled = ui->underwaterCheckBox->isChecked();
    _iceberg->setDrawRibs(underwaterEnabled);
    _underwater->setVisible(underwaterEnabled);
}

void SimulationWindow::speedChanged(int sliderValue) {
    float speed = static_cast<float>(sliderValue) / 20;
    _iceberg->setSpeed(speed);
}
