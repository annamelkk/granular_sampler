#include "Arduino.h"
#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioRealFFT.h"
#include "AudioTools/CoreAudio/AudioEffects/SoundGenerator.h"

// DAC pins
#define DIN_PIN 23
#define LRCK_PIN 22
#define BCLK_PIN 21
#define VOLUME 11000

AudioInfo info(44100, 2, 16);

SineWaveGenerator<int16_t>     sine(VOLUME);
SquareWaveGenerator<int16_t>   square(VOLUME);
SawToothGenerator<int16_t> sawtooth(VOLUME);

GeneratedSoundStream<int16_t> sound(sine);
I2SStream out;
StreamCopy copier(out, sound);

int current_wave = 0;

void set_wave(int wave) {
  switch (wave) {
    case 0:
      sine.begin(info, N_B4);
      sound.setInput(sine);
      Serial.println("Sine");
      break;
    case 1:
      square.begin(info, N_B4);
      sound.setInput(square);
      Serial.println("Square");
      break;
    /*
    case 2:
      triangle.begin(info, N_B4);
      sound.setInput(triangle);
      Serial.println("Triangle");
      break;
    */
    case 2:
      sawtooth.begin(info, N_B4);
      sound.setInput(sawtooth);
      Serial.println("Sawtooth");
      break;
  }
}

void setup(void) {

  Serial.begin(115200);

  auto config = out.defaultConfig(TX_MODE);
  config.copyFrom(info);
  config.pin_bck = BCLK_PIN;
  config.pin_ws = LRCK_PIN;
  config.pin_data = DIN_PIN;
  out.begin(config);

  set_wave(0);

  Serial.println("Send 0-2 to switch waves:");
  Serial.println("0=Sine 1=Square 2=Sawtooth");
}

void loop() {
  copier.copy();

  if (Serial.available()) {
    char c = Serial.read();
    if (c >= '0' && c <= '2')
      set_wave(c - '0');
  }
}
