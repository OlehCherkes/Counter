#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define EEPROM_ADDR 0

#define BUT_PIN 2         // пин кнопки
#define ANALOG_ACS756 A0  // пин ACS756
#define ANALOG_VOLTAGE A2 // напряжение
#define RELAY_1 3         // реле №1
#define RELAY_2 4         // реле №2
#define RELAY_3 5         // реле №3

LiquidCrystal_I2C lcd(0x27, 20, 4);

bool flag = false;
float volt;
const float factor = 0.04;   // чуствительность датчика (0.02V)
const float QOV = 0.5 * 5;   // установка покоя для датчика
int count = 0;        //счетчик нажатия кнопок
 
byte customChar[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111};

byte customChar1[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111};

byte customChar2[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111};
  
  byte customChar3[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111};
  

int buttonCount();
void printValue();
void relaySwitch(); 
float voltageCurrent();
void animation();

void setup()
{
  pinMode(BUT_PIN, INPUT);
  pinMode(ANALOG_VOLTAGE, INPUT);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, customChar);
  lcd.createChar(1, customChar1);
  lcd.createChar(2, customChar2);
  lcd.createChar(3, customChar3);

  count = EEPROM.read(EEPROM_ADDR);

  Serial.begin(9600);
  animation();
}

void loop()
{
  buttonCount();
  printValue(buttonCount(), voltageCurrent());
  relaySwitch(buttonCount());
  voltageCurrent(); 
}

int buttonCount()
{
  bool btnState = digitalRead(BUT_PIN);
  if (btnState && flag) {  
    flag = false;
  }
  if (!btnState && !flag) {  
    flag = true; 
    EEPROM.update(EEPROM_ADDR, count); 
    count++;
    if (count > 3) count = 1;
  }
  return count;
}

void printValue(int count, float voltage)
{   
  float current;

    lcd.setCursor(0, 0);
    lcd.print("RELAY ON: ");
    switch(count) 
    {
      case 1:
       lcd.write((uint8_t)1);
       lcd.write((uint8_t)0);
       lcd.write((uint8_t)0);

       lcd.setCursor(0, 1);
       lcd.print("UPOLAR: ");
       lcd.print("40.00");
       lcd.print("V ");
       
       lcd.setCursor(0, 2);
       lcd.print("BPOLAR: ");
       lcd.print("68.00");
       lcd.print("V ");
       break;
      case 2:
       lcd.write((uint8_t)0);
       lcd.write((uint8_t)2);
       lcd.write((uint8_t)0);
       
       lcd.setCursor(0, 1);
       lcd.print("UPOLAR: ");
       lcd.print("69.00");
       lcd.print("V ");
       
       lcd.setCursor(0, 2);
       lcd.print("BPOLAR: ");
       lcd.print("116.00");
       lcd.print("V ");
       break;
      case 3:
       lcd.write((uint8_t)0);
       lcd.write((uint8_t)0);
       lcd.write((uint8_t)3);

       lcd.setCursor(0, 1);
       lcd.print("UPOLAR: ");
       lcd.print("143.00");
       lcd.print("V ");
       
       lcd.setCursor(0, 2);
       lcd.print("BPOLAR: ");
       lcd.print("236.00");
       lcd.print("V ");
       break;
      default: 
        lcd.write((uint8_t)0);
        lcd.write((uint8_t)0);
        lcd.write((uint8_t)0);
      }
      
     //lcd.setCursor(0, 2);
     //lcd.print("VOLTAGE: ");
     //volt = (5.0 / 1024.0) * analogRead(ANALOG_VOLTAGE) * 22.3;
     //lcd.print(volt);
     //lcd.print("V");

     lcd.setCursor(0, 3);
     lcd.print("CURRENT: ");
     voltage = (voltage > 0.02) ? voltage : 0;
     current = (voltage / factor) - 0.4;
     current = (current >= 0) ? current : 0;
     lcd.print(current);
     Serial.println(voltage);
     lcd.print("A ");
  }

void relaySwitch(int count) 
{
  switch(count)
  {
    case 1:
      digitalWrite(RELAY_1, HIGH);
      digitalWrite(RELAY_2, LOW);
      digitalWrite(RELAY_3, LOW);
      break;
    case 2: 
      digitalWrite(RELAY_2, HIGH);
      digitalWrite(RELAY_1, LOW);
      digitalWrite(RELAY_3, LOW);
      break;
     case 3: 
      digitalWrite(RELAY_3, HIGH);
      digitalWrite(RELAY_1, LOW);
      digitalWrite(RELAY_2, LOW);
      break;
    default:
      digitalWrite(RELAY_1, LOW);
      digitalWrite(RELAY_2, LOW);
      digitalWrite(RELAY_3, LOW);
      break;
    }
  }

float voltageCurrent() 
{
  float voltage = (5.0 / 1024.0) * analogRead(ANALOG_ACS756);
  voltage = voltage - QOV + 0.021; // обнуляем датчик
  Serial.println(voltage);
  return voltage;
  }

void animation() {
  delay(200);
  int d = 70;
  lcd.setCursor(7, 1);
  lcd.print("L");
  delay(d);
  lcd.setCursor(8, 1);
  lcd.print("_");
  delay(d);
  lcd.setCursor(9, 1);
  lcd.print("X");
  delay(d);
  lcd.setCursor(10, 1);
  lcd.print("|");
  delay(d);
  lcd.setCursor(11, 1);
  lcd.print("|");
  delay(d);
  lcd.setCursor(12, 1);
  lcd.print("|");
  delay(d);
  
  lcd.setCursor(4, 2);
  lcd.print("R");
  delay(d);
  lcd.setCursor(5, 2);
  lcd.print("E");
  delay(d);
  lcd.setCursor(6, 2);
  lcd.print("L");
  delay(d);
  lcd.setCursor(7, 2);
  lcd.print("A");
  delay(d);
  lcd.setCursor(8, 2);
  lcd.print("Y");
  delay(d);
  lcd.setCursor(9, 2);
  lcd.print(" ");
  delay(d);
  lcd.setCursor(10, 2);
  lcd.print("S");
  delay(d);
  lcd.setCursor(11, 2);
  lcd.print("W");
  delay(d);
  lcd.setCursor(12, 2);
  lcd.print("I");
  delay(d);
  lcd.setCursor(13, 2);
  lcd.print("T");
  delay(d);
  lcd.setCursor(14, 2);
  lcd.print("C");
  delay(d);
  lcd.setCursor(15, 2);
  lcd.print("H");
  delay(2000);
  lcd.clear();
}
