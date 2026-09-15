//#include <Arduino_CAN.h>
#include <Adafruit_NeoPixel.h>
//CAN通信
#define CAN_RX 5
#define CAN_TX 4

//7セグメントLED
#define DIGI0 A3
#define DIGI1 A2
#define DIGI2 A1
#define CS 10
#define SER 11
#define SCK 13

//電圧測定用
#define VSENS A0

//ステータスLED
#define RGB 8

//ブザー
#define BZ A4
#define BZ_Frequency 500

//DIPスイッチ
#define ID0 0
#define ID1 1

//電源分配出力
#define SW1 3
#define SW2 2
#define SW3 6
#define SW4 7

int control_period= 1000; // 制御周期

//各基準電圧
const float V_ref = 4.67;  // ADCの基準電圧 (USBで電源供給時は5V, LiPo経由だと4.7Vくらい)
const float V_max = 10;
const float V_min = 1;

//ブザー用フラグ
bool buzzer_on;
uint32_t buzzer_start = 0;
bool low_voltage_alarm = false;

const int melody[] = {2000, 1300};
const int duration[] = {120, 180};

Adafruit_NeoPixel pixels(1, RGB, NEO_GRB + NEO_KHZ800);

uint8_t digit_pattern[16] = {
  B11101110,  // 0
  B10000010,  // 1
  B11011100,  // 2
  B11010110,  // 3
  B10110010,  // 4
  B01110110,  // 5
  B01111110,  // 6
  B11000010,  // 7
  B11111110,  // 8
  B11110110,  // 9
};

uint8_t emoji_pattern[][3] = {
  // 真顔
  {
    B11110000,
    B00000100,
    B11110000,
  },
  // お目々閉じ
  {
    B00010000,
    B00000100,
    B00010000,
  },
  // ニコッ
  {
    B11110000,
    B00001110,
    B11110000,
  },
};

void setup() {
  Serial.begin(115200);  //シリアル初期化
  analogReadResolution(10); //ADCの分解能を10bitに設定(最大14bit)

  //LEDピン設定
  pinMode(DIGI0, OUTPUT);
  pinMode(DIGI1, OUTPUT);
  pinMode(DIGI2, OUTPUT);
  pinMode(BZ, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(SER, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(VSENS, INPUT);

  //出力ピン設定
  pinMode(SW1, OUTPUT);
  pinMode(SW2, OUTPUT);
  pinMode(SW3, OUTPUT);
  pinMode(SW4, OUTPUT);
  pinMode(RGB, OUTPUT);

  digitalWrite(CS, HIGH);


  // 顔を表示
  /*display_pattern_for_duration(emoji_pattern[0], 1000);
  display_pattern_for_duration(emoji_pattern[1], 100);
  display_pattern_for_duration(emoji_pattern[0], 200);
  display_pattern_for_duration(emoji_pattern[1], 100);
  display_pattern_for_duration(emoji_pattern[0], 2000);
  display_pattern_for_duration(emoji_pattern[2], 1500);*/
  
  //ステータスLEDを点灯(通常時は青)
  pixels.begin();
  pixels.setBrightness(64);  //輝度(0～255)
  pixels.setPixelColor(0, pixels.Color(0, 255, 255));  //LEDの色
  pixels.show();

  StartSound();

  //電源分配開始
  digitalWrite(SW1, HIGH);
  digitalWrite(SW2, HIGH);
  digitalWrite(SW3, HIGH);
  digitalWrite(SW4, HIGH);

}


void loop() {
  static float voltage = 0;

  static long unsigned int rx_id;
  static unsigned char len;
  static unsigned char rx_buf[8];

  static uint32_t last_voltage_check = millis();
  if (millis() - last_voltage_check > control_period) {
    last_voltage_check = millis();
    voltage = get_voltage();
    Serial.println(voltage);  //デバッグ用

    // 電圧が1-10V未満ならブザーを鳴らす
   if (voltage < V_max && voltage > V_min) {
      if (!low_voltage_alarm) {
      //tone(BZ, BZ_Frequency);
      buzzer_start = millis();
      buzzer_on = true;
      low_voltage_alarm = true;

      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
      pixels.show();
    }
      if (buzzer_on && millis() - buzzer_start >= 3000) {
        noTone(BZ);
        buzzer_on = false;
    }
  }
      else {
      noTone(BZ);
      low_voltage_alarm = false;
      pixels.setPixelColor(0, pixels.Color(0, 255, 255));
      pixels.show();
    }
  }

  //電圧の表示
  uint32_t v = (uint32_t)(voltage * 10);  // 例: 12.3V → 123
  uint8_t tens = (v / 100) % 10;          // 十の位
  uint8_t ones = (v / 10) % 10;           // 一の位
  uint8_t deci = v % 10;                  // 小数第一位
  const uint8_t voltage_digits[3] = {
    digit_pattern[tens],
    digit_pattern[ones] | B00000001,  //小数点表示
    digit_pattern[deci],
  };
  display_pattern(voltage_digits);

}

//電圧フィードバック
float get_voltage() {
  float voltage = analogRead(VSENS) * (V_ref / 1023.0) * ((10.0 + 2.2) / 2.2); 
  return voltage;
}

//7セグの表示
void display_pattern(const uint8_t pattern[3]) {
  digitalWrite(DIGI0, LOW);
  digitalWrite(DIGI1, LOW);
  digitalWrite(DIGI2, LOW);
  digitalWrite(CS, LOW);
  shiftOut(SER, SCK, LSBFIRST, pattern[0]);
  digitalWrite(CS, HIGH);
  digitalWrite(DIGI0, HIGH);
  digitalWrite(DIGI1, LOW);
  digitalWrite(DIGI2, LOW);
  delay(1);
  digitalWrite(DIGI0, LOW);
  digitalWrite(DIGI1, LOW);
  digitalWrite(DIGI2, LOW);
  digitalWrite(CS, LOW);
  shiftOut(SER, SCK, LSBFIRST, pattern[1]);
  digitalWrite(CS, HIGH);
  digitalWrite(DIGI0, LOW);
  digitalWrite(DIGI1, HIGH);
  digitalWrite(DIGI2, LOW);
  delay(1);
  digitalWrite(DIGI0, LOW);
  digitalWrite(DIGI1, LOW);
  digitalWrite(DIGI2, LOW);
  digitalWrite(CS, LOW);
  shiftOut(SER, SCK, LSBFIRST, pattern[2]);
  digitalWrite(CS, HIGH);
  digitalWrite(DIGI0, LOW);
  digitalWrite(DIGI1, LOW);
  digitalWrite(DIGI2, HIGH);
  delay(1);
}

//7セグの表示(時間指定)
void display_pattern_for_duration(const uint8_t pattern[3], const uint32_t duration) {
  uint32_t start = millis();
  while (true) {
    display_pattern(pattern);
    if (millis() - start > duration) {
      break;
    }
  }
}

void StartSound() {
  for (int i = 0; i < 2; i++) {
    tone(BZ, melody[i]);
    delay(duration[i]);
  }
  noTone(BZ);
}