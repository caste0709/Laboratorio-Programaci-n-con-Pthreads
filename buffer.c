#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define TOTAL_ITEMS 10000

typedef struct{
    int *buffer;
    int capacidad;
    int in;
    int out;
    int count;
    sem_t vacios;
    sem_t llenos;
    pthread_mutex_t mutex;
} Buffer;

typedef struct{
    Buffer *buf;
    int base;
    int cantidad;
    long producidos;
    long suma;
} ProductorData;

typedef struct{
    Buffer *buf;
    int cantidad;
    int *resultados;
    long consumidos;
    long suma;
} ConsumidorData;

typedef struct{
    int productores;
    int consumidores;
    int tam_buffer;
} Config;

double tiempo_actual(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void insertar(Buffer *b, int valor){
    sem_wait(&b->vacios);
    pthread_mutex_lock(&b->mutex);

    b->buffer[b->in] = valor;
    b->in = (b->in + 1) % b->capacidad;
    b->count++;

    pthread_mutex_unlock(&b->mutex);
    sem_post(&b->llenos);
}

int extraer(Buffer *b){
    sem_wait(&b->llenos);
    pthread_mutex_lock(&b->mutex);

    int valor = b->buffer[b->out];
    b->out = (b->out + 1) % b->capacidad;
    b->count--;

    pthread_mutex_unlock(&b->mutex);
    sem_post(&b->vacios);

    return valor;
}

void *productor(void *p){
    ProductorData *data = (ProductorData *)p;
    data->producidos = 0;
    data->suma = 0;

    for(int i = 1; i <= data->cantidad; i++){
        int valor = data->base + i;
        insertar(data->buf, valor);
        data->producidos++;
        data->suma += valor;
    }

    return NULL;
}

void *consumidor(void *p){
    ConsumidorData *data = (ConsumidorData *)p;
    data->consumidos = 0;
    data->suma = 0;

    for(int i = 0; i < data->cantidad; i++){
        int valor = extraer(data->buf);
        data->resultados[i] = valor;
        data->consumidos++;
        data->suma += valor;
    }

    return NULL;
}

void ejecutar(Config cfg, int total){
    int P = cfg.productores;
    int C = cfg.consumidores;

    Buffer buf;
    buf.buffer = malloc(cfg.tam_buffer * sizeof(int));
    buf.capacidad = cfg.tam_buffer;
    buf.in = 0;
    buf.out = 0;
    buf.count = 0;
    sem_init(&buf.vacios, 0, cfg.tam_buffer);
    sem_init(&buf.llenos, 0, 0);
    pthread_mutex_init(&buf.mutex, NULL);

    pthread_t *hilos_p = malloc(P * sizeof(pthread_t));
    pthread_t *hilos_c = malloc(C * sizeof(pthread_t));
    ProductorData *datos_p = malloc(P * sizeof(ProductorData));
    ConsumidorData *datos_c = malloc(C * sizeof(ConsumidorData));
    int *resultados = malloc(total * sizeof(int));

    double t0 = tiempo_actual();

    int base = 0;
    for(int i = 0; i < P; i++){
        int cant = total / P + (i < total % P ? 1 : 0);
        datos_p[i].buf = &buf;
        datos_p[i].base = base;
        datos_p[i].cantidad = cant;
        base += cant;
        pthread_create(&hilos_p[i], NULL, productor, &datos_p[i]);
    }

    int offset = 0;
    for(int i = 0; i < C; i++){
        int cant = total / C;
        if (i < total % C) {
            cant = cant + 1;
        }
        datos_c[i].buf = &buf;
        datos_c[i].cantidad = cant;
        datos_c[i].resultados = resultados + offset;
        offset += cant;
        pthread_create(&hilos_c[i], NULL, consumidor, &datos_c[i]);
    }

    for(int i = 0; i < P; i++){
        pthread_join(hilos_p[i], NULL);
    }
    for(int i = 0; i < C; i++){
        pthread_join(hilos_c[i], NULL);
    }

    double t = tiempo_actual() - t0;

    long total_prod = 0, suma_prod = 0;
    for(int i = 0; i < P; i++){
        total_prod += datos_p[i].producidos;
        suma_prod += datos_p[i].suma;
    }

    long total_cons = 0, suma_cons = 0;
    for(int i = 0; i < C; i++){
        total_cons += datos_c[i].consumidos;
        suma_cons += datos_c[i].suma;
    }

    int *vistos = calloc(total + 1, sizeof(int));
    for(int i = 0; i < total; i++){
        vistos[resultados[i]]++;
    }
    int sin_perdidas_ni_duplicados = 1;
    for(int v = 1; v <= total; v++){
        if(vistos[v] != 1){
            sin_perdidas_ni_duplicados = 0;
            break;
        }
    }

    int correcto = (total_prod == total) && (total_cons == total) &&
                   (suma_prod == suma_cons) && sin_perdidas_ni_duplicados &&
                   (buf.count == 0);

    printf("P=%d C=%d Buffer=%d | Producidos=%ld Consumidos=%ld | SumaP=%ld SumaC=%ld | Correcto: %s | Tiempo: %.6f s\n",
           P, C, cfg.tam_buffer, total_prod, total_cons, suma_prod, suma_cons,
           correcto ? "SI" : "NO", t);

    free(vistos);
    free(resultados);
    free(datos_p);
    free(datos_c);
    free(hilos_p);
    free(hilos_c);
    free(buf.buffer);
    sem_destroy(&buf.vacios);
    sem_destroy(&buf.llenos);
    pthread_mutex_destroy(&buf.mutex);
}

int main(void){
    Config configs[] = {
        {2, 2, 1},
        {2, 2, 5},
        {2, 2, 50},
        {4, 2, 5},
        {2, 4, 5},
        {4, 4, 10},
        {8, 8, 20},
        {8, 2, 10},
        {2, 8, 10}
    };
    int n = sizeof(configs) / sizeof(configs[0]);

    printf("Total de elementos: %d\n\n", TOTAL_ITEMS);

    for(int i = 0; i < n; i++){
        ejecutar(configs[i], TOTAL_ITEMS);
    }

    return 0;
}