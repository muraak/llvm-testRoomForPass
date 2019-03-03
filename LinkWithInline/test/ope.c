
#include "ope.h"

extern int gA;

void incA() {
    gA++;
}

int readA() {
    return gA;
}

void refIncA() {
    int *tmp = &gA;
    (*tmp)++;
}
