//Include ArduPi library
#include "arduPi.h"

//Include the Math library
#include <math.h>

#define TH 690

/*********************************************************
 *  IF YOUR ARDUINO CODE HAS OTHER FUNCTIONS APART FROM  *
 *  setup() AND loop() YOU MUST DECLARE THEM HERE        *
 * *******************************************************/

/**************************
 * YOUR ARDUINO CODE HERE *
 * ************************/

int main (){
        setup();
        while(1){
                loop();
        }
        return (0);
}

void setup(void) {
}

void loop(void){
  float analogReadingArduino;
  analogReadingArduino = analogRead(5);

  if(analogReadingArduino > TH){
    printf("Getting lighter\n");
  } else {
    printf("Getting darker\n");
  }

  delay(3000);
}