#include "SpriteItem.h"
#include "../logica/ObstaculosNivel1.h"
#include <algorithm>

SpriteItem::SpriteItem(EntidadLogica* ent, const QString& rutaImagen, QColor respaldo, int frames)
    : entidad(ent), colorRespaldo(respaldo), tieneImagen(false),
    mirarIzquierda(false), totalFrames(frames), frameActual(0), contadorTicks(0) {
    if (!rutaImagen.isEmpty() && spriteSheet.load(rutaImagen)) {
        tieneImagen = true;
    }
    sincronizarConLogica();
}

QRectF SpriteItem::boundingRect() const {
    if (!entidad) return QRectF(0, 0, 10, 10);
    if (auto* lampara = dynamic_cast<LamparaPendulo*>(entidad)) {
        // Incluye toda el área desde el pivote en el techo hasta la base de la lámpara
        double minX = std::min(0.0, lampara->getAnclajeX() - lampara->getX()) - 5.0;
        double minY = std::min(0.0, lampara->getAnclajeY() - lampara->getY()) - 5.0;
        double maxX = std::max(entidad->getAncho(), lampara->getAnclajeX() - lampara->getX()) + 5.0;
        double maxY = entidad->getAlto() + 5.0;
        return QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
    }
    return QRectF(0, 0, entidad->getAncho(), entidad->getAlto());
}

void SpriteItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    if (!entidad || !entidad->estaActiva()) return;

    painter->setRenderHint(QPainter::Antialiasing);

    // Si es una lámpara péndulo, dibujamos su cadena desde el techo
    if (auto* lampara = dynamic_cast<LamparaPendulo*>(entidad)) {
        painter->save();
        painter->setPen(QPen(QColor(90, 60, 30), 3));
        double pivoteLocalX = lampara->getAnclajeX() - lampara->getX();
        double pivoteLocalY = lampara->getAnclajeY() - lampara->getY();
        painter->drawLine(QPointF(pivoteLocalX, pivoteLocalY),
                          QPointF(entidad->getAncho() / 2.0, 0));
        painter->restore();
    }

    QRectF rect = QRectF(0, 0, entidad->getAncho(), entidad->getAlto());

    if (tieneImagen && totalFrames > 0) {
        int anchoFrame = spriteSheet.width() / totalFrames;
        int altoFrame = spriteSheet.height();
        QRect sourceRect(frameActual * anchoFrame, 0, anchoFrame, altoFrame);

        painter->save();
        if (mirarIzquierda) {
            painter->translate(rect.width(), 0);
            painter->scale(-1, 1);
        }
        painter->drawPixmap(rect.toRect(), spriteSheet, sourceRect);
        painter->restore();
    } else {
        // Dibujo provisional estilizado mientras conectas los sprites PNG
        painter->setBrush(colorRespaldo);
        painter->setPen(QPen(Qt::black, 2));
        if (dynamic_cast<RocaParabolica*>(entidad)) {
            painter->drawEllipse(rect);
        } else {
            painter->drawRoundedRect(rect, 8, 8);
        }
    }
}

void SpriteItem::sincronizarConLogica() {
    if (!entidad) return;
    setPos(entidad->getX(), entidad->getY());

    // Rotación visual si es roca rodando/cayendo
    if (auto* roca = dynamic_cast<RocaParabolica*>(entidad)) {
        setTransformOriginPoint(entidad->getAncho() / 2.0, entidad->getAlto() / 2.0);
        setRotation(roca->getAnguloRotacion());
    }

    // Avanzar frame de animación cada 6 ticks (~10 FPS de animación de sprite)
    if (tieneImagen && totalFrames > 1) {
        contadorTicks++;
        if (contadorTicks >= 6) {
            frameActual = (frameActual + 1) % totalFrames;
            contadorTicks = 0;
        }
    }
    update();
}

void SpriteItem::cambiarSpriteSheet(const QString& nuevaRuta, int frames) {
    if (spriteSheet.load(nuevaRuta)) {
        tieneImagen = true;
        totalFrames = frames;
        frameActual = 0;
    }
}