#ifndef ESCENANIVEL1_H
#define ESCENANIVEL1_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QKeyEvent>
#include <QSet>
#include <vector>

#include "../logica/Vegeta.h"
#include "../logica/ObstaculosNivel1.h"
#include "../logica/ProyectilKi.h"
#include "SpriteItem.h"

class EscenaNivel1 : public QGraphicsScene {
    Q_OBJECT

private:
    QTimer* timerBucle;
    double dt;
    double nivelSueloY;
    double metaTunelX;

    // Entidades de la capa lógica (Memoria dinámica)
    Vegeta* vegeta;
    SpriteItem* spriteVegeta;

    std::vector<Obstaculo*> obstaculos;
    std::vector<SpriteItem*> spritesObstaculos;

    std::vector<ProyectilKi*> proyectiles;
    std::vector<SpriteItem*> spritesProyectiles;

    // Control de teclado fluido
    QSet<int> teclasPresionadas;
    bool mirandoIzquierda;

    // Generador periódico de derrumbes
    double tiempoSpawnRoca;
    double tiempoLogConsola;

    // Elementos del HUD
    QGraphicsRectItem* barraVidaFondo;
    QGraphicsRectItem* barraVida;
    QGraphicsRectItem* barraKi;
    QGraphicsTextItem* textoEstado;
    QGraphicsTextItem* textoMensajeCentral;

    bool juegoTerminado;

    void inicializarEscenario();
    void inicializarHUD();
    void procesarEntradaJugador();
    void generarRocaDerrumbe();
    void verificarColisiones();
    void limpiarEntidadesInactivas();
    void actualizarHUD();
    void finalizarNivel(bool victoria);

public:
    explicit EscenaNivel1(QObject* parent = nullptr);
    ~EscenaNivel1() override;

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void actualizarFrame();
};

#endif // ESCENANIVEL1_H