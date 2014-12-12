#include "widget.h"
#include <QApplication>
#include <iostream>

#include "parser.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    /*std::string v = "x+14.53*x^2^3-4/x";
    expr_t e = parse(v);
    std::cout << v << std::endl;
    for (size_t i = 0; i < e.size(); ++i) {
        if (e[i].name.empty()) {
            std::cout << e[i].value << " ";
        } else {
            std::cout << e[i].name << " ";
        }
    }
    std::cout << std::endl;
    std::cout << evaluate(e, 2) << std::endl;*/

    return a.exec();
}
