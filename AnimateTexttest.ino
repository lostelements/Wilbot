#include <SPI.h>
#include "LedMatrix.h"


#define NUMBER_OF_DEVICES 1
#define CS_PIN 4
LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CS_PIN);
int x = 0;

/* Define Emojis crate on their side */

byte happy[] = {0x3c, 0x42, 0xb5, 0xa1, 0xa1, 0xb5, 0x42, 0x3c};
byte frown[] =  {0x3c, 0x42, 0xa5, 0xa1, 0xa1, 0xa5, 0x42, 0x3c};
byte sad[] =  {0x3c, 0x42, 0xa5, 0x91, 0x91, 0xa5, 0x42, 0x3c};

byte clear[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void displayFace(int faceIndex)
{

  /*#define face  faces[faceIndex]*/

 /* iterates over all 8 columns and sets their values*/

 
 ledMatrix.clear();
   ledMatrix.commit();
    if (faceIndex == 1)
    {
    for (int i=0; i < 8; i++){
      ledMatrix.sendByte(0,i+1,happy[i]);
    }
    }
   if (faceIndex == 2)
   {
    for (int i=0; i < 8; i++){
      ledMatrix.sendByte(0,i+1,frown[i]);
    }
   }
    if (faceIndex == 3)
    {
    for (int i=0; i < 8; i++){
      ledMatrix.sendByte(0,i+1,sad[i]);
    }
    }
  
  

}
  
void setup() {
  ledMatrix.init();
  ledMatrix.setIntensity(4);
  ledMatrix.setCharWidth(8);
  ledMatrix.setText("DINNER TIME");
  ledMatrix.setNextText("NOW!");
 
  displayFace(1);
  delay (20000);
 
   displayFace(2);
  delay (20000);
 
   displayFace(3);
  delay (20000);
}



void loop() {

  
  ledMatrix.clear();
  ledMatrix.scrollTextLeft();
  ledMatrix.drawText();
  ledMatrix.commit();
  delay(100);
  x=x+1;
  if (x == 400) {
     ledMatrix.setNextText("DINNER TIME"); 
  }
}
