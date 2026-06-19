# Plan: Splashscreens, Fork, Plugin- und Remote-Konfiguration

Stand: 2026-06-19

## Ausgangslage im Projekt

- Zielplattform ist aktuell `esp32c3` in `platformio.ini`.
- Das Display wird ueber `Screen` in `src/screen.cpp` gesteuert. Der sichtbare Inhalt liegt im 16x16-Renderbuffer und bleibt stabil, bis neuer Inhalt in diesen Buffer geschrieben wird.
- Auf ESP32 laeuft das Zeichnen in einem separaten Task (`screenDrawingTask`). Dieser Task ruft derzeit erst nach `baseSetup()` `Screen.setup()` auf.
- `baseSetup()` initialisiert zuerst Pins und Konfiguration, startet dann bei `ENABLE_SERVER` den WiFiManager (`connectToWiFi()`), danach Webserver/OTA und erst danach Plugins/Scheduler.
- Die Plugins sind aktuell keine extern ladbaren Module. Sie sind C++-Klassen, werden per `#include` in `src/main.cpp` eingebunden, kompiliert und beim Boot mit `pluginManager.addPlugin(new ...)` registriert.
- Es gibt bereits lokale Konfiguration (`Config` in `src/config.cpp`) und Web-Endpunkte unter `/api/config`. Diese Konfiguration liegt in `Preferences`, nicht in einer Datei auf SPIFFS.
- Die Partitionstabelle enthaelt OTA-Slots und 256 KB SPIFFS. OTA ist also grundsaetzlich vorgesehen, der Platz fuer herunterladbare Daten ist aber begrenzt.

## 1. Splashscreen beim Boot

### Wunsch

Beim Einschalten soll ein starres Willkommensbild angezeigt werden, solange der ESP startet, mindestens aber 5 Sekunden.

### Bewertung

Gut machbar. Das Panel ist fuer diesen Anwendungsfall geeignet, weil der Inhalt durch den Renderbuffer stabil stehen bleibt. Wichtig ist aber die Startreihenfolge: Auf ESP32 wird `Screen.setup()` im aktuellen Code erst nach `baseSetup()` gestartet. Da `baseSetup()` den WiFiManager blockierend ausfuehren kann, waere ein Splashscreen waehrend WiFiManager aktuell zu spaet.

### Empfohlene Umsetzung

1. Display sehr frueh initialisieren:
   - `Screen.setup()` vor `connectToWiFi()` ausfuehren.
   - Sicherstellen, dass `Screen.setup()` nur einmal pro Boot aufgerufen wird, damit der Timer nicht doppelt initialisiert wird.
2. Kleine Splashscreen-Hilfe einfuehren:
   - neue Dateien z. B. `include/splash.h` und `src/splash.cpp`
   - Funktionen wie `Splash.showBoot()`, `Splash.showWifiPortal()`, `Splash.showError()`
   - intern nur `Screen.clear()`, `Screen.setPixel(...)`, vorhandene `drawCharacter(...)`/`readBytes(...)` oder feste 16x16-Bitmaps verwenden.
3. Mindestdauer von 5 Sekunden sauber steuern:
   - Boot-Zeitpunkt merken: `unsigned long splashStartedAt = millis();`
   - Nach Abschluss der Initialisierung warten, bis `millis() - splashStartedAt >= 5000`.
   - Dabei keine lange Busy-Wait-Schleife ohne `delay(1)`/`vTaskDelay(...)` verwenden.
4. Plugins erst nach der Splashscreen-Phase aktivieren:
   - `pluginManager.init()` darf den Splashscreen nicht sofort ueberschreiben.
   - Entweder `pluginManager.init()` nach der Mindestdauer ausfuehren oder waehrend Splash `currentStatus = LOADING` setzen.

### Risiken / Hinweise

- `currentStatus = LOADING` verhindert aktuell, dass aktive Plugins zeichnen. Fuer Splashscreens passt das gut.
- `Screen.loadFromStorage()` sollte nicht unkontrolliert direkt nach dem Splash laufen, wenn das alte Bild nicht wiederhergestellt werden soll.
- Die aktuelle Rotation wird in `Screen.setup()` aus Storage geladen. Splash-Bitmaps sollten deshalb entweder mit Rotation funktionieren oder bewusst unrotiert angezeigt werden.

## 2. WiFiManager-Icon

### Wunsch

Wenn der WiFiManager startet, soll ein WiFi-Icon angezeigt werden.

### Bewertung

