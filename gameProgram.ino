
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)
#define numberTarget 5
#define sikiMin 50
//サーボ位置設定
int target_1 = 0;
int target_2 = 2;
int target_3 = 4;
int target_4 = 6;
int target_5 = 8;


int servo_down = 163;
int servo_up = 80;

int switch1 = A0;
int switch2 = A1;
int switch3 = A2;
int switch4 = A3;
int switch5 = A4;
int sikiSw = A5;

int checkLed = 13;
int startSw = 12;
int loopLed = 11;

//関数宣言
void servo_write(int,int);
//void actionMotor(void);
void upTarget(int);
void downTarget(int);
int switchState(void);
int checkHit(void);
void countUp(void);


int startFlag = 1;
int seed = 0;
int countSwitch = 0;
int interval_switch = 400;
int stateTarget[numberTarget];

int limitTime = 1800;
static long int score = 0;
static int vScoreP = 54;
static int vScoreM = -154;
int siki;

int choosen;
int hitNumber;
int target_list[numberTarget] = {target_1,target_2,target_3,target_4,target_5};
int switch_list[numberTarget] = {switch1,switch2,switch3,switch4,switch5};

volatile int shotCt = 0,sw = 0;
volatile unsigned long time_before = 0,time_now;
unsigned long time_chat = 30;
   
int i,j,k;

void setup() {
  //コンソール用意
  Serial.begin(9600);
  Serial.println("running...");
 // attachInterrupt(5,countUp,CHANGE);
  int i;
  pinMode(switch_list[0],INPUT);
  pinMode(switch_list[1],INPUT);
  pinMode(switch_list[2],INPUT);
  pinMode(switch_list[3],INPUT);
  pinMode(switch_list[4],INPUT);
  pinMode(sikiSw,INPUT);
  pinMode(startSw,INPUT);
  pinMode(checkLed,OUTPUT);
  /*pinMode(loopLed,OUTPUT);
  pinMode(10,INPUT);*/
  Serial.print(1);
  pwm.begin();
  pwm.setPWMFreq(60);
  //初期化
  Serial.print(2);
  for (i = 0;i > numberTarget;i++)
  {
    stateTarget[i] = 0;
    //beforeSt[i] = 0;
   
  }
  //セットアップ確認用点滅
  servo_write(target_1,servo_down);
  servo_write(target_2,servo_down);
  servo_write(target_3,servo_down);
  servo_write(target_4,servo_down);
  servo_write(target_5,servo_down);
  Serial.print(3);
  delay(1000);
  digitalWrite(checkLed,HIGH);
  servo_write(target_1,servo_up);
  servo_write(target_2,servo_up);
  servo_write(target_3,servo_up);
  servo_write(target_4,servo_up);
  servo_write(target_5,servo_up);
  Serial.print(4);
  delay(1000);
  digitalWrite(checkLed,LOW);
  servo_write(target_1,servo_down);
  servo_write(target_2,servo_down);
  servo_write(target_3,servo_down);
  servo_write(target_4,servo_down);
  servo_write(target_5,servo_down);
  Serial.print(5);
  delay(1000);
  digitalWrite(checkLed,HIGH);
  //待機
  while(startFlag)
  {
    delay(500);
    for(i = 0;i<numberTarget;i++)
    {
      //if(analogRead(switch_list[i]) < siki)
      {
        Serial.print(i);
        Serial.print(" ");
        Serial.println(analogRead(switch_list[i]));
      }
    }
    Serial.println(siki);
    siki = sikiMin+analogRead(sikiSw);
    if(digitalRead(startSw) == 0)
    {
      startFlag = 0;
      digitalWrite(checkLed,LOW);
      
      Serial.println("start");
      randomSeed(seed);
      
    }
    seed++;
    if(seed>500)seed = 0;
  }
}

void loop()
{
  choosen = switchState();
  countSwitch++;
  //Serial.println(choosen);
  hitNumber = checkHit();
  if(choosen == -2)
  {
    downTarget(random(numberTarget));
    downTarget(random(numberTarget));
    downTarget(random(numberTarget));
  }
  else upTarget(choosen);
  downTarget(hitNumber);
  //actionMotor();
  delay(6);
  limitTime-=1;
  if(limitTime<800)interval_switch=350;
  if(limitTime<500)interval_switch=300;
  if(limitTime<300)interval_switch=250;
  if(limitTime<150)interval_switch=200;
  Serial.println(limitTime);
  while(limitTime < 1)
  {
  
    Serial.println("game over!");//score);
    servo_write(target_1,servo_down);
    servo_write(target_2,servo_down);
    servo_write(target_3,servo_down);
    servo_write(target_4,servo_down);
    servo_write(target_5,servo_down);
  }
}

int switchState(void )
{
  //カウントが規定の値に達したとき、ランダムでどれかを選択。
  //カウントが少ない場合、-1を返す。
  int i;
  int choice;
  
  //Serial.println("カウント！");
  //Serial.println(countSwitch);
  if(countSwitch > interval_switch)
  {
    digitalWrite(loopLed,HIGH);
    Serial.println("100！！");
    choice = random(numberTarget);
    i = 0;
    if((stateTarget[1]==1)&&(stateTarget[2]==1)&&(stateTarget[3]==1)&&(stateTarget[4]==1)&&(stateTarget[0]==1))
    {
      Serial.println(-2);
      countSwitch = 0;
      
      score-=vScoreM;
      return -2;
    }
    while(stateTarget[choice] == 1)
    {
      choice = random(numberTarget);
      Serial.println("loop");
    }
    stateTarget[choice] = 1;
    digitalWrite(loopLed,LOW);
    //Serial.println("choice");
    //Serial.println(choice);
    countSwitch = 0;  
  }else choice = -1;
  return choice;
}

int checkHit(void)
{
  int i,hit;
  hit = -1;
  for (i = 0;i<numberTarget;i++)
  {
    // Serial.println(i);
    if(digitalRead(switch_list[i]) < siki)
    {
      if(stateTarget[i]==1)score += vScoreM;
      stateTarget[i] = 0;
      hit = i;
      
      
      //Serial.println(i);
    }
  }  
  return hit;
}

void upTarget(int choosen_up)
{
  if(choosen_up != -1)
  {
    servo_write(target_list[choosen_up],servo_up);
  }
}

void downTarget(int hitNumber_down)
{
  stateTarget[hitNumber_down] = 0;
  if(hitNumber_down != -1)
  {
    servo_write(target_list[hitNumber_down],servo_down);
  }
}
/*
void actionMotor(void)
{
  int i;
  
  for (i = 0;i<numberTarget;i++)
  {
    if(beforeSt[i] != stateTarget[i])
    {
      if(stateTarget[i] == 1)servo_write(target_list[i],170);
      if(stateTarget[i] == 0)servo_write(target_list[i],90);
    }
  }
  
}*/
/*void countUp(void)
{
  
  time_now = millis();
  if(time_now-time_before > time_chat)
  {
    if(sw==0)
    {
      Serial.println("shot");
      shotCt++;
      Serial.println(shotCt);
    }
    sw = !sw;
  }  
  time_before = time_now;
}*/

void servo_write(int n,int ang)
{
  ang = map(ang,0,180,SERVOMIN,SERVOMAX);
  pwm.setPWM(n,0,ang);
}
