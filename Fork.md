# Fork-Setup

Dieses Projekt basiert auf dem ursprünglichen Repository:

```text
https://github.com/ph1p/ikea-led-obegraensad
```

Für eigene Anpassungen wurde ein Fork bzw. eigenes Repository verwendet:

```text
https://github.com/Splintix98/ikea-led-obegraensad-ESP32C3
```

## Ziel

Das ursprüngliche Projekt ersetzt den Controller eines IKEA OBEGRÄNSAD LED-Panels durch einen Arduino-kompatiblen Mikrocontroller und ermöglicht eine eigene Ansteuerung der LEDs.

In diesem Fork werden Anpassungen für folgende Hardware vorgenommen:

* ESP32-C3 SuperMini Plus
* SN74AHCT125N Level Shifter
* IKEA OBEGRÄNSAD LED-Panel
* LD2410 Präsenzsensor

## Verwendeter Arbeitsbranch

Die Anpassungen befinden sich im Branch:

```text
esp32-c3-supermini-plus
```

Der Branch wurde lokal ausgehend von `main` erstellt:

```bash
git switch -c esp32-c3-supermini-plus
```

## Remote-Konfiguration

Das ursprüngliche Repository wurde als `upstream` eingetragen:

```text
upstream        https://github.com/ph1p/ikea-led-obegraensad.git
```

Das eigene Repository wurde als `origin` eingetragen:

```text
origin          https://github.com/Splintix98/ikea-led-obegraensad-ESP32C3.git
```

Prüfung der Remotes:

```bash
git remote -v
```

Erwartetes Ergebnis:

```text
origin    https://github.com/Splintix98/ikea-led-obegraensad-ESP32C3.git (fetch)
origin    https://github.com/Splintix98/ikea-led-obegraensad-ESP32C3.git (push)
upstream  https://github.com/ph1p/ikea-led-obegraensad.git (fetch)
upstream  https://github.com/ph1p/ikea-led-obegraensad.git (push)
```

## Initialer Commit

Die lokalen Anpassungen wurden in folgendem Commit gesichert:

```text
9e3f8ba Adapt project for ESP32 C3 SuperMini Plus
```

Commit-Befehl:

```bash
git add .
git commit -m "Adapt project for ESP32 C3 SuperMini Plus"
```

Dabei wurden folgende Dateien geändert bzw. hinzugefügt:

```text
modified:   include/constants.h
modified:   platformio.ini
modified:   src/main.cpp
modified:   src/screen.cpp
created:    NIK_README.md
created:    Plan.md
created:    esp32_c3_supermini_plus_documentation_V3.html
created:    tenstar_robot_esp32_c3_supermini_plus_wissensbasis.md
```

## Push in das eigene Repository

Der Branch wurde nach `origin` gepusht und als Tracking-Branch eingerichtet:

```bash
git push -u origin HEAD
```

Ergebnis:

```text
HEAD -> esp32-c3-supermini-plus
branch 'esp32-c3-supermini-plus' set up to track 'origin/esp32-c3-supermini-plus'
```

Damit zeigen zukünftige Pushes auf diesen Branch im eigenen Repository.

## Aktueller Remote-Branch-Stand

Vorhandene relevante Remote-Branches:

```text
origin/esp32-c3-supermini-plus
upstream/main
```

Das Original-Repository enthält zusätzlich weitere Branches, die für die eigene Arbeit aktuell nicht direkt relevant sind.

## Änderungen aus dem Original-Projekt übernehmen

Falls das Original-Repository später aktualisiert wird, können Änderungen wie folgt übernommen werden:

```bash
git fetch upstream
git merge upstream/main
```

Alternativ kann statt `merge` auch `rebase` verwendet werden:

```bash
git fetch upstream
git rebase upstream/main
```

Für dieses Projekt ist `merge` in der Regel einfacher und risikoärmer, solange keine besonders lineare Commit-Historie erforderlich ist.

## Eigene Änderungen weiterführen

Normale Arbeitsweise auf dem eigenen Branch:

```bash
git status
git add .
git commit -m "Beschreibung der Änderung"
git push
```

Da der Branch bereits mit `origin/esp32-c3-supermini-plus` verknüpft ist, reicht künftig normalerweise:

```bash
git push
```

## Hinweis zu Zeilenenden unter Windows

Beim `git add .` wurden Warnungen wie diese angezeigt:

```text
LF will be replaced by CRLF the next time Git touches it
```

Das ist unter Windows üblich und bedeutet, dass Git Zeilenenden beim nächsten Bearbeiten ggf. von LF auf CRLF konvertiert.

Falls das nicht gewünscht ist, kann später eine `.gitattributes`-Datei ergänzt werden, um einheitliche Zeilenenden festzulegen. Für den aktuellen Fork-Vorgang war die Warnung nicht kritisch.

## Hinweis zum Tippfehler

Der eingegebene Befehl:

```bash
git remove -v
```

war ein Tippfehler. Korrekt ist:

```bash
git remote -v
```

Der Tippfehler hatte keine Auswirkung auf das Repository.