Sehr gut machbar, aber der richtige Hook ist entscheidend. Im Code gibt es bereits:

```cpp
wifiManager.setWebServerCallback([&wifiWebServerStarted]() { wifiWebServerStarted = true; });
```

Dieser Callback signalisiert, dass der Config-Portal-Webserver gestartet wurde. Dort kann zusaetzlich `Splash.showWifiPortal()` aufgerufen werden.

### Empfohlene Umsetzung

1. In `connectToWiFi()` vor `wifiManager.autoConnect(...)` einen Boot-/Connecting-Zustand anzeigen.
2. Im `setWebServerCallback` das WiFi-Icon anzeigen.
3. Nach erfolgreicher Verbindung optional kurz ein OK-Symbol oder direkt den normalen Boot-Splash weiter stehen lassen.
4. Wenn der WiFiManager-Server aktiv war, rebootet der Code bereits. Das ist sinnvoll, weil laut Kommentar sonst der WiFiManager-Webserver mit dem eigenen Server interferiert.

### Risiken / Hinweise

- `wifiManager.autoConnect(...)` ist blockierend. Das Icon muss vor oder im WiFiManager-Callback gesetzt werden.
- Wenn kein WLAN vorhanden ist, kann das Portal bis zu `setConfigPortalTimeout(180)` Sekunden aktiv sein. Das Icon bleibt in dieser Zeit stabil stehen.

## 3. Plugin-Nummer nur temporaer anzeigen

Status: erledigt.

### Wunsch

Beim Auswaehlen eines Plugins soll die Plugin-Nummer fuer 2 Sekunden angezeigt werden. Danach soll das Panel einmal komplett blank refresht werden. Erst danach soll der Content des gewaehlten Plugins sichtbar werden.

### Recherche / Ist-Zustand

Die Anzeige der Nummer passiert aktuell in `PluginManager::renderPluginId(int pluginId)`:

```cpp
Screen.clear();
Screen.drawNumbers(...);
while (millis() - startTime < 800) { ... }
```

Beim Wechsel setzt `PluginManager::setActivePlugin(...)` bereits `currentStatus = LOADING`, ruft `renderPluginId(...)` auf, setzt danach `currentStatus = NONE` und startet `activePlugin->setup()`.

Das erklaert das Stoerverhalten: `Screen.clear()` loescht nur den Renderbuffer vor dem Zeichnen der Nummer. Nach der Nummer wird der Buffer nicht erneut geleert. Wenn das neue Plugin in `setup()` oder den ersten `loop()`-Durchlaeufen nicht alle 256 Pixel ueberschreibt, bleiben die gesetzten Pixel der Nummer sichtbar, bis ein Plugin sie irgendwann explizit ueberschreibt.

### Bewertung

Sehr gut machbar und architektonisch an der richtigen Stelle loesbar. Die zentrale Stelle ist `PluginManager::renderPluginId(...)`, nicht jedes einzelne Plugin. Dadurch muessen Plugins nicht kuenstlich lernen, alte Plugin-ID-Pixel zu loeschen.

### Empfohlene Umsetzung

1. Anzeigezeit von 800 ms auf 2000 ms erhoehen.
2. Nach der Wartezeit den Renderbuffer leeren:
   - `Screen.clear();`
3. Danach eine kurze Blank-Refresh-Zeit abwarten, bevor `currentStatus` wieder auf `NONE` gesetzt wird:
   - z. B. 50-100 ms mit `vTaskDelay(pdMS_TO_TICKS(...))` auf ESP32 bzw. `delay(...)` auf ESP8266.
   - Der Screen-Timer rendert in dieser Zeit den leeren Buffer mindestens einmal sichtbar auf das Panel.
4. Erst danach darf das neue Plugin zeichnen:
   - `currentStatus` bleibt waehrend ID-Anzeige und Blank-Refresh auf `LOADING`.
   - Danach `currentStatus = NONE` und `activePlugin->setup()`.
5. Die Zeiten als Konstanten definieren:
   - `PLUGIN_ID_DISPLAY_MS = 2000`
   - `PLUGIN_ID_BLANK_REFRESH_MS = 75`

### Moeglicher Zielablauf

```text
Pluginwechsel
-> currentStatus = LOADING
-> alte Plugin-Loop stoppt
-> Screen.clear()
-> Plugin-ID zeichnen
-> 2000 ms warten
-> Screen.clear()
-> 75 ms blank refresh warten
-> currentStatus = NONE
-> activePlugin->setup()
-> activePlugin->loop() zeichnet neuen Content
```

