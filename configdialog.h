#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

struct Config
{
    bool labelsVisible;     // Видимые названия осей
    QString labelX;         // Наименование осей
    QString labelY;
    int xUpper;             // Диапазон значений осей
    int xLower;
    int yUpper;
    int yLower;
    QString color;          // Цвет графика
    bool linesVisible;      // Видимые линии
    double pointSize;       // Размер точек координат
    uint generateInterval_; // Временой интервал генерирования координат
};

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(const Config &config, QWidget *parent = nullptr);
    ~ConfigDialog();

private:
    Ui::ConfigDialog *ui;

    void setFormData(const Config &config);
    void onAcceptClicked();

signals:
    void accepted(Config);
};

#endif // CONFIGDIALOG_H
