#include <Wire.h>
#include<DFPlayer_Mini_Mp3.h>
#define NUMBER 5

//プロトタイプ宣言
void checkHit();
void checkState();
void upTarget(int);
void downTarget(int);
void shuffle(int*);
void countTime(void);

typedef struct
{
    int targetState;
    int sensorState;
    unsigned long int timeBefore;
    int sensorNumber;
    int ledNumber;
}type_Target;


static int sensorThr = 100;
unsigned long int totalTime=0;
unsigned int hitTime=0;
static type_Target target[NUMBER]={{0,0,0,A0,12},{0,0,0,A1,11},{0,0,0,A2,10},{0,0,0,A3,9},{0,0,0,A4,8}};
int rnd[NUMBER];
int i,j;
int thrSw = A5;
int minThr= 250;
bool main1Flag=false,main2Flag=false;
bool targetOn=false;

static int loopTime1=0;
static int loop1=0;
static bool loopS = false;

/////////////////////////////////////////////////
void timeKeep();
static float  average = 0;
unsigned long int interval = 100;
unsigned long int main2Start;
unsigned long int time_before; 
static int choice = 0;
static unsigned long int score = 0;

/////////////////////////////////////////////////
void initState(void)
{
  sensorThr = 100;
  totalTime=0;
  hitTime=0;
  average = 0;
  interval = 0;
  //target[NUMBER]={{0,0,0,A0,12},{0,0,0,A1,11},{0,0,0,A2,10},{0,0,0,A3,9},{0,0,0,A4,8}};
  minThr= 250;
  score=0;
  main1Flag=true;
  main2Flag=false;
  targetOn=false;
  loop1 = 0;
  loopTime1=0;
}

void setup()
{
    for(i=0;i<NUMBER;i++)
    {
      pinMode(target[i].sensorNumber,INPUT);
      pinMode(target[i].ledNumber,OUTPUT);
    }
      //シリアル＆ピン初期化
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(100);
  Serial.println("running...");
  Serial.println("serialInitialize has done...");
  mp3_set_serial(Serial1);
  mp3_set_volume(30);
  pinMode(thrSw,INPUT);
  delay(200);
  //動作確認(雰囲気)
  Serial.println("targetSetting has done...");
  delay(500);
  Serial.println("checking connection...");
  delay(500);
  Serial.println("checking setup...");
  delay(500);
  Serial.println("setup has finished completely!");
  mp3_play(1);
}

void loop()
{//start
  waitStart();
  
  main1Flag = true;
  main2Flag = false;
  targetOn = false;
  loopS = true;
  loop1 = 0;
  loopTime1=0;
  startMotion();
  
  while(main1Flag)
  {
    
    if(loop1==4)
    {
      main1Flag=false;
      targetOn=true;
      main2Flag=true;
      for(i=0;i<5;i++)
      {
        target[i].sensorState=0;
        target[i].targetState=0;
        downTarget[i];
      }
      average = ((totalTime/1000.0)/hitTime);
      Serial.print("average :");
      Serial.print(average);
      Serial.println("sec");
      break;
    }
    if(!targetOn)
    {
      if(loopTime1==0)
      {
        delay(1000);
        Serial.println("make");
        for(i=0;i<NUMBER;i++)rnd[i]=i;
        shuffle(rnd);
        mp3_play(11);
        delay(2000);
        upTarget(rnd[loopTime1]);
        loopTime1++;
        loop1++;
        targetOn=true;
      }else
      {
        delay(1500-(loopTime1*120));
        Serial.println("up");
        upTarget(rnd[loopTime1]);
        loopTime1++;
        targetOn=true;
      }
    }
    if(loopTime1==5)loopTime1=0;  
    checkState();
    checkHit();
    delay(10);
  }

  interval = average*1200;
  targetOn=false;
  for (i=0;i<NUMBER;i++)
  {
    downTarget(i);
  }
  startMotion();

  
  mp3_play(11);
  delay(1000);
  digitalWrite(target[2].ledNumber,HIGH);
  loopS = true;
  while(loopS)
  {
    if(analogRead(target[2].sensorNumber) < sensorThr)
    {
      loopS = false;
      digitalWrite(target[2].ledNumber,LOW);
      mp3_play(10);
      main2Start = millis();
    }
  }
  loopS = true;
  while(main2Flag)
  {
    timeKeep();
    checkState();
    checkHit();
    delay(10);
  }
  for(i=0;i<5;i++)downTarget(i);
  mp3_play(10);
  Serial.print("SCORE: ");
  Serial.println(score);
  Serial.print("AVERAGE: ");
  Serial.println(average);
  Serial.println("RestartIng...");
  while(Serial.read() != 's')
  {
    delay(1000);
    initState();
  }
 
}

