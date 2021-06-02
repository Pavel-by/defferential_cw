#ifndef UNDERWATER_H
#define UNDERWATER_H

#include <figure.h>
#include <simulation/iceberg.h>


class Underwater : public Figure
{
    //Q_OBJECT

public:
    Underwater();
    ~Underwater() override;

    void attachIceberg(Iceberg* iceberg);
    void detachIceberg();

private slots:
    void icebergChanged();

private:
    Iceberg* _iceberg = nullptr;
};

#endif // UNDERWATER_H
