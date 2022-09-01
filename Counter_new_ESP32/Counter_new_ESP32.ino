#include <SPI.h> // библиотека SPI
#include <EEPROM.h>
#include "Adafruit_GFX.h" // библиотека графики
#include "Adafruit_ST7796S_kbv.h" // библиотека TFT екрана 
#include "GyverEncoder.h"
#include "GyverButton.h"

#include "Fonts/FreeSans18pt7b.h"

// подключение библиотека TFT екрана по шине SPI
#define TFT_CS 15 // цифровой выход D15
#define TFT_DC 2 // цифровой выход D2
#define TFT_RST 5 // цифровой выход D5

#define BATTERY 4

#define EEPROM_SIZE 64

// подключаем енкодер
const int encoder_a = 27; // Green - pin 2 - Digital
const int encoder_b = 26; // White - pin 3 - Digital
long encoder, prev_encoder, coils_turns, encoder_degrees, count = 0;

int page = 0;

struct {
  int diameter;
  int width_;
  float wire;
  bool rotary;
} my_data;

#define BTN_PIN 13
#define BTN2_PIN 14
#define BTN3_PIN 12

GButton butt1(BTN_PIN, HIGH_PULL, NORM_OPEN);
GButton butt2(BTN2_PIN, HIGH_PULL, NORM_OPEN);
GButton butt3(BTN3_PIN, HIGH_PULL, NORM_OPEN);

int width_symbolsymbol = 56;
int height_symbolsymbol = 76;

bool flag = false;
bool flag2 = false;
bool flag3 = false;
bool flag4 = false;
bool flag5 = false;
bool flag6 = false;
int flag_page = false;
bool flag_clear = false;
bool flag_page2 = false;
bool flag_lens = false;
bool flag_angle = false;

int pointer = 0;

double lens_count;
int angle_count;

int step_battary;


Adafruit_ST7796S_kbv tft = Adafruit_ST7796S_kbv(TFT_CS, TFT_DC, TFT_RST); // создание/инициализация обьекта TFT екрана ST7796S по шине SPI

void printCount(long count);
long counter_turns();
void encoderPinChangeA();
void encoderPinChangeB();
void printSetings();
void lens();
void angle();
void battery_charge();
void save();
void mode_set();
void speed_test();

