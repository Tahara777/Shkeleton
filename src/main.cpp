#include <M5Core2.h>
#include "MFRC522_I2C.h"
#include <Adafruit_NeoPixel.h>

#include <SD.h>
#include <M5GFX.h>// M5GFXライブラリをインクルード
M5GFX display;
M5Canvas Sprite(&display);
M5Canvas BaseSprite(&Sprite);
M5Canvas MaskSprite(&Sprite);

/**RFID**/
MFRC522_I2C mfrc522(0x28, -1); // Create MFRC522 instance.


/**LED**/
#define PIN 26 
#define NUMPIXELS 21 //LEDの数を指定
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); //800kHzでNeoPixelを駆動

int ledBrightness = 0; // LEDの明るさカウンタ
int ledPosition = 0; // LEDの位置カウンタ


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
float TotalTime = 5000;//演出合計時間ms
unsigned long startMillis = 0;
unsigned long currentMillis = 0;
unsigned long previousLEDTime= 0;
unsigned long previousLCDTime= 0;

void Fingertip2Wrist(int ledPosition, int ledBrightness);
bool isNewCard();
int identifyCard();
void LEDcontrol(int B, unsigned long C, unsigned long D);
void LCDcontrol(int B, unsigned long C, unsigned long D);
void uid_display_proc();
void MaskReveal_Sphere();
void MaskReveal_Square();
void ResetLCD();
void AllLEDon();

// ---------------------------------------------------------------
void setup() {
  pixels.begin(); //NeoPixelの初期化
  pixels.clear(); // NeoPixelのリセット

  Serial.begin(19200); // シリアル通信の開始

	M5.begin();

  display.begin();
  display.fillScreen(TFT_BLACK);
  mfrc522.PCD_Init(); // MFRC522モジュールの初期化

	Wire.begin();

  // canvasのサイズをディスプレイサイズに設定
  Sprite.createSprite(display.width(), display.height());
  BaseSprite.createSprite(display.width(), display.height());
  MaskSprite.createSprite(display.width(), display.height());

  // SDカードからディレクトリ直下の画像を読み込んでsprite1に描画
  if (!BaseSprite.drawJpgFile(SD, "/D_K.jpg")) {
    Serial.println("画像の読み込みに失敗しました");
    return;
  }
  MaskSprite.fillSprite(TFT_BLACK);
  BaseSprite.pushSprite(0,0);
  MaskSprite.pushSprite(0,0);

  // canvasをディスプレイに表示
  Sprite.pushSprite(0, 0);

}

// ---------------------------------------------------------------
void loop() {
  currentMillis = millis(); // 現在のミリ秒を取得
    
  if(!isCard){
    pixels.clear(); // NeoPixelのリセット

    if(isNewCard()){
      CardID = identifyCard();
      isCard = true;
      startMillis = currentMillis; 
    }
  }
  else{
    LEDcontrol(CardID, startMillis, currentMillis);
    LCDcontrol(CardID, startMillis, currentMillis);
    if(isNewCard()){//リセット動作が行われた場合
      isCard = false; // isCardをbooleanで更新
      ResetLCD();
    }
  }
}

