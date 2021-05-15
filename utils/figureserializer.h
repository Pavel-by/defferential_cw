#include <figure.h>

#pragma once


class FigureSerializer
{
public:
    FigureSerializer();

    void load(const QString& filename, Figure* figure);
    void save(Figure *figure, const QString& filename);

private:
    void showDialog(const QString& message);
};

