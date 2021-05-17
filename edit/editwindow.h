#pragma once

#include <QMainWindow>
#include <drawer.h>

#include <simulation/iceberg.h>
#include <simulation/water.h>
#include "ui_editwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class EditWindow; }
QT_END_NAMESPACE

class EditWindow : public QMainWindow
{
    Q_OBJECT

public:
    EditWindow(QWidget *parent = nullptr);
    ~EditWindow() override;

public slots:
    void launchSimulation();
    void loadFigure();
    void saveFigure();
    void heightChanged();

protected:
    Ui::EditWindow *ui;

private:
    Iceberg *_iceberg;
    Water *_water;
    Drawer *_drawer;
};

