
#include "ope.h"

extern int gA;

__attribute__((always_inline))
void incA() {
    gA++;
}

