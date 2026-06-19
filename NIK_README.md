# NIK README

Kompakte Projektnotizen fuer Niklas' OBEGRAENSAD-Umbau.

## Ziel

IKEA OBEGRAENSAD LED-Panel ohne originalen Controller betreiben. Der originale Controller wurde entfernt bzw. ersetzt. Als Controller wird aktuell ein ESP32-C3 SuperMini Plus mit Arduino/PlatformIO verwendet.

## Hardware

- IKEA OBEGRAENSAD LED-Panel
- ESP32-C3 SuperMini Plus
- SN74AHCT125N Level Shifter, abgekuerzt `LS`
- 84 nF Keramikkondensator als Abblockkondensator am Level Shifter
- LD2410 Presence Sensor, nur `OUT`-Pin genutzt
- Panel-Button an ESP GPIO5 gegen GND

## Warum Level Shifter

Der ESP32-C3 liefert 3.3V-Logik, das Panel wird mit 5V betrieben. Ohne Level Shifter gab es falsche/flackernde Pixel. Mit SN74AHCT125N ist das Panel stabil. Der Level Shifter wird mit 5V versorgt und wandelt die ESP-Signale fuer das Panel auf 5V-Pegel.

Abblockkondensator:

```text
84 nF Keramikkondensator direkt zwischen LS Pin 14 VCC und LS Pin 7 GND
```

## Anschlussbelegung

### Versorgung / Masse / Enables

```text
LS Pin 14 VCC + Panel 5V + ESP 5V + LD2410 5V

LS Pin 7 GND
+ Panel GND
+ ESP GND
+ LS Pin 1  1OE negiert
+ LS Pin 4  2OE negiert
+ LS Pin 10 3OE negiert
+ LS Pin 13 4OE negiert
+ Panel Button 2
+ LD2410 GND
```

Alle `OE`-Pins des SN74AHCT125N sind aktiv LOW und werden deshalb dauerhaft auf GND gelegt.

### ESP zum Level Shifter

```text
ESP GPIO0 -> LS Pin 2  1A
ESP GPIO6 -> LS Pin 5  2A
ESP GPIO4 -> LS Pin 9  3A
ESP GPIO7 -> LS Pin 12 4A
```

### Level Shifter zum Panel

```text
LS Pin 3  1Y -> Panel EN
LS Pin 6  2Y -> Panel IN
LS Pin 8  3Y -> Panel CLK
LS Pin 11 4Y -> Panel CLA
```

### Button

```text
Panel Button 1 -> ESP GPIO5
Panel Button 2 -> GND
```

### LD2410 Presence Sensor

```text
LD2410 5V  -> 5V
LD2410 GND -> GND
LD2410 OUT -> ESP GPIO10
```

Nur der digitale `OUT`-Pin wird genutzt. UART des LD2410 bleibt frei.

## Firmware-Konfiguration

Wichtige Dateien:

```text
platformio.ini
include/constants.h
src/main.cpp
src/screen.cpp
Plan.md
```

Aktuelle Zielumgebung:

```ini
[platformio]
default_envs = esp32c3
```

ESP32-C3 Pinbelegung in `include/constants.h`:

```cpp
#if defined(CONFIG_IDF_TARGET_ESP32C3)

#define PIN_ENABLE 0
#define PIN_DATA   6
#define PIN_CLOCK  4
#define PIN_LATCH  7
#define PIN_BUTTON 5
#define PIN_PRESENCE -1

#endif
```

Hinweis: `PIN_PRESENCE` ist aktuell noch deaktiviert. Fuer LD2410 OUT spaeter auf `10` setzen:

```cpp
#define PIN_PRESENCE 10
```

In `src/main.cpp` wird Presence nur initialisiert, wenn `PIN_PRESENCE >= 0`.

## SPI / Display

Display-Ansteuerung erfolgt in `src/screen.cpp` ueber SPI:

```cpp
SPI.begin(PIN_CLOCK, -1, PIN_DATA, -1);
SPI.beginTransaction(SPISettings(SCREEN_SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
```

Aktuelle SPI-Frequenz:

```cpp
#define SCREEN_SPI_FREQUENCY 10000000
```

Tests mit 1 MHz und 4 MHz waren nicht zielfuehrend; bei zu niedriger Frequenz kann der Display-Refresh den ESP32-C3/WiFi-Webserver stoeren, weil der SPI-Transfer zu lange dauert.

## Panel-Testmodus

Es gibt einen Barebones-Testmodus in `include/constants.h`:

```cpp
#define PANEL_TEST_MODE 0
```

Wert:

```text
0 = normaler Betrieb mit WiFi/Webserver/Plugins
1 = Barebones Paneltest ohne WiFi/Webserver/Plugins
```

Der Testmodus zeigt:

- Full-panel blink
- 5-Pixel-Schlange ueber das gesamte Panel

Der Modus ist aktuell deaktiviert.

## Build

PlatformIO liegt lokal hier:

```powershell
C:\Users\Niklas\.platformio\penv\Scripts\platformio.exe
```

Build:

```powershell
C:\Users\Niklas\.platformio\penv\Scripts\platformio.exe run -e esp32c3
```

Hinweis: In der Codex-Sandbox braucht dieser Build Freigabe, weil PlatformIO globale Lock-/Cache-Dateien unter `C:\Users\Niklas\.platformio` schreibt.

## WiFi / Persistenz

Beim normalen Flashen werden WiFi-Credentials und andere gespeicherte Daten nicht automatisch geloescht. Sie liegen im NVS/Preferences-Bereich.

Komplett loeschen:

```powershell
C:\Users\Niklas\.platformio\penv\Scripts\platformio.exe run -e esp32c3 -t erase
```

Danach Firmware neu flashen.

## Offene Punkte

- LD2410 OUT anschliessen und `PIN_PRESENCE` auf `10` setzen.
- Presence-Logik mit Glitch-/Entprellfilter umsetzen.
- Splashscreen/WiFi-Icon aus `Plan.md` priorisieren.
- Spaeter ggf. Remote-Konfiguration oder Remote-Pattern aus GitHub umsetzen.
