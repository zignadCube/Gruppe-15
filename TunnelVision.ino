
#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN    3  //Datapin
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    100  //Antall led
CRGB leds[NUM_LEDS];

#define NUM_BOX 6  //Antall soner
const int firstBox = 4; //Pinen til den første boksen
int box[NUM_BOX];

//Diverse variabler
int pos = 0;
int prevCount = 0;
int nextCount = 0;

//Diverse konstanter
const int countMax = 400;
const int countTresh = 300;
const int diff = 14; //Størrelsen på hver sone
const int buff = diff; //Buffer for hvor langt lysene kan gå

//Diverse vektorer
int count[NUM_BOX]; //Vektor med informasjon om hvor lenge det er siden noen ble registrert i en sone
int posvec[NUM_BOX]; //Posisjonen til sonene
int lightVec[NUM_BOX*2]; //Posisjonen til LEDene som lyser

const int del = 2; //Delay for lysets bevegelse
int delCount = del;
const int lightStep = 1; //Hvor langt lyset flytter seg

//Startverdier for bakgrunnsfarge
uint8_t r0 = 200;
uint8_t g0 = 70;
uint8_t b0 = 4;

bool activeState = LOW; //Spesifiserer om sensorene er aktivt lave eller aktivt høye

#define BRIGHTNESS          96 
#define FRAMES_PER_SECOND  120

void setup() {
  delay(1000);

  //Fyller inn verdier i vektorene
  for(int i=0; i<NUM_BOX; i++){
    box[i]=firstBox+i;
    pinMode(box[i], INPUT);

    count[i]=0;
    posvec[i]=(i+1)*diff;
    lightVec[i*2]=-1;
    lightVec[i*2+1]=-1;
  }
  
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
 

  FastLED.setBrightness(BRIGHTNESS);
}

  
void loop()
{

  FastLED.show(); //Oppdater lysene
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { colourUpdate(); } //Oppdaterer bakgrunnsfargen
  lysTing(); //Hovedfunksjonen
  
 
}


void lysTing(){
  lysFade(); //Fader lysene mot bakgrunnsfargen


  //Sjekker om boksene er aktive
  for(int i=0; i<NUM_BOX; i++){
    if(digitalRead(box[i])==activeState&&count[i]<countTresh){
      count[i]=countMax;
    }
       
  }


  for(int i=0; i<NUM_BOX; i++){

    if(count[i]>=countMax-1){
      //Forhindrer indeksfeil
      if(i==NUM_BOX-1){
        nextCount=countTresh;
      }
      else{
        nextCount=count[i+1];
      }
      
      if(i==0){
        prevCount=countTresh;
      }
      else{
        prevCount=count[i-1];
      }
      
      //Sjekkr om noen går bakover
      if(nextCount>0 && nextCount>prevCount){
        lightVec[2*i]=posvec[i]+diff;
      }
      
      //Sjekker om noen går forover
      if(prevCount>0 && prevCount>=nextCount){
        lightVec[2*i+1]=posvec[i]-diff;        
      }            
    }
    
    if(count[i]>0){
      count[i]--;
    }  
      
  }


  if(delCount<1){ //Delay for å styre farten
    delCount=del;  
    for(int i=0; i<NUM_BOX*2; i++){
  
      if(lightVec[i]>=0){        
        leds[lightVec[i]-1] = CRGB::Red; //Setter fargen på lysene
        leds[lightVec[i]] = CRGB::Red;
        leds[lightVec[i]+1] = CRGB::Red;
        if(i%2==0){
          lightVec[i]-=lightStep; //Beveger lyset bakover
          if(lightVec[i]<posvec[i/2]-buff){//Stopper lyset om det går for langt
            lightVec[i]=-1;
          }
        }
        else{
          lightVec[i]+=lightStep;//Beveger lyset framover
          if(lightVec[i]>posvec[(i-1)/2]+buff){//Stopper lyset om det går for langt
            lightVec[i]=-1;                
          }
        }     
      }
    }
  }
  else{
    delCount--; //Teller videre på delayen
  }

}

void lysFade(){ //Fader fargene mot bakgrunnsfargen
  for(int i=0; i<NUM_LEDS; i++){
    if(leds[i].r>r0){
      leds[i].r--;
    }
    else if(leds[i].r<r0){
      leds[i].r++;
    }

    if(leds[i].g>g0){
      leds[i].g--;
    }
    else if(leds[i].g<g0){
      leds[i].g++;
    }

    if(leds[i].b>b0){
      leds[i].b--;
    }
    else if(leds[i].b<b0){
      leds[i].b++;
    }
    
  }
    
}

void colourUpdate(){ //Oppdaterer bakgrunnsfargen
  r0++;
  g0++;
  b0++;
  
}
