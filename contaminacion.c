#include "contaminacion.h"

Contaminacion promedio_historico(const Zona *z) {
    Contaminacion p = {0, 0, 0, 0};

    for (int i = 0; i < DIAS; i++) {
        p.co2  += z->historial[i].co2;
        p.so2  += z->historial[i].so2;
        p.no2  += z->historial[i].no2;
        p.pm25 += z->historial[i].pm25;
    }

    p.co2  /= DIAS;
    p.so2  /= DIAS;
    p.no2  /= DIAS;
    p.pm25 /= DIAS;

    return p;
}

Contaminacion prediccion_ponderada(const Zona *z) {
    Contaminacion p = {0, 0, 0, 0};
    float peso_total = 0.0f;

    for (int i = 0; i < DIAS; i++) {
        float peso = (float)(i + 1);  // 1..30
        peso_total += peso;

        p.co2  += z->historial[i].co2  * peso;
        p.so2  += z->historial[i].so2  * peso;
        p.no2  += z->historial[i].no2  * peso;
        p.pm25 += z->historial[i].pm25 * peso;
    }

    p.co2  /= peso_total;
    p.so2  /= peso_total;
    p.no2  /= peso_total;
    p.pm25 /= peso_total;

    return p;
}

Contaminacion ajustar_por_clima(Contaminacion base, Clima c) {
    float f_temp = 1.0f;
    float f_viento = 1.0f;
    float f_humedad_pm = 1.0f;

    if (c.temperatura < 15.0f) c.temperatura = 15.0f;
    if (c.temperatura > 35.0f) c.temperatura = 35.0f;
    f_temp = 1.0f + ((c.temperatura - 15.0f) / 20.0f) * 0.08f; // hasta +8%

    if (c.viento < 0.0f) c.viento = 0.0f;
    if (c.viento > 10.0f) c.viento = 10.0f;
    f_viento = 1.0f - (c.viento / 10.0f) * 0.12f; // hasta -12%

    if (c.humedad < 30.0f) c.humedad = 30.0f;
    if (c.humedad > 90.0f) c.humedad = 90.0f;
    f_humedad_pm = 1.0f - ((c.humedad - 30.0f) / 60.0f) * 0.06f; // hasta -6%

    base.co2  *= (f_temp * f_viento);
    base.so2  *= (f_temp * f_viento);
    base.no2  *= (f_temp * f_viento);
    base.pm25 *= (f_temp * f_viento * f_humedad_pm);

    return base;
}

int hay_alerta(Contaminacion c) {
    return (c.co2 > LIM_CO2 ||
            c.so2 > LIM_SO2 ||
            c.no2 > LIM_NO2 ||
            c.pm25 > LIM_PM25);
}

void escribir_recomendaciones(FILE *f) {
    fprintf(f, "Recomendaciones:\n");
    fprintf(f, "- Reducir trafico vehicular\n");
    fprintf(f, "- Control temporal de industrias\n");
    fprintf(f, "- Suspender actividades al aire libre\n");
    fprintf(f, "- Promover transporte publico\n\n");
}

/* ===== Persistencia binaria ===== */
int guardar_datos_bin(const char *ruta, const Zona zonas[], int n) {
    FILE *fp = fopen(ruta, "wb");
    if (!fp) return 0;

    size_t w = fwrite(zonas, sizeof(Zona), (size_t)n, fp);
    fclose(fp);
    return (w == (size_t)n);
}

int cargar_datos_bin(const char *ruta, Zona zonas[], int n) {
    FILE *fp = fopen(ruta, "rb");
    if (!fp) return 0;

    size_t r = fread(zonas, sizeof(Zona), (size_t)n, fp);
    fclose(fp);
    return (r == (size_t)n);
}

/* ===== Reporte TXT ===== */
int guardar_reporte_txt(const char *ruta, const Zona zonas[], int n, Clima clima_actual) {
    FILE *f = fopen(ruta, "w");
    if (!f) return 0;

    fprintf(f, "REPORTE CONTAMINACION - 30 dias + prediccion 24h\n");
    fprintf(f, "Clima actual -> Temp: %.1f C | Viento: %.1f m/s | Humedad: %.1f %%\n\n",
            clima_actual.temperatura, clima_actual.viento, clima_actual.humedad);

    for (int i = 0; i < n; i++) {
        Contaminacion prom = promedio_historico(&zonas[i]);
        Contaminacion pred_base = prediccion_ponderada(&zonas[i]);
        Contaminacion pred = ajustar_por_clima(pred_base, clima_actual);

        fprintf(f, "=====================================\n");
        fprintf(f, "Zona: %s\n", zonas[i].nombre);

        fprintf(f, "Promedio ultimos 30 dias:\n");
        fprintf(f, "CO2: %.2f | SO2: %.2f | NO2: %.2f | PM2.5: %.2f\n",
                prom.co2, prom.so2, prom.no2, prom.pm25);

        fprintf(f, "Prediccion proximas 24 horas (ponderado + ajuste clima):\n");
        fprintf(f, "CO2: %.2f | SO2: %.2f | NO2: %.2f | PM2.5: %.2f\n",
                pred.co2, pred.so2, pred.no2, pred.pm25);

        if (hay_alerta(pred)) {
            fprintf(f, "ALERTA: Niveles superiores a limites referenciales\n");
            escribir_recomendaciones(f);
        } else {
            fprintf(f, "Estado: Niveles dentro de rangos aceptables\n\n");
        }
    }

    fclose(f);
    return 1;
}

