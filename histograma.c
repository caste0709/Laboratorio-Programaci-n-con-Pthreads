#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define REPETICIONES 5

typedef struct{
    int min;
    int max;
    int valores_d;
}Intervalo;

double tiempo_actual(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void secuencial(int *array, size_t tam, Intervalo *intervalos){
    for(int i = 0; i < 10; i++){
        intervalos[i].min = i*10;
        intervalos[i].max = i*10 + 9;
        intervalos[i].valores_d = 0;
    }

    for(size_t j = 0; j < tam; j++){
        for(int in = 0; in < 10; in++){
            if(array[j] >= intervalos[in].min && array[j] <= intervalos[in].max){
                intervalos[in].valores_d++;
            }
        }
    }
}

int main(void){
    size_t N[3] = {100, 1000, 10000};

    for(int n = 0; n < 3; n++){
        size_t tam = N[n];
        int array[tam];

        printf("Arreglo con: %zu valores\n", tam);

        const unsigned int sem = 42;
        srand(sem);

        for(size_t j = 0; j < tam; j++){
            array[j] = rand() % 100;
        }

        Intervalo intervalos[10];
        double t_total = 0.0;

        for(int r = 0; r < REPETICIONES; r++){
            double t0 = tiempo_actual();
            secuencial(array, tam, intervalos);
            t_total += tiempo_actual() - t0;
        }

        double t_sec = t_total / REPETICIONES;

        int suma = 0;
        for(int y = 0; y < 10; y++){
            printf("Intervalo %d-%d: %d valores\n",
                   intervalos[y].min, intervalos[y].max, intervalos[y].valores_d);
            suma += intervalos[y].valores_d;
        }

        printf("Suma de categorias = %d (N = %zu)\n", suma, tam);
        printf("Suma == N: %s\n", (suma == (int)tam) ? "SI" : "NO");
        printf("Tiempo de ejecucion: %.9f s\n\n", t_sec);
    }

    return 0;
}