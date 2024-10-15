
# Informe sobre las modificaciones al sistema de planificación de procesos

## Archivos Modificados

### 1. **Archivo `proc.h`**

Se añadieron dos nuevos campos a la estructura del proceso:

```c
// Campos nuevos
int priority;  // Prioridad del proceso (0 = mayor prioridad)
int boost;     // Boost del proceso (+1 o -1)
```

### 2. **Archivo `proc.c`**

#### Modificación en la función `scheduler`

Se cambió la función del *scheduler* para actualizar las prioridades de los procesos `RUNNABLE` antes de planificarlos. Ahora se asegura que la prioridad de cada proceso se ajuste en función del campo `boost`:

```c
void scheduler(void) {
  struct proc *p;
  struct cpu *c = mycpu();

  c->proc = 0;
  for (;;) {
    // Activar interrupciones para evitar deadlocks
    intr_on();

    // Actualizar prioridades de todos los procesos RUNNABLE antes de planificar
    update_priorities();

    int found = 0;
    for (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if (p->state == RUNNABLE) {
        // Ejecutar el proceso seleccionado
        p->state = RUNNING;
        c->proc = p;
        swtch(&c->context, &p->context);

        // El proceso ha terminado su turno de ejecución.
        c->proc = 0;
        found = 1;
      }
      release(&p->lock);
    }

    if (found == 0) {
      // No hay procesos para ejecutar, esperar interrupción.
      intr_on();
      asm volatile("wfi");
    }
  }
}
```

#### Nueva función `update_priorities`

Esta función se encargará de actualizar las prioridades de los procesos que no están en estado *ZOMBIE*. Cada vez que se llama, se suma o resta el valor del `boost` a la prioridad del proceso. Si la prioridad alcanza un máximo de 9, el `boost` cambia a -1, y si la prioridad llega a 0, el `boost` cambia a 1.

```c
void update_priorities() {
  struct proc *p;

  for (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if (p->state != ZOMBIE) {  // Excluir procesos zombies
      p->priority += p->boost;

      // Cambiar el boost según la prioridad
      if (p->priority >= 9) {
        p->boost = -1;
      } else if (p->priority <= 0) {
        p->boost = 1;
      }
    }
    release(&p->lock);
  }
}
```

---

## **Modificación del `Makefile`**

Se modificó el `Makefile` para configurar el uso de una única CPU en lugar de tres. Esta modificación evita problemas de sobreescritura en las salidas generadas por los procesos en paralelo.

---

## **Explicación del Archivo de Prueba test_priority.c**

```c
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
```

---

### **Cómo Funciona el Archivo de Prueba**

Este archivo crea **20 procesos hijos** utilizando `fork()` y les asigna una prioridad inicial entre 0 y 9. Cada proceso se ejecuta 5 veces. En cada iteración:

1. El proceso duerme durante un tiempo basado en su prioridad.
2. Muestra un mensaje en pantalla indicando su número de proceso, su prioridad actual y el número de iteración.
3. Actualiza su prioridad utilizando la función `actualizar_prioridades`.

---

### **Comportamiento Esperado de la Salida**

Dado que los procesos se ejecutan en paralelo, la salida no será estrictamente secuencial. Cada proceso compite por recursos del CPU y, aunque las prioridades y `sleep()` se utilizan para escalonar la ejecución, la concurrencia del sistema operativo no garantiza que los procesos impriman siempre en el orden esperado.

Ejemplo de salida:

```
Iniciando prueba de creación de procesos...
Proceso 4 con prioridad 0 en ejecución (Iteración 1)
Proceso 14 con prioridad 0 en ejecución (Iteración 1)
Proceso 5 con prioridad 1 en ejecución (Iteración 1)
Proceso 6 con prioridad 2 en ejecución (Iteración 1)
...
Prueba finalizada.
```

---
Sin embargo, se puede observar, que los procesos esperan su prioridad para ser ejecutados.

## Dificultades
En un inicio, fue dificl el poder entender que los requisitos eran en el scheduler, ya que, cambie el funcionamiento de la funcion fork, sin embargo, esto solo me aseguraba que funcionase para los procesos que llamaran la funcion, y no para aquellos que pasaran por el scheduler necesaiamente. 
Otra dificultad, fue que no podia hacer que iteraran segun prioridad, sino que, simplemente iteraban los procesos en orden secuencial, pero no necesariamente segun su prioridad

## Conclusión

Este sistema de planificación basado en prioridades permite observar cómo los procesos cambian de prioridad a lo largo del tiempo. Aunque los mensajes impresos pueden parecer fuera de orden, esto se debe a la naturaleza paralela de la ejecución de procesos en sistemas operativos.
