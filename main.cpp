#include <QApplication>
#include <QGraphicsView>
#include "gui/EscenaNivel1.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    EscenaNivel1* escena = new EscenaNivel1();

    QGraphicsView vista(escena);
    vista.setWindowTitle("Ferrocarril de Antioquia 2.5D - Nivel 1: La Quiebra");
    vista.setFixedSize(1026, 602);
    vista.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista.setRenderHint(QPainter::Antialiasing);
    vista.show();

    return app.exec();
}