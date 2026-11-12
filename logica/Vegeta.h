#ifndef VEGETA_H
#define VEGETA_H

#include "EntidadLogica.h"
#include <ostream>
#include <string>

enum class EstadoAnimo {
    NORMAL,
    IRA_SAIYAJIN,      // Vida < 30%: +40% velocidad y daño, pero menos fricción (resbala)
    ENFOQUE_ESTRATEGA  // Tras esquivar 3 obstáculos: salto mejorado por 8 segundos
};

class Vegeta : public EntidadLogica {
private:
    int vida;
    int vidaMaxima;
    double energiaKi;
    double gravedad;
    double friccionSuelo;
    bool enSuelo;

    // Atributos de personalidad (Momento II)
    EstadoAnimo estadoAnimo;
    int obstaculosEsquivadosSeguidos;
    double tiempoRestanteEnfoque;

public:
    Vegeta(double x, double y);

    void actualizarFisica(double dt) override;

    // Acciones del jugador o de la IA
    void moverHorizontal(int direccion, double fuerzaBase = 280.0);
    void saltar(double impulsoBase = -520.0);
    bool consumirKiParaAtaque(double costo = 20.0);

    // Eventos de juego
    void recibirDanio(int cantidad);
    void registrarEsquivaExitosa();
    void aterrizarEn(double ySuelo);

    // Getters de estado
    int getVida() const { return vida; }
    double getEnergiaKi() const { return energiaKi; }
    bool estaEnSuelo() const { return enSuelo; }
    EstadoAnimo getEstadoAnimo() const { return estadoAnimo; }
    std::string getNombreEstado() const;

    // Función amiga para telemetría / depuración en flujo estándar o archivo
    friend std::ostream& operator<<(std::ostream& os, const Vegeta& v);
};

#endif // VEGETA_H