### Risiken / Hinweise

- Der Blank-Refresh sollte bewusst kurz sein. Zu lang wirkt der Pluginwechsel traege, zu kurz kann das Panel je nach Refresh-Timing sichtbar noch nicht komplett leer gewesen sein.
- Einige Plugins zeichnen eventuell erst zeitverzoegert im `loop()`. Dann sieht man nach dem Blank-Refresh kurz ein leeres Panel. Das ist gewuenscht, weil keine alten Nummernpixel stehen bleiben.
- Aktuell wird `renderPluginId(...)` bei aktivem Scheduler uebersprungen (`if (Scheduler.isActive) return;`). Wenn Scheduler-Wechsel ebenfalls eine kurze ID-Anzeige bekommen sollen, muss diese Sonderregel separat entschieden werden.
- Diese Aenderung passt gut zu einem spaeteren generischen `Splash`-/`StatusScreen`-Modul, sollte aber nicht davon abhaengen.

## 4. Fork des GitHub-Projekts

Status: erledigt.

### Wunsch

Das Projekt soll geforkt werden, um eigene Erweiterungen bereitzustellen.

### Bewertung

Sinnvoll. Der aktuelle Remote zeigt auf das Original:

```text
origin https://github.com/ph1p/ikea-led-obegraensad.git
```

Ein Fork ist der richtige Ort fuer eigene Board-Pins, eigene Plugins, Splashscreens, Remote-Konfiguration und ggf. automatische Firmware-Updates.

### Empfohlene Repo-Struktur im Fork

```text
firmware/
  manifest.json
  config/default.json
  plugins/
    README.md
    patterns/
    native/
```

Pragmatischer fuer dieses bestehende Projekt waere aber, die aktuelle Struktur zu behalten und nur folgende Dateien/Ordner zu ergaenzen:

```text
remote/
  manifest.json
  config.json
  plugins/
    patterns/
      example.json
```

### Risiken / Hinweise

- Wenn langfristig Pulls vom Originalprojekt eingespielt werden sollen, sollten lokale Aenderungen klein und gut getrennt bleiben.
- Board-spezifische Aenderungen fuer den ESP32-C3 sollten moeglichst hinter `CONFIG_IDF_TARGET_ESP32C3` bleiben.

## 5. Eigene Plugins ueber GitHub bereitstellen

### Wunsch

Der ESP soll regelmaessig, z. B. woechentlich, nach neuen Plugins im Repository suchen und diese ggf. herunterladen.

### Bewertung

In der gewuenschten Form nur eingeschraenkt machbar. Der aktuelle Plugin-Begriff bedeutet C++-Code, der mit der Firmware kompiliert wird. Ein ESP32 kann nicht einfach neue C++-Klassen aus GitHub herunterladen, linken und zur Laufzeit in den bestehenden Prozess laden.

Es gibt drei realistische Varianten:

### Variante A: Firmware-Update statt Runtime-Plugin

Eigene C++-Plugins werden im Fork entwickelt und als neue Firmware gebaut. Der ESP prueft woechentlich ein Manifest und installiert bei neuer Version per OTA eine komplette Firmware.

Bewertung: technisch sauberste Variante fuer echte C++-Plugins.

Vorteile:

- passt zum bestehenden C++-Pluginmodell
- nutzt vorhandene OTA-Partitionen
- keine eigene Plugin-VM noetig
- neue Plugins koennen volle Hardware-/API-Funktionalitaet nutzen

Nachteile:

- Update ist ein Firmware-Update, kein einzelner Plugin-Download
- braucht Release-Prozess mit `.bin`-Artefakt
- sollte Versionierung, Rollback-Verhalten und Authentizitaetspruefung haben

### Variante B: Datengetriebene Plugins

Der ESP laedt keine C++-Plugins, sondern Daten: Animationen, Pattern, Pixelbilder, einfache Zeitplaene oder Parameter fuer vorhandene generische Plugins.

Bewertung: beste erste Ausbaustufe fuer "Plugins aus dem Repo", wenn damit vor allem neue Effekte/Bilder gemeint sind.

Vorteile:

- klein, risikoarm, passt zu 16x16-Display
- kann in SPIFFS oder Preferences gespeichert werden
- kein Neustart fuer neue Pattern noetig
- GitHub kann einfache JSON-Dateien bereitstellen

Nachteile:

- keine beliebige neue Logik
- benoetigt ein festes Plugin, das diese Daten interpretiert

