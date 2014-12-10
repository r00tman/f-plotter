#include "plotwidget.h"
#include <QPainter>

ld PlotWidget::optimal_xgrid_size() {
    ld res = closest_10_power((m_cs.invX(width())-m_cs.invX(0))/5);
    return res;
}

ld PlotWidget::optimal_ygrid_size() {
    ld res = closest_10_power((m_cs.invY(height())-m_cs.invY(0))/5);
    return res;
}

ld PlotWidget::closest_10_power(ld v) {
    return pow(10,floor(log10(v)));
}


PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent) {
    m_cs.xscale = m_cs.yscale = 50;
    m_cs.xpitch += width()/2.0l/m_cs.xscale;
    m_cs.ypitch += height()/2.0l/m_cs.yscale;
}

void PlotWidget::drawGrid(QPainter &painter) {
    ld minX = m_cs.invX(0);
    ld maxX = m_cs.invX(width());

    ld minY = m_cs.invY(0);
    ld maxY = m_cs.invY(height());

    ld xgrid_step = optimal_xgrid_size();
    ld ygrid_step = optimal_ygrid_size();

    for (ld x = floor(minX/xgrid_step); x <= ceil(maxX/xgrid_step); ++x) {
        painter.setPen(QPen(Qt::gray));
        painter.drawLine(m_cs.transformX(x*xgrid_step), 0, m_cs.transformX(x*xgrid_step), height());

        ld ypos = height()-m_cs.transformY(0);
        if (ypos < 0) {
            ypos = 0;
        } else if (ypos > height()) {
            ypos = height();
        }

        QPointF textpos(m_cs.transformX(x*xgrid_step), ypos + 15);
        if (textpos.y() > height()) {
            textpos.setY(ypos-10);
        }
        if (textpos.x() < 0) {
            textpos.setX(10);
        } else if (textpos.x() > width()) {
            textpos.setX(width()-50);
        }

        painter.setPen(QPen(Qt::black));

        painter.drawLine(m_cs.transformX(x*xgrid_step), ypos+10, m_cs.transformX(x*xgrid_step), ypos-10);
        painter.drawText(textpos, QString::number((double)(x*xgrid_step)));
    }

    for (ld y = floor(minY/ygrid_step); y <= ceil(maxY/ygrid_step); ++y) {
        painter.setPen(QPen(Qt::gray));
        painter.drawLine(0, height()-m_cs.transformY(y*ygrid_step), width(), height()-m_cs.transformY(y*ygrid_step));

        ld xpos = m_cs.transformX(0);
        if (xpos < 0) {
            xpos = 0;
        } else if (xpos > width()) {
            xpos = width();
        }
        QPointF textpos(xpos+15, height()-m_cs.transformY(y*ygrid_step)+5);
        if (textpos.x() > width()) {
            textpos.setX(xpos-50);
        }
        if (textpos.y() < 0) {
            textpos.setY(10);
        } else if (textpos.y() > height()) {
            textpos.setY(height()-10);
        }

        painter.setPen(QPen(Qt::black));
        painter.drawLine(xpos+10, height()-m_cs.transformY(y*ygrid_step), xpos-10, height()-m_cs.transformY(y*ygrid_step));
        painter.drawText(textpos, QString::number((double)(y*ygrid_step)));
    }
}

void PlotWidget::drawCS(QPainter &painter) {
    painter.setPen(QPen(Qt::black));

    ld xpos = m_cs.transformX(0);
    painter.drawLine(xpos, 0, xpos, height());

    ld ypos = height()-m_cs.transformY(0);
    painter.drawLine(0, ypos, width(), ypos);
}

void PlotWidget::plotFunction(QPainter &painter) {
    painter.setPen(QPen(Qt::blue));

    bool first = true;
    QPointF last;
    for (int x = 0; x < width(); ++x) {
        ld csx = m_cs.invX(x);
        ld y = m_cs.transformY(m_function(csx));
        QPointF current(x, height()-y);
        if (!first) {
            painter.drawLine(last, current);
        }
        last = current;
        first = false;
    }
}

void PlotWidget::paintEvent(QPaintEvent */*ev*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    drawGrid(painter);

    drawCS(painter);

    plotFunction(painter);

    // drawing lines of m_opt_elements
    const float DRAWING_INF = 10000;

    for (auto el : m_opt_elements) {
        painter.setPen(el.second);
        QLineF oldLine = el.first;
        QLineF newLine;

        newLine.setP1(QPointF(m_cs.transformX(oldLine.p1().x()), height()-m_cs.transformY(oldLine.p1().y())));
        newLine.setP2(QPointF(m_cs.transformX(oldLine.p2().x()), height()-m_cs.transformY(oldLine.p2().y())));
        newLine.setLength(DRAWING_INF);
        painter.drawLine(newLine);

        newLine.setP2(QPointF(m_cs.transformX(oldLine.p1().x()), height()-m_cs.transformY(oldLine.p1().y())));
        newLine.setP1(QPointF(m_cs.transformX(oldLine.p2().x()), height()-m_cs.transformY(oldLine.p2().y())));
        newLine.setLength(DRAWING_INF);
        painter.drawLine(newLine);
    }
}

void PlotWidget::resizeEvent(QResizeEvent *ev) {
    if (ev->size().width() > 0 && ev->oldSize().width() > 0) {
        m_cs.xscale *= ev->size().width()*1.0l/ev->oldSize().width();
    }
    if (ev->size().height() > 0 && ev->oldSize().height() > 0) {
        m_cs.yscale *= ev->size().height()*1.0l/ev->oldSize().height();
    }
}

void PlotWidget::setLRrange(ld l, ld r) {
    m_cs.xscale = width() / (r - l);
    m_cs.xpitch = -l;
}

void PlotWidget::minmax(ld &mn, ld &mx) {
    mn = std::numeric_limits<double>::max();
    mx = std::numeric_limits<double>::min();

    for (int x = 0; x < width(); ++x) {
        ld csx = m_cs.invX(x);
        ld csy = m_function(csx);
        mn = std::min(mn, csy);
        mx = std::max(mx, csy);
    }
}

void PlotWidget::autoFitY() {
    ld mn, mx;
    minmax(mn, mx);
    m_cs.ypitch = -mn;
    if (mx - mn < 1e-10) {
        m_cs.yscale = 100;
        m_cs.ypitch = -mn+height()/m_cs.yscale/2;
    } else {
        m_cs.yscale = height() / (mx - mn);
    }
}
