#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// ---------- TFT PINS (change if needed) ----------
#define TFT_CS   5      // Chip Select
#define TFT_DC   2      // Data/Command
#define TFT_RST  4      // Reset

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

// ---------- BUTTONS ----------
#define MODE_BUTTON 33
#define IR_BUTTON   32

// ---------- UI STATES ----------
enum UiMode { UI_IDLE, UI_MODE_GAS, UI_MODE_ENV, UI_MODE_THRESH };
UiMode uiMode = UI_IDLE;
bool irMode = false;   // IR screen override

// ---------- COLORS ----------
uint16_t bgColor      = ILI9341_BLACK;
uint16_t textColor    = ILI9341_WHITE;
uint16_t highlight    = ILI9341_CYAN;
uint16_t alertColor   = ILI9341_RED;

// ---------- DUMMY DATA ----------
// (Replace these with real sensor values later)
float co     = 12.4;
float co2    = 421;
float no2    = 5.1;
float nh3    = 3.7;
float alcohol= 7.9;
float o3     = 4.3;
float o2     = 20.8;

float tempC  = 25.7;
float rh     = 42.1;
float press  = 1013.2;

float dewPoint   = 17.5;
float feelsLike  = 27.2;
float absHum     = 9.3;
float altitude   = 500;

float irTemp     = 28.4;

// ---------- BUTTON ISR ----------
void IRAM_ATTR modeButtonISR() {
  if (!irMode) {
    uiMode = (UiMode)((uiMode + 1) % 4); // Cycle 0→1→2→3→0
  }
}

void IRAM_ATTR irButtonISR() {
  irMode = !irMode;
}

// ---------- DRAW FUNCTIONS ----------
void drawTitle(String title, String time = "14:35:21") {
  tft.fillRect(0, 0, 320, 25, ILI9341_BLUE);
  tft.setCursor(5, 5);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print(title);

  // Time on right side
  tft.setCursor(210, 5);
  tft.print(time);
}

// ----- IDLE SCREEN -----
void drawIdleScreen() {
  tft.fillScreen(bgColor);
  drawTitle("IDLE");

  tft.setTextColor(textColor);
  tft.setTextSize(2);

  // Gases
  tft.setCursor(10, 40);
  tft.print("CO:  "); tft.print(co); tft.print("%");

  tft.setCursor(10, 70);
  tft.print("CO2:"); tft.print(co2); tft.print("ppm");

  tft.setCursor(10, 100);
  tft.print("NO2: "); tft.print(no2); tft.print("%");

  tft.setCursor(10, 130);
  tft.print("NH3: "); tft.print(nh3); tft.print("%");

  tft.setCursor(10, 160);
  tft.print("ALC: "); tft.print(alcohol); tft.print("%");

  tft.setCursor(10, 190);
  tft.print("O3:  "); tft.print(o3); tft.print("%");

  tft.setCursor(180, 40);
  tft.print("O2*: "); tft.print(o2); tft.print("%");

  // Ambient
  tft.setCursor(180, 70);
  tft.print("T: "); tft.print(tempC); tft.print("C");

  tft.setCursor(180, 100);
  tft.print("RH:  "); tft.print(rh); tft.print("%");

  tft.setCursor(180, 130);
  tft.print("P: "); tft.print(press);
}

// ----- MODE 1: GAS EXTENDED -----
void drawGasMode() {
  tft.fillScreen(bgColor);
  drawTitle("GAS DETAIL");

  tft.setTextColor(textColor);
  tft.setTextSize(2);

  // Group A: CO + CO2
  tft.setCursor(10, 40);
  tft.print("CO:  "); tft.print(co); tft.print("%");

  tft.setCursor(10, 70);
  tft.print("CO2: "); tft.print(co2); tft.print("ppm");

  // Group B: NO2 + NH3 + Alcohol
  tft.setCursor(10, 110);
  tft.print("NO2: "); tft.print(no2); tft.print("%");

  tft.setCursor(10, 140);
  tft.print("NH3: "); tft.print(nh3); tft.print("%");

  tft.setCursor(10, 170);
  tft.print("ALC: "); tft.print(alcohol); tft.print("%");

  // Group C: O3 + O2
  tft.setCursor(10, 210);
  tft.print("O3:  "); tft.print(o3); tft.print("%");

  tft.setCursor(170, 210);
  tft.print("O2*: "); tft.print(o2); tft.print("%");
}

// ----- MODE 2: ENV EXTENDED -----
void drawEnvMode() {
  tft.fillScreen(bgColor);
  drawTitle("ENV DETAIL");

  tft.setTextColor(textColor);
  tft.setTextSize(2);

  tft.setCursor(10, 50);
  tft.print("Temp:  "); tft.print(tempC); tft.print("C");

  tft.setCursor(10, 80);
  tft.print("Feels: "); tft.print(feelsLike); tft.print("C");

  tft.setCursor(10, 110);
  tft.print("RH:    "); tft.print(rh); tft.print("%");

  tft.setCursor(10, 140);
  tft.print("DewPt: "); tft.print(dewPoint); tft.print("C");

  tft.setCursor(10, 170);
  tft.print("AbsHum: "); tft.print(absHum); tft.print("g/m3");

  tft.setCursor(10, 200);
  tft.print("Press: "); tft.print(press); tft.print("hPa");
}

// ----- MODE 3: THRESHOLDS / ALARMS -----
void drawThreshMode() {
  tft.fillScreen(bgColor);
  drawTitle("THRESH & ALARMS");

  tft.setTextColor(textColor);
  tft.setTextSize(2);

  tft.setCursor(10, 50);
  tft.print("CO  TH: 50 ppm");

  tft.setCursor(10, 80);
  tft.print("CO2 TH: 1000 ppm");

  tft.setCursor(10, 110);
  tft.print("NO2 TH: 100%");

  tft.setCursor(10, 140);
  tft.print("NH3 TH: 100%");

  tft.setCursor(10, 170);
  tft.print("O3  TH: 100%");

  tft.setCursor(10, 200);
  tft.print("O2  MIN: 19.5%");
}

// ----- IR MODE SCREEN -----
void drawIRScreen() {
  tft.fillScreen(ILI9341_DARKGREY);
  drawTitle("IR MODE");

  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(3);

  tft.setCursor(20, 70);
  tft.print("IR: ");
  tft.print(irTemp);
  tft.print("C");

  tft.setCursor(20, 130);
  tft.print("Ambient: ");
  tft.print(tempC);
  tft.print("C");

  float delta = irTemp - tempC;

  tft.setCursor(20, 180);
  tft.print("Delta: ");
  tft.print(delta);
  tft.print("C");
}

// ---------- MAIN LOOP ----------
void updateUI() {
  if (irMode) {
    drawIRScreen();
    return;
  }

  switch (uiMode) {
    case UI_IDLE:        drawIdleScreen(); break;
    case UI_MODE_GAS:    drawGasMode(); break;
    case UI_MODE_ENV:    drawEnvMode(); break;
    case UI_MODE_THRESH: drawThreshMode(); break;
  }
}

void setup() {
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(bgColor);

  pinMode(MODE_BUTTON, INPUT_PULLUP);
  pinMode(IR_BUTTON, INPUT_PULLUP);

  attachInterrupt(MODE_BUTTON, modeButtonISR, FALLING);
  attachInterrupt(IR_BUTTON, irButtonISR, FALLING);
}

void loop() {
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw > 3000) {
    lastDraw = millis();
    updateUI();
  }
}
