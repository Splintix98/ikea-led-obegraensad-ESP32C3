# Tenstar Robot ESP32-C3 SuperMini Plus – LLM-Wissensbasis

Diese Markdown-Datei fasst die relevanten technischen Informationen aus den bereitgestellten Tenstar-Robot-/AliExpress-Produktunterlagen zum **ESP32-C3 SuperMini Plus** zusammen. Sie ist als kompakte Wissensquelle für spätere LLM-Kontexte gedacht.

## Quellencharakter und Vorsichtshinweise

- Die Informationen stammen aus Hersteller-/Händlergrafiken und Beispielcode der Produktseite.
- Mehrere Angaben sind offensichtlich übersetzt und teilweise unpräzise. Kritische elektrische Angaben sollten vor produktivem Einsatz gegen das offizielle Espressif-Datenblatt, das Board-Schematic und Messungen geprüft werden.
- Die Dokumentation nennt teilweise **11 digitale I/Os/PWM** und **4 ADC-Eingänge**, zeigt im Pinout aber auch Labels **A0 bis A5**. Diese Diskrepanz sollte nicht ungeprüft übernommen werden.
- Die Beispiele verwenden Arduino-IDE-/Arduino-ESP32-Konventionen. Bei neueren Arduino-ESP32-Core-Versionen können APIs oder Board-Optionen leicht abweichen.

---

## 1. Board-Überblick

**Board:** Tenstar Robot ESP32-C3 SuperMini Plus  
**Chip:** Espressif ESP32-C3 / ESP32C3FN4  
**CPU:** 32-bit RISC-V Single-Core, bis 160 MHz  
**FPU:** 32-bit Single-Precision Floating Point  
**Funk:** 2.4 GHz WiFi 802.11 b/g/n, Bluetooth 5 LE  
**Speicher laut Produktangabe:** 400 KB SRAM, 384 KB ROM, 4 MB Flash  
**Bauform:** sehr klein, ca. 22.5 mm × 18 mm, einseitige Bestückung  
**Low-Power-Angabe:** Deep Sleep ca. 43 µA laut Produktseite

### Variante: SuperMini C3 vs. SuperMini C3 Plus

| Variante | Merkmale laut Produktgrafik |
|---|---|
| SuperMini C3 | Onboard LED |
| SuperMini C3 Plus | FPC-Antenne, Antennenanschluss, RGB-LED |

Hinweis: Der Beispielcode aus den Unterlagen nutzt **GPIO8** als LED-Pin. Bei der Plus-Variante wird zusätzlich eine RGB-LED erwähnt, aber kein separater RGB-LED-Beispielcode bereitgestellt.

---

## 2. Schnittstellen und Funktionen

Laut Produktunterlagen unterstützt das Board:

- WiFi 802.11 b/g/n, 2.4 GHz
- WiFi Station Mode
- WiFi SoftAP Mode
- WiFi SoftAP + Station Mode
- Bluetooth 5.0 / Bluetooth LE
- UART
- I2C
- SPI
- PWM auf digitalen GPIOs
- ADC auf analogen Eingängen
- USB-C-Anschluss für Stromversorgung, Flashing und USB-Serial
- BOOT-Taster für Download-/Bootloader-Modus
- RESET-Taster

---

## 3. Pinout und Arduino-Pin-Mapping

### Wichtige Pins aus der Dokumentation

| Funktion / Label | Arduino-/Board-Pin | GPIO / Mapping |
|---|---:|---:|
| UART TX | TX | GPIO21 |
| UART RX | RX | GPIO20 |
| I2C SDA | SDA | GPIO8 |
| I2C SCL | SCL | GPIO9 |
| SPI SS | SS | GPIO7 |
| SPI MOSI | MOSI | GPIO6 |
| SPI MISO | MISO | GPIO5 |
| SPI SCK | SCK | GPIO4 |
| Analog A0 | A0 | GPIO0 |
| Analog A1 | A1 | GPIO1 |
| Analog A2 | A2 | GPIO2 |
| Analog A3 | A3 | GPIO3 |
| Analog A4 | A4 | GPIO4 |
| Analog A5 | A5 | GPIO5 |
| Onboard-LED-Beispiel | LED | GPIO8 |
| Versorgung | 5V | 5V / externer Versorgungseingang laut Produktseite |
| Versorgung | 3V3 | 3.3 V |
| Masse | GND | GND |