### Variante C: Kleine Skript-/Effekt-Engine

Eine eigene Mini-Engine interpretiert heruntergeladene Effektdefinitionen, z. B. JSON-Operationen wie `drawPixel`, `line`, `scrollText`, `randomSparkle`.

Bewertung: flexibel, aber deutlich mehr Entwicklungsaufwand. Erst sinnvoll, wenn Variante B zu begrenzt ist.

Vorteile:

- mehr Dynamik als reine Bilder/Animationen
- weiterhin ohne C++-Runtime-Loading

Nachteile:

- eigene Sprache/API muss entworfen, validiert und getestet werden
- Speicherverbrauch und Fehlerfaelle werden komplexer

### Empfehlung

Kurzfristig Variante B, mittelfristig Variante A.

1. Als erstes `RemoteContentManager` bauen, der `manifest.json`, `config.json` und Pattern-Dateien laden kann.
2. Danach optional einen "RemoteAnimationPlugin" oder "RemotePatternPlugin" einfuehren.
3. Fuer echte neue C++-Plugins den Fork per GitHub Actions bauen lassen und woechentliche OTA-Firmware-Updates ueber ein Release-Manifest implementieren.

## 6. Remote-Konfigurationsdatei aus dem GitHub-Repo

### Wunsch

Ueber das GitHub-Repo soll eine Konfigurationsdatei bereitgestellt werden koennen.

### Bewertung

Gut machbar. Es gibt bereits `Config::fromJson(...)` und `Config::save()`. Eine Remote-Konfiguration kann also als JSON geladen, validiert und in Preferences gespeichert werden.

### Empfohlenes Format

```json
{
  "schema": 1,
  "version": "2026.06.17-1",
  "config": {
    "weatherLocation": "Hamburg",
    "ntpServer": "de.pool.ntp.org",
    "tzInfo": "CET-1CEST,M3.5.0,M10.5.0/3",
    "autoStartSchedule": false
  }
}
```

### Empfohlene Umsetzung

1. Neue Config-Werte ergaenzen:
   - `remoteConfigUrl`
   - `remoteManifestUrl`
   - `remoteUpdateEnabled`
   - `remoteCheckIntervalSeconds`
   - `lastRemoteCheck`
   - `lastAppliedRemoteConfigVersion`
2. Remote-Download nur starten, wenn WLAN verbunden ist.
3. JSON herunterladen, Groesse begrenzen, Schema pruefen.
4. Nur bekannte Felder uebernehmen.
5. Version merken, damit dieselbe Config nicht immer wieder angewendet wird.
6. Nach Aenderung von Zeit-/NTP-Werten ggf. `configTzTime(...)` neu ausfuehren oder Reboot vormerken.

### Risiken / Hinweise

- GitHub Raw URLs laufen ueber HTTPS. Auf ESP32 muss TLS sauber geloest werden. Fuer private Repos waeren Tokens noetig, die nicht hardcodiert in der Firmware landen sollten.
- Fuer ein oeffentliches Repo ist `raw.githubusercontent.com` am einfachsten, aber ohne Signatur kann jeder, der das Repo kontrolliert, die Geraetekonfiguration steuern.
- Konfiguration sollte nie beliebigen Code oder unvalidierte URLs ausfuehren.

## 7. Woechentlicher Check nach Updates

### Wunsch

Der ESP soll regelmaessig, z. B. woechentlich, nach neuen Plugins oder Konfiguration suchen.

### Bewertung

Machbar. Der bestehende `loop()` prueft bereits periodisch WLAN-Reconnects und Scheduler. Ein woechentlicher Remote-Check sollte als eigener Manager laufen und sehr selten aktiv werden.

### Empfohlene Umsetzung

1. `RemoteContentManager` einfuehren:
   - `begin()`
   - `update()`
   - `checkNow()`
   - `fetchManifest()`
   - `applyConfigIfNewer()`
   - optional `downloadPatternsIfNewer()`
2. `RemoteContentManager.update()` in `loop()` aufrufen, aber intern sofort zurueckkehren, wenn kein Check faellig ist.
3. Intervall in Preferences speichern, Default z. B. 7 Tage.
4. Bei fehlender Uhrzeit mit `millis()` arbeiten; nach NTP-Sync kann auf echte Zeit gewechselt werden.
5. Fehler mit Backoff behandeln, nicht in jeder Loop erneut GitHub anfragen.

### Manifest-Vorschlag

