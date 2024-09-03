#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: yosoytupadre <numero>\n");
        exit(1);
    }

    int n = atoi(argv[1]); 
    int ancestro;

    printf("Mi PID es: %d, mi PPID es: %d\n", getpid(), getppid());

    
    if ((ancestro = getancestor(n)) == -1) {
        printf("No hay suficientes ancestros: %d\n", ancestro);
    } else {
        
        printf("Mi ancestro (0) es: %d\n", getancestor(0));
        printf("Mi padre es: %d\n", getancestor(1));
        printf("Mi abuelo es: %d\n", getancestor(2));
    }

    exit(0);
}