### Arduino-Konstanten aus der Produktunterlage

```cpp
static const uint8_t TX   = 21;
static const uint8_t RX   = 20;

static const uint8_t SDA  = 8;
static const uint8_t SCL  = 9;

static const uint8_t SS   = 7;
static const uint8_t MOSI = 6;
static const uint8_t MISO = 5;
static const uint8_t SCK  = 4;

static const uint8_t A0   = 0;
static const uint8_t A1   = 1;
static const uint8_t A2   = 2;
static const uint8_t A3   = 3;
static const uint8_t A4   = 4;
static const uint8_t A5   = 5;
```

### Relevante Warnung zum Pin-Mapping

Die Dokumentation erklärt, dass A0–A5, GPIO0–GPIO10 und D-/Board-Pin-Bezeichnungen teils nur Arduino-Mapping-/Komfortlabels sind. Für robuste Projekte sollte man explizit prüfen, ob ein Codebeispiel Arduino-Pinnummern, GPIO-Nummern oder Board-Silk-Labels verwendet.

---

## 4. Stromversorgung und Hardware-Setup

### Externe Stromversorgung

Laut Produktunterlage:

- Externe Versorgung: Pluspol an **5V**, Minuspol an **GND**.
- Unterstützter Bereich laut Text: **3.3 V bis 6 V**.
- USB und externe Stromversorgung sollen nicht gleichzeitig verwendet werden: **entweder USB oder externe Versorgung**.
- Beim Löten/Kontaktieren dürfen Plus und Minus nicht kurzgeschlossen werden, da sonst Batterie und Board beschädigt werden können.

### USB-Kabel

Für Setup, Flashing und Serial Monitor wird ein **USB-Type-C-Datenkabel** benötigt. Reine Ladekabel funktionieren nicht, weil sie keine Datenleitungen haben.

### Benötigte Hardware für die Basisbeispiele

- 1× ESP32-C3 SuperMini / SuperMini Plus
- 1× Computer
- 1× USB-Type-C-Datenkabel

### WiFi-Antenne

Die Produktgrafik unterscheidet bei der Antenne zwischen:

- Shielding layer
- Wire core

Bei der SuperMini-C3-Plus-Variante wird eine FPC-Antenne mit Antenneninterface gezeigt.

---

## 5. Arduino-IDE-Setup

### Arduino-ESP32 Board Package installieren

1. Arduino IDE installieren und starten.
2. In Arduino IDE öffnen: `File` → `Preferences`.
3. Bei **Additional Boards Manager URLs** diese URL eintragen:

```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

4. Öffnen: `Tools` → `Board` → `Boards Manager...`.
5. Nach `esp32` suchen.
6. Paket **esp32 by Espressif Systems** installieren.

### Board auswählen

In Arduino IDE:

```text
Tools → Board → ESP32 → ESP32C3 Dev Module
```

Die Liste ist lang; laut Anleitung muss man ggf. nach unten scrollen.

### Port auswählen

In Arduino IDE:

```text
Tools → Port → <COM-Port des Boards>
```

Beispiel aus der Anleitung: `COM3` oder höher. `COM1` und `COM2` sind unter Windows typischerweise für klassische Hardware-Serial-Ports reserviert.

### Beispielhafte IDE-Optionen aus der Produktgrafik

| Option | Wert aus der Produktgrafik |
|---|---|
| Board | ESP32C3 Dev Module |
| Upload Speed | 921600 |
| USB CDC On Boot | Enabled oder Disabled je nach Serial-Modus, siehe Abschnitt „Serial“ |
| CPU Frequency | 160 MHz (WiFi) |
| Flash Frequency | 80 MHz |
| Flash Mode | QIO |
| Flash Size | 4MB (32Mb) |
| Partition Scheme | Default 4MB with spiffs (1.2MB APP / 1.5MB SPIFFS) |
| Core Debug Level | None |

---

## 6. Serial / UART

Das Board hat laut Produktunterlage zwei Serial-Wege:

1. **USB serial port** über USB-C
2. **UART serial port** über externe USB-Serial-Adapter-Verbindung

### USB-Serial

Standardmäßig ist USB Serial aktiviert. Dann kann man das Board per USB-C mit dem PC verbinden und Daten im Arduino Serial Monitor lesen.

Für USB-Serial-Druckausgaben im Arduino Serial Monitor muss **USB CDC On Boot = Enabled** gesetzt sein, wenn sonst keine Ausgabe erscheint.

### Externer UART über USB-Serial-Adapter

Wenn statt USB-CDC der Hardware-UART genutzt werden soll:

- Pin **20** als TX verwenden.
- Pin **21** als RX verwenden.
- USB-Serial-Adapter anschließen.
- In Arduino IDE **USB CDC On Boot = Disabled** setzen.

Achtung: Die Produktunterlage formuliert „pin 20 as TX and pin 21 as RX“, während die Arduino-Konstanten `TX = 21` und `RX = 20` zeigen. Für eine reale UART-Verdrahtung muss die Richtung beachtet werden: TX des einen Gerätes geht auf RX des anderen Gerätes. Das Pinout vor dem Anschluss prüfen.

---

## 7. Troubleshooting

### COM-Port wird in der Arduino IDE nicht erkannt

Download-/Bootloader-Modus erzwingen:

1. Methode 1: **BOOT** gedrückt halten und Board einschalten/anstecken.
2. Methode 2: **BOOT** gedrückt halten, **RESET** drücken, **RESET** loslassen, danach **BOOT** loslassen.

Die Anleitung weist darauf hin, dass der Download-Modus je Verbindung ggf. erneut aktiviert werden muss. Bei instabiler Porterkennung kann der Windows-Port-Erkennungston als Hinweis dienen.

### Programm startet nach Upload nicht

Nach erfolgreichem Upload den **RESET**-Button drücken, damit das Programm startet.

### Serial Monitor zeigt keine Ausgabe

Für USB-Serial-Ausgabe:

```text
Tools → USB CDC On Boot → Enabled
```

Für externen UART-Betrieb kann dagegen **Disabled** erforderlich sein.

---

## 8. I2C und 0.96-Zoll-OLED

### Verdrahtung laut Produktunterlage

| ESP32C3SuperMini | 0.96 OLED |
|---|---|
| 5V | VCC |
| GND | GND |
| SCL | SCL |
| SDA | SDA |

### Arduino-Mapping

- `SDA = GPIO8`
- `SCL = GPIO9`

### Bibliothek

Für das OLED-Beispiel soll in der Arduino IDE die Bibliothek **u8g2** installiert werden:

```text
Sketch → Include Library → Manage Libraries... → Suche nach "u8g2" → Installieren
```

Die Produktunterlage enthält kein vollständiges OLED-Beispielprogramm, sondern nur die Anschluss-/Installationsschritte.

---

## 9. LED- und GPIO-Beispiele

Die Beispiele nutzen **GPIO8** als LED-Pin.

### Digital Pin: Onboard-LED blinken lassen

```cpp
// Upload the code to the board, and the on-board LED will light up every second.

// define led according to pin diagram
int led = 8;

void setup() {
  // initialize digital pin led as an output
  pinMode(led, OUTPUT);
}

void loop() {
  digitalWrite(led, HIGH); // turn the LED on
  delay(1000);             // wait for a second
  digitalWrite(led, LOW);  // turn the LED off
  delay(1000);             // wait for a second
}
```

### Digital PWM: LED dimmen

```cpp
// Upload the following code to see the on-board LED gradually dim.

int ledPin = 8; // LED connected to GPIO8 according to the pin diagram

