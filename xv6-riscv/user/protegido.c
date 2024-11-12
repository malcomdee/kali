#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: protegido <número de caso>\n");
        exit(1);
    }

    int caso = atoi(argv[1]);
    char *addr = sbrk(0);  // Obtener la dirección actual del heap
    char *ptr;

    switch(caso) {
        case 1:
            // Caso 1: Proteger una página correctamente
            sbrk(4096);  // Reservar una página
            if (mprotect(addr, 1) == -1) {
                printf("Caso 1: mprotect falló\n");
            } else {
                printf("Caso 1: mprotect exitoso\n");
            }
            printf("Intentando escribir en la página protegida...\n");
            *addr = 'A';  // Debería causar un fallo de página
            printf("Caso 1: Valor en la dirección: %c\n", *addr);
            break;

        case 2:
            // Caso 2: Dirección no alineada
            sbrk(4096);  // Reservar una página
            if (mprotect(addr + 1, 1) == -1) {
                printf("Caso 2: mprotect falló con dirección no alineada (correcto)\n");
            } else {
                printf("Caso 2: mprotect debería haber fallado con dirección no alineada\n");
            }
            break;

        case 3:
            // Caso 3: Longitud negativa
            sbrk(4096);  // Reservar una página
            if (mprotect(addr, -1) == -1) {
                printf("Caso 3: mprotect falló con longitud negativa (correcto)\n");
            } else {
                printf("Caso 3: mprotect debería haber fallado con longitud negativa\n");
            }
            break;

        case 4:
            // Caso 4: Dirección inválida (página no asignada)
            if (mprotect((void *)0x0, 1) == -1) {
                printf("Caso 4: mprotect falló con dirección inválida (correcto)\n");
            } else {
                printf("Caso 4: mprotect debería haber fallado con dirección inválida\n");
            }
            break;

        case 5:
            // Caso 5: Proteger múltiples páginas
            sbrk(8192);  // Reservar dos páginas adicionales
            addr = sbrk(0) - 8192;  // Dirección de las dos páginas reservadas
            if (mprotect(addr, 2) == -1) {
                printf("Caso 5: mprotect falló al proteger múltiples páginas\n");
            } else {
                printf("Caso 5: mprotect exitoso al proteger múltiples páginas\n");
            }
            printf("Intentando escribir en las páginas protegidas...\n");
            ptr = addr;
            *ptr = 'B';  // Debería causar un fallo de página
            printf("Caso 5: Valor en la dirección: %c\n", *ptr);
            break;

        case 6:
            // Caso 6: Usar munprotect para restaurar permisos
            sbrk(8192);  // Reservar dos páginas adicionales
            addr = sbrk(0) - 8192;
            if (mprotect(addr, 2) == -1) {
                printf("Caso 6: mprotect falló al proteger múltiples páginas\n");
                break;
            }
            printf("Caso 6: mprotect exitoso al proteger múltiples páginas\n");
            if (munprotect(addr, 2) == -1) {
                printf("Caso 6: munprotect falló\n");
            } else {
                printf("Caso 6: munprotect exitoso\n");
            }
            printf("Intentando escribir en las páginas después de munprotect...\n");
            ptr = addr;
            *ptr = 'C';  // Debería tener éxito
            printf("Caso 6: Valor en la dirección: %c\n", *ptr);
            break;

        case 7:
            // Caso 7: munprotect con dirección no alineada
            sbrk(4096);  // Reservar una página
            if (munprotect(addr + 1, 1) == -1) {
                printf("Caso 7: munprotect falló con dirección no alineada (correcto)\n");
            } else {
                printf("Caso 7: munprotect debería haber fallado con dirección no alineada\n");
            }
            break;

        case 8:
            // Caso 8: Proteger páginas fuera del rango del proceso
            addr = (char *)0x800000000000;  // Dirección fuera del rango típico
            if (mprotect(addr, 1) == -1) {
                printf("Caso 8: mprotect falló con dirección fuera del rango (correcto)\n");
            } else {
                printf("Caso 8: mprotect debería haber fallado con dirección fuera del rango\n");
            }
            break;

        case 9:
            // Caso 9: Proteger con len = 0
            sbrk(4096);  // Reservar una página
            if (mprotect(addr, 0) == -1) {
                printf("Caso 9: mprotect falló con len = 0 (correcto)\n");
            } else {
                printf("Caso 9: mprotect debería haber fallado con len = 0\n");
            }
            break;

        default:
            printf("Caso no reconocido\n");
            break;
    }

    exit(0);
}
