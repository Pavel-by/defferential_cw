#pragma once

#include <QObject>

class Figure : QObject
{
    Q_OBJECT

signals:
    void changed();

public:
    Figure();


};

