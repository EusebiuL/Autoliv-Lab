
#include <Arduino.h>

#define RED_LED_PIN 10
#define GREEN_LED_PIN 11
#define MOTOR1_PIN1 3  
#define MOTOR1_PIN2 5
#define MOTOR2_PIN1 6
#define MOTOR2_PIN2 9
#define L1_PIN 0  //DIN FATA
#define L2_PIN 1  //DIN SPATE
#define L3_PIN 7  //ULTRASUNET
#define DIS_INFRA(x) (double)((2650/(x+95))-3)
#define DIS_ULTRA(x) (int)((x/29)/2)
#define TH_SPOT 15
#define TH_WALL 21
#define TH_L3 4
typedef enum state1{S0,S1,S2,S3,S4,S5,S6} stari1;
stari1 stare1=S0;
typedef enum state2{L0,L1,L2,L3} stari2;
stari2 stare2=L0;
int car_det=0,eps=3,d_l1,d_l2,d_l3,verifled=0;
int flag_L0=0,flag_L1=0,flag_L3=0;
int Ultra()
{
 pinMode(L3_PIN,OUTPUT);
 digitalWrite(L3_PIN,0);
 delayMicroseconds(2);
 digitalWrite(L3_PIN,1);
 delayMicroseconds(10);
 digitalWrite(L3_PIN,0);
 pinMode(L3_PIN,INPUT);
 int value=pulseIn(L3_PIN,1);
 return DIS_ULTRA(value);
}

void fwd(int val)
{
  analogWrite(MOTOR1_PIN1,0); //in spate
  analogWrite(MOTOR1_PIN2,val); //in fata
  
  }
void bck(int val)
{
  analogWrite(MOTOR1_PIN2,0);
  analogWrite(MOTOR1_PIN1,val);
  
  } 
 void stopm()
{
  analogWrite(MOTOR1_PIN1,0);
  analogWrite(MOTOR1_PIN2,0);
}
void left(int val)
{
  analogWrite(MOTOR2_PIN1,0);
  analogWrite(MOTOR2_PIN2,val);
  }
void right(int val)
{
  analogWrite(MOTOR2_PIN2,0);
   analogWrite(MOTOR2_PIN1,val);
   
  }
void straight()
{
    analogWrite(MOTOR2_PIN2,0);
   analogWrite(MOTOR2_PIN1,0);
  }
int ReadUltra()
{
  int a=Ultra();
  static int oldvalue;
  if(a<1)
  {
    a=oldvalue;
  }
  oldvalue=a;
  return a;
  }
 /* void parkingAlg()
    {
     
      if( d_l2<=TH_WALL ) { right(200);bck(80);}
      else {//stopm();
      straight();
      left(200);
      bck(80);
      flag_car_det=1;}
      if ( (abs(d_l1-d_l2)<4 || d_l3<=TH_L3 ) && (flag_car_det==1))
          {stopm();}
         else {if( abs(d_l1-d_l2)<4 || d_l3>TH_L3) {bck(50);}
                straight();
                stopm();
                }
      while(d_l3>=TH_L3 +1) fwd(50);
      stopm();
      }*/
  void led()
  {
    if(stare1!=S5 && stare1!=S6) digitalWrite(RED_LED_PIN,1);
      else if (stare1==S5) 
                {
                if(verifled==0)
                {
                digitalWrite(RED_LED_PIN,0);
                verifled=1;
                }
                digitalWrite(GREEN_LED_PIN,1);
                delay(800);
                digitalWrite(GREEN_LED_PIN,0);
                delay(800);
                 }
                 else digitalWrite(GREEN_LED_PIN,1);
   
    }
