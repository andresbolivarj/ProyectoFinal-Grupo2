#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_UNAPulpe.h"

class UNAPulpe : public QMainWindow
{
    Q_OBJECT

public:
    UNAPulpe(QWidget *parent = nullptr);
    ~UNAPulpe();

private:
    Ui::UNAPulpeClass ui;
};

