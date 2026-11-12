#ifndef PROYECTILKI_H
#define PROYECTILKI_H

#include "EntidadLogica.h"

class ProyectilKi : public EntidadLogica {
private:
    double distanciaRecorrida;
    double alcanceMaximo;

public:
    ProyectilKi(double x, double y, int direccion)
        : EntidadLogica(x, y, 32.0, 18.0),
        distanciaRecorrida(0.0), alcanceMaximo(750.0) {
        velX = direccion * 650.0; // Velocidad rápida de ráfaga Ki
        velY = 0.0;
    }

    void actualizarFisica(double dt) override {
        double dx = velX * dt;
        posX += dx;
        distanciaRecorrida += (dx < 0 ? -dx : dx);
        if (distanciaRecorrida >= alcanceMaximo) {
            desactivar();
        }
    }
};

#endif // PROYECTILKI_H