void St2()
{
  switch(stare2)
  {
    case L0: {
              if(!flag_L0) {stopm();right(200);flag_L0++;}  
              while( d_l2<=TH_WALL ) { bck(80); d_l2=DIS_INFRA(analogRead(L2_PIN));}  
              stare2=L1;
              if(flag_L0==1) {stopm();flag_L0++;}
              break;
               }
    case L1:{
      if(!flag_L1) {
            straight();
            left(200);
            bck(80);flag_L1++;}
      if ( abs(d_l1-d_l2)<eps && d_l3>TH_L3 )
          {stare2=L2;}
      else stare2=L3;
      break;
    }
    case L2: {
                while( abs(d_l1-d_l2)<eps && d_l3>TH_L3) {bck(70);d_l3=Ultra();}
                stare2=L3;
                stopm();
                straight();
                break;
  }
  case L3: {
           if(!flag_L3){stopm();flag_L3++;}
          while(d_l3>=TH_L3 +1) {fwd(50);d_l3=Ultra();}
          stopm();
          stare1=S6; 
          break;}
  
  default:break;
}}
void St1()
{
    switch(stare1)
    {
      
      case S0:{
          if(d_l1<TH_SPOT && d_l2<TH_SPOT)
       {       stare1=S1;
                car_det++;      
      }
             else if(d_l1<TH_SPOT && d_l2 >= TH_SPOT)
                      stare1=S2;
                      else if(d_l1>=TH_SPOT && d_l2< TH_SPOT)
                             { stare1=S3;
                             car_det++;
                             }
        
         break;
      }
      case S1:{
        fwd(200);
        if( (d_l1>=TH_SPOT) && (d_l2>=TH_SPOT)&&car_det<2)
            {stare1=S4;}
        if(car_det==2) 
                { stopm();stare1=S5;}
        break;
        }
      case S2:{
        fwd(200);
        if( (d_l1>=TH_SPOT) && (d_l2>=TH_SPOT) )
            stare1=S4; 
        else if( (d_l1<TH_SPOT) && (d_l2<TH_SPOT))
              {stare1=S1;car_det++;}
        break;
        }
      case S3:{
        fwd(200);
        if( (d_l1>=TH_SPOT) && (d_l2>=TH_SPOT))
            {stare1=S4;}
         break;
        }
      case S4:{
        fwd(200);
        if(d_l1<TH_SPOT && d_l2>=TH_SPOT)
            stare1=S2;
        break;
        }
      case S5:{
       St2();
       
        break;}
      case S6: break;
      default: break;
    }
   }
    
void setup()
{
  Serial.begin(9600);
 pinMode(RED_LED_PIN,OUTPUT);
 pinMode(GREEN_LED_PIN,OUTPUT);
 pinMode(MOTOR1_PIN1,OUTPUT);
 pinMode(MOTOR1_PIN2,OUTPUT);
 pinMode(MOTOR2_PIN1,OUTPUT);
 pinMode(MOTOR2_PIN2,OUTPUT);
 pinMode(L1_PIN,INPUT);
 pinMode(L2_PIN,INPUT);
 pinMode(L3_PIN,INPUT);
}

 
 
void loop()
{

/*int a = DIS_INFRA (analogRead(L1_PIN));
digitalWrite(RED_LED_PIN,1);
 digitalWrite(MOTOR1_PIN1,0); //high value - in spate
 digitalWrite(MOTOR1_PIN2,100); //high value - in fata
 digitalWrite(MOTOR2_PIN1,0); //   in dreapta
 digitalWrite(MOTOR2_PIN2,250); // in stanga
 delay(1500);
 digitalWrite(MOTOR1_PIN2,0);
 digitalWrite(MOTOR2_PIN1,0);
 digitalWrite(MOTOR2_PIN2,0);
 digitalWrite(RED_LED_PIN,0);
 delay(1000);
 digitalWrite(GREEN_LED_PIN,1);
 digitalWrite(MOTOR1_PIN1,100); //high value - in spate
 digitalWrite(MOTOR1_PIN2,0); //high value - in fata
 digitalWrite(GREEN_LED_PIN,0);

 
  */
      /*digitalWrite(RED_LED_PIN,1);
      d_l1=DIS_INFRA(analogRead(L1_PIN));
      d_l2=DIS_INFRA(analogRead(L2_PIN));
      d_l3=Ultra();
      St();
      if(stare==S1)
          {
            car_det++;
            fwd(80);
            delay(1000);
            if(car_det==2) parkingAlg();
            }
          else;
      St();*/
      d_l1=DIS_INFRA(analogRead(L1_PIN));
      d_l2=DIS_INFRA(analogRead(L2_PIN));
      d_l3=Ultra();
      
      
      Serial.print("Senzorul 1: ");
      Serial.print(d_l1);
      Serial.println(" ");
      Serial.print("Senzorul 2: ");
      Serial.print(d_l2);
      Serial.println(" ");
       Serial.print("Senzorul 3: ");
      Serial.print(d_l3);
      Serial.println(" ");
     
      
      St1();
      led();
       Serial.print("Cardet este: ");
      Serial.print(car_det);
      Serial.println(" ");
      Serial.print("Starea este: ");
      Serial.println(stare1);
      
}

