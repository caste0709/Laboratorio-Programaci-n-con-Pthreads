#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define REPETICIONES 5

typedef struct{
    int *array;
    size_t inicio;
    size_t final;
    int *hist;
    pthread_mutex_t *mutex;
} ThreadData;

double tiempo_actual(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void *histograma_mutex(void *p){

    ThreadData *data = (ThreadData *)p;

    for(size_t j = data->inicio; j < data->final; j++){
        int valor = data->array[j];
        int categoria = valor / 10;

        pthread_mutex_lock(data->mutex);
        data->hist[categoria]++;
        pthread_mutex_unlock(data->mutex);
    }

    return NULL;
}

void paralelo(int *array, size_t tam, int num_threads, int *hist){
    ThreadData datos[num_threads];
    pthread_t hilos[num_threads];
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    for(int k = 0; k < 10; k++){
        hist[k] = 0;
    }

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
        datos[i].hist = hist;
        datos[i].mutex = &mutex;
        inicio = datos[i].final;

        pthread_create(&hilos[i], NULL, histograma_mutex, &datos[i]);
    }

    for(int i = 0; i < num_threads; i++){
        pthread_join(hilos[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
}

int main(void){
    size_t N[3] = {100, 1000, 10000};
    int hilos_prueba[4] = {1, 2, 4, 8};

    for(int n = 0; n < 3; n++){
        size_t tam = N[n];
        int array[tam];

        printf("Arreglo con: %zu valores\n", tam);

        const unsigned int sem = 42;
        srand(sem);

        for(size_t j = 0; j < tam; j++){
            array[j] = rand() % 100;
        }

        for(int h = 0; h < 4; h++){
            int num_threads = hilos_prueba[h];
            int hist[10];
            double t_total = 0.0;

            for(int r = 0; r < REPETICIONES; r++){
                double t0 = tiempo_actual();
                paralelo(array, tam, num_threads, hist);
                t_total += tiempo_actual() - t0;
            }

            double t_par = t_total / REPETICIONES;

            int suma = 0;
            for(int k = 0; k < 10; k++){
                suma += hist[k];
            }

            if(h == 0){
                for(int k = 0; k < 10; k++){
                    printf("Intervalo %d-%d: %d valores\n", k * 10, k * 10 + 9, hist[k]);
                }
            }

            printf("Hilos: %d | Suma = %d | Suma == N: %s | Tiempo: %.9f s\n",
                   num_threads, suma, (suma == (int)tam) ? "SI" : "NO", t_par);
        }
        printf("\n");
    }

    return 0;
}