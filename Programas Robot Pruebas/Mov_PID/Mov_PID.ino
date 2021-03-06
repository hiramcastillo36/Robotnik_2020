#include <Wire.h>
#include <DueFlashStorage.h>
#include <Adafruit_NeoPixel.h>
#include "UltrasonicoLib.h"
#include "IRLocator360.h"
#include "Motor.h"
#include "SparkFun_BNO080_Arduino_Library.h"

//..........Constantes.....................
#define in 140
#define inn 19600
#define umbral 3000
float kp=0.4;
#define ki 0
#define kd 0
#define PIN 48
#define PIN2 52
#define PIN3 50
#define PIN4 46
#define NUMPIXELS 3 
const int magx = -100;
const int magy = -140;
float vel=0.2;
int contador=0;
int ina[]={0,36,40,49};
int inb[]={0,38,42,23};
int pwm[]={0,13,12,2};    
int ports[]={A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11};
int sensorX[]={1,0,-1,0};
int sensorY[]={0,1,0,-1};
int mov[]={180,270,0,90};
int botones[]={8,44,53};
int leds[]={9,10,11};
int periodo = 1000;
int camara;
int atras, y, z;
unsigned long TiempoAhora = 0;
unsigned long tiempo = 0;
unsigned long tiempo1, tiempo2;
double magn;
double targetRotation=0.0;
double Set=0.0;
double h,reg;
float a, b, c, valor;
bool im,lado;
bool sensor[4];

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel pixel(NUMPIXELS, PIN2, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel pixe(NUMPIXELS, PIN3, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel pix(NUMPIXELS, PIN4, NEO_RGB + NEO_KHZ800);
DueFlashStorage dueFlashStorage;
Motor mot(ina,inb,pwm,225,225);
IRLocator360 IR;
BNO080 myIMU;
Ultrasonico us1(4,3); //derecha
Ultrasonico us2(6,5); //izquierda 
Ultrasonico us3(19,18); //atras

double getFilter(double rot){
  double rt=rot;
  if(rt<=180)
    return rt/180.0;
  return (rt-360)/180.0;
}

double error(double rot){
    double err=360.00-rot;
    if(err>180.00)
      err=err-360.00;
    return err;    
}

double getRawRotation(){
  myIMU.dataAvailable();
  float x = myIMU.getQuatI();
  float y = myIMU.getQuatJ();
  float z = myIMU.getQuatK();
  float w = myIMU.getQuatReal();
  float quatRadianAccuracy = myIMU.getQuatRadianAccuracy();
  double siny_cosp = 2.0 * (w * z + x * y);
  double cosy_cosp = 1.0 - 2.0 * (y * y + z * z);
  return atan2(siny_cosp, cosy_cosp)*180.0/3.141592653;
}

double mag(){
  myIMU.dataAvailable();
  float x = myIMU.getMagX();
  float y = myIMU.getMagY();
  float z = myIMU.getMagZ();
  return atan2(x-magx, y-magy)*180.00/M_PI;
}

double getRotation(){
  // Leer la rotacion en cuanto al Norte Magnetico del IMU
  double rot=0;
  Set < 0 ? rot=360.00+Set : rot = Set;
  // Obtener la rotacion relativa al frente inicial (rotacion "0")
  double rt=rot+getRawRotation();
  // Condiciones para asegurarnos de que el valor de rotacion
  // regresado (como resultado) por la funcion sea valido 
  // (entre 0 y 360).
  if(rt<0)
    rt+=360;
  if(rt>360)
    rt-=360;
  // Regresar la rotacion actual del robot
  return rt;
}

double escribir(int alineacion){
  dueFlashStorage.write(3, alineacion%255);
  dueFlashStorage.write(4, alineacion/255);
  delay(50);
}

int angulo(){
    int angle=0;
    int sumax=0;
    int sumay=0;
    contador=0;
    sensor[0]>0 ?  sumax = sumax + sensorX[0] : sumay= sumay + sensorY[0]; // 0
    sensor[1]>0 ?  sumay = sumay + sensorY[1] : sumax= sumax + sensorX[1]; // 90
    sensor[2]>0 ?  sumax = sumax + sensorX[2] : sumay= sumay + sensorY[2]; // 180
    sensor[3]>0 ?  sumay = sumay + sensorY[3] : sumax= sumax + sensorX[3]; // 270
    if(sumax!=0&&sumay!=0){
        angle=atan((sumay/sumax))*180.00/M_PI;
        if(angle<0){
            if(sensor[1])
                angle=angle+360;
            else
                angle=angle+180;
        }
        else
           if(sumax!=-1&&sumay!=-1)angle=angle+180;
    }
    else{
        if(sumax==0&&sumay==0)
            return -1;
        else{
            for(int i=0; i<=3; i++){
                if(sensor[i]){
                    angle=angle+mov[i];
                    contador++;
                }
            }
        }
    }
    if(contador==3){
        if(angle==360||angle==540)
            angle=angle-360;
        else
            angle=angle-180;
    }
    return angle;
}

bool isLine(int a, int b){
  for(int i=a; i<=b; i++){
    if(analogRead(ports[i])>=umbral)
      return true; 
  }
  return false;
}

void setup() {
  Serial.begin(19200);
  Wire.begin();
  myIMU.begin();
  myIMU.enableRotationVector(50);
  myIMU.enableMagnetometer(50);
  while(millis()<600){
    targetRotation=dueFlashStorage.read(3)+dueFlashStorage.read(4)*255;
    Set=getRawRotation()+(targetRotation-mag());
    delay(20);
  }  
  IR.sensorInitialization();
  analogReadResolution(12);
  tiempo1 = millis();
  pixels.begin();
  pixel.begin();
  pixe.begin();  
  pix.begin();
  for(int i=0; i<NUMPIXELS; i++) { //GRB
    pixels.setPixelColor(i, pixels.Color(0, 50, 50));
    pixel.setPixelColor(i, pixels.Color(0, 50, 50));
    pixe.setPixelColor(i, pixels.Color(0, 50, 50));    
    pix.setPixelColor(i, pixels.Color(0, 50, 50));
    pix.show();    
    pixe.show();
    pixel.show(); 
    pixels.show();
  }
  for(int i=0; i<3; i++){
    pinMode(leds[i],OUTPUT);
    pinMode(botones[i],INPUT_PULLUP);
  }
  us1.begin();
  us2.begin();
  us3.begin(); 
  delay(20);
}
void setABC(double g, float pwm, double power){
  a=cos((power+g-(30))*M_PI/180)*pwm;
  b=cos((power+g-90)*M_PI/180)*pwm;
  c=-cos((power+g+30)*M_PI/180)*pwm;
}
void loop() {
  magn=mag();
  tiempo2 = millis();
  int intensidad=IR.signalStrength1200hz();
  int angle=IR.angleDirection600hz();  
  bool x;
  bool banderasl=0;
  double erro = getRawRotation() - Set;
  erro < 0 ? erro= 360.00+erro : erro=erro;  
  sensor[0]=isLine(6,8);//bien
  sensor[1]=isLine(9,11);
  sensor[2]=isLine(3,5);
  sensor[3]=isLine(0,2);
  int sl=angulo();
  double imu=error(erro);
  valor=(imu/180.00*350);
  double error=0-imu;
  double power=error;
  setABC(0, 200, power*kp);
  mot.set(valor-a,1);
  mot.set(valor+b,2);
  mot.set(valor-c,3);
  Serial.print(valor-a);
  Serial.print("\t");
  Serial.print(valor+b);
  Serial.print("\t");
  Serial.print(valor-c);
  Serial.print("\t");
  Serial.println(power*vel);
  if(imu>=-5&&imu<=5)
    digitalWrite(9,HIGH);
  else
    digitalWrite(9,LOW);  
  if(!digitalRead(8)==1){
    mot.off();  
    magn > 0 ? magn=magn : magn=360.00+magn;
    escribir(magn);   
    Set=getRawRotation();
  }  
}
