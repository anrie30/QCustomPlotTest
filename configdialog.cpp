#include "configdialog.h"
#include "ui_configdialog.h"

ConfigDialog::ConfigDialog(const Config &config, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    setModal(true);
//    qRegisterMetaType<Config>("Config");
    setFormData(config);

    connect(ui->checkLabels, &QCheckBox::stateChanged, [&](int state){
        if(state) {
            ui->textLabelX->setEnabled(true);
            ui->textLabelY->setEnabled(true);
            ui->labelNames->setEnabled(true);
        } else {
            ui->textLabelX->setDisabled(true);
            ui->textLabelY->setDisabled(true);
            ui->labelNames->setDisabled(true);
        }
    });

    connect(ui->buttonAccept, &QPushButton::clicked, this, &ConfigDialog::onAcceptClicked);
    connect(ui->buttonReject, &QPushButton::clicked, [&](){emit rejected();});
//    connect(ui->buttonReject, &QPushButton::clicked, [&](){
//        emit rejected();
//        deleteLater();
//        close();
//    });
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::setFormData(const Config &config)
{
    ui->checkLabels->setChecked(config.labelsVisible);
    ui->textLabelX->setText(config.labelX);
    ui->textLabelY->setText(config.labelY);

    if(!config.labelsVisible) {
        ui->textLabelX->setDisabled(true);
        ui->textLabelY->setDisabled(true);
        ui->labelNames->setDisabled(true);
    }

    ui->labelX->setText("Диапазон " + config.labelX);
    ui->labelY->setText("Диапазон " + config.labelY);
    ui->spinRangeXLower->setValue(config.xLower);
    ui->spinRangeXUpper->setValue(config.xUpper);
    ui->spinRangeYLower->setValue(config.yLower);
    ui->spinRangeYUpper->setValue(config.yUpper);
    ui->checkLines->setChecked(config.linesVisible);
    ui->spinPointSize->setValue(config.pointSize);
    ui->textColor->setText(config.color);
    ui->spinInterval->setValue(config.generateInterval_);
}

void ConfigDialog::onAcceptClicked()
{
    Config config;

    config.labelsVisible = ui->checkLabels->isChecked();
    config.labelX = ui->textLabelX->text();
    config.labelY = ui->textLabelY->text();
    config.xLower = ui->spinRangeXLower->value();
    config.xUpper = ui->spinRangeXUpper->value();
    config.yLower = ui->spinRangeYLower->value();
    config.yUpper = ui->spinRangeYUpper->value();
    config.linesVisible = ui->checkLines->isChecked();
    config.pointSize = ui->spinPointSize->value();
    config.color = ui->textColor->text();
    config.generateInterval_ = ui->spinInterval->value();

    emit accepted(config);
}