const unsigned char Battery[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xfc, 0x00, 0x7f, 0xff, 0xfe, 0x00,
  0xc0, 0x00, 0x06, 0x00, 0xc0, 0x00, 0x06, 0x00, 0xc0, 0x00, 0x06, 0x00, 0xc0, 0x00, 0x07, 0x80,
  0xc0, 0x00, 0x07, 0x80, 0xc0, 0x00, 0x07, 0x80, 0xc0, 0x00, 0x07, 0x80, 0xc0, 0x00, 0x06, 0x00,
  0xc0, 0x00, 0x06, 0x00, 0xc0, 0x00, 0x06, 0x00, 0x7f, 0xff, 0xfe, 0x00, 0x7f, 0xff, 0xfc, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

const unsigned char Battery_Full[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x7b, 0xc0, 0x00, 0x0f, 0x7b, 0xc0, 0x00,
  0x0f, 0x7b, 0xc0, 0x00, 0x0f, 0x7b, 0xc0, 0x00, 0x0f, 0x7b, 0xc0, 0x00, 0x0f, 0x7b, 0xc0, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

const unsigned char Battery_Middle[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x78, 0x00, 0x00, 0x0f, 0x78, 0x00, 0x00,
  0x0f, 0x78, 0x00, 0x00, 0x0f, 0x78, 0x00, 0x00, 0x0f, 0x78, 0x00, 0x00, 0x0f, 0x78, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

const unsigned char Battery_Low[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,
  0x0f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

const unsigned char Battery_Extremely[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
  0x0c, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

const unsigned char L_R [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x0f, 0x00, 0x40,
  0x08, 0x80, 0x40, 0x08, 0x40, 0x40, 0x08, 0x40, 0x40, 0x08, 0x80, 0x41, 0xcf, 0x00, 0x40, 0x08,
  0x80, 0x40, 0x08, 0x40, 0x40, 0x08, 0x40, 0x7e, 0x08, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char R_L [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x08, 0x00, 0x44,
  0x08, 0x00, 0x42, 0x08, 0x00, 0x42, 0x08, 0x00, 0x44, 0x08, 0x00, 0x78, 0xe8, 0x00, 0x44, 0x08,
  0x00, 0x42, 0x08, 0x00, 0x42, 0x08, 0x00, 0x42, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char Save [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0x00,
  0x00, 0x01, 0xe0, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x0c, 0xe0, 0x00, 0x00, 0x00, 0x01,
  0xe0, 0x0d, 0xe0, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x0d, 0xe0, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x0c,
  0xe0, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x00,
  0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
  0x01, 0xc0, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x01, 0xc0,
  0x00, 0xe0, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x01, 0xcf, 0xfc, 0xe0,
  0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x01, 0xcf, 0xfc, 0xe0, 0x00, 0x00,
  0x00, 0x01, 0xc0, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x01,
  0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char Normal [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
  0x80, 0x00, 0x30, 0x80, 0x00, 0x30, 0x80, 0x00, 0x28, 0x80, 0x00, 0x24, 0x80, 0x00, 0x24, 0x80,
  0x00, 0x22, 0x80, 0x00, 0x22, 0x80, 0x00, 0x21, 0x80, 0x00, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char Formula [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e,
  0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x00, 0x00, 0x3e, 0x00,
  0x00, 0x20, 0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char Speed [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xf9, 0xe6, 0x30, 0x85, 0x16, 0x30, 0xf9, 0xe5, 0x50, 0x89, 0x04, 
  0x90, 0x85, 0x04, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() {
  Serial.begin(115200); // запуск сирийного порта

  EEPROM.begin(EEPROM_SIZE);
  coils_turns = EEPROM.read(0);
  my_data.diameter = EEPROM.read(8);
  my_data.width_ = EEPROM.read(16);
  EEPROM.get(24, my_data.wire);  
  my_data.rotary = EEPROM.read(32);

  tft.begin(); // запуск работы TFT екрана
  tft.setFont(&FreeSans18pt7b); // шрифт 12/18/24
  tft.fillScreen(0x0000); // закрашивание экрана в черный цыет
  tft.setRotation(1); // функуия поворота экрана

  pinMode(encoder_a, INPUT_PULLUP);
  pinMode(encoder_b, INPUT_PULLUP);

  attachInterrupt(encoder_a, encoderPinChangeA, CHANGE);
  attachInterrupt(encoder_b, encoderPinChangeB, CHANGE);

  butt1.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt1.setTimeout(1000);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt1.setClickTimeout(120);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt1.setStepTimeout(50);    // настройка таймаута междутаймаута между прокрутками кнопки

  butt2.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt2.setTimeout(200);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt2.setClickTimeout(70);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt2.setStepTimeout(50);    // настройка таймаута междутаймаута между прокрутками кнопки

  butt3.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt3.setTimeout(200);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt3.setClickTimeout(70);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt3.setStepTimeout(50);    // настройка таймаута междутаймаута между прокрутками кнопки
}

void loop()
{

  //tft.drawLine(240, 0, 240, 320, 0xFFFF);
  //tft.drawLine(0, 160, 480, 160, 0xFFFF);
  butt1.tick();
  butt2.tick();
  butt3.tick();

  if (butt1.isSingle()) {
    flag_page++;
    if (flag_page == 5) flag_page = 0;
  }

  // обнуление счетчіка
  if (butt1.isHolded()) {
    encoder = 0;
    coils_turns = 0;
    EEPROM.put(0, coils_turns);
    EEPROM.commit();
  }

  // отрисовка страниц
  if (flag_page == 0) {
    // очистка екрана
    if (flag_clear == false) {
      tft.fillRect(0, 0,  480, 320, 0x0000);
      flag_clear = true;
    }
    // главная страница

    printCount(counter_turns());
    lens(coils_turns);
    angle();
    battery_charge();
    save();
    mode_set();
    speed_test();
  }

  else if (flag_page >= 1) {
    // очистка екрана
    if (flag_clear) {
      tft.fillRect(0, 0,  480, 320, 0x0000);
      flag_clear = false;
    }

    // страница настроек
    printSetings(flag_page);


    flag = false;
  }
}

// счетчик
long counter_turns() {
  if (encoder > 360 * 4)
  {
    coils_turns++;
    encoder = 0;
  }

  if (encoder < -360 * 4)
  {
    coils_turns--;
    encoder = 0;
  }
  return coils_turns = constrain(coils_turns, 0, 10000000);
}

// отрисовка счетчика
void printCount(long count) {
  static long prev_count;
  if (flag == false) {

    if (my_data.rotary == false) tft.drawBitmap(25, 120, L_R, 20, 20, 0xFF20);
    else tft.drawBitmap(25, 120, R_L, 20, 20, 0xFF20);

    tft.setTextColor(0x0778); // цвет шрифта 0x0778 , 0xFF20
    tft.setTextSize(3); // размер шрифта

    if (count >= 0 && count < 10) {
      if (flag2) {
        tft.fillRect(212 - (width_symbolsymbol / 2), 123,  width_symbolsymbol * 2, height_symbolsymbol, 0x0000);
        flag2 = false;
      }
      tft.fillRect(212, 123,  width_symbolsymbol, height_symbolsymbol, 0x0000);
      tft.setCursor(212, 196);
    }

    else if (count >= 10 && count < 100) {
      if (flag3) {
        tft.fillRect(184 - (width_symbolsymbol / 2), 123,  width_symbolsymbol * 3, height_symbolsymbol, 0x0000);
        flag3 = false;
      }
      if (count % 10 == 0 || count % 10 == 9) tft.fillRect(184, 123,  width_symbolsymbol * 2, height_symbolsymbol, 0x0000);
      else
        tft.fillRect(240, 123,  width_symbolsymbol, height_symbolsymbol, 0x0000);
      tft.setCursor(212 - (width_symbolsymbol / 2), 196);
      flag2 = true;
    }

    else if (count >= 100 && count < 1000) {
      if (flag4) {
        tft.fillRect(156 - (width_symbolsymbol / 2), 123,  width_symbolsymbol * 4, height_symbolsymbol, 0x0000);
        flag4 = false;
      }
      if (count % 10 == 0 || count % 10 == 9) tft.fillRect(212, 123,  width_symbolsymbol * 2, height_symbolsymbol, 0x0000);
      if (count % 100 == 0 || count % 10 == 9) tft.fillRect(156, 123,  width_symbolsymbol * 3, height_symbolsymbol, 0x0000);
      else
        tft.fillRect(268, 123,  width_symbolsymbol, height_symbolsymbol, 0x0000);
      tft.setCursor(212 - (width_symbolsymbol / 2 * 2), 196);
      flag3 = true;
    }

    else if (count >= 1000 && count < 10000) {
      if (flag5) {
        tft.fillRect(128 - (width_symbolsymbol / 2), 123,  width_symbolsymbol * 5, height_symbolsymbol, 0x0000);
        flag5 = false;
      }
      if (count % 10 == 0 || count % 10 == 9) tft.fillRect(240, 123,  width_symbolsymbol * 2, height_symbolsymbol, 0x0000);
      if (count % 100 == 0 || count % 10 == 9) tft.fillRect(184, 123,  width_symbolsymbol * 3, height_symbolsymbol, 0x0000);
      if (count % 1000 == 0 || count % 10 == 9) tft.fillRect(128, 123,  width_symbolsymbol * 4, height_symbolsymbol, 0x0000);
      else
        tft.fillRect(297, 123,  width_symbolsymbol, height_symbolsymbol, 0x0000);
      tft.setCursor(212 - (width_symbolsymbol / 2 * 3), 196);
      flag4 = true;
    }

    else if (count >= 10000 && count < 100000) {
      if (count % 10 == 0 || count % 10 == 9) tft.fillRect(268, 123,  width_symbolsymbol * 2, height_symbolsymbol, 0x0000);
      if (count % 100 == 0 || count % 10 == 9) tft.fillRect(211, 123,  width_symbolsymbol * 3, height_symbolsymbol, 0x0000);
      if (count % 1000 == 0 || count % 10 == 9) tft.fillRect(156, 123,  width_symbolsymbol * 4, height_symbolsymbol, 0x0000);
      if (count % 10000 == 0 || count % 10 == 9) tft.fillRect(100, 123,  width_symbolsymbol * 5, height_symbolsymbol, 0x0000);
      else
        tft.fillRect(326, 123,  width_symbolsymbol, height_symbolsymbol, 0x0000);
      tft.setCursor(212 - (width_symbolsymbol / 2 * 4), 196);
      flag5 = true;
    }

    else if (count >= 100000 && count < 1000000) {
      if (flag6) {
        tft.fillRect(100 - (width_symbolsymbol / 2), 123,  width_symbolsymbol * 6, height_symbolsymbol, 0x0000);
        flag6 = false;
      }
      if (count % 10 == 0 || count % 10 == 9) tft.fillRect(296, 123,  width_symbolsymbol * 2, height_symbolsymbol, 0x0000);
      if (count % 100 == 0 || count % 10 == 9) tft.fillRect(239, 123,  width_symbolsymbol * 3, height_symbolsymbol, 0x0000);
      if (count % 1000 == 0 || count % 10 == 9) tft.fillRect(184, 123,  width_symbolsymbol * 4, height_symbolsymbol, 0x0000);
      if (count % 10000 == 0 || count % 10 == 9) tft.fillRect(128, 123,  width_symbolsymbol * 5, height_symbolsymbol, 0x0000);
      if (count % 100000 == 0 || count % 10 == 9) tft.fillRect(72, 123,  width_symbolsymbol * 6, height_symbolsymbol, 0x0000);
      else
        tft.fillRect(355, 123,  width_symbolsymbol, height_symbolsymbol, 0x000F);
      tft.setCursor(212 - (width_symbolsymbol / 2 * 5), 196);
    }

    else if (count >= 1000000) {
      tft.fillRect(10, 123,  455, height_symbolsymbol, 0x0000);
      tft.setCursor(212 - (width_symbolsymbol / 2 * 6), 196);
      flag6 = true;
    }
    tft.drawRoundRect(20, 118, 440, height_symbolsymbol + 10, 3, 0xFF20);

    tft.print(count);
    prev_count = count;
    flag = true;
    flag_page2 = false;
    pointer = 0;
  }
  if (prev_count != count) flag = false;
}

// фкнкция отрисовки длины провода
void lens(int coils_turns)
{
  static double prev_lens;
  //tft.fillRect(218, 245,  18, 26, 0x0F00);
  tft.setTextSize(0);

  // расчет длины провода
  if (my_data.wire == 0 || my_data.width_ == 0)
  {
    lens_count = PI * (my_data.diameter) / 1000 * coils_turns;
  }
  else if (my_data.wire != 0 && my_data.width_ != 0)
  {
    int numbers = my_data.width_ / my_data.wire;
    if (my_data.width_ % numbers) my_data.diameter = my_data.diameter + my_data.wire;
    lens_count = PI * (my_data.diameter) / 1000 * coils_turns;
  }


  if (flag_lens == false) {
    if (lens_count < 10) {
      tft.setCursor(199 + 9 * 8, 270);
      tft.print("m");
      tft.setCursor(199, 270);
    }
    else if (lens_count >= 10 && lens_count < 100) {
      tft.setCursor(199 + 9 * 8, 270);
      tft.print("m");
      tft.setCursor(199 - 9 * 2, 270);
    }
    else if (lens_count >= 100 && lens_count < 1000) {
      tft.setCursor(199 + 9 * 9, 270);
      tft.print("m");
      tft.setCursor(199 - 9 * 3, 270);
    }
    else if (lens_count >= 1000 && lens_count < 10000) {
      tft.setCursor(199 + 9 * 10, 270);
      tft.print("m");
      tft.setCursor(199 - 9 * 4, 270);
    }
    else if (lens_count >= 10000 && lens_count < 100000) {
      tft.setCursor(199 + 9 * 11, 270);
      tft.print("m");
      tft.setCursor(199 - 9 * 5, 270);
    }
    else if (lens_count >= 100000 && lens_count < 1000000) {
      tft.setCursor(199 + 9 * 12, 270);
      tft.print("m");
      tft.setCursor(199 - 9 * 6, 270);
    }
    else {
      tft.setCursor(199 + 9 * 12, 270);
      tft.print("m");
      tft.setCursor(199 - 9 * 7, 270);
    }
    prev_lens = lens_count;
    tft.print(lens_count);
    flag_lens = true;
  }
  if (prev_lens != lens_count) {
    tft.fillRect(100, 245, 280, 26, 0x0000);
    flag_lens = false;
  }
}

// фкнкция отрисовки угла поворота
void angle() {
  static double prev_angle;
  if (flag_angle == false)
  {
    tft.setTextSize(0);
    if (encoder / 2 >= 0 && encoder / 2 < 10)
    {
      tft.drawCircle (255, 60, 3, 0x0778);
      tft.drawCircle (255, 60, 2, 0x0778);
      tft.setCursor(230, 80);
    }
    else if (encoder / 2 >= 10 && encoder / 2 < 100)
    {
      tft.drawCircle (255 + 9, 60, 3, 0x0778);
      tft.drawCircle (255 + 9, 60, 2, 0x0778);
      tft.setCursor(230 - 9, 80);
    }
    else if (encoder / 2 >= 100 && encoder / 2 < 1000)
    {
      tft.drawCircle (255 + 18, 60, 3, 0x0778);
      tft.drawCircle (255 + 18, 60, 2, 0x0778);
      tft.setCursor(230 - 18, 80);
    }

    if (encoder / 3 > - 10 && encoder / 3 < 0)
    {
      tft.drawCircle (257, 60, 3, 0x0778);
      tft.drawCircle (257, 60, 2, 0x0778);
      tft.setCursor(230 - 9, 80);
    }
    else if (encoder / 3 > -100 && encoder / 3 <= -10)
    {
      tft.drawCircle (257 + 9, 60, 3, 0x0778);
      tft.drawCircle (257 + 9, 60, 2, 0x0778);
      tft.setCursor(230 - 18, 80);
    }
    else if (encoder / 3 > -1000 && encoder / 3 <= -100)
    {
      tft.drawCircle (257 + 18, 60, 3, 0x0778);
      tft.drawCircle (257 + 18, 60, 2, 0x0778);
      tft.setCursor(230 - 27, 80);
    }

    if (encoder >= 0) {
      tft.print(constrain(encoder / 4, 0, 360));
    }
    else tft.print(constrain(encoder / 4, -360, 0));

    prev_angle = encoder;
    flag_angle = true;
  }
  if (prev_angle != encoder)
  {
    tft.fillRect(190, 55, 100, 26, 0x0000);
    flag_angle = false;
  }
  //tft.fillRect(218, 245,  18, 26, 0x0F00);

}

// функция отрисовки страницы настроек
void printSetings(int pointer_value) {
  static int prev_pointer_value;
  if (flag_page2 == false) {
    tft.setTextColor(0x0778);
    tft.setTextSize(0); // размер шрифта
    tft.setCursor(10, 40);

    tft.drawRoundRect(5, 10 + (((pointer_value - 1) * 40)), 470, 40, 3, 0xFF20);

    tft.print(F("DIAMETER:\n"
                " WIDTH:\n"
                " WIRE GAUGE:\n"
                " ROTARY:\n"));

    for (uint8_t i = 0; i < 4; i++) {
      switch (i) {

        case 0:
          {
            tft.setCursor(340, 40);
            tft.print(my_data.diameter);
            break;
          }
        case 1:
          {
            tft.setCursor(340, 80);
            if (my_data.width_ > 0) tft.print(my_data.width_);
            else {
              tft.setTextColor(0xFF20);
              tft.print("-");
              tft.setTextColor(0x0778);
            }
            break;
          }
        case 2:
          {
            tft.setCursor(340, 120);
            if (my_data.wire > 0) tft.print(my_data.wire);
            else {
              tft.setTextColor(0xFF20);
              tft.print("-");
              tft.setTextColor(0x0778);
            }
            break;
          }
        case 3:
          {
            tft.setCursor(340, 160);
            if (my_data.rotary)
            {
              tft.print("R-L");
            }
            else
              tft.print("L-R");
            break;
          }
      }
    }

    prev_pointer_value = pointer_value;
    flag_page2 = true;
  }
  if (prev_pointer_value != pointer_value) {
    /*if (pointer == 3) {
      tft.drawRoundRect(5, 260, 110, 40, 3, 0x0000);
      }*/
    tft.drawRoundRect(5, 10 + (((prev_pointer_value - 1) * 40)) , 470, 40, 3, 0x0000);
    flag_page2 = false;
  }

  if (butt2.isSingle() || butt2.isStep() && flag_page >= 1)
  {
    tft.fillRect(300, 11 + (((pointer_value - 1) * 40)), 160, 38, 0x0000);

    if (pointer_value == 1) {
      my_data.diameter += 1;
      EEPROM.put(8, my_data.diameter);
      EEPROM.commit();
    }
    else if (pointer_value == 2) {
      my_data.width_ += 1;
      EEPROM.put(16, my_data.width_);
      EEPROM.commit();
    }
    else if (pointer_value == 3) {
      my_data.wire += 0.01f;
      EEPROM.writeFloat(24, my_data.wire);
      EEPROM.commit();
    }
    else if (pointer_value == 4) {
      my_data.rotary = !my_data.rotary;
      EEPROM.put(32, my_data.rotary);
      EEPROM.commit();
    }

    flag_page2 = false;
  }

  if (butt3.isSingle() || butt3.isStep() && flag_page >= 1)
  {
    tft.fillRect(300, 11 + (((pointer_value - 1) * 40)), 160, 38, 0x0000);

    if (pointer_value == 1) {
      my_data.diameter -= 1;
      my_data.diameter = constrain(my_data.diameter, 0, 10000);
      EEPROM.put(8, my_data.diameter);
      EEPROM.commit();
    }
    else if (pointer_value == 2) {
      my_data.width_ -= 1;
      my_data.width_ = constrain(my_data.width_, 0, 10000);
      EEPROM.put(16, my_data.width_);
      EEPROM.commit();
    }
    else if (pointer_value == 3) {
      my_data.wire -= 0.01f;
      my_data.wire = constrain(my_data.wire, 0, 10000);
      EEPROM.writeFloat(24, my_data.wire);
      EEPROM.commit();
    }
    else if (pointer_value == 4) {
      my_data.rotary = !my_data.rotary;
      EEPROM.put(32, my_data.rotary);
      EEPROM.commit();
    }

    flag_page2 = false;
  }

  
  flag_lens = false;
  flag_angle = false;
}

// фунция щаряда батареи
void battery_charge() {
  static int prev_step_battary;
  tft.drawBitmap(425, 10, Battery, 25, 25, 0xFFFF);
  if (analogRead(BATTERY) > 3000) step_battary = 1;
  else if (analogRead(BATTERY) > 2500 && analogRead(BATTERY) <= 3000) step_battary = 2;
  else if (analogRead(BATTERY) > 2000 && analogRead(BATTERY) <= 2500) step_battary = 3;
  else if (analogRead(BATTERY) <= 2000) step_battary = 4;

  switch (step_battary) {
    case 1:
      tft.drawBitmap(425, 10, Battery_Full, 25, 25, 0x0778);
      break;
    case 2:
      tft.drawBitmap(425, 10, Battery_Middle, 25, 25, 0x0778);
      break;
    case 3:
      tft.drawBitmap(425, 10, Battery_Low, 25, 25, 0x0778);
      break;
    case 4:
      tft.drawBitmap(425, 10, Battery_Extremely, 25, 25, 0xF000);
      break;
  }

  if (prev_step_battary != step_battary)
  {
    tft.fillRect(420, 5, 40, 30, 0x0000);
  }
  Serial.println(analogRead(BATTERY));
  prev_step_battary = step_battary;
}

// функция сохранения
void save()
{
  static long prev_save;
  if (butt1.isDouble())
  {
    EEPROM.put(0, coils_turns);
    EEPROM.commit();

    tft.drawBitmap(410, 135, Save, 50, 50, 0xFF20);
  }
  else if (prev_save == EEPROM.read(0))
  {
    tft.drawBitmap(410, 135, Save, 50, 50, 0xFF20);
  }
  if (prev_save != coils_turns) tft.fillRect(420, 145, 30, 30, 0x0000);
  prev_save = coils_turns;
}

// фугкция отрисовки режима
void mode_set() {
  if (my_data.wire == 0 && my_data.width_ == 0) {
    if (millis() % 1000 < 500) tft.drawBitmap(25, 183, Normal, 20, 20, 0x0778);
    else tft.drawBitmap(25, 183, Normal, 20, 20, 0x0000);
  }
  else
  {
    if (millis() % 1000 < 500) tft.drawBitmap(25, 183, Formula, 20, 20, 0x0778);
    else tft.drawBitmap(25, 183, Formula, 20, 20, 0x0000);
  }
}

// функция скорости вращения
void speed_test()
{
  static long timer_speed;
  static int count_speed, prev_count_speed;
  if (millis() - timer_speed > 1000)
  {
    tft.drawBitmap(25, 212, Speed, 20, 20, 0xFF20);
    tft.fillRect(50, 207, 150, 26, 0x0000);
    tft.setTextColor(0xFF20);
    tft.setCursor(50, 232);
    tft.print(abs(count_speed) * 60);
    tft.setTextColor(0x0778);
    prev_count_speed = coils_turns;
    timer_speed = millis();
  }
  count_speed = coils_turns - prev_count_speed;
}

// функции поворота енкодера
void encoderPinChangeA() {
  if (my_data.rotary == false)
  {
    encoder += (gpio_get_level(GPIO_NUM_26)) == (gpio_get_level(GPIO_NUM_27)) ? -1 : 1;
  }
  else
  {
    encoder += (gpio_get_level(GPIO_NUM_26)) != (gpio_get_level(GPIO_NUM_27)) ? -1 : 1;
  }
}

void encoderPinChangeB() {
  if (my_data.rotary == false)
  {
    encoder += (gpio_get_level(GPIO_NUM_26)) != (gpio_get_level(GPIO_NUM_27)) ? -1 : 1;
  }
  else
  {
    encoder += (gpio_get_level(GPIO_NUM_26)) == (gpio_get_level(GPIO_NUM_27)) ? -1 : 1;
  }
}
