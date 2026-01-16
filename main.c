#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "contaminacion.h"

static void simular_datos(Zona zonas[], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < DIAS; j++) {
            zonas[i].historial[j].co2  = 350.0f + (float)(rand() % 120);
            zonas[i].historial[j].so2  =  8.0f + (float)(rand() % 25);
            zonas[i].historial[j].no2  = 15.0f + (float)(rand() % 40);
            zonas[i].historial[j].pm25 = 10.0f + (float)(rand() % 30);
        }
    }
}

static Clima leer_clima(void) {
    Clima c;
    printf("Ingrese temperatura (C): ");
    scanf("%f", &c.temperatura);
    printf("Ingrese velocidad del viento (m/s): ");
    scanf("%f", &c.viento);
    printf("Ingrese humedad (%%): ");
    scanf("%f", &c.humedad);
    return c;
}

int main(void) {
    srand((unsigned)time(NULL));

    Zona zonas[ZONAS] = {
        {"Centro"},
        {"Norte"},
        {"Sur"},
        {"Este"},
        {"Oeste"}
    };

    // Intentar cargar datos previos
    if (!cargar_datos_bin("datos.bin", zonas, ZONAS)) {
        // Si no existe, simular y guardar
        simular_datos(zonas, ZONAS);
        guardar_datos_bin("datos.bin", zonas, ZONAS);
    }

    // Clima actual (entrada del usuario)
    Clima clima = leer_clima();

    // Generar reporte
    if (guardar_reporte_txt("reporte_contaminacion.txt", zonas, ZONAS, clima)) {
        printf("\nReporte generado: reporte_contaminacion.txt\n");
        printf("Datos persistidos: datos.bin\n");
    } else {
        printf("\nERROR: No se pudo generar el reporte.\n");
    }

    return 0;
}
