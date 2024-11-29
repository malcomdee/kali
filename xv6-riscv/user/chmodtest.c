// En user/chmodtest.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int
main(void)
{
  int fd;
  char *filename = "testfile";
  char buffer[] = "Hello, xv6!";
  int n;

  // Creación del Archivo
  fd = open(filename, O_CREATE | O_RDWR);
  if(fd < 0){
    printf("Error al crear el archivo\n");
    exit(1);
  }

  // Escritura Inicial
  if(write(fd, buffer, sizeof(buffer)) != sizeof(buffer)){
    printf("Error al escribir en el archivo\n");
    exit(1);
  }
  close(fd);

  // Cambio de Permisos a Solo Lectura
  if(chmod(filename, 1) < 0){
    printf("Error al cambiar permisos a solo lectura\n");
    exit(1);
  }

  // Prueba de Escritura con Solo Lectura
  fd = open(filename, O_WRONLY);
  if(fd >= 0){
    n = write(fd, buffer, sizeof(buffer));
    if(n >= 0){
      printf("Error: Se pudo escribir en un archivo de solo lectura\n");
      close(fd);
      exit(1);
    }
    close(fd);
  } else {
    printf("No se pudo abrir el archivo en modo escritura, como se esperaba\n");
  }

  // Cambio de Permisos de Vuelta a Lectura/Escritura
  if(chmod(filename, 3) < 0){
    printf("Error al restaurar permisos de lectura/escritura\n");
    exit(1);
  }

  // Escritura Final
  fd = open(filename, O_WRONLY);
  if(fd >= 0){
    n = write(fd, buffer, sizeof(buffer));
    if(n < 0){
      printf("Error al escribir en el archivo después de restaurar permisos\n");
      close(fd);
      exit(1);
    }
    close(fd);
  } else {
    printf("Error al abrir el archivo en modo escritura después de restaurar permisos\n");
    exit(1);
  }

  printf("Pruebas de permisos completadas exitosamente\n");

  // --- Segunda Parte: Pruebas del Permiso Inmutable ---

  // Cambio de Permisos a inmutable
  if(chmod(filename, 5) < 0){
    printf("Error al cambiar permisos a inmutable\n");
    exit(1);
  }

  // Prueba de Escritura con Archivo Inmutable
  fd = open(filename, O_WRONLY);
  if(fd >= 0){
    n = write(fd, buffer, sizeof(buffer));
    if(n >= 0){
      printf("Error: Se pudo escribir en un archivo inmutable\n");
      close(fd);
      exit(1);
    } else {
      printf("Correcto: No se pudo escribir en un archivo inmutable\n");
    }
    close(fd);
  } else {
    printf("Correcto: No se pudo abrir el archivo en modo escritura, como se esperaba para un archivo inmutable\n");
  }

  // Intentar cambiar permisos de vuelta a Lectura/Escritura
  if(chmod(filename, 3) == 0){
    printf("Error: Se pudo cambiar los permisos de un archivo inmutable\n");
    exit(1);
  } else {
    printf("Correcto: No se pudo cambiar los permisos de un archivo inmutable, como se esperaba\n");
  }

  printf("Pruebas de archivo inmutable completadas exitosamente\n");

  exit(0);
}
