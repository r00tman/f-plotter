#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDoubleSpinBox>

#include "common.h"

#include "plotwidget.h"

class Widget : public QWidget
{
    Q_OBJECT
private:
    PlotWidget *m_plot_widget;
    QDoubleSpinBox *ltext, *rtext;

    ld xc;
    bool started;

public:
    Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void setLR();
    void nextIt();
    void autoSolve();
};

#endif // WIDGET_H
