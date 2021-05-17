#include "editwindow.h"

#include <simulation/simulationwindow.h>

#include <QFileDialog>

#include <utils/figureserializer.h>

EditWindow::EditWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::EditWindow)
{
    setWindowTitle("Preparing figure");
    _drawer = new Drawer();

    ui->setupUi(static_cast<QMainWindow*>(this));
    ui->verticalLayout->addWidget(_drawer, 1);

    _iceberg = new Iceberg();
    _water = new Water();
    _drawer->addFigure(_iceberg);
    _drawer->addFigure(_water);
    _drawer->figureWrapper().figure = _iceberg;
    QObject::connect(ui->launchSimulationButton, SIGNAL(clicked()), this, SLOT(launchSimulation()));
    QObject::connect(ui->loadFigureButton, SIGNAL(clicked()), this, SLOT(loadFigure()));
    QObject::connect(ui->saveFigureButton, SIGNAL(clicked()), this, SLOT(saveFigure()));
}

EditWindow::~EditWindow() {
    delete ui;
    delete _water;
    delete _iceberg;
    delete _drawer;
}

void EditWindow::launchSimulation() {
    SimulationWindow * sim = new SimulationWindow(_iceberg->generatePoly());
    sim->show();
}

void EditWindow::loadFigure() {
    QString filename = QFileDialog::getOpenFileName(this, "Load figure");
    FigureSerializer().load(filename, _iceberg);
}

void EditWindow::saveFigure() {
    QString filename = QFileDialog::getSaveFileName(this, "Load figure");
    FigureSerializer().save(_iceberg, filename);
}
