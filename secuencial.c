#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define REPETICIONES 5

double medir_tiempo(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int suma_total(int *array, size_t tam){
    int suma = 0;
    for(size_t i = 0; i < tam; i++){
        suma += array[i];
    }
    return suma;
}

int min_val(int *array, size_t tam){
    int min = array[0];
    for(size_t i = 1; i < tam; i++){
        if(array[i] < min){
            min = array[i];
        }
    }
    return min;
}

int max_val(int *array, size_t tam){
    int max = array[0];
    for(size_t i = 1; i < tam; i++){
        if(array[i] > max){
            max = array[i];
        }
    }
    return max;
}

double promedio(int tam, int suma){
    double prom = (double)suma / tam;
    return prom;
}

int main(){

    size_t N[3] = {1000, 100000, 1000000};

    for(int i = 0; i < 3; i++){

        size_t tam = N[i];
        int *array = malloc(tam * sizeof(int));

        printf("Arreglo con: %zu valores\n", tam);

        const unsigned int sem = 42;
        srand(sem);

        for(size_t j = 0; j < tam; j++){
            array[j] = rand() % 50 + 1;
        }

        int suma, min, max;
        double t_total = 0.0;

        for(int r = 0; r < REPETICIONES; r++){
            double t0 = medir_tiempo();
            suma = suma_total(array, tam);
            min = min_val(array, tam);
            max = max_val(array, tam);
            t_total += medir_tiempo() - t0;
        }

        double t_sec = t_total / REPETICIONES;
        double prom = promedio(tam, suma);

        printf("Suma: %d\n", suma);
        printf("Minimo valor: %d\n", min);
        printf("Maximo Valor: %d\n", max);
        printf("Promedio: %.2f\n", prom);
        printf("Tiempo de ejecucion: %.6f s\n\n", t_sec);

        free(array);
    }

    return 0;
}