```json
{
  "schema": 1,
  "device": "ikea-led-obegraensad",
  "minFirmware": "0.1.0",
  "config": {
    "version": "2026.06.17-1",
    "url": "https://raw.githubusercontent.com/<user>/<repo>/<branch>/remote/config.json",
    "sha256": "<optional>"
  },
  "patterns": [
    {
      "id": "welcome",
      "version": "1",
      "url": "https://raw.githubusercontent.com/<user>/<repo>/<branch>/remote/plugins/patterns/welcome.json",
      "sha256": "<optional>"
    }
  ],
  "firmware": {
    "version": "0.2.0",
    "url": "https://github.com/<user>/<repo>/releases/download/v0.2.0/firmware.bin",
    "sha256": "<recommended>"
  }
}
```

## 8. Sicherheits- und Robustheitsbewertung

- Remote-Updates sollten standardmaessig abschaltbar sein.
- Fuer Firmware-Updates sollte mindestens Version + SHA-256 geprueft werden. Besser waere eine Signatur.
- Downloads muessen Groessenlimits haben, besonders wegen ESP32-C3 RAM und 256 KB SPIFFS.
- Fehlerhafte Remote-Konfiguration darf das Geraet nicht unbenutzbar machen. Immer lokale Defaults behalten.
- OTA-Firmware sollte nur aus einem kontrollierten Release-Pfad kommen, nicht aus beliebigen URLs in einer ungeschuetzten Config.
- GitHub API hat Rate Limits. Fuer oeffentliche Daten sind Raw-Dateien einfacher als API-Requests.

## 9. Umsetzungsvorschlag in Phasen

### Phase 1: Splashscreens und Plugin-ID-Anzeige

- `Screen.setup()` frueher und genau einmal initialisieren.
- `Splash`-Modul mit Boot-Bild und WiFi-Icon bauen.
- Boot-Splash mindestens 5 Sekunden anzeigen.
- WiFiManager-Portal-Callback nutzt WiFi-Icon.
- Erledigt: Plugin-ID beim manuellen Wechsel 2 Sekunden anzeigen, danach Blank-Refresh, danach erst Plugin-Content zulassen.
- Test auf ESP32-C3 mit serieller Ausgabe.

### Phase 2: Remote-Konfiguration

- `Config` um Remote-Update-Felder erweitern.
- `RemoteContentManager` fuer Manifest/Config-Download einfuehren.
- JSON validieren und ueber bestehende `Config::fromJson(...)` anwenden.
- Manuelle API fuer `checkNow` ergaenzen, damit nicht immer eine Woche gewartet werden muss.

### Phase 3: Datengetriebene Remote-Plugins

- Neues Pattern-/Animation-JSON-Format definieren.
- Speicherort festlegen: SPIFFS fuer groessere Daten, Preferences nur fuer kleine Metadaten.
- Generisches Plugin bauen, das heruntergeladene Patterns anzeigt.
- Manifest-Versionierung und lokale Cache-Verwaltung implementieren.

### Phase 4: Firmware-Update aus dem Fork

- GitHub Actions baut `firmware.bin` fuer `esp32c3`.
- Release enthaelt Firmware und Manifest mit Version/SHA-256.
- ESP prueft woechentlich Manifest und installiert neue Firmware per OTA.
- Rollback-/Fehlerverhalten dokumentieren und testen.

### Phase 5: LD2410 Presence OUT (niedrige Prioritaet)

Ziel: Einen LD2410-Praesenzsensor nur ueber dessen digitalen `OUT`-Pin anbinden. UART wird bewusst nicht genutzt, damit die Integration klein bleibt und keine Sensorkonfiguration oder Distanzdaten verarbeitet werden muessen.

Empfohlene Verdrahtung fuer ESP32-C3 SuperMini Plus:

```text
LD2410 VCC -> 5V
LD2410 GND -> gemeinsames GND
LD2410 OUT -> ESP GPIO10
```

Begruendung:

- Die Display-Signale sind bereits belegt: `GPIO0`, `GPIO4`, `GPIO6`, `GPIO7`.
- `GPIO5` ist der Button.
- `GPIO20/GPIO21` bleiben fuer spaeteren UART-Zugriff frei, falls doch einmal LD2410-Konfiguration oder Distanzdaten gebraucht werden.
- `GPIO10` ist fuer einen einfachen digitalen Eingang ein guter naechster Kandidat, sofern er auf dem konkreten Board erreichbar ist.

Empfohlene Firmware-Umsetzung:

