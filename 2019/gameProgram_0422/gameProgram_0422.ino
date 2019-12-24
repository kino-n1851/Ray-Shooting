//#include <Adafruit_PWMServoDriver.h>

#include <Wire.h>

#include<DFPlayer_Mini_Mp3.h>
#define numberTarget 5
#define sikiMin 150
#define countLimit 5000

int checkLed = 13;

//的関係
typedef struct type_Target
{
  int state_Target;
  int timeBefore;
  int sensorNumber;
  int ledNumber;
  int type_target;
};

void down_target(int);
void up_target(int);
void setTime_target(int);  
int hit_target(int); 
int getScore(int);
//-------------------------
//メイン関数
int countTime(void);
int checkHit(int state[]);
void operateTarget(int state[],int);
void startMotion(void);

void waitStart(void);
void setParam(void);

static type_Target target[numberTarget]={{0,0,A0,12,1},{0,0,A1,11,1},{0,0,A2,10,1},{0,0,A3,9,1},{0,0,A4,8,1}};
static int sensorState[numberTarget] = {0,0,0,0,0};
int waitFlag = 1;
int seed = 0;
int sikiSw = A5;

static unsigned long int timeCt = 0;
static unsigned int beforeTime = 0;
static long int score = 0;
static int siki = 400;
static int gameState = 1;
int comboCT=0;

int interval_switch = 70;
int i,j;
int choice;
int mainLoopFL = 1;

void setup() {
  //シリアル＆ピン初期化
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(100);
  Serial.println("running...");
  Serial.println("serialInitialize has done...");
  mp3_set_serial(Serial1);
  mp3_set_volume(20);
  for(i=0;i < numberTarget;i++)pinMode(target[i].sensorNumber,INPUT);
  for(i=0;i < numberTarget;i++)pinMode(target[i].ledNumber,OUTPUT);
  pinMode(checkLed,OUTPUT);
  pinMode(sikiSw,INPUT);
  delay(200);
  //動作確認(雰囲気)
  Serial.println("targetSetting has done...");
  digitalWrite(checkLed,HIGH);
  delay(500);
  Serial.println("checking connection...");
  digitalWrite(checkLed,LOW);
  delay(500);
  Serial.println("checking setup...");
  digitalWrite(checkLed,HIGH);
  delay(500);
  Serial.println("setup has finished completely! 2");
  mp3_play(1);

}

void loop() {
  setParam();
  waitStart();
  startMotion();
  while(mainLoopFL)
  {
    checkHit(sensorState);
    operateTarget(sensorState,countTime());
    //Serial.println(timeCt);
    delay(10);
    if(gameState == 0)mainLoopFL = 0;
  }
  while(1){delay(1000);}
  Serial.println("restart...");
}



//----------------------------------------
void setParam()//リスタート時のパラメータセット
{
  int loopS = 1;
  char inputC;
  Serial.println("difficulty(a b c):");
  while(loopS)
  {
    inputC=Serial.read();
    if(inputC=='a')
    {
      waitFlag = 1;
      timeCt = 0;
      beforeTime = 0;
      score = 0;
      gameState = 1;
      interval_switch = 150;
      loopS = 0;
      mainLoopFL = 1;
    }
    else if(inputC=='b')
    {
      waitFlag = 1;
      timeCt = 0;
      beforeTime = 0;
      score = 0;
      gameState = 1;
      interval_switch = 150;
      loopS = 0;
       mainLoopFL = 1;
    }
    else if(inputC=='c')
    {
      waitFlag = 1;
      timeCt = 0;
      beforeTime = 0;
      score = 0;
      gameState = 1;
      interval_switch = 150;
      loopS = 0;
       mainLoopFL = 1;
    }

    delay(100);
  }

  Serial.println("OK  1");

}
//-----------------------------------------

