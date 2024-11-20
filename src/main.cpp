#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Biblioteca para controle do display I2C

#define ADDR 0x48
#define U33 3.342

int n = 3;
float r, u, n_u = 0.256, r0;
long dig_sum, dig;
int value, f;

// Inicializa o display I2C com endereço 0x27, tamanho 20x4
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Function declaration
float read_u();

void setup() {
  Serial.begin(9600);

  // Inicializa o display
  lcd.begin(20, 4);  // Initialize with 20 columns and 4 rows
  lcd.backlight();
  lcd.clear();

  // Exibe mensagem inicial no display
  lcd.setCursor(0, 0);
  lcd.print("Arduino Miliohm");
  lcd.setCursor(0, 1);
  lcd.print("Inicializando...");

  Wire.begin();
  pinMode(4, INPUT_PULLUP); // null
}

void loop() {
  r = ((100.00 * u) / (U33 - u)) - r0;

  if (digitalRead(4) == LOW && r < 0.1) {
    r0 = ((100.00 * u) / (U33 - u));
    delay(200);
  }

  for (int i = 0; i < 10; i++) {
    read_u();
    dig_sum = dig_sum + value;
    delay(10);
  }

  read_u();
  if (value > 0x7FFF - 0xF && n == 1) {
    delay(10);
    n = 0;
    n_u = 2.048;
  }
  read_u();
  if (value > 0x7FFF - 0xF && n == 2) {
    delay(10);
    n = 1;
    n_u = 1.024;
  }
  read_u();
  if (value > 0x7FFF - 0xF && n == 3) {
    delay(10);
    n = 2;
    n_u = 0.512;
  }
  read_u();
  if (value < 16380 && n == 0) {
    delay(10);
    n = 1;
    n_u = 1.024;
  }
  read_u();
  if (value < 16380 && n == 1) {
    delay(10);
    n = 2;
    n_u = 0.512;
  }
  read_u();
  if (value < 16380 && n == 2) {
    delay(10);
    n = 3;
    n_u = 0.256;
  }

  dig = dig_sum / 10;
  dig_sum = 0;
  u = dig * n_u / 0x7FFF;

  // Atualiza o display com o valor calculado
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Resistencia (R):");
  lcd.setCursor(0, 1);

  if (r < 10) {
    lcd.print(r, 3); // Exibe 3 casas decimais
  } else if (r >= 10 && r < 100) {
    lcd.print(r, 2); // Exibe 2 casas decimais
  } else if (r >= 100 && r <= 150) {
    lcd.print(r, 1); // Exibe 1 casa decimal
  } else {
    lcd.print("Fora de escala");
  }

  lcd.setCursor(0, 2);
  lcd.print("R0: ");
  lcd.print(r0, 5);

  Serial.println(r0, 5);
}

float read_u() {
  Wire.beginTransmission(ADDR);
  Wire.write(0b10001100 + n);
  Wire.endTransmission();
  Wire.requestFrom(ADDR, 2);
  while (Wire.available() < 2)
    ;
  value = (Wire.read() << 8) + (Wire.read());
  return value;
}
