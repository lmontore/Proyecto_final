#include "SpriteItem.h"
#include "../logica/ObstaculosNivel1.h"
#include <QImage>
#include <algorithm>

SpriteItem::SpriteItem(EntidadLogica* ent, const QString& rutaImagen, QColor respaldo, int frames)
    : entidad(ent), colorRespaldo(respaldo), tieneImagen(false),
    mirarIzquierda(false), totalFrames(frames), frameActual(0), contadorTicks(0) {
    cambiarSpriteSheet(rutaImagen, frames);
    sincronizarConLogica();
}

QRectF SpriteItem::boundingRect() const {
    if (!entidad) return QRectF(0, 0, 10, 10);
    if (auto* lampara = dynamic_cast<LamparaPendulo*>(entidad)) {
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

    // Cadena de la lámpara oscilante
    if (auto* lampara = dynamic_cast<LamparaPendulo*>(entidad)) {
        painter->save();
        painter->setPen(QPen(QColor(65, 42, 20), 3));
        double pivoteLocalX = lampara->getAnclajeX() - lampara->getX();
        double pivoteLocalY = lampara->getAnclajeY() - lampara->getY();
        painter->drawLine(QPointF(pivoteLocalX, pivoteLocalY),
                          QPointF(entidad->getAncho() / 2.0, 4));
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

    if (auto* roca = dynamic_cast<RocaParabolica*>(entidad)) {
        setTransformOriginPoint(entidad->getAncho() / 2.0, entidad->getAlto() / 2.0);
        setRotation(roca->getAnguloRotacion());
    }

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
    if (nuevaRuta.isEmpty() || rutaActual == nuevaRuta) return;
    QImage img(nuevaRuta);
    if (!img.isNull()) {
        img = img.convertToFormat(QImage::Format_ARGB32);
        // Borra cualquier píxel blanco o casi blanco (incluyendo artefactos JPEG)
        for (int y = 0; y < img.height(); ++y) {
            QRgb* linea = reinterpret_cast<QRgb*>(img.scanLine(y));
            for (int x = 0; x < img.width(); ++x) {
                QColor c(linea[x]);
                if (c.red() > 232 && c.green() > 232 && c.blue() > 232) {
                    linea[x] = qRgba(255, 255, 255, 0);
                }
            }
        }
        spriteSheet = QPixmap::fromImage(img);
        rutaActual = nuevaRuta;
        tieneImagen = true;
        totalFrames = frames;
        frameActual = 0;
    }
}