# Laboratorio: Programación con Pthreads

**Universidad Nacional de San Agustín**
Escuela Profesional de Ciencia de la Computación
Curso: Computación Paralela y Distribuida

**Alumno:** Castelo Choque, Joaquin Andreé

## Descripción

Implementaciones en C con Pthreads de tres problemas

1. **Estadísticas de un arreglo:** suma, mínimo, máximo y promedio.
2. **Histograma de 10 categorías:** con dos estrategias paralelas, histograma privado por thread e histograma compartido protegido con mutex.
3. **Productores y consumidores:** buffer circular acotado sincronizado con mutex y semáforos.

## Compilación y ejecución

```
gcc archivo.c -o m
./m
```

Si el compilador lo pide, agrega `-pthread`:

```
gcc archivo.c -o m -pthread
```

## Entorno de pruebas

- Procesador: 11th Gen Intel Core i5-11320H @ 3.20 GHz (4 núcleos físicos, 8 threads lógicos)
- RAM: 24 GB
- Sistema operativo de 64 bits

## Notas

- Los datos se generan con `rand()` y semilla fija 42, para que los resultados sean reproducibles.
- En las Partes I y II cada tiempo es el promedio de 5 repeticiones.
