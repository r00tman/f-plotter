#include "widget.h"

#include <functional>

#include <QApplication>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPen>

/*---------------------------------------------------------------------*/
/*- Из задачи                                                         -*/
/*---------------------------------------------------------------------*/
ld a = 0, b = 0.99, eps1 = 1e-12, eps2 = 1e-6;

ld integrate(func_type fun, ld a, ld b) {
    const ld h = 0.001; // integration step
    ld result = (fun(a)+fun(b))/2;
    for (ld it = 1; it < floor((b-a)/h); it += 1) {
        result += fun(it*h);
    }
    result *= h;
    return result;
}

ld f1(ld /*x*/) {
    return 0.2;
}

ld f1d(ld /*x*/) {
    return 0;
}

ld f2(ld x, ld y) {
    return -x*std::exp(-y*x*x/2)/pow(1-y*y, 1/4.0);
}

ld f2d(ld x, ld y) {
    return std::exp(-y*x*x/2)*(y*x*x-1)/pow(1-y*y, 1/4.0);
}

ld f(ld x) {
    return f1(x)+integrate(std::bind1st(func_2dtype(f2), x), a, b);
}

ld fd(ld x) {
    return f1d(x)+integrate(std::bind1st(func_2dtype(f2d), x), a, b);
}
/*---------------------------------------------------------------------*/

// Newton's method
ld auto_solve(func_type fun, func_type fund, ld x0) {
    ld xc = x0;
    for (;;) {
        ld xn = xc-fun(xc)/fund(xc);
        if (std::abs(xc - xn) < eps2) {
            xc = xn;
            break;
        }
        xc = xn;
    }
    return xc;
}

Widget::Widget(QWidget *parent)
    : QWidget(parent) {
    QVBoxLayout *lay = new QVBoxLayout;
    QHBoxLayout *setlay = new QHBoxLayout;
    QPushButton *setxy = new QPushButton("Set range");
    QPushButton *nextit = new QPushButton("Next iteration");
    QPushButton *autosolve = new QPushButton("Auto solve");

    const double INF = 1e12;

    setxy->setDefault(true);
    ltext = new QDoubleSpinBox();
    ltext->setMinimum(-INF);
    ltext->setMaximum(INF);
    rtext = new QDoubleSpinBox();
    rtext->setMinimum(-INF);
    rtext->setMaximum(INF);

    setlay->addWidget(ltext);
    setlay->addWidget(rtext);
    setlay->addWidget(setxy);
    setlay->addWidget(nextit);
    setlay->addWidget(autosolve);
    setlay->addStretch();

    lay->addLayout(setlay);

    m_plot_widget = new PlotWidget();
    m_plot_widget->m_function = f;

    lay->addWidget(m_plot_widget);

    this->setLayout(lay);

    connect(setxy, SIGNAL(clicked()), this, SLOT(setLR()));
    connect(nextit, SIGNAL(clicked()), this, SLOT(nextIt()));
    connect(autosolve, SIGNAL(clicked()), this, SLOT(autoSolve()));

    setMinimumHeight(100); // just something reasonable not crushing all of the resizing thing (preventing division by zero)

    started = false;
}

Widget::~Widget() {

}

void Widget::setLR() {
    m_plot_widget->setLRrange(this->ltext->value(), this->rtext->value());
    m_plot_widget->autoFitY();
    started = false;    
    m_plot_widget->m_opt_elements.clear();
    repaint();
}

void Widget::nextIt() {
    if (!started) {
        // set a starting point for the Newton's method (middle of the screen)
        xc = (m_plot_widget->m_cs.invX(0)+m_plot_widget->m_cs.invX(m_plot_widget->width()))/2;
        started = true;
    }
    ld xn = xc-f(xc)/fd(xc);

    // Tangent
    QLineF line(QPointF(xc, f(xc)), QPointF(xc+1, f(xc)+fd(xc)*1));

    QPen pen(Qt::red);

    // Paint old lines in gray
    if (m_plot_widget->m_opt_elements.size() > 0) {
        m_plot_widget->m_opt_elements[m_plot_widget->m_opt_elements.size()-1].second
                = QPen(Qt::darkGray);
    }

    // Add a new one
    m_plot_widget->m_opt_elements.push_back(PlotWidget::line_type(line, pen));

    xc = xn;

    repaint();
}

void Widget::autoSolve() {
    ld x0 = (m_plot_widget->m_cs.invX(0)+m_plot_widget->m_cs.invX(m_plot_widget->width()))/2;
    QString str;
    for (eps2 = 0.1; eps2 > 1e-12; eps2 /= 10) {
        ld x = auto_solve(f, fd, x0);
        str += "x=" + QString::number((double)x, '.', 12) +
                " f(x)=" + QString::number((double)f(x)) +
                " eps2=" + QString::number((double)eps2) + "\n";
    }
    QMessageBox::information(this, "Solution", str);
}
