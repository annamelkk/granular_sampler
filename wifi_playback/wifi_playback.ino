/*
www.makerguides.com

Libraries:
- ESP32 Core 3.3.6
- [arduino-audio-tools](https://github.com/pschatzmann/arduino-audio-tools) 
  Version: 1.2.2
- [arduino-libhelix](https://github.com/pschatzmann/arduino-libhelix)
  Version: 0.9.2
*/

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
#include "AudioTools/Communication/HTTP/ICYStream.h"

// PCM5102A
#define DIN_PIN 7   // serial data
#define LRCK_PIN 8  // word select
#define BCLK_PIN 6  // serial clock
#define VOLUME 0.3   // Volume

const char* ssid = "Ucom0588";
const char* password = "pentagon";
const char* url = "https://jazz.stream.laut.fm/jazz";

ICYStream icystream;
I2SStream i2s;
VolumeStream volume(i2s);
EncodedAudioStream mp3decode(&volume, new MP3DecoderHelix());
StreamCopy copier(mp3decode, icystream);

void callbackMetadata(MetaDataType type, const char* str, int len) {
  Serial.printf("%s: %s\n", toStr(type), str);
}

void setup() 
{
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Not connected to WiFi yet");
    delay(500);
  }
  Serial.println("WiFi connected");

  auto config = i2s.defaultConfig(TX_MODE);
  config.pin_bck = BCLK_PIN;
  config.pin_ws = LRCK_PIN;
  config.pin_data = DIN_PIN;
  config.sample_rate = 44100;
  config.channels = 2;
  config.bits_per_sample = 16;
  i2s.begin(config);

  auto vconfig = volume.defaultConfig();
  volume.begin(vconfig);
  volume.setVolume(VOLUME);

  mp3decode.begin();

  icystream.setMetadataCallback(callbackMetadata);  // before begin!
  icystream.begin(url);

  Serial.println("Audio stream started");
}

void loop() {
  copier.copy();
}
