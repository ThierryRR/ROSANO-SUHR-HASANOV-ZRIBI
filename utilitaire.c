#include "boolean.h"
volatile int temps = 0;

void temps_init() {
    temps++;
}
END_OF_FUNCTION(temps_init);