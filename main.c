#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define DATA "datosClima.txt"


#define LIMITE_CO2   1000.0f   // ppm (partes por millon)
#define LIMITE_SO2   40.0f     // ug/m3
#define LIMITE_NO2   25.0f     // ug/m3
#define LIMITE_PM25  15.0f     // ug/m3

// Estructura para datos de contaminación
struct Contaminacion{
    float co2;
    float so2;
    float no2;
    float pm25;
}contaminacion;

// Estructura para Zona con ID y nombre
struct Zona {
    int id;
    char nombre[50];
}zona;

void ingresarZona();
int leerNumeroEnteroEntre(char *mensaje,int min,int max);
void guardarPalabra(char *mensaje, char destino[], int longitud);
float leerNumeroFlotanteEntre(char *mensaje,int min,int max);
void preventivas(float CO2, float SO2, float NO2, float PM25);
float calcularAQI_PM25(float pm);
float calcularAQI_NO2(float no2_ppb);
float calcularAQI_SO2(float so2_ppb);
float calcularAQI_CO2(float co2);
void imprimirClasificacionAQI(char nombre[], float AQI);
void prediccion();
void promediosHistoricos();
int imprimirZonas();
void exportacionDatos();
void generacionRecomendaciones(float co2, float so2, float no2, float pm25);
void recomendacionesPorZona();
void ordenar();

