
#include "ope.h"

extern int gA;

__attribute__((always_inline))
void incA() {
    gA++;
}

__attribute__((always_inline))
int readA() {
    return gA;
}

__attribute__((always_inline))
void refIncA() {
    int *tmp = &gA;
    (*tmp)++;
}