void setup() {
  // declaring LED pin as output
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // fade in from min to max in increments of 5 points:
  for (int fadeValue = 0; fadeValue <= 255; fadeValue += 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }

  // fade out from max to min in increments of 5 points:
  for (int fadeValue = 255; fadeValue >= 0; fadeValue -= 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
}
```

### Analog Pin: Potentiometer steuert Blinkintervall

```cpp
// Connect the potentiometer to pin A5 and upload the following code
// to control the flashing interval of the LED by turning the potentiometer knob.

const int sensorPin = A5;
const int ledPin = 8;

void setup() {
  pinMode(sensorPin, INPUT); // declare the sensorPin as an INPUT
  pinMode(ledPin, OUTPUT);   // declare the ledPin as an OUTPUT
}

void loop() {
  // read the value from the sensor:
  int sensorValue = analogRead(sensorPin);

  // turn the ledPin on
  digitalWrite(ledPin, HIGH);

  // stop the program for <sensorValue> milliseconds:
  delay(sensorValue);

  // turn the ledPin off:
  digitalWrite(ledPin, LOW);

  // stop the program for <sensorValue> milliseconds:
  delay(sensorValue);
}
```

---

## 10. WiFi-Funktionen

### Allgemeines

Das Board kann per Arduino-ESP32-WiFi-Library als Station, Access Point oder kombiniert betrieben werden.

---

### WiFi-Netzwerke scannen – Station Mode

Zweck: ESP32-C3 SuperMini scannt verfügbare WiFi-Netzwerke in der Umgebung.

```cpp
#include "WiFi.h"
 
void setup()
{
  Serial.begin(115200);
 
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
 
  Serial.println("Setup done");
}
 
void loop()
{
  Serial.println("scan start");
 
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");

  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");

    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }

  Serial.println("");
 
  // Wait a bit before scanning again
  delay(5000);
}
```

Beispielausgabe aus der Dokumentation:

```text
16:07:02.401 -> Setup done
16:07:02.401 -> scan start
16:07:02.401 -> scan done
16:07:02.401 -> 4 networks found
16:07:02.401 -> 1: 2nd_Floor_V1 (-64)*
16:07:02.401 -> 2: 2nd_Floor_V2 (-71)*
16:07:02.401 -> 3: Dialog 4G 034 (-83)*
16:07:02.401 -> 4: walawedura (-88)*
```

---

### Mit WiFi-Netzwerk verbinden

Zweck: ESP32-C3 verbindet sich als Station mit einem bestehenden WLAN.

```cpp
#include <WiFi.h>
 
const char* ssid = "your-ssid";       // your WiFi name
const char* password = "your-password"; // your WiFi password
 
void setup()
{
  Serial.begin(115200);
  delay(10);
 
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
}
```

Beispielausgabe aus der Dokumentation:

```text
16:11:08.316 -> Connecting to 2nd_Floor_V1
16:11:08.316 -> ....
16:11:08.316 -> WiFi connected
16:11:08.316 -> IP address:
16:11:08.316 -> 192.168.2.116
```

---

### WiFi-Hotspot / SoftAP

Zweck: ESP32-C3 SuperMini als WiFi Access Point, vergleichbar mit einem Smartphone-Hotspot.

```cpp
#include "WiFi.h"

void setup()
{
  Serial.begin(115200);
  WiFi.softAP("ESP_AP", "123456789");
}

