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
  static unsigned long last_interrupt_time_bt2 = 0; // Static variable for debouncing
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time_bt2 > 200) { // 200ms debounce period
    jamming = !jamming;
    last_interrupt_time_bt2 = interrupt_time; // Update time after action
  }
}

void scanChannels(RF24 &radio) {
  int start_channel = channels * 5;

  // Initialize/clear the 5 channels in the current group within the global array
  for (int k = 0; k < 5; k++) {
    int current_channel_idx = start_channel + k;
    if (current_channel_idx < CHANNELS) {
      channel[current_channel_idx] = 0; // Reset only the current group's channels
    }
  }

  for (int j = 0; j < num_reps; j++) { // num_reps is 50
    for (int k = 0; k < 5; k++) {      // Iterate through the 5 channels in the group
      int current_channel_to_scan = start_channel + k;

      if (current_channel_to_scan >= CHANNELS) { // Boundary check for CHANNELS (64)
        continue;
      }

      radio.setChannel(current_channel_to_scan);
      delayMicroseconds(40);
      if (radio.testRPD()) {
        channel[current_channel_to_scan]++;
      }
    }
  }
}

void outputChannels() {
  display.clearDisplay();
  int start_channel = channels * 5;

  int maxVal = 0;
  // Initialize maxChannelInGroup to the first valid channel in the group or start_channel itself
  maxChannelInGroup = start_channel;
  for(int k=0; k<5; ++k) {
    if(start_channel + k < CHANNELS) {
      maxChannelInGroup = start_channel + k;
      break;
    }
  }


  // Find maxVal and maxChannelInGroup within the current 5-channel group
  for (int k = 0; k < 5; k++) {
    int current_channel_idx = start_channel + k;
    if (current_channel_idx >= CHANNELS) { // Boundary check
      continue;
    }
    if (channel[current_channel_idx] > maxVal) {
      maxVal = channel[current_channel_idx];
      maxChannelInGroup = current_channel_idx;
    }
  }

  int bar_width = display.width() / 5; // e.g., 128 / 5 = 25 (integer division)

  // Draw bars for the 5 channels in the group
  for (int k = 0; k < 5; k++) {
    int current_channel_to_display = start_channel + k;
    if (current_channel_to_display >= CHANNELS) { // Boundary check
      continue;
    }

    int barHeight = 0;
    if (maxVal > 0) { // Avoid division by zero if maxVal is 0 and map issues
        barHeight = map(channel[current_channel_to_display], 0, maxVal, 0, display.height());
    }

    // Use fillRect for wider bars, leave a small gap (e.g., 2 pixels) for clarity
    display.fillRect(k * bar_width, display.height() - barHeight, bar_width > 1 ? bar_width - 1 : bar_width, barHeight, WHITE);
    // Note: The global channel array `channel[current_channel_to_display]` is NOT reset here.
  }

  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Grp:");
  display.print(channels);
  display.print(" Max:"); // Abbreviated to fit
  display.print(maxChannelInGroup);
  display.print(" Lvl:");
  display.print(maxVal);

  display.display();
}

void jammer(RF24 &radio) {
  const char text[] = "xxxxxxxxxxxxxxxx";
  for (int i = (channels * 5); i < (channels * 5) + 5; i++) {
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
