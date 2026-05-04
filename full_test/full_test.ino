// =============== LIBRARIES =================

#include "Arduino.h"

#include <SD.h>
#include <SPI.h>
#include <drivers/i2s.h>

// ── Display Tools ───────────────────

#include <Adafruit_GFX.h>    
#include <Adafruit_ST7789.h> 

// ── Audio Tools ───────────────────

#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioRealFFT.h"
#include "AudioTools/CoreAudio/AudioEffects/SoundGenerator.h"



// ===============   PINS    =================

// ── Shared SPI bus ───────────────────
#define SPI_SCK   6
#define SPI_MOSI  5
#define SPI_MISO  4

// ── Shared SPI bus ───────────────────────

#define SPI_SCK   6
#define SPI_MOSI  5
#define SPI_MISO  4

// ── TFT ST7789 ───────────────────────
#define TFT_CS      7
#define TFT_DC      1
#define TFT_RST     -1

// ── SD card ──────────────────────────
#define SD_CS     2

// ── PCM5100 I2S DAC ──────────────────
#define BCLK_PIN  21
#define LRCK_PIN  22
#define DIN_PIN   23


// ── Buttons (INPUT PULLUP) ──────────────────────
#define BTN_PLAY  0    // Play / Stop
#define BTN_NEXT  3    // Next sample
#define BTN_PREV  9    // Prev sample
#define BTN_HOLD  14   // Freeze position

// ── Pots (2) — ADC ───────────────────
#define POT_POS   18   // Grain position
#define POT_SIZE  19   // Grain size

// ── Granular Engine ─────────────────────
#define MAX_GRAINS    16
#define MAX_WAV_SECS  8
#define SAMPLE_RATE   22050
#define WAV_BUF_SIZE (SAMPLE_RATE * MAX_WAV_SECS) // 10 seconds at 44100 Hz

// ── Fixed synth params ─────────────────────
#define DEFAULT_SPEED   1.0f // 1.0 = normal 0.5=oct down 2.0=oct up
#define DEFAULT_DENSITY 8.0f // grains per second
#define DEFAULT_VOL     0.8f


// ── Free / spare ─────────────────────
// GPIO 15, 20


// =============== GLOBAL VARIABLES =================

// Initialize the display object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Initialize the audio obj
AudioInfo info(SAMPLE_RATE, 2, 16);

I2SSTREAM out;


int16_t wav_buf[WAV_BUF_SIZE]; // loaded sample
static  int32_t  wav_len  = 0;
static  bool              = false;

struct Grain
{
  bool  active;
  float read_pos; // fractional read position
  float speed;    // playback rate (1.0 = normal)
  int   size;     // grain length in samples
  int   age;      // elapsed samples since spawn
  float pan_L;    // left gain 0-1
  float pan_R;    // right gian
};

static Grain grains[MAX_GRAINS];


static volatile float param_pos     = 0.5f;
static volatile float param_size    = 0.15f;
static volatile float param_speed   = DEFAULT_SPEED;
static volatile float param_density = DEFAULT_DENSITY;
static volatile float param_vol     = DEFAULT_VOL;

static int grain_timer = 0;
static int grain_interval = SAMPLE_RATE / (int)DEFAULT_DENSITY;

static SPIClass         spi(FSPI);
static Adafruit_ST7789  tft(TFT_cS, TFT_DC, TFT_RST);


const uint8_t BTN_PINS[4] = {BTN_PLAY, BTN_NEXT, BTN_PREV, BTN_HOLD};
static bool btn_last[4] = {HIGH,HIGH,HIGH,HIGH};
static uint32_t btn_time[4] = {0,0,0,0};
#define DEBOUNCE_MS 25

#define MAX_FILES 16
static char file_name[MAX_FILES][32];
static int  file_count = 0;
static int  file_index = 0;


void setup() 
{
  Serial.begin(115200);

// ── SD card reader mount ─────────────────────

  spi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CD);
  if ( !(SD.begin(SD_CS, spi, 4000000)) )
  {
    Serial.println("SD mount failed");
    return;
  }

  Serial.println("SD mounted OK");
  File root = SD.open("/");
  File file = root.openNextFile();
  while(file)
  {
    Serial.println(file.name());
    file = root.openNextFile();
  }

// ──    TFT screen init    ─────────────────────

  tft.init(240, 240, SPI_MODE2);
  tft.invertDisplay(true);


// ── Audio config I2S init ─────────────────────


  auto config = out.defaultConfig(TX_MODE);
  config.copyFrom(info);
  config.pin_bck = BCLK_PIN;
  config.pin_ws = LRCK_PIN;
  config.pin_data = DIN_PIN;
  out.begin(config);

  set_wave(0);

}

void loop() {
}