void loop()
{
  Serial.print("Host Name:");
  Serial.println(WiFi.softAPgetHostname());

  Serial.print("Host IP:");
  Serial.println(WiFi.softAPIP());

  Serial.print("Host IPV6:");
  Serial.println(WiFi.softAPIPv6());

  Serial.print("Host SSID:");
  Serial.println(WiFi.SSID());

  Serial.print("Host Broadcast IP:");
  Serial.println(WiFi.softAPBroadcastIP());

  Serial.print("Host mac Address:");
  Serial.println(WiFi.softAPmacAddress());

  Serial.print("Number of Host Connections:");
  Serial.println(WiFi.softAPgetStationNum());

  Serial.print("Host Network ID:");
  Serial.println(WiFi.softAPNetworkID());

  Serial.print("Host Status:");
  Serial.println(WiFi.status());

  delay(1000);
}
```

Beispielausgabe aus der Dokumentation:

```text
16:17:41.918 -> Host Name:espressif
16:17:41.918 -> Host IP:192.168.4.1
16:17:41.918 -> Host IPV6:0000:0000:0000:0000:0000:0000:0000:0000
16:17:41.918 -> Host SSID:
16:17:41.918 -> Host Broadcast IP:192.168.4.255
16:17:41.918 -> Host mac Address:10:91:A8:03:5A:1D
16:17:41.918 -> Number of Host Connections:0
16:17:41.918 -> Host Network ID:192.168.4.0
16:17:41.918 -> Host Status:255
```

---

## 11. Bluetooth-Funktionen

### Bluetooth-Geräte scannen

Zweck: ESP32-C3 SuperMini scannt Bluetooth-LE-Geräte in der Umgebung.

```cpp
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; // In seconds
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); // create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); // active scan uses more power, but gets results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults(); // delete results from BLEScan buffer to release memory
  delay(2000);
}
```

Beispielausgabe aus der Dokumentation:

```text
16:39:45.491 -> Scanning...
16:39:45.491 -> Advertised Device: Name: , Address: 03:da:08:60:42:a7, ...
16:39:45.491 -> Advertised Device: Name: , Address: 41:9f:75:89:e0:4c, ...
16:39:45.491 -> Advertised Device: Name: HUAWEI Band 6-AB2, Address: 74:45:2d:bd:9a:...
16:39:47.881 -> Devices found: 4
16:39:47.881 -> Scan done!
```

---

### ESP32-C3 als BLE-Server

Zweck: Smartphone sucht das Board als BLE-Gerät, verbindet sich damit und schreibt einen String in eine BLE-Characteristic. Der ESP32 gibt den empfangenen String auf dem Serial Monitor aus.

BLE-Gerätename im Beispiel:

```text
MyESP32
```

Service UUID:

```text
4fafc201-1fb5-459e-8fcc-c5c9c331914b
```

Characteristic UUID:

```text
beb5483e-36e1-4688-b7f5-ea07361b26a8
```

Code:

```cpp
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.println("*********");
      Serial.print("New value: ");

      for (int i = 0; i < value.length(); i++) {
        Serial.print(value[i]);
      }

      Serial.println();
      Serial.println("*********");
    }
  }
};

