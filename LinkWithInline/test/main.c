//#include <stdio.h>
#include "ope.h"


void testCase01();

int gA = 0;


int main(void) {
    
    testCase01();

    return 0;
}

void testCase01() {
    
    incA();
    readA();
    refIncA();

    //printf("gA is: %d\n", gA);
}

