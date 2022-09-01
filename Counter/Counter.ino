#include "U8glib.h"
#include "GyverButton.h"
#include <EEPROM.h>

#define EEPROM_ADDR 0

#define seconds() (millis()/1000ul)

#define hallSensorPin 2
#define PIN 3

U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);

GButton butt1(PIN);

int value = 0;
bool flag = false;
uint32_t timer;
uint32_t timer2;
uint8_t draw_state = 0;

//----------------------- счетчик --------------------//
void coil_counter()
{
  if (digitalRead(hallSensorPin) == 1 && !flag)
  {
    flag = true;
  }
  if (!digitalRead(hallSensorPin) && flag)
  {
    flag = false;
    value++;
  }

  value = (value >= 10000000) ? value : value;
  u8g.setFont(u8g_font_gdb14); //u8g_font_gdb17 u8g_font_fur11
  u8g.setPrintPos(0, 32);
  u8g.print(value);
  Serial.println(digitalRead(hallSensorPin));
  // ---------------- сброс счетчика -------------//
  if (butt1.isHolded())
  {
    value = 0;
    Serial.println("Reset");
  }
}

//----------------------- обратный счетчик --------------------//
void coil_counter_reverse()
{
  if (digitalRead(hallSensorPin) == 1 && !flag)
  {
    flag = true;
  }
  if (!digitalRead(hallSensorPin) && flag)
  {
    flag = false;
    value--;
    value = (value < 0) ? 0 : value;
  }

  value = (value >= 10000000) ? value : value;
  u8g.setFont(u8g_font_gdb14);
  u8g.setPrintPos(0, 32);
  u8g.print(value);
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr(85, 30, "R");

  // ---------------- сброс счетчика -------------//
  if (butt1.isHolded())
  {
    value = 0;
    draw_state = 0;
    Serial.println("Reset");
  }
}

// ------------------  для вывода таймера ---------------- //
void timer_counter()
{
  u8g.setFont(u8g_font_gdb14); //u8g_font_gdb17 u8g_font_fur11
  u8g.setPrintPos(10, 32);
  if (value == 0) {
    timer = seconds();
  }

  if (value == 0)
  {
    u8g.print("00:00:00");
  }

  if (value > 0) {
    if ((seconds() - timer) / 3600ul < 10) {
      u8g.print("0");
      u8g.print((seconds() - timer) / 3600ul);
    }
    else
    {
      u8g.print((seconds() - timer) / 3600ul);
    }

    u8g.print(":");

    if (((seconds() - timer) % 3600ul) / 60ul < 10) {
      u8g.print("0");
      u8g.print(((seconds() - timer) % 3600ul) / 60ul);
    }
    else
    {
      u8g.print(((seconds() - timer) % 3600ul) / 60ul);
    }

    u8g.print(":");

    if (((seconds() - timer) % 3600ul) % 60ul < 10)
    {
      u8g.print("0");
      u8g.print(((seconds() - timer) % 3600ul) % 60ul);
    }
    else
    {
      u8g.print(((seconds() - timer) % 3600ul) % 60ul);
    }
  }

  // ---------------- сброс таймера -------------//
  if (butt1.isHolded())
  {
    value = 0;
    Serial.println("Reset");
  }
}

// ----------------- EEPROM ----------------------//
void eprom ()
{
}

//-------------------- анимация ------------------------//
void coil_pixel(uint8_t a)
{
  u8g.drawCircle(0, 23, 15);
  u8g.drawCircle(25, 23, 15);
}

void draw(void) {
  switch (draw_state) {
    case 0: attachInterrupt(0, coil_counter, LOW); coil_counter(); break;
    case 1: attachInterrupt(0, coil_counter_reverse, LOW); coil_counter_reverse(); break;
    //case 2: timer_counter(); break;
  }
}

void setup(void) {
  //u8g.setRot180();
  Serial.begin(9600);

  pinMode(hallSensorPin, INPUT_PULLUP);

  //attachInterrupt(0, coil_counter_reverse, LOW);

  butt1.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt1.setTimeout(700);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt1.setClickTimeout(200);   // настройка таймаута между кликами (по умолчанию 300 мс)

  // HIGH_PULL - кнопка подключена к GND, пин подтянут к VCC (PIN --- КНОПКА --- GND)
  // LOW_PULL  - кнопка подключена к VCC, пин подтянут к GND
  butt1.setType(HIGH_PULL);

  // NORM_OPEN - нормально-разомкнутая кнопка
  // NORM_CLOSE - нормально-замкнутая кнопка
  butt1.setDirection(NORM_OPEN);

  value = EEPROM.read(EEPROM_ADDR);
}

void loop(void) {
  butt1.tick();

  if (butt1.isSingle()) {
    draw_state++;
  }
  if (draw_state >= 2) draw_state = 0;

  if (butt1.isDouble())
  {
    EEPROM.update(EEPROM_ADDR, value);
    draw_state = 2;
  }


  u8g.firstPage();
  do {
    draw();
  } while (u8g.nextPage());
}
