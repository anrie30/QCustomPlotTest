#include "coordinatesgenerator.h"

CoordinatesGenerator::CoordinatesGenerator(GraphParameters param, uint interval, QObject *parent)
    : QObject(parent), interval_(interval), param_(param)
{
}

CoordinatesGenerator::~CoordinatesGenerator()
{
    timer_->stop();
    timer_->deleteLater();
}

void CoordinatesGenerator::generate()
{
    int x = QRandomGenerator::global()->bounded(param_.xLower, param_.xUpper);
    int y = QRandomGenerator::global()->bounded(param_.yLower, param_.yUpper);
    emit coordinatesReady(x, y);
}

void CoordinatesGenerator::start()
{
    qInfo() << "Запуск генерации координат";
    if(!timer_) {
        timer_ = new QTimer();
        connect(timer_, &QTimer::timeout, this, &CoordinatesGenerator::generate);
    }
    timer_->start(interval_);
}

void CoordinatesGenerator::pause()
{
    qInfo() << "Пауза";
    timer_->stop();
}

void CoordinatesGenerator::stop()
{
    qInfo() << "Остановка генерации";
    timer_->stop();
    emit stopped();
}