void setup() {
  Serial.begin(115200);

  BLEDevice::init("MyESP32");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello World");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() {
  delay(2000);
}
```

### Smartphone-Test mit LightBlue-App

Ablauf laut Anleitung:

1. Code hochladen und Serial Monitor öffnen.
2. LightBlue-App auf dem Smartphone installieren.
3. Bluetooth am Smartphone aktivieren und Smartphone nahe an den ESP32-C3 SuperMini legen.
4. Gerät **MyESP32** suchen.
5. In LightBlue zum Tab **Bonded** wechseln.
6. Neben **MyESP32** auf **CONNECT** klicken/tippen.
7. Unten die Characteristic mit **Readable, Writable** auswählen.
8. Im Dropdown **Data format** den Wert **UTF-8 String** auswählen.
9. Unter **WRITTEN VALUES** den Text `Hello` eintragen.
10. Auf **WRITE** klicken/tippen.
11. Im Arduino Serial Monitor erscheint der empfangene String.

Erwartete Serial-Ausgabe:

```text
22:51:29.991 -> *********
22:51:29.991 -> New value: Hello
22:51:29.991 -> *********
```

---

## 12. SoftwareSerial

Die Produktunterlage sagt:

- Für zusätzliche serielle Ports soll die **SoftwareSerial**-Library genutzt werden.
- Es wird kein konkretes SoftwareSerial-Beispiel bereitgestellt.

Achtung: Auf ESP32/ESP32-C3 ist Hardware-UART üblicherweise stabiler als SoftwareSerial. Vor produktivem Einsatz prüfen, ob SoftwareSerial auf der verwendeten Arduino-ESP32-Core-Version für ESP32-C3 sinnvoll unterstützt wird.

---

## 13. ChatGPT-/HTTP-Client-Beispielidee

Die Produktunterlage beschreibt nur ein Konzept, keinen vollständigen Code:

1. ESP32-C3 SuperMini mit dem Netzwerk verbinden.
2. Eingebettete Webseite auf dem ESP32 bereitstellen.
3. Frage über die eingebaute Webseite senden.
4. ESP32 zeichnet die Frage auf.
5. ESP32 sendet per HTTP-Client eine Anfrage an eine OpenAI-/ChatGPT-API.
6. Antwort wird empfangen und über Serial ausgegeben.

Wichtig: Die Produktunterlage enthält keine aktuelle, vollständige oder sicher nutzbare API-Implementierung. Für reale Nutzung müssen Authentifizierung, HTTPS/TLS, API-Endpunkt, Request-/Response-Format, Token-/Kostenkontrolle und Geheimnisschutz separat korrekt implementiert werden.

---

## 14. Minimaler Arbeitsablauf für erste Inbetriebnahme

1. USB-C-Datenkabel verwenden, kein reines Ladekabel.
2. Arduino IDE installieren.
3. Espressif Board Manager URL hinzufügen.
4. Paket `esp32 by Espressif Systems` installieren.
5. Board `ESP32C3 Dev Module` wählen.
6. Passenden COM-Port wählen.
7. Für USB-Serial: `USB CDC On Boot = Enabled` setzen.
8. Blink-Beispiel mit `int led = 8;` hochladen.
9. Falls Upload/Port scheitert: BOOT/RESET-Sequenz für Download-Modus verwenden.
10. Nach erfolgreichem Upload ggf. RESET drücken.
11. Serial Monitor auf `115200` Baud stellen, wenn Beispiele `Serial.begin(115200)` verwenden.

---

## 15. Kompakte Faktenliste für LLM-RAG

- Tenstar Robot ESP32-C3 SuperMini Plus basiert auf Espressif ESP32-C3 / ESP32C3FN4.
- CPU: 32-bit RISC-V Single-Core bis 160 MHz.
- Funk: WiFi 2.4 GHz 802.11 b/g/n und Bluetooth 5 LE.
- Flash laut Produktseite: 4 MB.
- Boardgröße laut Produktgrafik: ca. 22.5 mm × 18 mm.
- SuperMini-C3-Plus-Merkmale laut Vergleich: FPC-Antenne, Antenneninterface, RGB-LED.
- Beispielcode nutzt GPIO8 als LED-Pin.
- Arduino-Boardauswahl: `ESP32C3 Dev Module`.
- Board Manager URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`.
- USB-Serial: `USB CDC On Boot = Enabled`, falls Serial Monitor nichts ausgibt.
- Externer UART laut Doku: Pin 20/21 verwenden und `USB CDC On Boot = Disabled`; Pinrichtung TX/RX vor Verdrahtung prüfen.
- Arduino-Konstanten laut Doku: `TX=21`, `RX=20`, `SDA=8`, `SCL=9`, `SS=7`, `MOSI=6`, `MISO=5`, `SCK=4`, `A0=0`, `A1=1`, `A2=2`, `A3=3`, `A4=4`, `A5=5`.
- I2C-OLED-Verdrahtung: `5V→VCC`, `GND→GND`, `SCL→SCL`, `SDA→SDA`; Bibliothek `u8g2` installieren.
- WiFi-Beispiele: Netzwerke scannen, mit WLAN verbinden, SoftAP/Hotspot betreiben.
- BLE-Beispiele: BLE-Scan und BLE-Server mit read/write Characteristic.
- BLE-Server-Beispielname: `MyESP32`.
- BLE-Service-UUID: `4fafc201-1fb5-459e-8fcc-c5c9c331914b`.
- BLE-Characteristic-UUID: `beb5483e-36e1-4688-b7f5-ea07361b26a8`.
- Troubleshooting: BOOT beim Einstecken halten oder BOOT halten → RESET drücken → RESET loslassen → BOOT loslassen, um Download-Modus zu erzwingen.
- Nach Upload ggf. RESET drücken, wenn Programm nicht startet.
