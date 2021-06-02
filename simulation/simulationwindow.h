#ifndef SIMULATIONWINDOW_H
#define SIMULATIONWINDOW_H

#include <QMainWindow>
#include <drawer.h>
#include "iceberg.h"
#include "water.h"
#include <simulation/underwater.h>

QT_BEGIN_NAMESPACE
namespace Ui { class SimulationWindow; }
QT_END_NAMESPACE

class SimulationWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void underwaterClicked();
    void speedChanged(int speed);

public:
    explicit SimulationWindow(Polyhedron* poly, QWidget *parent = nullptr);
    ~SimulationWindow() override;

protected:
    Ui::SimulationWindow *ui;

private:
    Drawer *drawer;
    Iceberg* _iceberg;
    Water *_water;
    Underwater *_underwater;
};

#endif // SIMULATIONWINDOW_H
