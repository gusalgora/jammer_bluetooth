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
#define BT1 2
#define BT2 3

bool jamming = true; 
int channels = 0;
const int num_reps = 50;

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
  display.clearDisplay();

  // Encuentra el máximo valor para normalizar la escala
  int maxVal = 0;
  int maxChannel = 0;
  for (int i = 0; i < CHANNELS; i++) {
    if (channel[i] > maxVal) {
      maxVal = channel[i];
      maxChannel = i;
    }
  }

  // Dibuja cada canal como una barra vertical en la pantalla
  for (int i = 0; i < CHANNELS; i++) {
    int barHeight = map(channel[i], 0, maxVal, 0, display.height());
    display.drawLine(i * 2, display.height(), i * 2, display.height() - barHeight, WHITE);
    channel[i] = 0; // Resetea el valor del canal después de mostrarlo
  }

  // Muestra el canal con la señal más fuerte
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Max Ch: ");
  display.print(maxChannel);
  display.print(" Level: ");
  display.print(maxVal);

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
  if (jamming) {
    jammer(radio1);
    jammer(radio2);
  } else {
    scanChannels(radio1);
    scanChannels(radio2);
    outputChannels();
  }

  delay(500); // Ajusta el retardo según la frecuencia deseada de escaneo o jamming
}
