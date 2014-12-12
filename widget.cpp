#include "widget.h"

#include <functional>

#include <QApplication>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPen>

#include "parser.h"

/*---------------------------------------------------------------------*/
/*- Из задачи                                                         -*/
/*---------------------------------------------------------------------*/
ld a = 0, b = 0.99, eps1 = 1e-12, eps2 = 1e-6;

expr_t expr = parse("x^3+2*x-x+1");

ld f(ld x) {
    return evaluate(expr, x);
}

ld fd(ld x) {
    return (f(x+eps1)-f(x))/eps1;
}
/*---------------------------------------------------------------------*/

// Newton's method
ld auto_solve(func_type fun, func_type fund, ld x0) {
    const size_t MAXITCOUNT = 1000000;
    ld xc = x0;
    for (size_t itcount = 0; itcount < MAXITCOUNT; ++itcount) {
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
    QPushButton *setf = new QPushButton("Set f(x)");
    QPushButton *nextit = new QPushButton("Next iteration");
    QPushButton *autosolve = new QPushButton("Auto solve");

    const double INF = 1e12;

    setxy->setDefault(true);
    ltext = new QDoubleSpinBox();
    ltext->setMinimum(-INF);    
    ltext->setMaximum(INF);
    ltext->setValue(-5);
    rtext = new QDoubleSpinBox();
    rtext->setMinimum(-INF);
    rtext->setMaximum(INF);
    rtext->setValue(5);

    ltext->setMaximumWidth(100);
    rtext->setMaximumWidth(100);

    setlay->addWidget(ltext);
    setlay->addWidget(rtext);
    setlay->addWidget(setxy);

    m_fun_text = new QLineEdit();
    m_fun_text->setMinimumWidth(100);
    setlay->addWidget(m_fun_text);

    setlay->addWidget(setf);

    setlay->addWidget(nextit);
    setlay->addWidget(autosolve);
    setlay->addStretch();

    lay->addLayout(setlay);

    m_plot_widget = new PlotWidget();
    m_plot_widget->m_function = f;

    lay->addWidget(m_plot_widget);

    this->setLayout(lay);

    connect(setxy, SIGNAL(clicked()), this, SLOT(setLR()));
    connect(setf, SIGNAL(clicked()), this, SLOT(setF()));
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

void Widget::setF() {
    expr = parse(m_fun_text->text().toStdString());
    setLR();
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
