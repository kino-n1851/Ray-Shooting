#include <DFPlayer_Mini_Mp3.h>
#define numberTarget 5
#define sikiMin 50

int switch1 = A0;
int switch2 = A1;
int switch3 = A2;
int switch4 = A3;
int switch5 = A4;
int sikiSw = A5;

int targetLed_1 = 12;
int targetLed_2 = 11;
int targetLed_3 = 10;
int targetLed_4 = 9;
int targetLed_5 = 8;

int checkLed = 13;
//int startSw = ;
//int loopLed = 11;

//関数宣言

int writeLed(int,int);
int switchState(void);
int checkHit(void);

int startFlag = 1;
int seed = 0;
int countSwitch = 0;
int interval_switch = 100;
int stateTarget[numberTarget];

static int limitTime = 6000;
static long int score = 0;
static int vScoreP = 54;

static int siki = 400;


int choosen;
int hitNumber;
int targetLed_list[numberTarget] = {targetLed_1,targetLed_2,targetLed_3,targetLed_4,targetLed_5};
int switch_list[numberTarget] = {switch1,switch2,switch3,switch4,switch5};

int shotCt = 0,sw = 0;
/*unsigned long time_before = 0,time_now;
unsigned long time_chat = 30;*/

int i,j,k;
void setup() 
{
 //コンソール用意
  Serial.begin(9600);
  Serial1.begin (9600);
  mp3_set_serial (Serial1);
  mp3_set_volume(15);
  Serial.println("running...");
  for(i=0;i < numberTarget;i++)pinMode(switch_list[i],INPUT);
  pinMode(sikiSw,INPUT);
  //pinMode(startSw,INPUT);
  //pinMode(checkLed,OUTPUT);
  
  Serial.print(1);
  delay(100); 
  //初期化
  Serial.print(2);
  for (i = 0;i < numberTarget;i++)
  {
    stateTarget[i] = 0;
    //beforeSt[i] = 0;
   
  }
  //セットアップ確認用点滅
  
  Serial.print(3);
  delay(1000);
  digitalWrite(checkLed,HIGH);
  /*servo_write(target_1,servo_up);
  servo_write(target_2,servo_up);
  servo_write(target_3,servo_up);
  servo_write(target_4,servo_up);
  servo_write(target_5,servo_up);*/
  Serial.print(4);
  delay(1000);
  digitalWrite(checkLed,LOW);
  /*servo_write(target_1,servo_down);
  servo_write(target_2,servo_down);
  servo_write(target_3,servo_down);
  servo_write(target_4,servo_down);
  servo_write(target_5,servo_down);*/
  Serial.print(5);
  delay(1000);
  digitalWrite(checkLed,HIGH);
  mp3_play(4);
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
    if(Serial.read() == 's')
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

void loop() {
  // put your main code here, to run repeatedly:
  choosen = switchState();
  countSwitch++;
  //Serial.println(choosen);
  hitNumber = checkHit();
  if(choosen == -2)
  {
    mp3_play(4);
    for(i=0;i<numberTarget;i++)writeLed(0,random(numberTarget));
  }
  else writeLed(1,choosen);
  writeLed(0,hitNumber);
  //actionMotor();
  delay(6);
  limitTime-=1;
  if(limitTime<800)interval_switch=150;
  if(limitTime<500)interval_switch=150;
  if(limitTime<300)interval_switch=100;
  if(limitTime<150)interval_switch=100;
  //Serial.println(limitTime);
  if(limitTime<1)
  {
    for(i=0;i<5;i++)
    {
      for(j=0;j<5;j++) 
      {
        writeLed(1,j);
      }
      delay(200);
      for(j=0;j<5;j++) 
      {
        writeLed(0,j);
      }
      delay(200);
    }
    Serial.println(score);
    Serial.println("game over!");
    while(limitTime < 1)
    {
      delay(100);
    }
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
    //digitalWrite(loopLed,HIGH);
    Serial.println("100！！");
    choice = random(numberTarget);
    i = 0;
    if((stateTarget[1]==1)&&(stateTarget[2]==1)&&(stateTarget[3]==1)&&(stateTarget[4]==1)&&(stateTarget[0]==1))
    {
      Serial.println(-2);
      countSwitch = 0;
      return -2;
    }
    while(stateTarget[choice] == 1)
    {
      choice = random(numberTarget);
      Serial.println("loop");
    }
    stateTarget[choice] = 1;
    //digitalWrite(loopLed,LOW);
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
    //Serial.println(siki);
    if(analogRead(switch_list[i]) < siki)
    {
      if(stateTarget[i]==1)
      {
        mp3_play(1);
        score += vScoreP;
        Serial.println("attack");
        stateTarget[i] = 0;
        hit = i;
      }
     
      //Serial.println("hit");
      //Serial.println(analogRead(switch_list[i]));
      //Serial.println(i);
    }
  }  
  return hit;
}

 int writeLed(int mode,int num)
 {
  if(mode==1)
  {
    digitalWrite(targetLed_list[num],HIGH);
    stateTarget[num]=1;
  }else if(mode==0)
  {
    digitalWrite(targetLed_list[num],LOW);
    stateTarget[num]=0; 
  }else return -1;
  return 0;
 }
