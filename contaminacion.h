#ifndef CONTAMINACION_H
#define CONTAMINACION_H

#include <stdio.h>

#define ZONAS 5
#define DIAS 30


#define LIM_CO2  400.0f
#define LIM_SO2   20.0f
#define LIM_NO2   40.0f
#define LIM_PM25  25.0f

typedef struct {
    float co2;
    float so2;
    float no2;
    float pm25;
} Contaminacion;

typedef struct {
    float temperatura;   // °C
    float viento;        // m/s
    float humedad;       // %
} Clima;

typedef struct {
    char nombre[30];
    Contaminacion historial[DIAS];
} Zona;


Contaminacion promedio_historico(const Zona *z);
Contaminacion prediccion_ponderada(const Zona *z);
Contaminacion ajustar_por_clima(Contaminacion base, Clima c);

int hay_alerta(Contaminacion c);
void escribir_recomendaciones(FILE *f);

int guardar_datos_bin(const char *ruta, const Zona zonas[], int n);
int cargar_datos_bin(const char *ruta, Zona zonas[], int n);

int guardar_reporte_txt(const char *ruta, const Zona zonas[], int n, Clima clima_actual);

#endif

