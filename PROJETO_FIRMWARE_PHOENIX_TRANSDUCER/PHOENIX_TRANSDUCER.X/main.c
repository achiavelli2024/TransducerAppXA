
#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "configbits.h"
#include "app.h"


int main(int argc, char** argv){
    while(1)
        AppRun();
    
    return (EXIT_SUCCESS);
}

