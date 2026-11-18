#include "EscenaNivel1.h"
#include <QRandomGenerator>
#include <QFont>
#include <QBrush>
#include <QPen>
#include <cmath>
#include <fstream>
#include <iostream>

EscenaNivel1::EscenaNivel1(QObject* parent)
    : QGraphicsScene(parent), dt(0.016), nivelSueloY(545.0), metaTunelX(840.0),
    mirandoIzquierda(false), tiempoSpawnRoca(0.0), tiempoLogConsola(0.0),
    juegoTerminado(false) {

    setSceneRect(0, 0, 1024, 600);
    inicializarEscenario();

    // Crear a Vegeta sobre el inicio de la vía férrea (Capa Lógica + Capa Gráfica con 4 frames)
    vegeta = new Vegeta(80.0, 555.0 - (0.19 * 80.0) - 80.0);
    spriteVegeta = new SpriteItem(vegeta, ":/assets/sprites/vegeta_idle.png", QColor(30, 90, 220), 4);
    addItem(spriteVegeta);

    // Lámparas de carburo oscilantes (Física de Péndulo Simple)
    auto* lamp1 = new LamparaPendulo(380.0, 40.0, 260.0, 0.65);
    obstaculos.push_back(lamp1);
    auto* sLamp1 = new SpriteItem(lamp1, ":/assets/sprites/lampara.png", QColor(255, 190, 40), 1);
    spritesObstaculos.push_back(sLamp1);
    addItem(sLamp1);

    auto* lamp2 = new LamparaPendulo(680.0, 40.0, 290.0, -0.75);
    obstaculos.push_back(lamp2);
    auto* sLamp2 = new SpriteItem(lamp2, ":/assets/sprites/lampara.png", QColor(255, 190, 40), 1);
    spritesObstaculos.push_back(sLamp2);
    addItem(sLamp2);

    viendoComic = true;
    QPixmap imgComic(":/assets/fondos/comic_nivel1.jpg");
    panelComic = addPixmap(imgComic.scaled(1024, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    panelComic->setZValue(100); // Encima del HUD y de Vegeta

    inicializarHUD();

    // Bucle principal a ~60 FPS
    timerBucle = new QTimer(this);
    connect(timerBucle, &QTimer::timeout, this, &EscenaNivel1::actualizarFrame);
    timerBucle->start(16);
}

EscenaNivel1::~EscenaNivel1() {
    delete vegeta;
    for (auto* obs : obstaculos) delete obs;
    for (auto* proj : proyectiles) delete proj;
}

void EscenaNivel1::inicializarEscenario() {
    QPixmap fondo(":/assets/fondos/fondo_quiebra.png");
    if (!fondo.isNull()) {
        addPixmap(fondo.scaled(1024, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        setBackgroundBrush(QColor(42, 54, 48));
        addRect(0, nivelSueloY, 1024, 90, QPen(Qt::NoPen), QBrush(QColor(75, 58, 45)));
        addRect(0, nivelSueloY - 6, 1024, 6, QPen(Qt::NoPen), QBrush(QColor(150, 155, 160)));
    }
}

void EscenaNivel1::inicializarHUD() {
    addRect(15, 15, 220, 75, QPen(QColor(200, 200, 200)), QBrush(QColor(15, 15, 25, 190)));

    barraVidaFondo = addRect(25, 25, 200, 14, QPen(Qt::black), QBrush(Qt::darkRed));
    barraVida = addRect(25, 25, 200, 14, QPen(Qt::NoPen), QBrush(QColor(50, 210, 80)));

    addRect(25, 44, 200, 10, QPen(Qt::black), QBrush(QColor(20, 30, 60)));
    barraKi = addRect(25, 44, 200, 10, QPen(Qt::NoPen), QBrush(QColor(60, 180, 255)));

    textoEstado = addText("ESTADO: NORMAL", QFont("Consolas", 9, QFont::Bold));
    textoEstado->setDefaultTextColor(Qt::white);
    textoEstado->setPos(22, 60);

    textoMensajeCentral = addText("", QFont("Arial", 22, QFont::Bold));
    textoMensajeCentral->setDefaultTextColor(Qt::yellow);
    textoMensajeCentral->setPos(200, 240);
}

void EscenaNivel1::keyPressEvent(QKeyEvent* event) {
    if (viendoComic) {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) {
            viendoComic = false;
            removeItem(panelComic);
            delete panelComic;
            panelComic = nullptr;
        }
        return;
    }
    if (event->isAutoRepeat()) return;
    teclasPresionadas.insert(event->key());

    if (!juegoTerminado) {
        if (event->key() == Qt::Key_W || event->key() == Qt::Key_Up || event->key() == Qt::Key_Space) {
            vegeta->saltar();
        }
        if (event->key() == Qt::Key_J || event->key() == Qt::Key_Z) {
            if (vegeta->consumirKiParaAtaque(20.0)) {
                int dir = mirandoIzquierda ? -1 : 1;
                double spawnX = mirandoIzquierda ? (vegeta->getX() - 30) : (vegeta->getX() + vegeta->getAncho());
                double spawnY = vegeta->getY() + (vegeta->getAlto() * 0.35);

                auto* ki = new ProyectilKi(spawnX, spawnY, dir);
                proyectiles.push_back(ki);

                auto* sKi = new SpriteItem(ki, ":/assets/sprites/ki.png", QColor(255, 235, 50), 1);
                sKi->setMirarIzquierda(mirandoIzquierda);
                spritesProyectiles.push_back(sKi);
                addItem(sKi);
            }
        }
    }
}

void EscenaNivel1::keyReleaseEvent(QKeyEvent* event) {
    if (event->isAutoRepeat()) return;
    teclasPresionadas.remove(event->key());
}

void EscenaNivel1::procesarEntradaJugador() {
    if (teclasPresionadas.contains(Qt::Key_A) || teclasPresionadas.contains(Qt::Key_Left)) {
        vegeta->moverHorizontal(-1);
        mirandoIzquierda = true;
        spriteVegeta->setMirarIzquierda(true);
    } else if (teclasPresionadas.contains(Qt::Key_D) || teclasPresionadas.contains(Qt::Key_Right)) {
        vegeta->moverHorizontal(1);
        mirandoIzquierda = false;
        spriteVegeta->setMirarIzquierda(false);
    }
}

void EscenaNivel1::generarRocaDerrumbe() {
    double spawnX = QRandomGenerator::global()->bounded(220, 820);
    double vx = QRandomGenerator::global()->bounded(-140, 60);
    double vy = QRandomGenerator::global()->bounded(-120, 20);

    auto* roca = new RocaParabolica(spawnX, 10.0, vx, vy);
    obstaculos.push_back(roca);

    auto* sRoca = new SpriteItem(roca, ":/assets/sprites/roca.png", QColor(130, 105, 85), 1);
    spritesObstaculos.push_back(sRoca);
    addItem(sRoca);
}

void EscenaNivel1::actualizarFrame() {

    if (viendoComic || juegoTerminado) return;
    if (juegoTerminado) return;

    procesarEntradaJugador();

    // 1. Suelo en perspectiva 2.5D siguiendo los rieles hacia el túnel
    double sueloPerspectivaY = 555.0 - (0.19 * vegeta->getX());
    vegeta->actualizarFisica(dt);
    vegeta->aterrizarEn(sueloPerspectivaY);

    if (vegeta->getX() < 0) vegeta->setPosicion(0, vegeta->getY());
    if (vegeta->getX() >= metaTunelX) {
        finalizarNivel(true);
        return;
    }

    // Cambiar animación de Vegeta según velocidad horizontal
    if (std::abs(vegeta->getVelX()) > 10.0) {
        spriteVegeta->cambiarSpriteSheet(":/assets/sprites/vegeta_run.png", 4);
    } else {
        spriteVegeta->cambiarSpriteSheet(":/assets/sprites/vegeta_idle.png", 4);
    }
    spriteVegeta->sincronizarConLogica();

    // 2. Generar derrumbes cada 1.8 segundos
    tiempoSpawnRoca += dt;
    if (tiempoSpawnRoca >= 1.8) {
        generarRocaDerrumbe();
        tiempoSpawnRoca = 0.0;
    }

    // 3. Actualizar físicas de obstáculos
    for (size_t i = 0; i < obstaculos.size(); ++i) {
        obstaculos[i]->actualizarFisica(dt);
        double sueloRocaY = 555.0 - (0.19 * obstaculos[i]->getX());
        if (obstaculos[i]->getTipo() == TipoObstaculo::ROCA_PARABOLICA &&
            obstaculos[i]->getY() > sueloRocaY) {
            obstaculos[i]->desactivar();
            vegeta->registrarEsquivaExitosa();
        }
        spritesObstaculos[i]->sincronizarConLogica();
    }

    // 4. Actualizar proyectiles Ki
    for (size_t i = 0; i < proyectiles.size(); ++i) {
        proyectiles[i]->actualizarFisica(dt);
        spritesProyectiles[i]->sincronizarConLogica();
    }

    // 5. Colisiones y limpieza de memoria
    verificarColisiones();
    limpiarEntidadesInactivas();
    actualizarHUD();

    // 6. Telemetría con friend std::ostream&
    tiempoLogConsola += dt;
    if (tiempoLogConsola >= 2.0) {
        std::ofstream log("telemetria_vegeta.txt", std::ios::app);
        if (log.is_open()) {
            log << *vegeta << "\n";
        }
        tiempoLogConsola = 0.0;
    }
}

void EscenaNivel1::verificarColisiones() {
    for (auto* ki : proyectiles) {
        if (!ki->estaActiva()) continue;
        for (auto* obs : obstaculos) {
            if (!obs->estaActiva()) continue;
            if (obs->getTipo() == TipoObstaculo::ROCA_PARABOLICA && ki->colisionaCon(*obs)) {
                ki->desactivar();
                obs->desactivar();
            }
        }
    }

    for (auto* obs : obstaculos) {
        if (!obs->estaActiva()) continue;
        if (vegeta->colisionaCon(*obs)) {
            vegeta->recibirDanio(obs->getDanio());
            if (obs->getTipo() == TipoObstaculo::ROCA_PARABOLICA) {
                obs->desactivar();
            } else {
                vegeta->setVelocidad(-220.0, -180.0);
            }
            if (vegeta->getVida() <= 0) {
                finalizarNivel(false);
                return;
            }
        }
    }
}

void EscenaNivel1::limpiarEntidadesInactivas() {
    for (int i = static_cast<int>(obstaculos.size()) - 1; i >= 0; --i) {
        if (!obstaculos[i]->estaActiva()) {
            removeItem(spritesObstaculos[i]);
            delete spritesObstaculos[i];
            delete obstaculos[i];
            spritesObstaculos.erase(spritesObstaculos.begin() + i);
            obstaculos.erase(obstaculos.begin() + i);
        }
    }

    for (int i = static_cast<int>(proyectiles.size()) - 1; i >= 0; --i) {
        if (!proyectiles[i]->estaActiva()) {
            removeItem(spritesProyectiles[i]);
            delete spritesProyectiles[i];
            delete proyectiles[i];
            spritesProyectiles.erase(spritesProyectiles.begin() + i);
            proyectiles.erase(proyectiles.begin() + i);
        }
    }
}

void EscenaNivel1::actualizarHUD() {
    double ratioVida = static_cast<double>(vegeta->getVida()) / 100.0;
    barraVida->setRect(25, 25, 200.0 * ratioVida, 14);
    barraKi->setRect(25, 44, 200.0 * (vegeta->getEnergiaKi() / 100.0), 10);

    QString estado = QString::fromStdString(vegeta->getNombreEstado());
    textoEstado->setPlainText("ESTADO: " + estado);
    if (estado == "IRA_SAIYAJIN") {
        textoEstado->setDefaultTextColor(QColor(255, 80, 80));
    } else if (estado == "ENFOQUE_ESTRATEGA") {
        textoEstado->setDefaultTextColor(QColor(80, 230, 255));
    } else {
        textoEstado->setDefaultTextColor(Qt::white);
    }
}

void EscenaNivel1::finalizarNivel(bool victoria) {
    juegoTerminado = true;
    timerBucle->stop();
    if (victoria) {
        textoMensajeCentral->setDefaultTextColor(QColor(80, 255, 120));
        textoMensajeCentral->setPlainText("¡LLEGASTE AL TUNEL DE LA QUIEBRA!");
    } else {
        textoMensajeCentral->setDefaultTextColor(QColor(255, 70, 70));
        textoMensajeCentral->setPlainText("DERROTADO EN LA EXCAVACION");
    }
}