void checkState()//配列に現在の状況を記録 命中:1 
{
  for(i=0;i<NUMBER;i++)
  {
    target[i].sensorState = (analogRead(target[i].sensorNumber) < sensorThr);
    //Serial.print(target[i].sensorState);
  }
  //Serial.println();
}

void checkHit()
{
    int num = 0;
    for(i=0;i<NUMBER;i++)
    {
        if((target[i].sensorState==1)&&(target[i].targetState==1))
        {//Hit
          Serial.println("Hit!");
          hitTarget(i);
          target[i].sensorState=0;
        }
        //Serial.print(target[i].targetState);
    }
    //Serial.println("target");
}

void hitTarget(int num)
{
  if(main1Flag)
  {
    downTarget(num);
    mp3_play(2);
    totalTime+=(millis()-target[num].timeBefore);
    Serial.println((millis()-target[num].timeBefore)/1000.0);
    hitTime+=1;
  }
  else if(main2Flag)
  {
    if((millis()-target[num].timeBefore)<800)
    {
      score+=250;
    }else if((millis()-target[num].timeBefore)<1200)
    {
      score+=180;
    }else if((millis()-target[num].timeBefore)<2000)
    {
      score+=150;
    }else if((millis()-target[num].timeBefore)<5000)
    {
      score+=80;
    }else
    {
      score+=40;
    }

    downTarget(num);
    mp3_play(2);
  }
}


void downTarget(int num)
{
  Serial.print("down");
  Serial.println(num);
  targetOn=false;
  digitalWrite(target[num].ledNumber,LOW);
  target[num].targetState=0;
}

void upTarget(int num)
{
  Serial.print("up");
  Serial.println(num);
  digitalWrite(target[num].ledNumber,HIGH);
  target[num].targetState=1;
  target[num].timeBefore = millis();
}


void shuffle(int rnd[])
{
  int i,j,x,tmp,number;
  number = 5;sizeof(rnd);
  for(int a=0;a<5;a++)Serial.print(rnd[a]);
  Serial.println();
  for(i=number;i > 0;i--)
  {
    x=(millis()%i);
    tmp = rnd[x];
    for(j=x;j<(number-1);j++)
    {
      rnd[j]=rnd[j+1];
    }
    rnd[number-1] = tmp;
  }
  for(int a=0;a<5;a++)Serial.print(rnd[a]);
  Serial.println();
}

void startMotion(void)
{
  loopS = true;
  int seed=0;
  for(j=0;j<5;j++)
  {
    for(i=0;i<NUMBER;i++)upTarget(i);
    delay(300);
    for(i=0;i<NUMBER;i++)downTarget(i);
    delay(300);
  }
  digitalWrite(target[2].ledNumber,HIGH);
  while(loopS)
  {
    if(analogRead(target[2].sensorNumber) < sensorThr)
    {
      loopS = false;
      digitalWrite(target[2].ledNumber,LOW);
      mp3_play(10);
    }
    seed++;//50パターンの乱数を生成
    if(seed>50)seed = 0;
    delay(10);
  }
  loopS = true;
  randomSeed(seed);
}

void waitStart(void)//ゲーム中:0終了後:1
{
  int waitFlag = 1;
  while(waitFlag)
  {
    delay(500);
    for(i = 0;i<NUMBER;i++)
    {
        //状態確認用ダイアログ
      {
        Serial.print(i);
        Serial.print(" ");
        Serial.println(analogRead(target[i].sensorNumber));
        
      }
    }
    Serial.println("press s to start");
    sensorThr = minThr+analogRead(thrSw);
    Serial.println(sensorThr);
    
    if(Serial.read() == 's')
    {
      waitFlag = 0;
      
      Serial.println("start");
      mp3_play(11);
    }

  }
}
///////////////////////////////////////////////////////////////////
void timeKeep(void)
{
  for(i=0;i<5;i++)
  {
    if((target[i].targetState==1)&&(millis()-target[i].timeBefore)>6000)
    {
      downTarget(i);
      score-=100;
      Serial.println("houti");
    }
  }
 
  if((millis()-time_before)>interval)
  {
    choice = random(NUMBER);
    if((target[0].targetState==1)&&(target[1].targetState==1)&&(target[2].targetState==1)&&(target[3].targetState==1)&&(target[4].targetState==1))
    {
      //miss
      Serial.println("miss");//要追記
      mp3_play(6);
      score-=50;
      for(i = 0;i<NUMBER;i++)
      {
        target[i].targetState=0;
        downTarget(i);
      } 
    }
    while(target[choice].targetState == 1)
    {
      choice = random(NUMBER);
      Serial.println("loop");
    }
    time_before=millis(); 
    target[choice].targetState = 1;
    upTarget(choice);
    target[choice].timeBefore=millis();
  }
  if((millis()-main2Start)==20000)
  {
    interval*=0.8f;
    mp3_play(10);
  }
  if((millis()-main2Start)>30000)main2Flag=false;
}
