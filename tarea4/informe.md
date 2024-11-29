# Informe de Implementación de Permisos en xv6 (RISC-V)

## Introducción

Este informe detalla la implementación de un sistema de permisos básicos en xv6 (RISC-V), incluyendo la adición de un permiso especial para marcar archivos como inmutables. Se describen las modificaciones realizadas en el código fuente, el funcionamiento y lógica detrás de los cambios, así como las dificultades encontradas y las soluciones implementadas.

## Funcionamiento y Lógica

El objetivo principal fue extender el sistema de archivos de xv6 para soportar permisos básicos en los archivos. Los permisos implementados son:

- Permiso de lectura (1): Permite leer el archivo. - Permiso de escritura (2): Permite escribir en el archivo. - Permiso de lectura y escritura (3): Combina los permisos de lectura y escritura. - Permiso inmutable (5): Marca el archivo como inmutable, impidiendo cualquier modificación futura, incluyendo cambios de permisos.

La lógica detrás de estos permisos es controlar el acceso a los archivos y protegerlos contra modificaciones no autorizadas. El permiso inmutable agrega una capa adicional de protección al evitar que incluso el propietario del archivo cambie sus permisos o contenido una vez establecido.

## Explicación de las Modificaciones Realizadas

Se realizaron modificaciones en los siguientes archivos: sysfile.c, user.h, usys.pl, Makefile, syscall.c, syscall.h, file.c, fs.c, stat.h, file.h, fs.h.
A continuación se mostrarán algunos de los archivos modificados:

### 1. Modificación de la Estructura dinode

Para almacenar los permisos sin aumentar el tamaño de la estructura dinode, se aprovechó el campo existente type. Se utilizaron los 8 bits inferiores para el tipo de archivo y los 8 bits superiores para los permisos. Esto evitó problemas con la alineación y el tamaño de los bloques en el sistema de archivos.

Cambios en fs.h:

```c
Copiar código
#define PERM_READ      0x1
#define PERM_WRITE     0x2
#define PERM_IMMUTABLE 0x4

#define TYPE_MASK 0x00FF
#define PERM_MASK 0xFF00

#define GET_TYPE(t) ((t) & TYPE_MASK)
#define GET_PERM(t) (((t) & PERM_MASK) >> 8)
#define SET_TYPE_PERM(type, perm) (((perm) << 8) | ((type) & TYPE_MASK))

struct dinode {
  ushort type;           // 8 bits inferiores: tipo, 8 bits superiores: permisos
  ushort major;
  ushort minor;
  ushort nlink;
  uint size;
  uint addrs[NDIRECT+1];
};
```
### 2. Modificaciones en las Operaciones de Archivos

Se actualizaron las funciones de apertura, lectura y escritura para respetar los nuevos permisos:

- sys_open en sysfile.c: Verifica los permisos antes de permitir la apertura de un archivo en modos de lectura o escritura. Si el archivo es inmutable, no permite abrirlo en modo escritura.

- fileread en file.c: Antes de leer, verifica si el archivo tiene permiso de lectura.

- filewrite en file.c: Antes de escribir, verifica si el archivo tiene permiso de escritura y no es inmutable.

### 3. Implementación de la Llamada al Sistema chmod

Se creó la llamada al sistema chmod para cambiar los permisos de un archivo:

```c
uint64
sys_chmod(void)
{
  char path[MAXPATH];
  int mode;
  struct inode *ip;

  if(argstr(0, path, MAXPATH) < 0)
    return -1;
  argint(1, &mode);

  begin_op();
  if((ip = namei(path)) == 0){
    end_op();
    return -1;
  }
  ilock(ip);

  ushort perm = GET_PERM(ip->type);

  if(perm == 5){
    iunlockput(ip);
    end_op();
    return -1;
  }

  if(mode == 5){
    perm = 5;
  } else {
    perm = mode & 0xFF;
  }

  ushort ftype = GET_TYPE(ip->type);
  ip->type = SET_TYPE_PERM(ftype, perm);

  iupdate(ip);
  iunlockput(ip);
  end_op();
  return 0;
}
```
### 4. Programa de Prueba chmodtest.c

Se creó un programa de prueba para verificar el correcto funcionamiento de los permisos:

Creación y escritura inicial en un archivo.
Cambio de permisos a solo lectura y verificación de restricciones.
Restauración de permisos y verificación de escritura.
Establecimiento del permiso inmutable y verificación de restricciones adicionales.
## Dificultades Encontradas y Soluciones Implementadas

### 1. Aumento del Tamaño de struct dinode

Problema: Al agregar un nuevo campo para los permisos en struct dinode, el tamaño de la estructura aumentó, causando fallos en mkfs debido a la aserción relacionada con el tamaño de los bloques.

Solución: Se decidió utilizar los bits superiores del campo type para almacenar los permisos, evitando así aumentar el tamaño de la estructura. Esto requirió definir máscaras y macros para manipular correctamente el tipo y los permisos.

### 2. Conflictos en Definiciones de Tipos

Problema: Hubo redefiniciones de T_FILE y T_DIR en diferentes archivos (fs.h y stat.h), lo que causaba errores de compilación.

Solución: Se unificaron las definiciones en un solo archivo (stat.h) y se eliminaron las redefiniciones en fs.h. Se aseguró que los valores asignados a estos tipos fueran consistentes en todo el sistema.

### 3. Acceso Inadecuado a ip->type y ip->perm

Problema: Después de modificar la forma en que se almacenaban los permisos, algunas funciones seguían accediendo directamente a ip->type sin utilizar las macros, lo que causaba errores en la interpretación del tipo de archivo.

Solución: Se revisó todo el código, aplicando las macros GET_TYPE y GET_PERM en cada acceso a ip->type o dip->type. Se eliminaron las referencias a ip->perm, ya que este campo ya no existía.

## Conclusión

La implementación de permisos básicos y un permiso especial de inmutabilidad en xv6 fue exitosa. Se aprendió la importancia de considerar el impacto de los cambios en estructuras críticas y la necesidad de mantener la coherencia en las definiciones a lo largo del código. Las dificultades encontradas reforzaron la comprensión del sistema de archivos de xv6 y cómo extender sus funcionalidades de manera efectiva.