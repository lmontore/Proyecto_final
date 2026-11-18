#ifndef SPRITEITEM_H
#define SPRITEITEM_H

#include <QGraphicsItem>
#include <QPainter>
#include <QPixmap>
#include <QColor>
#include <QString>
#include "../logica/EntidadLogica.h"

class SpriteItem : public QGraphicsItem {
private:
    EntidadLogica* entidad;
    QPixmap spriteSheet;
    QString rutaActual;
    QColor colorRespaldo;
    bool tieneImagen;
    bool mirarIzquierda;

    // Control de animación por frames
    int totalFrames;
    int frameActual;
    int contadorTicks;

public:
    SpriteItem(EntidadLogica* ent, const QString& rutaImagen = "",
               QColor respaldo = Qt::blue, int frames = 1);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void sincronizarConLogica();
    void setMirarIzquierda(bool izq) { mirarIzquierda = izq; }
    void cambiarSpriteSheet(const QString& nuevaRuta, int frames = 1);
};

#endif // SPRITEITEM_H