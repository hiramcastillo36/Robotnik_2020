#include <Wire.h>
#include <MechaQMC5883.h>
MechaQMC5883 qmc;

float deg,vu;
float zero = 0.00f, current;
long long int tiempo;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  qmc.init();
   
  pinMode(0, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  
  pinMode(37,OUTPUT);
  pinMode(35,OUTPUT);
  
  pinMode(43,OUTPUT);
  pinMode(47,OUTPUT);
  
  pinMode(49,OUTPUT);
  pinMode(45,OUTPUT);  

  pinMode(13,OUTPUT);
}
 
void motor_AA(float a){
  digitalWrite(47,LOW);
  digitalWrite(49,HIGH);
  analogWrite(10, a);
}
void motor_AT(float a){
  digitalWrite(47,HIGH);
  digitalWrite(49,LOW);
  analogWrite(10, a);
}
void motor_BA(float b){
  digitalWrite(45,LOW);
  digitalWrite(43,HIGH);
  analogWrite(9, b);
}
void motor_BT(float b){
  digitalWrite(45,HIGH);
  digitalWrite(43,LOW);
  analogWrite(9, b);
}
void motor_CA(float c){
  digitalWrite(35,HIGH);
  digitalWrite(37,LOW);
  analogWrite(11, c);
}
void motor_CT(float c){
  digitalWrite(35,LOW);
  digitalWrite(37,HIGH);
  analogWrite(11, c);
}
void off(){
  digitalWrite(47,HIGH);
  digitalWrite(49,HIGH);
  analogWrite(10, 255);
  
  digitalWrite(45,HIGH);
  digitalWrite(43,HIGH);
  analogWrite(9, 255);
  
  digitalWrite(35,HIGH);
  digitalWrite(37,HIGH);
  analogWrite(11, 255);
}

void setMotor(int a, int b, int c){
  if(a>=0){//adelant  
    a=abs(a);
    a=min(a,100);
    //adelante
    motor_AA(a);  
    digitalWrite(13,LOW);
  }
  else if(a<0){ //atras  
   a=abs(a);
    a=min(a,100);
    motor_AT(a);
    
    digitalWrite(13,LOW);
  } 
  if(b>=0){//adelante
    
    b=abs(b);
    b=min(b,100);
    motor_BA(b);  
    
    digitalWrite(13,LOW);
  }
  else if(b<0){ //atras
     
    b=abs(b);
    b=min(b,100);
    motor_BT(b); 
    
    digitalWrite(13,LOW);
  }
  if(c>=0){//adelante
     
    c=abs(c);
    c=min(c,100);
    motor_CA(c);
    digitalWrite(13,LOW);
    }
 else if(c<0){ //atras
     c=abs(c);
    c=min(c,100);
    motor_CT(c);
    digitalWrite(13,LOW);
  } 
  //agregar el ceor  
}

int angulo=0;

void loop() { 
  float a=cos((angulo-30)*M_PI/180)*100;
  float b=cos((angulo+30)*M_PI/180)*100;
  float c=-cos((angulo-90)*M_PI/180)*100;
  
  int x,y,z;
  qmc.read(&x,&y,&z);
  float heading = atan2(y,x);
  if(heading<0)heading+=2*M_PI;
  
  heading=heading*180/M_PI;
  
  current=heading; 
  if(current>180)
    current=current-360;
  vu=current;
  
  int kp=255;
  float valor=(vu/180*kp);
  
  setMotor(valor,valor,valor);
  
  Serial.print(a);
  Serial.print("  ");
  Serial.print(b);
  Serial.print("  ");
  Serial.print(c);
  Serial.print(" " );
  Serial.println(vu );
  
  if(millis()-tiempo>2){
    tiempo=millis();
    angulo+=10;  
  }
}
