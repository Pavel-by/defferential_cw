#ifndef UNDERWATER_H
#define UNDERWATER_H

#include <figure.h>
#include <simulation/iceberg.h>


class Underwater : public Figure
{
    Q_OBJECT

public slots:
    void icebergChanged();

public:
    Underwater();
    ~Underwater() override;

    void attachIceberg(Iceberg* iceberg);
    void detachIceberg();

private:
    Iceberg* _iceberg = nullptr;
};

#endif // UNDERWATER_H
