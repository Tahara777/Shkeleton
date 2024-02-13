
#include <M5Core2.h>
#include "MFRC522_I2C.h"
#include <Adafruit_NeoPixel.h>
#define PIN 26 
#define NUMPIXELS 21 //LEDの数を指定
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); //800kHzでNeoPixelを駆動

/**RFID**/
MFRC522_I2C mfrc522(0x28, -1); // Create MFRC522 instance.
int nocard = 0;
void uid_display_proc();

/**LED**/
#define Fingertip2WristMode = 1
#define Wrist2FingertipMode = 2
#define ChargeMode = 3
void Fingertip2Wrist(int cntP, int cntL);

int mode = 1;
int cntL = 0;//光演出のカウント
int cntP = 0;//LEDの場所のカウント

//LED番号再割り当て
int thumbLine[5] = {2,1,0};
int indexFingerLine[5] = {7,6,5,4,3};
int middleFingerLine[5] = {8,9,10,11,12};
int ringFingerLine[5] = {17,16,15,14,13};
int littleFingerLine[5] = {18,19,20};

bool isCard = false;



// ---------------------------------------------------------------
void setup() {
    pixels.begin(); //NeoPixelの初期化
    pixels.clear(); // NeoPixelのリセット

	M5.begin();
	//M5.lcd.setTextSize(2);
	//M5.Lcd.println("MFRC522 Test Jun/8/2022");
	Wire.begin();

	mfrc522.PCD_Init();
	//M5.Lcd.println("Please put the card\n\nUID:");
}

// ---------------------------------------------------------------
void loop() {
	M5.Lcd.setCursor(50,47);
	if (!mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial()) {
	nocard++;
	if (2 < nocard)
		{
		M5.Lcd.println(" No Card       ");
		Serial.println("*** No Card ***");
		}

	}
else
	{
	nocard = 0;
	uid_display_proc();
	isCard =! isCard;
	}


if(isCard){
	switch(mode){
      case 1:
        Fingertip2Wrist(cntP, cntL);
      break;
  
      default:
        break;
    }
    delay(100); //100ms待機

    cntL +=1;
    cntP +=1;
    if(cntP >= 5){
      cntP = 0;
    }
    if(cntL >= 250){
      cntL = 0;
    }  
}
else{
	pixels.clear(); // NeoPixelのリセット
    cntP = 0;
	cntL = 0;
}


    delay(200);
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

    delay(100);
}
// ---------------------------------------------------------------