#include <M5Stack.h>
#include <SimpleBeep.h>

const int SlePin = 5;
const int CdSPin = 35;

int CdSVal;
int SwcState = 1;
int cnt = 0;

  /*    　　　　　   */
 /*  LCD表示関数群  */
/*      　　　　　 */
/* タイトル表示関数 */
void printTitle() {
  M5.Lcd.setTextSize(5);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.drawString("Tuna Can Can", 41, 16);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.drawString("Tuna Can", 38, 13);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.drawString("Tuna Can", 35, 10);    
}

/* ソレノイド状態表示関数 */
void printSleState(String state) {
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.drawString("Brake Sys.  " + state, 10, 70);
}

/* ToFセンサ距離表示関数 */
void printDist(int dist) {
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(GREEN);
  m5.Lcd.setCursor(10,100);
  m5.Lcd.printf("Height   %4d\n", dist);
  /*
  if (3 <= dist && dist <= 30 ) {
    M5.Lcd.drawString("Height      " + String(dist) + " cm", 10, 100);
  } else if (dist < 3 || 40 < dist) {
    M5.Lcd.drawString("Height      N.D.", 10, 100);
  }
  */
}

/* カウントダウン表示関数 */
void printCount(int cnt) {
  M5.Lcd.setTextSize(5);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.drawString(String(cnt), 150, 170);
}

/* メッセージ表示関数 */
void printMsg() {
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.drawString("Are you ready?", 15, 150);
  M5.Lcd.drawString("Press the center button.", 15, 170);
  M5.Lcd.drawString("Let's start the game!!", 15, 190);
}

/* 結果表示関数 */
void printRst(int n) {
  M5.Lcd.setTextSize(4);
  M5.Lcd.setTextColor(RED);
  if (n == 1) {
    M5.Lcd.drawString("You Win!!", 50, 170);
  } else {
    M5.Lcd.drawString("You Lose...", 30, 170);
  }
}

  /* 　　　　　   */
 /*  メイン関数  */
/*   　　　　　 */
void setup() {
  M5.begin();
  //M5.Power.begin();
  M5.Lcd.setBrightness(200);
  sb.init();
  pinMode(SlePin, OUTPUT);
  digitalWrite(SlePin, LOW);
  pinMode(CdSPin, INPUT);
}

void loop() {
  printTitle();
  
  CdSVal = analogRead(CdSPin);
  switch (SwcState) {
    /* 初期状態 */
    case 1:
      digitalWrite(SlePin, LOW);
      printSleState("OFF");
      printDist(CdSVal);
      printMsg();
      if (M5.BtnB.wasPressed()) {
        M5.Lcd.clear(BLACK);
        digitalWrite(SlePin, HIGH);
        printSleState("ON");
        SwcState ++;
      }
      break;
    /* カウントダウン状態 */
    case 2:
      for (int t = 5; t > 0; t --) {
        M5.Lcd.clear(BLACK);
        printTitle();
        digitalWrite(SlePin, HIGH);
        printSleState("ON");
        printDist(CdSVal);
        printCount(t);
        sb.beep(3,C4,100);
        delay(1000);
      }
      M5.Lcd.clear(BLACK);
      digitalWrite(SlePin, LOW);
      printSleState("OFF");
      printDist(CdSVal);
      printCount(0);
      sb.beep(3,E4,300);
      SwcState ++;
      break;
    /* 落下状態 */
    case 3:
      if (M5.BtnB.wasPressed() && CdSVal >= 10) {
        M5.Lcd.clear(BLACK);
        digitalWrite(SlePin, HIGH);
        printSleState("ON");
        cnt = 0;
        SwcState ++;
      } else if (CdSVal < 10) {
        M5.Lcd.clear(BLACK);
        printSleState("OFF");
        cnt = 0;
        SwcState += 2;
      }
      break;
    /* 成功状態 */
    case 4:
      digitalWrite(SlePin, HIGH);
      printSleState("ON");
      printDist(CdSVal);
      printRst(1);
      if (cnt == 0) {
        for (int n = 0; n < 3; n++) {
          sb.beep(3,C4,100);
          delay(100);
          sb.beep(3,E4,200);
        }
      }
      cnt ++;
      if (M5.BtnB.wasPressed()) {
        M5.Lcd.clear(BLACK);
        SwcState = 1;
      }
      break;
    /* 失敗状態 */
    case 5:
      printSleState("OFF");
      printDist(0);
      printRst(0);
      if (cnt == 0) {
        for (int n = 0; n < 3; n++) {
          sb.beep(3,E4,100);
          delay(100);
        }
      }
      cnt ++;
      if (M5.BtnB.wasPressed()) {
        M5.Lcd.clear(BLACK);
        SwcState = 1;
      }
      break;
  }
  M5.update();
}