void waitStart(void)//ゲーム中:0終了後:1
{
  while(waitFlag)
  {
    delay(500);
    for(i = 0;i<numberTarget;i++)
    {
        //状態確認用ダイアログ
      {
        Serial.print(i);
        Serial.print(" ");
        Serial.println(analogRead(target[i].sensorNumber));
        
      }
    }
    Serial.println("press s to start");
    siki = sikiMin+analogRead(sikiSw);
    Serial.println(siki);
    
    if(Serial.read() == 's')
    {
      waitFlag = 0;
      digitalWrite(checkLed,LOW);
      
      Serial.println("start");
      mp3_play(11);
    }

  }
}
//--------------------------------------
void startMotion(void)
{
  int loopS = 1;
  for(j=0;j<5;j++)
  {
    for(i=0;i<numberTarget;i++)up_target(i);
    delay(300);
    for(i=0;i<numberTarget;i++)down_target(i);
    delay(300);
  }
  up_target(2);
  while(loopS)
  {
    if(analogRead(target[2].sensorNumber) < siki)
    {
      loopS = 0;
      down_target(2);
      mp3_play(10);
    }
    seed++;//50パターンの乱数を生成
    if(seed>50)seed = 0;
    delay(10);
  }
  randomSeed(seed);
}
//----------------------------------------

int countTime(void)//ただのカウントアップ
{//150
  timeCt++;
  if(timeCt==1500)interval_switch=120;
  if(timeCt==2000)
  {
    interval_switch=100;
  }
  if(timeCt==3000)interval_switch=200;
  if(timeCt==4000)
  {
    mp3_play(10);
    interval_switch=80;
  }
  
  if(timeCt>countLimit)
  {
    Serial.println("2 end");
    Serial.print("2 score:");
    Serial.println(score);
    for(i=0;i<numberTarget;i++)down_target(i);
    mp3_play(10);
    delay(3000);
    gameState = 0;
   }
  return timeCt;
}

//------------------------------------------
int checkHit(int sensorState[])//配列に現在の状況を記録 命中:1 
{
  for(i=0;i<numberTarget;i++)
  {
    sensorState[i]= (analogRead(target[i].sensorNumber) < siki);
    //Serial.println(state[i]);
  }
  return 0;
}
//-----------------------------------------
int getScore(int t)
{

  if((comboCT>=3)&&(t<100))
  {
    comboCT+=i;
    score+=100;
    mp3_play(4);
    Serial.print("combo ");
  }
  else if(t<50)
  {
    score+=90;
    comboCT+=1;
    mp3_play(3);
  }else if(t<100)
  {
    score+=70;
    comboCT+=1;
    mp3_play(3);
  }else if(t<180)
  {
    score+=50;
    comboCT=0;
    mp3_play(2);
  }else if(t<250)
  {
    score+=20;
    comboCT=0;
    mp3_play(2);
  }
  else {
    mp3_play(6);
    score-=20;
    //interval_switch+=5;
    comboCT=0;
  }
 if(comboCT>10)
 {
    comboCT=0;
    score+=180;
    mp3_play(11);
    //interval_switch-=10;
 }
}

//-------------------------------------------
void operateTarget(int sensorState[],int timeCt)//センサ値から的の動作を決定
{
  //各的ごとの処理
  for(i=0;i<numberTarget;i++)
  {
    if(target[i].state_Target == 1)
    {
      if(sensorState[i] == 1)
      {
        Serial.print("Hit　");//要追記
        Serial.println(i);
        target[i].state_Target=0;
        hit_target(i);
      }
       if((timeCt-target[i].timeBefore)==400)
       {
        down_target(i);
        mp3_play(6);
        score-=20;
       }
    }
  }
  //一回のみ処理
  if((timeCt-beforeTime)>interval_switch)
  {
    choice = random(numberTarget);
    if((target[0].state_Target==1)&&(target[1].state_Target==1)&&(target[2].state_Target==1)&&(target[3].state_Target==1)&&(target[4].state_Target==1))
    {
      //miss
      Serial.println("miss");//要追記
      mp3_play(6);
      
      for(i = 0;i<numberTarget;i++)
      {
        target[i].state_Target=0;
        down_target(i);
      } 
    }
    while(target[choice].state_Target == 1)
    {
      choice = random(numberTarget);
      Serial.println("loop");
    }
    target[choice].state_Target = 1;//要追記
    up_target(choice);
    beforeTime = timeCt;
    setTime_target(choice);
  }
}

//-------------------------------------------
void down_target(int number)
{
  digitalWrite(target[number].ledNumber,LOW);
}

void up_target(int number)
{
  digitalWrite(target[number].ledNumber,HIGH);
}

void setTime_target(int number)
{
  target[number].timeBefore = timeCt;
}

int hit_target(int number)
{
  down_target(number);
  Serial.print("time:");
  Serial.println(timeCt);
  //Serial.println(target[number].timeBefore);
  getScore((timeCt-target[number].timeBefore));
  down_target(number);
}
