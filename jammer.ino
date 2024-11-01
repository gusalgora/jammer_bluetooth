#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RF24.h>
#include <nRF24L01.h>

#define OLED_RESET 4
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

// Pines para el primer transmisor
#define CE1 7
#define CSN1 8
RF24 radio1(CE1, CSN1); 

// Pines para el segundo transmisor
#define CE2 9
#define CSN2 10
RF24 radio2(CE2, CSN2);

#define CHANNELS 64
int channel[CHANNELS];

int line;
char grey[] = " .:-=+*aRW";

#define BT1 2
#define BT2 3

byte count;
byte sensorArray[128];
byte drawHeight;

char filled = 'F'; 
char drawDirection = 'R'; 
char slope = 'W'; 

const uint8_t num_channels = 64;
int values[num_channels];
int valuesDisplay[32];
int channels = 0;
const int num_reps = 50;
bool jamming = true;

void setup() {
  Serial.begin(57600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.clearDisplay();

  // Configuración del primer transmisor
  radio1.begin();
  radio1.setPALevel(RF24_PA_HIGH);
  radio1.setDataRate(RF24_2MBPS);
  radio1.stopListening();

  // Configuración del segundo transmisor
  radio2.begin();
  radio2.setPALevel(RF24_PA_HIGH);
  radio2.setDataRate(RF24_2MBPS);
  radio2.stopListening();

  pinMode(BT1, INPUT_PULLUP);
  pinMode(BT2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BT1), pressBt01, FALLING);
  attachInterrupt(digitalPinToInterrupt(BT2), pressBt02, FALLING);
}

void pressBt01() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200) {
    if (channels < 13) {
      channels++;
    } else {
      channels = 0;
    }
  }
  last_interrupt_time = interrupt_time;
}

void pressBt02() {
  jamming = !jamming;
  delay(200);
}

void scanChannels(RF24 &radio) {
  for (int j = 0; j < num_reps; j++) {
    for (int i = 0; i < CHANNELS; i++) {
      radio.setChannel(i);
      delayMicroseconds(40);
      if (radio.testRPD()) {
        channel[i]++;
      }
    }
  }
}

void outputChannels() {
  int norm = 0;
  for (int i = 0; i < CHANNELS; i++)
    if (channel[i] > norm) norm = channel[i];
  
  Serial.print('|');
  for (int i = 0; i < CHANNELS; i++) {
    int pos = (norm != 0) ? (channel[i] * 10) / norm : 0;
    if (pos == 0 && channel[i] > 0) pos++;
    if (pos > 9) pos = 9;
    Serial.print(grey[pos]);
    channel[i] = 0;
  }
  Serial.print("| ");
  Serial.println(norm);
}

// Visualización en la pantalla OLED
void outputOLED(int norm) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  
  if (jamming) {
    display.println("Modo: Jamming");
  } else {
    display.println("Modo: Escaneo");
    display.println("Canal | Intensidad");
    for (int i = 0; i < CHANNELS; i++) {
      display.setCursor(0, 12 + i * 2);
      display.print(String(i) + " ");
      int pos = (norm != 0) ? (channel[i] * 10) / norm : 0;
      display.print(grey[pos]);
    }
  }

  display.display();
}

void jammer(RF24 &radio) {
  const char text[] = "xxxxxxxxxxxxxxxx";
  for (int i = (channels * 5) + 1; i < (channels * 5) + 23; i++) {
    radio.setChannel(i);
    radio.write(&text, sizeof(text));
  }
}

void loop() {
  int norm = 0;

  if (jamming) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println("Modo: Jamming");
    display.display();
    jammer(radio1);
    jammer(radio2);
  } else {
    scanChannels(radio1);
    scanChannels(radio2);

    for (int i = 0; i < CHANNELS; i++) {
      if (channel[i] > norm) norm = channel[i];
    }

    outputChannels();
    outputOLED(norm);
  }

  delay(1000); // Ajusta el retardo según la frecuencia deseada de escaneo o jamming
}
