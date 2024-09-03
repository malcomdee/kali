#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    printf("PID del proceso actual: %d\n", getpid());
    printf("PID del proceso padre: %d\n", getppid());
    exit(0);
}
