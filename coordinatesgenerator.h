#ifndef COORDINATESGENERATOR_H
#define COORDINATESGENERATOR_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>

struct GraphParameters
{
    int xUpper;
    int xLower;
    int yUpper;
    int yLower;
};

class CoordinatesGenerator : public QObject
{
    Q_OBJECT

    QTimer *timer_ = nullptr;
    uint interval_;
    GraphParameters param_;

public:
    CoordinatesGenerator(GraphParameters param, uint interval, QObject *parent = nullptr);
    ~CoordinatesGenerator();

private:
    void generate();

public slots:
    void start();
    void pause();
    void stop();

signals:
    void coordinatesReady(int,int);
    void stopped();
};

#endif // COORDINATESGENERATOR_H