1. `PIN_PRESENCE` fuer `CONFIG_IDF_TARGET_ESP32C3` von `-1` auf `10` setzen, sobald die Hardware angeschlossen ist.
2. Initialisierung in `baseSetup()` beibehalten:
   - aktuell wird `pinMode(PIN_PRESENCE, INPUT)` nur gesetzt, wenn `PIN_PRESENCE >= 0`.
   - Falls der LD2410-OUT offen/instabil wirkt, `INPUT_PULLDOWN` testen. Nicht pauschal aktivieren, ohne den realen OUT-Pegel des Moduls zu pruefen.
3. Kleine Presence-Hilfe statt direkter Logik in `loop()`:
   - z. B. `include/presence.h` und `src/presence.cpp`
   - `Presence.begin()`
   - `Presence.update()`
   - `Presence.isPresent()`
   - `Presence.lastChangedAt()`
4. Entprellen/Glitch-Filter vorsehen:
   - OUT-Zustand erst uebernehmen, wenn er z. B. 200-500 ms stabil war.
   - Das verhindert Flackern durch kurze Pegelwechsel oder Boot-Artefakte.
5. Verhalten erst minimal halten:
   - Presence-Zustand im Serial Monitor ausgeben.
   - Optional spaeter Web-API/Status-JSON um `presence` ergaenzen.
   - Noch keine automatische Display-Abschaltung oder Plugin-Steuerung im ersten Schritt.

Moegliche spaetere Nutzung:

- Display nur aktivieren, wenn Praesenz erkannt wird.
- Bei Abwesenheit nach Timeout Helligkeit reduzieren oder Screen clearen.
- Web-UI-Status anzeigen.
- Scheduler/Pluginwechsel nur bei Praesenz laufen lassen.

Risiken / Hinweise:

- LD2410-Module koennen je nach Variante `OUT` unterschiedlich konfigurieren oder invertieren. Deshalb zuerst per Serial-Log pruefen, ob `HIGH` wirklich Praesenz bedeutet.
- Wenn das Modul mit 5V versorgt wird, muss geprueft werden, ob `OUT` 3.3V-kompatibel ist. Bei Unsicherheit Pegel messen oder ueber Spannungsteiler/Levelshifter auf den ESP-Eingang fuehren.
- Der Sensor sollte keine Prioritaet vor Display-Stabilitaet, Splashscreen oder Remote-Konfiguration bekommen.

## 10. Offene Entscheidungen

- Soll der Boot-Splash ein fest kompiliertes 16x16-Bitmap sein oder aus Remote-Konfiguration/Storage kommen?
- Soll nach dem Boot das zuletzt gespeicherte Bild wiederhergestellt werden oder immer das persistierte Plugin starten?
- Erledigt fuer manuelle/API-Auswahl: Plugin-ID wird 2 Sekunden angezeigt, danach folgt ein 75-ms-Blank-Refresh.
- Bedeutet "Plugin" fuer den ersten Schritt C++-Firmware-Plugin oder reicht ein herunterladbares Pattern/Animation?
- Soll Remote-Firmware-Update automatisch installieren oder nur im Web-UI anzeigen und manuell bestaetigt werden?
- Soll LD2410 `OUT` bei Praesenz als `HIGH` oder invertiert behandelt werden? Das muss am realen Modul gemessen oder geloggt werden.
- Soll Presence spaeter nur als Status angezeigt werden oder aktiv Display/Plugins/Scheduler steuern?

## 11. Meine Gesamtbewertung

- Splashscreen und WiFi-Icon sind kleine, sinnvolle Aenderungen mit hoher Erfolgswahrscheinlichkeit.
- Die Plugin-ID-Anzeige sollte zentral im `PluginManager` bereinigt werden: ID 2 Sekunden zeigen, Buffer leeren, kurzen Blank-Refresh abwarten, dann Plugin starten.
- Remote-Konfiguration ist ebenfalls sinnvoll und baut gut auf dem bestehenden `Config`-System auf.
- "Neue Plugins herunterladen" sollte nicht als dynamisches C++-Pluginloading geplant werden. Dafuer ist dieses Firmwaremodell nicht ausgelegt.
- Der robuste Weg ist zweigleisig: einfache Remote-Plugins als Daten/Pattern herunterladen und echte C++-Plugins ueber automatische oder halbautomatische Firmware-OTA-Updates aus dem Fork verteilen.
- LD2410 ueber `OUT` ist als niedrige Prioritaet einfach integrierbar, sollte aber erst nach stabiler Display-Hardware und Grundfirmware umgesetzt werden.
