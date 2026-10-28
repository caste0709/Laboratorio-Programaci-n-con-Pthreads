#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>

#define REPETICIONES 5

typedef struct{
    int *array;
    size_t inicio;
    size_t final;
    long suma_parcial;
    int min_val_parcial;
    int max_val_parcial;
} ThreadData;

double tiempo_actual(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void *calculo_parcial(void *p){
    ThreadData *data = (ThreadData *)p;
    long suma = 0;
    int min = data->array[data->inicio];
    int max = data->array[data->inicio];

    for(size_t i = data->inicio; i < data->final; i++){
        int val = data->array[i];
        suma += val;
        if(val < min) min = val;
        if(val > max) max = val;
    }

    data->suma_parcial = suma;
    data->min_val_parcial = min;
    data->max_val_parcial = max;
    return NULL;
}

void paralelo(int *array, size_t tam, int num_threads, long *suma, int *min, int *max){
    pthread_t hilos[num_threads];
    ThreadData datos[num_threads];

    size_t division = tam / num_threads;
    size_t restante = tam % num_threads;
    size_t inicio = 0;

    for(int i = 0; i < num_threads; i++){
        size_t t = division;
        if((size_t)i < restante){
            t++;
        }

        datos[i].array = array;
        datos[i].inicio = inicio;
        datos[i].final = inicio + t;
        datos[i].suma_parcial = 0;
        datos[i].min_val_parcial = 0;
        datos[i].max_val_parcial = 0;
        inicio = datos[i].final;

        pthread_create(&hilos[i], NULL, calculo_parcial, &datos[i]);
    }

    long suma_total = 0;
    int min_total = INT_MAX;
    int max_total = INT_MIN;

    for(int i = 0; i < num_threads; i++){
        pthread_join(hilos[i], NULL);
        suma_total += datos[i].suma_parcial;
        if(datos[i].min_val_parcial < min_total){
            min_total = datos[i].min_val_parcial;
        }
        if(datos[i].max_val_parcial > max_total){
            max_total = datos[i].max_val_parcial;
        }
    }

    *suma = suma_total;
    *min = min_total;
    *max = max_total;
}

int main(void){

    size_t N[3] = {1000, 100000, 1000000};
    int hilos_prueba[4] = {3, 6, 9, 12};

    for(int i = 0; i < 3; i++){

        size_t tam = N[i];
        int *array = malloc(tam * sizeof(int));

        printf("Arreglo con: %zu valores\n", tam);

        const unsigned int sem = 42;
        srand(sem);

        for(size_t j = 0; j < tam; j++){
            array[j] = rand() % 50 + 1;
        }

        for(int h = 0; h < 4; h++){
            int num_threads = hilos_prueba[h];
            long suma;
            int min, max;
            double t_total = 0.0;

            for(int r = 0; r < REPETICIONES; r++){
                double t0 = tiempo_actual();
                paralelo(array, tam, num_threads, &suma, &min, &max);
                t_total += tiempo_actual() - t0;
            }

            double t_par = t_total / REPETICIONES;
            double prom = (double)suma / tam;

            printf("Hilos: %d | Resto: %zu | Suma: %ld | Min: %d | Max: %d | Promedio: %.2f | Tiempo: %.9f s\n",
                   num_threads, tam % num_threads, suma, min, max, prom, t_par);
        }

        printf("\n");
        free(array);
    }

    return 0;
}