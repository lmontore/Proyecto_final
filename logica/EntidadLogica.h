#ifndef ENTIDADLOGICA_H
#define ENTIDADLOGICA_H

class EntidadLogica {
protected:
    double posX;
    double posY;
    double velX;
    double velY;
    double ancho;
    double alto;
    bool activa;

public:
    EntidadLogica(double x, double y, double w, double h)
        : posX(x), posY(y), velX(0.0), velY(0.0), ancho(w), alto(h), activa(true) {}

    // Destructor virtual OBLIGATORIO cuando usamos polimorfismo y memoria dinámica
    virtual ~EntidadLogica() = default;

    // Método virtual puro: obliga a cada hijo a implementar su propio modelo físico
    virtual void actualizarFisica(double dt) = 0;

    // Detección de colisión AABB (Axis-Aligned Bounding Box) en C++ puro
    bool colisionaCon(const EntidadLogica& otra) const {
        if (!activa || !otra.activa) return false;
        return (posX < otra.posX + otra.ancho &&
                posX + ancho > otra.posX &&
                posY < otra.posY + otra.alto &&
                posY + alto > otra.posY);
    }

    // Getters y Setters esenciales
    double getX() const { return posX; }
    double getY() const { return posY; }
    double getVelX() const { return velX; }
    double getVelY() const { return velY; }
    double getAncho() const { return ancho; }
    double getAlto() const { return alto; }
    bool estaActiva() const { return activa; }

    void setPosicion(double x, double y) { posX = x; posY = y; }
    void setVelocidad(double vx, double vy) { velX = vx; velY = vy; }
    void desactivar() { activa = false; }
};

#endif // ENTIDADLOGICA_H