
## Wünsche:
* Laufschrift: "Jonas Sux!!!"
* Einbindung des Präsenzsensors
  * wenn Präsenz erkannt nachdem mindestens 5 Minuten keine Präsenz erkannt wurde: den Splashscreen anzeigen
* Neues Plugin: Kombination aus Wetter und "Clock": alle 5s Sekunden wird zwischen den beiden gewechselt. Dabei wird schnell ausgefadet (Helligkeit) und das andere dann eingefadet. Es muss jedoch darauf geachtet werden, dass die Plugins nicht jedes Mal neu geladen werden, da das zu lange dauern würde. Die Plugins müssen praktisch gecached werden bzw. im Hintergrund weiterlaufen, damit ein smoother Übergang gewährleistet werden kann.

## TODO:
* DONE Splashscreen-Implementierung funktioniert aktuell nicht vernünftig. Wenn das Gerät gestartet wird, wird der Splashscreen angezeigt, doch dann wird nicht weiter initialisiert. Das Gerät verbindet sich nicht mit dem WLAN und fängt auch nicht an das Standard-Plugin zu laden.
* Wenn Button mehrmals hintereinander recht schnell gedrückt wird, passiert einfach nichts. Es muss erst gewartet werden, bis die Plugin-Nummer fertig angezeigt wurde und das Plugin gestartet wurde, bevor erneut gedrückt und gewechselt werden kann.
* DONE Plugins aussortieren:
  * Breakout
  * Lines
  * Circle
  * Rain
  * Spiral
  * Checkerboard
  * Radar
  * Bubbles
  * Comet
  * Scanlines
  * Ticking Clock
  * Es müssen natürlich die Nummern der dann verbleibenden Plugins angezeigt werden. Wenn zB Nummer 2 ausgeblendet wird, sollte nicht von 1 zu 3 gesprungen werden.
* DONE GameOfLife Standard-Delay setzen auf 800 ms
