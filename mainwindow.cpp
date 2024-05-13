#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    qRegisterMetaType<Config>("Config");

    setButtonsActions();

    load();

    ui->customPlot->addGraph();
    repaintGraph(graph_);
}


MainWindow::~MainWindow()
{
//    save();
    delete ui;
    thread_.quit();
    thread_.wait();
    if(dialog_)
        dialog_->deleteLater();
}


void MainWindow::onButtonStart()
{
    createGenerator();
    emit startGeneration();
}


void MainWindow::createGenerator()
{
    qInfo() << "Создание генератора координат";
    if(!generator_) {
        // Заполнение параметров для генерации
        GraphParameters params;
        params.xLower = ui->customPlot->xAxis->range().lower;
        params.xUpper = ui->customPlot->xAxis->range().upper;
        params.yLower = ui->customPlot->yAxis->range().lower;
        params.yUpper = ui->customPlot->yAxis->range().upper;

        // Инициализация генератора
        generator_ = new CoordinatesGenerator(params, graph_.generateInterval_);
        connect(generator_, &CoordinatesGenerator::coordinatesReady, this, &MainWindow::handleResult);
        connect(this, &MainWindow::startGeneration, generator_, &CoordinatesGenerator::start);
        connect(ui->buttonStop, &QPushButton::clicked, generator_, &CoordinatesGenerator::stop);
        connect(ui->buttonPause, &QPushButton::clicked, generator_, &CoordinatesGenerator::pause);
        connect(generator_, &CoordinatesGenerator::stopped, this, &MainWindow::onGeneratorStopped);

        // Запуск потока
        generator_->moveToThread(&thread_);
        connect(&thread_, &QThread::finished, generator_, &QObject::deleteLater);
        thread_.start();
    }
}


void MainWindow::handleResult(int x, int y)
{
    addGraphData(x, y);
}


void MainWindow::addGraphData(double x, double y)
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


void MainWindow::repaintGraph(const Config &config)
{
    // Определяем наименование осей, если установлено
    if(config.labelsVisible) {
        ui->customPlot->xAxis->setLabel(config.labelX);
        ui->customPlot->yAxis->setLabel(config.labelY);
    }

    // Установка диапазона значений
    ui->customPlot->xAxis->setRange(config.xLower, config.xUpper);
    ui->customPlot->yAxis->setRange(config.yLower, config.yUpper);

    // Цвет графика
    ui->customPlot->graph(0)->setPen(QPen(QColor(config.color)));

    // Отключение линий
    if(!config.linesVisible)
        ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);

    // Отображение значений в виде точек
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,
                                                              config.pointSize));
    ui->customPlot->replot();
}


void MainWindow::onGeneratorStopped()
{
    thread_.quit();
    thread_.wait();
    generator_ = nullptr;
    generator_ = nullptr;
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

    connect(ui->buttonStart, &QPushButton::clicked, [&](){ui->actionSave->setDisabled(true);});
    connect(ui->buttonStart, &QPushButton::clicked, [&](){ui->actionLoad->setDisabled(true);});
    connect(ui->buttonStart, &QPushButton::clicked, [&](){ui->actionSetting->setDisabled(true);});

    connect(ui->buttonStop, &QPushButton::clicked, ui->buttonStop, &QPushButton::hide);
    connect(ui->buttonStop, &QPushButton::clicked, ui->buttonStart, &QPushButton::show);
    connect(ui->buttonStop, &QPushButton::clicked, ui->buttonPause, &QPushButton::hide);

    connect(ui->buttonStop, &QPushButton::clicked, [&](){ui->actionSave->setEnabled(true);});
    connect(ui->buttonStop, &QPushButton::clicked, [&](){ui->actionLoad->setEnabled(true);});
    connect(ui->buttonStop, &QPushButton::clicked, [&](){ui->actionSetting->setEnabled(true);});

    connect(ui->buttonPause, &QPushButton::clicked, ui->buttonStart, &QPushButton::show);
    connect(ui->buttonPause, &QPushButton::clicked, ui->buttonStop, &QPushButton::show);
    connect(ui->buttonPause, &QPushButton::clicked, ui->buttonPause, &QPushButton::hide);

    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionLoad, &QAction::triggered, [&](){
        load();
        repaintGraph(graph_);
    });

    connect(ui->actionSetting, &QAction::triggered, [&](){
        dialog_ = new ConfigDialog(graph_, this);
//        ConfigDialog dialog(graph_, this);
//        connect(&dialog, &ConfigDialog::accepted, this, [&](Config conf){
//            repaintGraph(conf);
//            dialog.close();
//            dialog.deleteLater();
//        });
//        connect(&dialog, &ConfigDialog::rejected, [&](){
//            dialog.close();
//            dialog.deleteLater();
//        });
//        dialog.show();
        connect(dialog_, &ConfigDialog::accepted, this, [&](Config conf){
            repaintGraph(conf);
            dialog_->close();
            dialog_->deleteLater();
            dialog_ = nullptr;
        });
        connect(dialog_, &ConfigDialog::rejected, [&](){
            dialog_->close();
            dialog_->deleteLater();
            dialog_ = nullptr;
        });
        dialog_->show();
    });
}


void MainWindow::save()
{
    QSettings set(CONF_PATH, QSettings::IniFormat);
    set.setValue("geometry", saveGeometry());
    set.setValue(INTERVAL, graph_.generateInterval_);

    set.setValue(LABELS, graph_.labelsVisible);
    set.setValue(LABEL_X, graph_.labelX);
    set.setValue(LABEL_Y, graph_.labelY);

    set.setValue(RANGE_X_L, graph_.xLower);
    set.setValue(RANGE_X_U, graph_.xUpper);
    set.setValue(RANGE_Y_L, graph_.yLower);
    set.setValue(RANGE_Y_U, graph_.yUpper);

    set.setValue(COLOR, graph_.color);
    set.setValue(LINES, graph_.linesVisible);
    set.setValue(POINTS, graph_.pointSize);
}


void MainWindow::load()
{
    QSettings set(CONF_PATH, QSettings::IniFormat);

    restoreGeometry(set.value("geometry").toByteArray());
    graph_.generateInterval_ = set.value(INTERVAL).toUInt();

    graph_.labelsVisible = set.value(LABELS).toBool();
    graph_.labelX = set.value(LABEL_X).toString();
    graph_.labelY = set.value(LABEL_Y).toString();

    graph_.xLower = set.value(RANGE_X_L).toDouble();
    graph_.xUpper = set.value(RANGE_X_U).toDouble();
    graph_.yLower = set.value(RANGE_Y_L).toDouble();
    graph_.yUpper = set.value(RANGE_Y_U).toDouble();

    graph_.color = set.value(COLOR).toString();
    graph_.linesVisible = set.value(LINES).toBool();
    graph_.pointSize = set.value(POINTS).toDouble();
}
