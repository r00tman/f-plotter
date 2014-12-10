#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <QPen>
#include <QPaintEvent>
#include <functional>

#include "common.h"
#include "coordinatesystem.h"

class PlotWidget : public QWidget
{
    Q_OBJECT
private:
    ld optimal_xgrid_size();
    ld optimal_ygrid_size();

    ld closest_10_power(ld v);

    void minmax(ld &mn, ld &mx);

public:
    typedef std::pair<QLineF, QPen> line_type;

    std::vector<line_type> m_opt_elements;
    CoordinateSystem m_cs;

    func_type m_function;
    void drawGrid(QPainter &painter);
    void drawCS(QPainter &painter);
    void plotFunction(QPainter &painter);

public:
    explicit PlotWidget(QWidget *parent = 0);
    void paintEvent(QPaintEvent *ev);
    void resizeEvent(QResizeEvent *ev);

    void setLRrange(ld l, ld r);
    void autoFitY();

};

#endif // PLOTWIDGET_H