int main() {
    int continuar = 1;
    int eleccion = 0;
    do {
        printf("\n----- Menu de Gestion Ambiental -----\n");
        printf("1. Agregar datos\n");
        printf("2. Prediccion de niveles futuros\n");
        printf("3. Calculo de promedios historicos\n");
        printf("4. Generacion de recomendaciones\n");
        printf("5. Exportacion de datos\n");
        printf("6. Mostrar zonas registradas\n");
        printf("7. Salir\n");
        printf("-------------------------------------\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &eleccion);
        switch (eleccion) {
            case 1:
                ingresarZona();
                ordenar();
                break;
            case 2:
                prediccion();
                break;
            case 3:
                printf("Funcion para calculo de promedios historicos\n");
                promediosHistoricos();
                break;
            case 4:
                recomendacionesPorZona();
                break;
            case 5:
                printf("Funcion para exportacion de datos\n");
                exportacionDatos();
                break;
            case 6:
                imprimirZonas();
                break;
            case 7:
                continuar = 0;
                printf("Saliendo.....\n");
                break;
            default:
                printf("Opcion no valida. Intente de nuevo.\n");
                break;
        }
    } while (continuar);
    return 0;
}

void generacionRecomendaciones(float co2, float so2, float no2, float pm25) {
    printf("\n--- Recomendaciones de mitigacion ---\n");
    int alerta = 0;
    if (co2 > LIMITE_CO2) {
        printf("CO2 alto: Reduzca el trafico vehicular y promueva el transporte publico.\n");
        alerta = 1;
    }
    if (so2 > LIMITE_SO2) {
        printf("SO2 alto: Considere el cierre temporal de industrias emisoras de SO2.\n");
        alerta = 1;
    }
    if (no2 > LIMITE_NO2) {
        printf("NO2 alto: Limite el uso de vehiculos diesel y controle emisiones industriales.\n");
        alerta = 1;
    }
    if (pm25 > LIMITE_PM25) {
        printf("PM2.5 alto: Suspenda actividades al aire libre y refuerce el control de polvos.\n");
        alerta = 1;
    }
    if (!alerta) {
        printf("Todos los niveles estan dentro de los limites recomendados.\n");
    }
}

void recomendacionesPorZona() {
    printf("\n--- Generacion de recomendaciones por zona (niveles actuales) ---\n");
    imprimirZonas();
    int id_zona = leerNumeroEnteroEntre("Ingrese el ID de la zona para recomendaciones: ", 1, 10000);
    FILE *archivo = fopen(DATA, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo de datos.\n");
        return;
    }
    float co2=0, so2=0, no2=0, pm25=0;
    int id;
    char nombre[50], fecha[20];
    float vco2, vso2, vno2, vpm25;
    int encontrado = 0;
    char fecha_ultimo[20] = "";
    // Buscar el ultimo registro de la zona seleccionada
    while (fscanf(archivo, "%d,%49[^,],%19[^,],%f,%f,%f,%f", &id, nombre, fecha, &vco2, &vso2, &vno2, &vpm25) == 7) {
        if (id == id_zona) {
            co2 = vco2; so2 = vso2; no2 = vno2; pm25 = vpm25;
            strncpy(fecha_ultimo, fecha, sizeof(fecha_ultimo));
            fecha_ultimo[sizeof(fecha_ultimo)-1] = '\0';
            encontrado = 1;
        }
    }
    fclose(archivo);
    if (encontrado) {
        printf("\nUltimos valores registrados para la zona %d (fecha %s):\nCO2: %.2f ppm\nSO2: %.2f ug/m3\nNO2: %.2f ug/m3\nPM25: %.2f ug/m3\n", id_zona, fecha_ultimo, co2, so2, no2, pm25);
        generacionRecomendaciones(co2, so2, no2, pm25);
    } else {
        printf("No se encontraron registros para la zona seleccionada.\n");
    }
}

void exportacionDatos(){
    FILE *exportacion = fopen("exportacion.txt","w");
    FILE *archivo = fopen(DATA, "r");
    if (archivo == NULL || exportacion == NULL) {
        printf("No se pudo abrir el archivo de datos.\n");
        return;
    }
    int zonas = 0;
    int encontrado = 1;
    int id;
    int eleccion;
    char nombre[100];
    char fecha[100];
    float co2,so2,no2,pm25;
    imprimirZonas();
   eleccion = leerNumeroEnteroEntre("Ingrese el id que desea exportar: ",0,2147483647);

    while(fscanf(archivo, "%d,%49[^,],%19[^,],%f,%f,%f,%f", &id, nombre, fecha, &co2, &so2, &no2, &pm25) == 7) {
        if(eleccion == id){
            fprintf(exportacion, "%d,%s,%s,%.2f,%.2f,%.2f,%.2f\n",id,nombre,fecha,co2,so2,no2,pm25);
        }
    }
    fclose(exportacion);
    fclose(archivo);
}
// Imprime los IDs y nombres unicos de las zonas registradas
int imprimirZonas() {
    FILE *archivo = fopen(DATA, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo de datos.\n");
    }
    int ids[100];
    char nombres[100][50];
    int num_ids = 0;
    int id;
    char nombre[50], fecha[20];
    int encontrado;
    while (fscanf(archivo, "%d,%50[^,],%20[^,]", &id, nombre, fecha) == 3) {
        encontrado = 0;
        for (int i = 0; i < num_ids; i++) {
            if (ids[i] == id && strcmp(nombres[i], nombre) == 0) {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) {
            ids[num_ids] = id;
            strncpy(nombres[num_ids], nombre, 50);
            nombres[num_ids][49] = '\0';
            num_ids++;
        }
        // Saltar el resto de la línea (los contaminantes)
        int c;
        while ((c = fgetc(archivo)) != '\n' && c != EOF);
    }
    fclose(archivo);
    printf("\nZonas registradas:\n");
    for (int i = 0; i < num_ids; i++) {
        printf("- ID: %d, Nombre: %s\n", ids[i], nombres[i]);
    }
}

void promediosHistoricos() {
    FILE *Eleccion = fopen(DATA, "r");
    if (Eleccion == NULL) {
        printf("No se pudo abrir el archivo de datos.\n");
        return;
    }
    int zonas = 0;
    int encontrado = 1;
    int id;
    int eleccion;
    char nombre[100];
    char fecha[100];
    float co2,so2,no2,pm25;
    int contador = 0;
    float suma_co2 = 0, suma_so2 = 0, suma_no2 = 0, suma_pm25 = 0;
    imprimirZonas();
    eleccion = leerNumeroEnteroEntre("Ingrese el id que desea exportar: ",0,2147483647);
    // Obtener la fecha actual
    time_t t_actual = time(NULL);
    struct tm tm_actual = *localtime(&t_actual);
    // Recorrer archivo
    while(fscanf(Eleccion, "%d,%49[^,],%19[^,],%f,%f,%f,%f", &id, nombre, fecha, &co2, &so2, &no2, &pm25) == 7) {
        // Convertir fecha del registro a time_t
        if(eleccion == id){
            int anio, mes, dia;
            if (sscanf(fecha, "%d-%d-%d", &anio, &mes, &dia) == 3) {
                struct tm tm_reg = {0};
                tm_reg.tm_year = anio - 1900;
                tm_reg.tm_mon = mes - 1;
                tm_reg.tm_mday = dia;
                time_t t_reg = mktime(&tm_reg);
                double diff_dias = difftime(t_actual, t_reg) / (60*60*24);
                if (diff_dias >= 0 && diff_dias <= 30) {
                    suma_co2 += co2;
                    suma_so2 += so2;
                    suma_no2 += no2;
                    suma_pm25 += pm25;
                    contador++;
                }
            }
        }
    }
    fclose(Eleccion);
    if (contador < 30) {
        printf("No hay datos de los ultimos 30 dias para calcular promedios.\n");
        return;
    }
    printf("\n--- Promedios de contaminantes (ultimos 30 dias) ---\n");
    printf("CO2: %.2f ppm\n", suma_co2 / contador);
    printf("SO2: %.2f ug/m3\n", suma_so2 / contador);
    printf("NO2: %.2f ug/m3\n", suma_no2 / contador);
    printf("PM25: %.2f ug/m3\n", suma_pm25 / contador);
}

void prediccion(){
    // Prediccion de niveles futuros usando promedio de los ultimos 7 dias
    printf("\n--- Prediccion de niveles futuros (promedio 7 dias) ---\n");
    imprimirZonas();
    int id_zona = leerNumeroEnteroEntre("Ingrese el ID de la zona para predecir: ", 1, 10000);
    FILE *archivo = fopen(DATA, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo de datos.\n");
        return;
    }
    float co2[7] = {0}, so2[7] = {0}, no2[7] = {0}, pm25[7] = {0};
    char fechas[7][20];
    int count = 0;
    int id;
    char nombre[50], fecha[20];
    float vco2, vso2, vno2, vpm25;
    while (fscanf(archivo, "%d,%49[^,],%19[^,],%f,%f,%f,%f", &id, nombre, fecha, &vco2, &vso2, &vno2, &vpm25) == 7) {
        if (id == id_zona) {
            if (count < 7) {
                co2[count] = vco2;
                so2[count] = vso2;
                no2[count] = vno2;
                pm25[count] = vpm25;
                strncpy(fechas[count], fecha, 20);
                fechas[count][19] = '\0';
                count++;
            }
        }
    }
    fclose(archivo);
    if (count == 0) {
        printf("No hay datos para la zona seleccionada.\n");
        return;
    }
    float suma_co2 = 0, suma_so2 = 0, suma_no2 = 0, suma_pm25 = 0;
    for (int i = 0; i < count; i++) {
        suma_co2 += co2[i];
        suma_so2 += so2[i];
        suma_no2 += no2[i];
        suma_pm25 += pm25[i];
    }
    printf("\nDatos usados para la prediccion (ultimos %d dias):\n", count);
    for (int i = 0; i < count; i++) {
        printf("%s: CO2=%.2f, SO2=%.2f, NO2=%.2f, PM25=%.2f\n", fechas[i], co2[i], so2[i], no2[i], pm25[i]);
    }
    // Calcular la fecha de mañana
    time_t t_actual = time(NULL);
    struct tm tm_maniana = *localtime(&t_actual);
    tm_maniana.tm_mday += 1;
    mktime(&tm_maniana); // Normaliza la fecha
    char fecha_maniana[20];
    strftime(fecha_maniana, sizeof(fecha_maniana), "%Y-%m-%d", &tm_maniana);
    float pred_co2 = suma_co2 / count;
    float pred_so2 = suma_so2 / count;
    float pred_no2 = suma_no2 / count;
    float pred_pm25 = suma_pm25 / count;
    printf("\nPrediccion para el dia %s en la zona %d:\n", fecha_maniana, id_zona);
    printf("CO2: %.2f ppm\n", pred_co2);
    printf("SO2: %.2f ug/m3\n", pred_so2);
    printf("NO2: %.2f ug/m3\n", pred_no2);
    printf("PM25: %.2f ug/m3\n", pred_pm25);
    printf("(Metodo: promedio movil de los ultimos %d dias)\n", count);
    generacionRecomendaciones(pred_co2, pred_so2, pred_no2, pred_pm25);
}

void ingresarZona() {
    FILE *datos = fopen(DATA, "a");
    if (datos == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }
    printf("\n----- Ingreso de Zona -----\n");
    zona.id = leerNumeroEnteroEntre("Ingrese el ID de la zona: ", 1, 10000);
    // Buscar si el ID ya existe y obtener el nombre si es así
    FILE *busca = fopen(DATA, "r");
    int id_encontrado = 0;
    if (busca != NULL) {
        int id_tmp;
        char nombre_tmp[50], fecha_tmp[20];
        while (fscanf(busca, "%d,%49[^,],%19[^,]", &id_tmp, nombre_tmp, fecha_tmp) == 3) {
            // Saltar el resto de la línea
            int c; 
            while ((c = fgetc(busca)) != '\n' && c != EOF);
            if (id_tmp == zona.id) {
                strncpy(zona.nombre, nombre_tmp, 50);
                zona.nombre[49] = '\0';
                id_encontrado = 1;
                break;
            }
        }
        fclose(busca);
    }
    if (id_encontrado == 0) {
        guardarPalabra("Ingrese el nombre de la zona (max 50 caracteres): ", zona.nombre, 50);
    } else {
        printf("Nombre de zona detectado automaticamente: %s\n", zona.nombre);
    }
    // Obtener la fecha y hora actual
    char fecha_hora[20];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(fecha_hora, sizeof(fecha_hora), "%04d-%02d-%02d %02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
    rewind(datos);
    //Ingreso de datos
    printf("partes por millon\n");
    contaminacion.co2 = leerNumeroFlotanteEntre("Ingrese la cantida de CO2(ppm): ",0,2147483647);
    printf("Unidad: microgramos de sustancia por metro cubico de aire\n");
    contaminacion.so2 = leerNumeroFlotanteEntre("Ingrese la cantida de SO2: ",0,2147483647);
    contaminacion.no2 = leerNumeroFlotanteEntre("Ingrese la cantida de NO2: ",0,2147483647);
    contaminacion.pm25 = leerNumeroFlotanteEntre("Ingrese la cantida de pm25: ",0,2147483647);
    fprintf(datos, "%d,%s,%s,%.2f,%.2f,%.2f,%.2f\n", zona.id, zona.nombre, fecha_hora, contaminacion.co2, contaminacion.so2, contaminacion.no2, contaminacion.pm25);
    printf("Zona ingresada correctamente. Fecha y hora: %s\n", fecha_hora);
    fclose(datos);
    preventivas(contaminacion.co2,contaminacion.so2,contaminacion.no2,contaminacion.pm25);
    printf("\n--- Alertas Preventivas de AQI ---\n");
    imprimirClasificacionAQI("CO2",calcularAQI_CO2(contaminacion.co2));
    imprimirClasificacionAQI("SO2",calcularAQI_SO2(contaminacion.so2));
    imprimirClasificacionAQI("NO2",calcularAQI_NO2(contaminacion.no2));
    imprimirClasificacionAQI("PM25",calcularAQI_PM25(contaminacion.pm25));
}

int leerNumeroEnteroEntre(char *mensaje,int min,int max){
    int valor = 0;
    printf("%s",mensaje);
    while(scanf("%d",&valor) != 1 || valor > max || valor < min) {
        printf("Dato mal ingresado\n");
        printf("%s",mensaje);
        while ((getchar()) != '\n');
    }
    while ((getchar()) != '\n');
    return valor;
}

float leerNumeroFlotanteEntre(char *mensaje,int min,int max){
    float valor = 0;
    printf("%s",mensaje);
    while(scanf("%f",&valor) != 1 || valor > max || valor < min) {
        printf("Dato mal ingresado\n");
        printf("%s",mensaje);
        while ((getchar()) != '\n');
    }
    while ((getchar()) != '\n');
    return valor;
}

void guardarPalabra(char *mensaje, char destino[], int longitud) {
    int esValido = 0;
    while (esValido == 0){
        printf("%s", mensaje);
        if (fgets(destino, longitud, stdin) != NULL) {
            size_t len = strlen(destino);
            if (len > 0 && destino[len - 1] == '\n') {
                destino[len - 1] = '\0';
            }
            esValido = 1;
            for (size_t i = 0; i < strlen(destino); i++) {
                if (isdigit(destino[i])) {
                    printf("Error: No se permiten numeros. Intente de nuevo.\n");
                    esValido = 0;
                    break;
                }
            }
        } else {
            printf("Error al leer la entrada.\n");
        }
    }
    for (int i = 1; destino[i] != '\0'; i++) {
        destino[i] = tolower(destino[i]);
    }
}

void ordenar(){
    // Estructura para almacenar los registros
    struct Registro {
        int id;
        char nombre[50];
        char fecha[20];
        float co2, so2, no2, pm25;
    }registros[1000], temp;
    int n = 0;
    FILE *datos = fopen(DATA, "r");
    if (datos == NULL) {
        printf("No se pudo abrir el archivo de datos.\n");
        return;
    }
    // Leer todos los registros
    while (fscanf(datos, "%d,%49[^,],%19[^,],%f,%f,%f,%f", &registros[n].id, registros[n].nombre, registros[n].fecha, &registros[n].co2, &registros[n].so2, &registros[n].no2, &registros[n].pm25) == 7 && n < 1000) {
        n++;
    }
    fclose(datos);
    // Ordenar por id y fecha
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (registros[j].id > registros[j+1].id ||(registros[j].id == registros[j+1].id && strcmp(registros[j].fecha, registros[j+1].fecha) > 0)) {
                temp = registros[j];
                registros[j] = registros[j+1];
                registros[j+1] = temp;
            }
        }
    }
    // Escribir en archivo temporal
    FILE *orden = fopen("temp.txt", "w");
    if (orden == NULL) {
        printf("No se pudo abrir el archivo temporal.\n");
        return;
    }
    for (int i = 0; i < n; i++) {
        fprintf(orden, "%d,%s,%s,%.2f,%.2f,%.2f,%.2f\n",
                registros[i].id, registros[i].nombre, registros[i].fecha,registros[i].co2, 
                registros[i].so2, registros[i].no2, registros[i].pm25);
    }
    fclose(orden);
    //Sobrescribir el archivo original
    remove(DATA);
    rename("temp.txt", DATA);
    printf("Registros ordenados: %d\n", n);
}


//preventivas Aire
void preventivas(float CO2, float SO2, float NO2, float PM25) {
    printf("\n--- Alertas Preventivas ---\n");
    // CO2
    if (CO2 > 1000.0) {
        printf("CO2: ALTO (>1000 ppm) ");
    } else if (CO2 > 800.0) {
        printf("CO2: MEDIO (801-1000 ppm) ");
    } else if (CO2 >= 400.0) {
        printf("CO2: BAJO (400-800 ppm) ");
    } else {
        printf("CO2: MUY BAJO (<400 ppm) ");
    }
    printf("Con: %.2f\n",CO2);
    // SO2
    if (SO2 > 40.0) {
        printf("SO2: ALTO (>40)");
    } else if (SO2 >= 21.0) {
        printf("SO2: MEDIO (21-40)");
    } else if (SO2 >= 0.0) {
        printf("SO2: BAJO (0-20)");
    }
     printf(" Con: %.2f\n",SO2);
    // NO2
    if (NO2 > 25.0) {
        printf("NO2: ALTO (>25)");
    } else if (NO2 >= 14.0) {
        printf("NO2: MEDIO (14-25)");
    } else if (NO2 >= 0.0) {
        printf("NO2: BAJO (0-13)");
    }
     printf(" Con: %.2f\n",NO2);
    // PM25
    if (PM25 > 15.0) {
        printf("PM25: ALTO (>15)");
    } else if (PM25 >= 8.0) {
        printf("PM25: MEDIO (8-15)");
    } else if (PM25 >= 0.0) {
        printf("PM25: BAJO (0-7)");
    }
     printf(" Con: %.2f\n",PM25);
}
// Funcion para clasificar el AQI de cada contaminante
void imprimirClasificacionAQI(char nombre[], float AQI) {
    printf("%s: ", nombre);
    if (AQI <= 50)
        printf("Bueno (0-50)");
    else if (AQI <= 100)
        printf("Moderado (51-100)");
    else if (AQI <= 150)
        printf("Danino para grupos sensibles (101-150)");
    else if (AQI <= 200)
        printf("Danino (151-200)");
    else if (AQI <= 300)
        printf("Muy danino (201-300)");
    else
        printf("Peligroso (>300)");
    printf("AQI: %.2f\n", AQI);
}
// AQI PM25
float calcularAQI_PM25(float pm) {
    if (pm <= 12.0f)
        return (50.0f / 12.0f) * pm;
    else if (pm <= 35.4f)
        return 50.0f + ((100.0f - 50.0f) / (35.4f - 12.1f)) * (pm - 12.1f);
    else if (pm <= 55.4f)
        return 100.0f + ((150.0f - 100.0f) / (55.4f - 35.5f)) * (pm - 35.5f);
    else
        return 151.0f;  // simplificado
}
// AQI NO2
float calcularAQI_NO2(float no2_ppb) {
    if (no2_ppb <= 53.0f)
        return (50.0f / 53.0f) * no2_ppb;
    else if (no2_ppb <= 100.0f)
        return 50.0f + ((100.0f - 50.0f) / (100.0f - 54.0f)) * (no2_ppb - 54.0f);
    else if (no2_ppb <= 360.0f)
        return 100.0f + ((150.0f - 100.0f) / (360.0f - 101.0f)) * (no2_ppb - 101.0f);
    else
        return 151.0f;  // simplificado
}
// AQI SO2
float calcularAQI_SO2(float so2_ppb) {
    if (so2_ppb <= 35.0f)
        return (50.0f / 35.0f) * so2_ppb;
    else if (so2_ppb <= 75.0f)
        return 50.0f + ((100.0f - 50.0f) / (75.0f - 36.0f)) * (so2_ppb - 36.0f);
    else if (so2_ppb <= 185.0f)
        return 100.0f + ((150.0f - 100.0f) / (185.0f - 76.0f)) * (so2_ppb - 76.0f);
    else
        return 151.0f;
}
// AQI CO2
float calcularAQI_CO2(float co2) {
    if (co2 <= 800.0f)
        return (50.0f / 800.0f) * co2;
    else if (co2 <= 1000.0f)
        return 50.0f + ((100.0f - 50.0f) / (1000.0f - 801.0f)) * (co2 - 801.0f);
    else
        return 101.0f;  // mala calidad
}


