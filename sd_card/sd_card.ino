#include "SD.h"
#include "SPI.h"

#define SD_CS   2
#define SPI_MOSI 5
#define SPI_MISO 4
#define SPI_SCK  6

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // wait for Serial to initialize

  Serial.println("\n--- SD Card Interface Test ---");

  // Initialize SPI with C6 specific pins
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);

  Serial.print("Mounting SD Card... ");
  if (!SD.begin(SD_CS)) {
    Serial.println("FAILED!");
    Serial.println("Check: 1. Wiring | 2. Card is FAT32 | 3. Voltage is 3.3V");
    return;
  }
  Serial.println("SUCCESS!");

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) Serial.println("MMC");
  else if (cardType == CARD_SD) Serial.println("SDSC");
  else if (cardType == CARD_SDHC) Serial.println("SDHC");
  else Serial.println("UNKNOWN");

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  // List files on the root
  File root = SD.open("/");
  Serial.println("Files on card:");
  while (File file = root.openNextFile()) {
    Serial.print("  - ");
    Serial.print(file.name());
    Serial.print("  Size: ");
    Serial.println(file.size());
    file.close();
  }
}

void loop() {}
