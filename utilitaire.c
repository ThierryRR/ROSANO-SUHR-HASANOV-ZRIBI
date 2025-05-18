#include "boolean.h"

// variable globale volatile qui sert de compteur de temps
volatile int temps = 0;
// fonction appelée périodiquement par un timer
void temps_init() {
    temps++;  // incrémente la variable de temps
}
END_OF_FUNCTION(temps_init);
