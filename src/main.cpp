#include <M5Core2.h>
#include "MFRC522_I2C.h" // MFRC522 RFIDリーダー用のI2Cライブラリをインクルード
#include <SD.h>
#include <M5GFX.h>// M5GFXライブラリをインクルード

M5GFX display;
M5Canvas Sprite(&display);
M5Canvas BaseSprite(&Sprite);
M5Canvas MaskSprite(&Sprite);

MFRC522 mfrc522(0x28); // MFRC522のインスタンスを作成


int nocard = 10;

void setup()
{
  M5.begin();
  display.begin();
  display.fillScreen(TFT_BLACK);

  Wire.begin(); // I2C通信の初期化
  mfrc522.PCD_Init(); // MFRC522モジュールの初期化

  // SDカードの初期化
  if (!SD.begin()) {
    Serial.println("SDカードの初期化に失敗しました");
    return;
  }


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


void loop() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        nocard ++;
    }
    else{
        nocard = 0;
    }  
    if(nocard < 3){
        MaskReveal();
    } else {

        Serial.println("*** No Card ***");
        MaskSprite.fillSprite(TFT_BLACK);
        BaseSprite.pushSprite(0,0);
        MaskSprite.pushSprite(0,0);
        Sprite.pushSprite(0, 0);
    }
    //delay(20); // 200ミリ秒待機
}

void MaskReveal() {
  int boxWidth = 20;  // 箱の幅
  int boxHeight = 20;  // 箱の高さ

  for(int i = 0; i < 100; i++){
    // ランダムな位置を計算
    int x = random(display.width() - boxWidth);
    int y = random(display.height() - boxHeight);
    MaskSprite.fillRect(x, y, boxWidth, boxHeight, TFT_WHITE);
    BaseSprite.pushSprite(0,0);
    MaskSprite.pushSprite(0,0,TFT_WHITE);
    Sprite.pushSprite(0, 0);
  }
    MaskSprite.fillRect(0, 0, display.width(), display.height(), TFT_WHITE);
    BaseSprite.pushSprite(0,0);
    MaskSprite.pushSprite(0,0,TFT_WHITE);
    Sprite.pushSprite(0, 0);
    delay(1000); // 小さな遅延を挟む（必要に応じて調整）
}