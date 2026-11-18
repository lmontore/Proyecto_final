#ifndef OBSTACULOSNIVEL1_H
#define OBSTACULOSNIVEL1_H

#include "EntidadLogica.h"
#include <cmath>

enum class TipoObstaculo {
    ROCA_PARABOLICA,
    LAMPARA_PENDULO,
    BARRIL_CENTRIFUGO
};

class Obstaculo : public EntidadLogica {
protected:
    int danio;
    TipoObstaculo tipo;

public:
    Obstaculo(double x, double y, double w, double h, int d, TipoObstaculo t)
        : EntidadLogica(x, y, w, h), danio(d), tipo(t) {}

    int getDanio() const { return danio; }
    TipoObstaculo getTipo() const { return tipo; }
};

// FÍSICA 1: Caída libre y tiro parabólico (Rocas y cartuchos de dinamita)
class RocaParabolica : public Obstaculo {
private:
    double gravedad;
    double anguloRotacion;
    double velRotacion;

public:
    RocaParabolica(double x, double y, double vx, double vy, double g = 980.0)
        : Obstaculo(x, y, 45.0, 45.0, 20, TipoObstaculo::ROCA_PARABOLICA),
        gravedad(g), anguloRotacion(0.0), velRotacion(180.0) {
        velX = vx;
        velY = vy;
    }

    void actualizarFisica(double dt) override {
        velY += gravedad * dt;
        posX += velX * dt;
        posY += velY * dt;
        anguloRotacion += velRotacion * dt;
    }

    double getAnguloRotacion() const { return anguloRotacion; }
};

// FÍSICA 2: Movimiento Armónico Simple / Péndulo amortiguado (Lámparas de carburo)
class LamparaPendulo : public Obstaculo {
private:
    double anclajeX;
    double anclajeY;
    double longitudCable;
    double angulo;          // theta (radianes)
    double velAngular;      // omega (rad/s)
    double gravedad;
    double amortiguamiento; // factor b

public:
    LamparaPendulo(double pivoteX, double pivoteY, double L, double anguloInicialRad, double g = 980.0)
        : Obstaculo(pivoteX, pivoteY + L, 36.0, 48.0, 15, TipoObstaculo::LAMPARA_PENDULO),
        anclajeX(pivoteX), anclajeY(pivoteY), longitudCable(L),
        angulo(anguloInicialRad), velAngular(0.0),
        gravedad(g), amortiguamiento(0.02) {
        actualizarCoordenadasCartesianas();
    }

    void actualizarFisica(double dt) override {
        // Ecuación diferencial discreta: alpha = -(g/L)*sin(theta) - b*omega
        double aceleracionAngular = -(gravedad / longitudCable) * std::sin(angulo) - amortiguamiento * velAngular;
        velAngular += aceleracionAngular * dt;
        angulo += velAngular * dt;
        actualizarCoordenadasCartesianas();
    }

    double getAnclajeX() const { return anclajeX; }
    double getAnclajeY() const { return anclajeY; }
    double getAnguloGrados() const { return angulo * 180.0 / 3.14159265; }

private:
    void actualizarCoordenadasCartesianas() {
        posX = anclajeX + longitudCable * std::sin(angulo) - (ancho / 2.0);
        posY = anclajeY + longitudCable * std::cos(angulo);
    }
};

#endif // OBSTACULOSNIVEL1_H