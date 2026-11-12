#include "Vegeta.h"

Vegeta::Vegeta(double x, double y)
    : EntidadLogica(x, y, 64.0, 80.0),
    vida(100), vidaMaxima(100), energiaKi(100.0),
    gravedad(980.0), friccionSuelo(0.85), enSuelo(false),
    estadoAnimo(EstadoAnimo::NORMAL),
    obstaculosEsquivadosSeguidos(0), tiempoRestanteEnfoque(0.0) {}

void Vegeta::actualizarFisica(double dt) {
    // 1. Regeneración pasiva lenta de Ki
    if (energiaKi < 100.0) {
        energiaKi += 5.0 * dt;
        if (energiaKi > 100.0) energiaKi = 100.0;
    }

    // 2. Temporizador del estado "Guerrero Estratega"
    if (estadoAnimo == EstadoAnimo::ENFOQUE_ESTRATEGA) {
        tiempoRestanteEnfoque -= dt;
        if (tiempoRestanteEnfoque <= 0.0) {
            tiempoRestanteEnfoque = 0.0;
            estadoAnimo = (vida < vidaMaxima * 0.3) ? EstadoAnimo::IRA_SAIYAJIN : EstadoAnimo::NORMAL;
        }
    }

    // 3. Aplicación de gravedad (Integración de Euler explícita)
    if (!enSuelo) {
        velY += gravedad * dt;
    }

    // 4. Fricción en el suelo (En estado de Ira frena menos -> más inestable/resbaladizo)
    double factorFriccion = (estadoAnimo == EstadoAnimo::IRA_SAIYAJIN) ? 0.96 : friccionSuelo;
    if (enSuelo) {
        velX *= factorFriccion;
        if (velX > -2.0 && velX < 2.0) velX = 0.0;
    }

    // 5. Actualización de posición
    posX += velX * dt;
    posY += velY * dt;
}

void Vegeta::moverHorizontal(int direccion, double fuerzaBase) {
    double multiplicador = (estadoAnimo == EstadoAnimo::IRA_SAIYAJIN) ? 1.4 : 1.0;
    velX = direccion * fuerzaBase * multiplicador;
}

void Vegeta::saltar(double impulsoBase) {
    if (enSuelo) {
        double multiplicadorSalto = (estadoAnimo == EstadoAnimo::ENFOQUE_ESTRATEGA) ? 1.25 : 1.0;
        velY = impulsoBase * multiplicadorSalto;
        enSuelo = false;
    }
}

bool Vegeta::consumirKiParaAtaque(double costo) {
    if (energiaKi >= costo) {
        energiaKi -= costo;
        return true;
    }
    return false;
}

void Vegeta::recibirDanio(int cantidad) {
    vida -= cantidad;
    if (vida < 0) vida = 0;
    obstaculosEsquivadosSeguidos = 0; // Pierde racha de concentración

    // Activa "Orgullo Saiyajin" si cae por debajo del 30%
    if (vida > 0 && vida < (vidaMaxima * 0.3)) {
        estadoAnimo = EstadoAnimo::IRA_SAIYAJIN;
    }
}

void Vegeta::registrarEsquivaExitosa() {
    obstaculosEsquivadosSeguidos++;
    if (obstaculosEsquivadosSeguidos >= 3) {
        estadoAnimo = EstadoAnimo::ENFOQUE_ESTRATEGA;
        tiempoRestanteEnfoque = 8.0;
        obstaculosEsquivadosSeguidos = 0;
    }
}

void Vegeta::aterrizarEn(double ySuelo) {
    if (posY + alto >= ySuelo && velY >= 0) {
        posY = ySuelo - alto;
        velY = 0.0;
        enSuelo = true;
    }
}

std::string Vegeta::getNombreEstado() const {
    switch (estadoAnimo) {
    case EstadoAnimo::IRA_SAIYAJIN: return "IRA_SAIYAJIN";
    case EstadoAnimo::ENFOQUE_ESTRATEGA: return "ENFOQUE_ESTRATEGA";
    default: return "NORMAL";
    }
}

std::ostream& operator<<(std::ostream& os, const Vegeta& v) {
    os << "[Vegeta | Pos: (" << v.posX << ", " << v.posY << ")"
       << " | Vel: (" << v.velX << ", " << v.velY << ")"
       << " | HP: " << v.vida << "/" << v.vidaMaxima
       << " | Ki: " << static_cast<int>(v.energiaKi) << "%"
       << " | Estado: " << v.getNombreEstado() << "]";
    return os;
}