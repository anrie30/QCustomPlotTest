#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setButtonsActions();
    load();
    createGraph();
}

MainWindow::~MainWindow()
{
    save();
    delete ui;
    thread.quit();
    thread.wait();
}

void MainWindow::createGraph()
{
    qInfo() << "Создание графика";
    QSettings set(CONF_PATH, QSettings::IniFormat);
    ui->customPlot->addGraph();

    // Устанавливаем стрелки на концах осей
    ui->customPlot->xAxis->setUpperEnding(QCPLineEnding::esFlatArrow);
    ui->customPlot->yAxis->setUpperEnding(QCPLineEnding::esFlatArrow);

    // Определяем наименование осей, если установлено
    if(set.value(LABELS).toBool()) {
        ui->customPlot->xAxis->setLabel(set.value(LABEL_X).toString());
        ui->customPlot->yAxis->setLabel(set.value(LABEL_Y).toString());
    }

    // Установка диапазона значений
    ui->customPlot->xAxis->setRange(set.value(RANGE_X_L).toDouble(), set.value(RANGE_X_U).toDouble());
    ui->customPlot->yAxis->setRange(set.value(RANGE_Y_L).toDouble(), set.value(RANGE_Y_U).toDouble());

    // Цвет графика
//    color_.setNamedColor(set.value(COLOR).toString());
    ui->customPlot->graph(0)->setPen(QPen(QColor(set.value(COLOR).toString())));

    // Отключение линий
    if(!set.value(LINES).toBool())
        ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);

    // Отображение значений в виде точек
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,
                                                              set.value(POINTS).toDouble()));

    ui->customPlot->replot();
}

void MainWindow::onButtonStart()
{
    createGenerator();
    emit startGeneration();
}

void MainWindow::createGenerator()
{
    qInfo() << "Создание генератора координат";
    if(!generator) {
        // Заполнение параметров для генерации
        GraphParameters params;
        params.xLower = ui->customPlot->xAxis->range().lower;
        params.xUpper = ui->customPlot->xAxis->range().upper;
        params.yLower = ui->customPlot->yAxis->range().lower;
        params.yUpper = ui->customPlot->yAxis->range().upper;

        // Инициализация генератора
        generator = new CoordinatesGenerator(params, generateInterval_);
        connect(generator, &CoordinatesGenerator::coordinatesReady, this, &MainWindow::handleResult);
        connect(this, &MainWindow::startGeneration, generator, &CoordinatesGenerator::start);
        connect(ui->buttonStop, &QPushButton::clicked, generator, &CoordinatesGenerator::stop);
        connect(ui->buttonPause, &QPushButton::clicked, generator, &CoordinatesGenerator::pause);
        connect(generator, &CoordinatesGenerator::stopped, this, &MainWindow::onGeneratorStopped);

        // Запуск потока
        generator->moveToThread(&thread);
        connect(&thread, &QThread::finished, generator, &QObject::deleteLater);
        thread.start();
    }
}

void MainWindow::handleResult(int x, int y)
{
    updateGraph(x, y);
}

void MainWindow::updateGraph(double x, double y)
{
    ui->customPlot->graph(0)->addData(x, y);

    // Расширение границ графика, если значения выходят за границы
    if(ui->customPlot->xAxis->range().lower > x)
        ui->customPlot->xAxis->setRangeLower(x);
    else if(ui->customPlot->xAxis->range().upper < x)
        ui->customPlot->xAxis->setRangeUpper(x);

    if(ui->customPlot->yAxis->range().lower > y)
        ui->customPlot->yAxis->setRangeLower(y);
    else if(ui->customPlot->yAxis->range().upper < y)
        ui->customPlot->yAxis->setRangeUpper(y);

    ui->customPlot->replot();
}

void MainWindow::onGeneratorStopped()
{
    thread.quit();
    thread.wait();
    generator = nullptr;
    generator = nullptr;
    ui->customPlot->graph(0)->clear();
}

void MainWindow::setButtonsActions()
{
    ui->buttonPause->hide();
    ui->buttonStop->hide();
    connect(ui->buttonStart, &QPushButton::clicked, this, &MainWindow::onButtonStart);
    connect(ui->buttonStart, &QPushButton::clicked, ui->buttonStop, &QPushButton::show);
    connect(ui->buttonStart, &QPushButton::clicked, ui->buttonPause, &QPushButton::show);
    connect(ui->buttonStart, &QPushButton::clicked, ui->buttonStart, &QPushButton::hide);
    connect(ui->buttonStop, &QPushButton::clicked, ui->buttonStop, &QPushButton::hide);
    connect(ui->buttonStop, &QPushButton::clicked, ui->buttonStart, &QPushButton::show);
    connect(ui->buttonStop, &QPushButton::clicked, ui->buttonPause, &QPushButton::hide);
    connect(ui->buttonPause, &QPushButton::clicked, ui->buttonStart, &QPushButton::show);
    connect(ui->buttonPause, &QPushButton::clicked, ui->buttonStop, &QPushButton::show);
    connect(ui->buttonPause, &QPushButton::clicked, ui->buttonPause, &QPushButton::hide);
}

void MainWindow::save()
{
    QSettings set(CONF_PATH, QSettings::IniFormat);
    set.setValue("geometry", saveGeometry());
}

void MainWindow::load()
{
    QSettings set(CONF_PATH, QSettings::IniFormat);
    restoreGeometry(set.value("geometry").toByteArray());
    generateInterval_ = set.value(INTERVAL).toUInt();
}
