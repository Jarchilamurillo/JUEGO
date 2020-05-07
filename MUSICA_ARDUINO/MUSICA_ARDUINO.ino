#include <SD.h>                           //include SD module library
#include <TMRpcm.h>                       //include speaker control library

#define SD_ChipSelectPin 4                //define CS pin

TMRpcm tmrpcm;                            //crete an object for speaker library

int x1 = 0;
int x2 = 0;
int x3 = 0;
int x4 = 0;

void setup(){
  pinMode(9, OUTPUT);
  tmrpcm.speakerPin = 9;
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  
  Serial.begin(9600);
  
  if (!SD.begin(SD_ChipSelectPin)) {      //see if the card is present and can be initialized
    Serial.println("SD fail");
    return;                               //don't do anything more if not
  }
  
  tmrpcm.setVolume(6);                    //0 to 7. Set volume level
  //tmrpcm.play("init.wav);  
}

void loop(){

  if(digitalRead(2)==HIGH &&  x1==0){ //MÚSICA DE INICIO
    x1++;
    x2=0;
    x3=0;
    x4=0;
    //tmrpcm.stopPlayback();
    tmrpcm.play("init.wav");
  }
  //else if(digitalRead(2)==LOW && digitalRead(3)==HIGH && digitalRead(5)==LOW && digitalRead(6)==LOW && x2==0){ //MÚSICA PARA IR ADELANTE
  if(digitalRead(3) == HIGH && x2==0){
    x2++;
    x3=0;
    tmrpcm.stopPlayback();
    tmrpcm.play("fwd.wav");
  }
  //else if(digitalRead(2)==LOW && digitalRead(3)==LOW && digitalRead(5)==HIGH && digitalRead(6)==LOW && x3==0){ // MÚSICA PARA CUANDO SE GIRA
  if(digitalRead(5)==HIGH && x3==0){ //PC_7
    x2=0;
    x3++;
    tmrpcm.stopPlayback();
    tmrpcm.play("turn.wav");
  }
  //else if(digitalRead(2)==LOW && digitalRead(3)==LOW && digitalRead(5)==LOW && digitalRead(6)==HIGH && x4==0){//MÚSICA CUANDO SE CHOCA
  if(digitalRead(6)==HIGH){
    tmrpcm.stopPlayback();
    tmrpcm.play("boom.wav");
  }
  
}
