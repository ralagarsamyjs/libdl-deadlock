#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "persistencestore.h"

void Initialization() {
    printf("persistence initialization is going to wait in loop \n");
    while (true) {
       sleep(2);
    }
}
