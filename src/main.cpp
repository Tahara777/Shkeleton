#include <M5Core2.h>
#include "MFRC522_I2C.h"
#include <Adafruit_NeoPixel.h>

/**RFID**/
MFRC522_I2C mfrc522(0x28, -1); // Create MFRC522 instance.


/**LED**/
#define PIN 26 
#define NUMPIXELS 21 //LEDの数を指定
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); //800kHzでNeoPixelを駆動
#define Fingertip2WristMode = 1
#define Wrist2FingertipMode = 2
#define ChargeMode = 3

int mode = 1;
int cntL = 0;//光演出のカウント
int cntP = 0;//LEDの場所のカウント

//LED番号再割り当て
int thumbLine[5] = {2,1,0};
int indexFingerLine[5] = {7,6,5,4,3};
int middleFingerLine[5] = {8,9,10,11,12};
int ringFingerLine[5] = {17,16,15,14,13};
int littleFingerLine[5] = {18,19,20};

//演出制御
bool isCard = false;//カード読み取りフラグ
#define ace "155 43 9 12"
int CardID = 0;
float TotalTime = 5;//演出合計時間
unsigned long startMills = 0;
unsigned long currentMills = 0;
unsigned long previousLEDTime= 0;


void Fingertip2Wrist(int cntP, int cntL);
bool isNewCard();
int identifyCard();
void LEDcontrol(int B, unsigned long C, unsigned long D);
void uid_display_proc();

// ---------------------------------------------------------------
void setup() {
  pixels.begin(); //NeoPixelの初期化
  pixels.clear(); // NeoPixelのリセット

  Serial.begin(19200); // シリアル通信の開始

	M5.begin();
	Wire.begin();
}

// ---------------------------------------------------------------
void loop() {
  currentMills = millis(); // 現在のミリ秒を取得
  //Serial.println(CardID);
  
  if(isNewCard()){
    CardID = identifyCard();
    isCard = true;
    //M5.Lcd.println(2);
    startMills = currentMills; // 正しくグローバル変数を更新
  }

  if(isCard){
    LEDcontrol(CardID, startMills, currentMills);
    if((currentMills - startMills) > 5000){
      isCard = false; // isCardをbooleanで更新
    }
  }
  else{
    pixels.clear(); // NeoPixelのリセット

  }
}

// ---------------------------------------------------------------
bool isNewCard(){//カード有無を判定
	if (!mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial()) {//カード読み取り関数
  return false;
	}
else{
	uid_display_proc();
  return true;
	}
}
// ---------------------------------------------------------------
int identifyCard(){
  String strBuf[mfrc522.uid.size];
  for (byte i = 0; i < mfrc522.uid.size; i++){
    strBuf[i] = String(mfrc522.uid.uidByte[i], HEX);

    if (strBuf[i].length() == 1){
      strBuf[i] = "0" + strBuf[i];
    }
  }
  String strUID = strBuf[0] + " " + strBuf[1] + " " + strBuf[2] + " " + strBuf[3];
  if (strUID.equalsIgnoreCase(ace)){
    //spriteImage.drawJpgFile(SD, "/trump/card_spade_01.jpg", 0, 0);
    // spriteImage.pushSprite(&lcd, x, y);
    return 1;
  }

}
// ---------------------------------------------------------------
void LEDcontrol(int ID, unsigned long StartTime, unsigned long CurrentTime){
  if(StartTime == CurrentTime){
    cntL =100;
    cntP =0;
  }
	switch(ID){
      case 1:
      if((CurrentTime - previousLEDTime) > 100){
        Fingertip2Wrist(cntP, cntL);
        previousLEDTime = CurrentTime;
        //M5.Lcd.println(previousLEDTime);

          cntL +=10;
          cntP +=1;
        if(cntP >= 5){
          cntP = 0;
        }
        if(cntL >= 250){
          cntL = 250;
        }
        break;
  
      default:
        break;
    }

    }  
}
// ---------------------------------------------------------------
void Fingertip2Wrist(int cntP_a, int cntL_a){
  pixels.clear(); // NeoPixelのリセット
  if(cntP_a < 2){
    pixels.setPixelColor(indexFingerLine[cntP_a], pixels.Color(cntL_a/2, cntL_a/3, cntL_a)); // i番目の色を設定
    pixels.setPixelColor(middleFingerLine[cntP_a], pixels.Color(cntL_a/2, cntL_a/3, cntL_a)); // i番目の色を設定
    pixels.setPixelColor(ringFingerLine[cntP_a], pixels.Color(cntL_a/2, cntL_a/3, cntL_a)); // i番目の色を設定
  }
  else{
    pixels.setPixelColor(thumbLine[cntP_a - 2], pixels.Color(cntL_a/2, cntL_a/3, cntL_a)); // i番目の色を設定
    pixels.setPixelColor(indexFingerLine[cntP_a], pixels.Color(cntL_a/2, cntL_a/3, cntL_a)); // i番目の色を設定
    pixels.setPixelColor(middleFingerLine[cntP_a], pixels.Color(cntL_a/2, cntL_a/3, cntL_a)); // i番目の色を設定
    pixels.setPixelColor(ringFingerLine[cntP_a], pixels.Color(cntL_a/2, cntL_a/3, cntL_a)); // i番目の色を設定
    pixels.setPixelColor(littleFingerLine[cntP_a - 2], pixels.Color(cntL_a/2, cntL_a/3, cntL_a)); // i番目の色を設定
  }
    pixels.setBrightness(60);// 0~255の範囲で明るさを設定 
    pixels.show();   //LEDに色を反映

    //delay(100);
}
// ---------------------------------------------------------------
void uid_display_proc()
{
	short aa[] = {1,0,0,0,0,0,0,0,0,0};
	char buf[60];
	sprintf(buf,"mfrc522.uid.size = %d",mfrc522.uid.size);
	Serial.println(buf);
	for (short it = 0; it < mfrc522.uid.size; it++) {
		aa[it] = mfrc522.uid.uidByte[it];
		}

	sprintf(buf, "%02x %02x %02x %02x",aa[0],aa[1],aa[2],aa[3]);
	M5.Lcd.println(buf);
	M5.Lcd.println("");
	Serial.println(buf);
}
// ---------------------------------------------------------------