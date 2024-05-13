#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include <QSettings>
#include "configdialog.h"
#include "qcustomplot.h"
#include "coordinatesgenerator.h"

#define CONF_PATH "../conf/settings.ini"
#define LABELS    "setLabels"
#define LABEL_X   "labelX"
#define LABEL_Y   "labelY"
#define RANGE_X_U "rangeX_upper"
#define RANGE_X_L "rangeX_lower"
#define RANGE_Y_U "rangeY_upper"
#define RANGE_Y_L "rangeY_lower"
#define COLOR     "graphColor"
#define LINES     "setLines"
#define POINTS    "pointSize"
#define INTERVAL  "timer_interval"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ConfigDialog* dialog_ = nullptr;
    QThread thread_;

    CoordinatesGenerator *generator_ = nullptr;  // Генерирует значение для обеих осей, получается случайно запутанный график
    Config graph_;

    void createGraph();                         // Инициализация графика
    void createGenerator();                     // Инициализация генератора координат
    void addGraphData(double x, double y);       // Отрисовка занчений на графике
    void repaintGraph(const Config &config);
    void setButtonsActions();
    void save();                                // Сохранение параметров графика
    void load();                                // Загрузка параметров

private slots:
    void handleResult(int x, int y);                     // Прием координат
    void onButtonStart();
    void onGeneratorStopped();

signals:
    void startGeneration();
};
#endif // MAINWINDOW_H
