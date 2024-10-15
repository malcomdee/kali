#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_PROCESOS 20  // Número de procesos a crear
#define ITERACIONES 5    // Cuántas veces ejecutaremos cada proceso

int prioridades[NUM_PROCESOS];
int boost[NUM_PROCESOS];

// Inicializa las prioridades y boosts para cada proceso
void inicializar_prioridades() {
  for (int i = 0; i < NUM_PROCESOS; i++) {
    prioridades[i] = i % 10;  // Alternar entre 0 y 9
    boost[i] = 1;  // Inicializar con boost positivo
  }
}

// Actualiza las prioridades en función del boost
void actualizar_prioridades(int i) {
  prioridades[i] += boost[i];

  if (prioridades[i] >= 9) {
    boost[i] = -1;  // Cambiar boost a negativo
  } else if (prioridades[i] <= 0) {
    boost[i] = 1;  // Cambiar boost a positivo
  }
}

// Crear y ejecutar procesos varias veces para mostrar cambios de prioridad
void crear_procesos(int n) {
  inicializar_prioridades();

  for (int i = 0; i < n; i++) {
    int pid = fork();  // Crear un proceso hijo
    if (pid == 0) {    // Código del proceso hijo
      for (int j = 0; j < ITERACIONES; j++) {  // Ejecutar varias veces
        sleep(prioridades[i]);  // Espera según la prioridad actual
        printf("Proceso %d con prioridad %d en ejecución (Iteración %d)\n", 
               getpid(), prioridades[i], j + 1);
        actualizar_prioridades(i);  // Actualizar prioridad del proceso
      }
      exit(0);  // Terminar el proceso hijo
    }
  }

  // Esperar a que todos los procesos terminen
  for (int i = 0; i < n; i++) {
    wait(0);
  }
}

int main(int argc, char *argv[]) {
  printf("Iniciando prueba de creación de procesos...\n");
  crear_procesos(NUM_PROCESOS);
  printf("Prueba finalizada.\n");
  exit(0);
}
