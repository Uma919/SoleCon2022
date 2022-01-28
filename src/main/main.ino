#include <M5Stack.h>
#include <SimpleBeep.h>
#include <VL53L0X.h>

VL53L0X vl;

const int SlePin = 5;
const int CdSPin = 35;

int CdSVal = 0;
int CdSStt = 0;
int SwCStt = 1;
double HgtVal = 0.0;
int HgtFlg = 0;
int BepFlg = 0;

  /*    　　　　　   */
 /*  LCD表示関数群  */
/*      　　　　　 */
/* タイトル表示関数 */
void printTitle() {
  M5.Lcd.setTextSize(5);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.drawString("TunaCanCan", 16, 16);
  M5.Lcd.setTextColor(LIGHTGREY);
  M5.Lcd.drawString("TunaCanCan", 13, 13);
  M5.Lcd.setTextColor(DARKGREY);
  M5.Lcd.drawString("TunaCanCan", 10, 10);    
}

/* ソレノイド状態表示関数 */
void printBrk(String brk) {
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.drawString("Brake  " + brk, 10, 70);
}

/* CdSセンサ状態表示関数 */
void printCdS(int cds) {
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(GREEN);
  if (cds <= 10) {
    if (CdSStt != 1) {
      M5.Lcd.clear(BLACK);
    }
    M5.Lcd.drawString("Fall   Detected", 10, 100);
    CdSStt = 1;
  } else {
    if (CdSStt != 0) {
      M5.Lcd.clear(BLACK);
    }
    M5.Lcd.drawString("Fall   N.D.", 10, 100);
    CdSStt = 0;
  }
}

/* メッセージ表示関数 */
void printMsg() {
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.drawString("Are you ready?", 15, 150);
  M5.Lcd.drawString("Press the center button.", 15, 170);
  M5.Lcd.drawString("Let's start the game!!", 15, 190);
}

/* カウントダウン表示関数 */
void printCnt(int cnt) {
  M5.Lcd.setTextSize(5);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.drawString(String(cnt), 150, 170);
}

/* 結果表示関数 */
void printRst(int rst) {
  M5.Lcd.setTextSize(4);
  M5.Lcd.setTextColor(RED);
  if (rst == 1) {
    M5.Lcd.drawString("You Win!!", 50, 150);
  } else {
    M5.Lcd.drawString("You Lose...", 30, 170);
  }
}

/* 高さ表示関数 */
double printHgt(int flg, double val) {
  if (flg == 0) {
    val = 0.0;
    for (int n = 0; n < 10; n++) {
      double v = vl.readRangeSingleMillimeters();
      v /= 10.0;
      val += v;
    }
    val /= 10.0;
    val -= 1.5;
  }
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(RED);
  m5.Lcd.setCursor(10,190);
  if (0.0 <= val && val <= 40.0) {
    M5.Lcd.drawString("Height   " + String(val) + " cm", 10, 190);
  } else {
    M5.Lcd.drawString("Height   N.D.", 10, 190);
  }
  return val;
}

  /* 　　　　　   */
 /*  メイン関数  */
/*   　　　　　 */
void setup() {
  M5.begin();
  M5.Lcd.setBrightness(200);
  Wire.begin();
  sb.init();
  vl.init();
  vl.setTimeout(500);
  pinMode(CdSPin, INPUT);
  pinMode(SlePin, OUTPUT);
  digitalWrite(SlePin, LOW);
}

void loop() {
  CdSVal = analogRead(CdSPin);
  printTitle();
  printCdS(CdSVal);
  
  switch (SwCStt) {
    /* 初期状態 */
    case 1:
      digitalWrite(SlePin, LOW);
      printBrk("OFF");
      printMsg();
      if (M5.BtnB.wasPressed()) {
        M5.Lcd.clear(BLACK);
        SwCStt ++;
      }
      break;
    /* カウントダウン状態 */
    case 2:
      digitalWrite(SlePin, HIGH);
      printBrk("ON");
      for (int t = 5; t > 0; t --) {
        M5.Lcd.clear(BLACK);
        printTitle();
        printBrk("ON");
        printCdS(CdSVal);
        printCnt(t);
        sb.beep(3,C4,100);
        delay(1000);
      }
      M5.Lcd.clear(BLACK);
      digitalWrite(SlePin, LOW);
      printTitle();
      printBrk("OFF");
      printCdS(CdSVal);
      printCnt(0);
      sb.beep(3,E4,300);
      SwCStt ++;
      break;
    /* 落下状態 */
    case 3:
      if (M5.BtnB.wasPressed() && CdSVal >= 10) {
        M5.Lcd.clear(BLACK);
        HgtFlg = 0;
        BepFlg = 0;
        SwCStt ++;
      } else if (CdSVal <= 10) {
        M5.Lcd.clear(BLACK);
        BepFlg = 0;
        SwCStt += 2;
      }
      break;
    /* 成功状態 */
    case 4:
      digitalWrite(SlePin, HIGH);
      printBrk("ON");
      printRst(1);
      HgtVal = printHgt(HgtFlg, HgtVal);
      HgtFlg = 1;
      if (BepFlg == 0) {
        for (int n = 0; n < 3; n++) {
          sb.beep(3,C4,100);
          delay(100);
          sb.beep(3,E4,200);
        }
      }
      BepFlg = 1;
      if (M5.BtnB.wasPressed()) {
        M5.Lcd.clear(BLACK);
        SwCStt = 1;
      }
      break;
    /* 失敗状態 */
    case 5:
      printBrk("OFF");
      printRst(0);
      if (BepFlg == 0) {
        for (int n = 0; n < 3; n++) {
          sb.beep(3,E4,100);
          delay(100);
        }
      }
      BepFlg = 1;
      if (M5.BtnB.wasPressed()) {
        M5.Lcd.clear(BLACK);
        SwCStt = 1;
      }
      break;
  }
  M5.update();
}