// ---------------------------------------------------------------
bool isNewCard(){//カード有無を判定
	if (!mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial()) {//カード読み取り関数
  return false;
	}
else{
	//uid_display_proc();
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
    ledBrightness =100;
    ledPosition =0;
  }
  if((CurrentTime - StartTime) < TotalTime){
	switch(ID){
      case 1:
      if((CurrentTime - previousLEDTime) > 100){//100ms間隔で更新
        Fingertip2Wrist(ledPosition, ledBrightness);
        previousLEDTime = CurrentTime;
        //M5.Lcd.println(previousLEDTime);

          ledBrightness +=10;
          ledPosition +=1;
        if(ledPosition >= 5){
          ledPosition = 0;
        }
        if(ledBrightness >= 250){
          ledBrightness = 250;
        }
      }
        break;
  
      default:
        break;
    }
  } 
  else{
    AllLEDon();
  }
}
// ---------------------------------------------------------------
void LCDcontrol(int ID, unsigned long StartTime, unsigned long CurrentTime){
  if(StartTime == CurrentTime){
    //ledBrightness =100;
    //ledPosition =0;
  }
  if((CurrentTime - StartTime) < TotalTime){
	switch(ID){
      case 1:
      if((CurrentTime - previousLCDTime) > 10){//100ms間隔で更新
          MaskReveal_Sphere();
        previousLCDTime = CurrentTime;
      }
        break;
  
      default:
        break;
    }  
  }
  else{
    MaskSprite.fillRect(0, 0, display.width(), display.height(), TFT_WHITE);
    BaseSprite.pushSprite(0,0);
    MaskSprite.pushSprite(0,0,TFT_WHITE);
    Sprite.pushSprite(0, 0);    
  }
}
// ---------------------------------------------------------------
void MaskReveal_Sphere() {
  int boxWidth = 30;  // 箱の幅
  int boxHeight = 30;  // 箱の高さ

    int x = random(display.width() - boxWidth);
    int y = random(display.height() - boxHeight);

    MaskSprite.fillCircle( x, y, 30,TFT_WHITE);

    BaseSprite.pushSprite(0,0);
    MaskSprite.pushSprite(0,0,TFT_WHITE);
    Sprite.pushSprite(0, 0);

}
// ---------------------------------------------------------------
void MaskReveal_Square() {
  int boxWidth = 30;  // 箱の幅
  int boxHeight = 30;  // 箱の高さ

    int x = random(display.width() - boxWidth);
    int y = random(display.height() - boxHeight);
    
    MaskSprite.fillRect(x, y, boxWidth, boxHeight, TFT_WHITE);
    
    BaseSprite.pushSprite(0,0);
    MaskSprite.pushSprite(0,0,TFT_WHITE);
    Sprite.pushSprite(0, 0);
}
// ---------------------------------------------------------------
void Fingertip2Wrist(int ledPosition_a, int ledBrightness_a){
  pixels.clear(); // NeoPixelのリセット
  if(ledPosition_a < 2){
    pixels.setPixelColor(indexFingerLine[ledPosition_a], pixels.Color(ledBrightness_a/2, ledBrightness_a/3, ledBrightness_a)); // i番目の色を設定
    pixels.setPixelColor(middleFingerLine[ledPosition_a], pixels.Color(ledBrightness_a/2, ledBrightness_a/3, ledBrightness_a)); // i番目の色を設定
    pixels.setPixelColor(ringFingerLine[ledPosition_a], pixels.Color(ledBrightness_a/2, ledBrightness_a/3, ledBrightness_a)); // i番目の色を設定
  }
  else{
    pixels.setPixelColor(thumbLine[ledPosition_a - 2], pixels.Color(ledBrightness_a/2, ledBrightness_a/3, ledBrightness_a)); // i番目の色を設定
    pixels.setPixelColor(indexFingerLine[ledPosition_a], pixels.Color(ledBrightness_a/2, ledBrightness_a/3, ledBrightness_a)); // i番目の色を設定
    pixels.setPixelColor(middleFingerLine[ledPosition_a], pixels.Color(ledBrightness_a/2, ledBrightness_a/3, ledBrightness_a)); // i番目の色を設定
    pixels.setPixelColor(ringFingerLine[ledPosition_a], pixels.Color(ledBrightness_a/2, ledBrightness_a/3, ledBrightness_a)); // i番目の色を設定
    pixels.setPixelColor(littleFingerLine[ledPosition_a - 2], pixels.Color(ledBrightness_a/2, ledBrightness_a/3, ledBrightness_a)); // i番目の色を設定
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
void ResetLCD(){
  MaskSprite.fillSprite(TFT_BLACK);
  BaseSprite.pushSprite(0,0);
  MaskSprite.pushSprite(0,0);

  // canvasをディスプレイに表示
  Sprite.pushSprite(0, 0);
}
// ---------------------------------------------------------------

void AllLEDon(){
  
  // 各指のLED配列に対してループを実行
  /*for(int i=0; i<5; i++){
    // 2未満のiに対しては、親指と小指を除外
    if(i < 2){
      pixels.setPixelColor(indexFingerLine[i], pixels.Color(i*20, i*30, i*40)); // i番目の色を設定
      pixels.setPixelColor(middleFingerLine[i], pixels.Color(i*20, i*30, i*40)); // i番目の色を設定
      pixels.setPixelColor(ringFingerLine[i], pixels.Color(i*20, i*30, i*40)); // i番目の色を設定
    } else {
      // 親指と小指のLEDも点灯
      pixels.setPixelColor(thumbLine[i-2], pixels.Color(i*20, i*30, i*40)); // i番目の色を設定    
      pixels.setPixelColor(indexFingerLine[i], pixels.Color(i*20, i*30, i*40)); // i番目の色を設定
      pixels.setPixelColor(middleFingerLine[i], pixels.Color(i*20, i*30, i*40)); // i番目の色を設定
      pixels.setPixelColor(ringFingerLine[i], pixels.Color(i*20, i*30, i*40)); // i番目の色を設定
      pixels.setPixelColor(littleFingerLine[i-2], pixels.Color(i*20, i*30, i*40)); // i番目の色を設定
    }
  }
  */

  for(int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255)); // 全てのLEDを白色で点灯
  }

  pixels.setBrightness(10); // 0~255の範囲で明るさを設定
  pixels.show(); // LEDに色を反映
}

