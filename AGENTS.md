# AGENTS.md

LLM context file. Optimize recall > prose. Project: `ikea-led-obegraensad` fork/local mod for Niklas.

## Project

- Firmware for IKEA OBEGRAENSAD 16x16 LED panel.
- Original IKEA controller removed/replaced.
- Current target board: ESP32-C3 SuperMini Plus, Arduino framework, PlatformIO.
- Current working hardware fix: SN74AHCT125N level shifter required. Without LS: random/flicker/wrong pixels. With LS: panel stable.
- User writes German, but this file is machine/context notes. Keep edits minimal and factual.

## Important local docs

- `NIK_README.md`: user-specific wiring + build + notes. Read before hardware/pin changes.
- `Plan.md`: roadmap/splash/fork/remote updates/presence notes.
- `tenstar_robot_esp32_c3_supermini_plus_wissensbasis.md`: local ESP32-C3 SuperMini Plus board notes.
- `esp32_c3_supermini_plus_documentation_V3.html`: full board doc dump.
- `README.md`: upstream project docs.

## Current hardware

- ESP32-C3 SuperMini Plus.
- IKEA OBEGRAENSAD panel.
- SN74AHCT125N level shifter, abbreviated `LS`.
- 84 nF ceramic cap used as decoupling cap from LS VCC to LS GND; acceptable substitute for 100 nF.
- LD2410 presence sensor planned/connected only via digital OUT, no UART.
- Panel button to ESP GPIO5 against GND.

## Wiring exact current user design

Power/common:

```text
LS Pin 14 VCC + Panel 5V + ESP 5V + LD2410 5V

LS Pin 7 GND
+ Panel GND
+ ESP GND
+ LS Pin 1  1OE negated
+ LS Pin 4  2OE negated
+ LS Pin 10 3OE negated
+ LS Pin 13 4OE negated
+ Panel Button 2
+ LD2410 GND
```

Reason: SN74AHCT125N OE pins are active LOW, so tie all OE to GND.

ESP -> LS:

```text
ESP GPIO0 -> LS Pin 2  1A
ESP GPIO6 -> LS Pin 5  2A
ESP GPIO4 -> LS Pin 9  3A
ESP GPIO7 -> LS Pin 12 4A
```

LS -> panel:

```text
LS Pin 3  1Y -> Panel EN
LS Pin 6  2Y -> Panel IN
LS Pin 8  3Y -> Panel CLK
LS Pin 11 4Y -> Panel CLA
```

Button:

```text
Panel Button 1 -> ESP GPIO5
Panel Button 2 -> GND
```

LD2410:

```text
LD2410 5V  -> 5V
LD2410 GND -> GND
LD2410 OUT -> ESP GPIO10
```

Only digital OUT planned. UART pins GPIO20/GPIO21 intentionally left free.

## Firmware important files

- `platformio.ini`: default env now `esp32c3`, USB CDC flags enabled.
- `include/constants.h`: board pins, `PANEL_TEST_MODE`, storage/server config.
- `src/screen.cpp`: SPI display driver, timer refresh, pixel mapping table usage.
- `include/screen.h`: `positions[]` physical/logical pixel mapping.
- `src/main.cpp`: WiFiManager, server init, plugin registration, ESP32 drawing task, panel test mode.
- `src/PluginManager.cpp`: plugin activation, plugin ID display behavior.
- `src/config.cpp`, `include/config.h`: Preferences-backed config.
- `frontend/src/app.tsx`: main web UI; Draw canvas mapping/rotation.
- `frontend/src/components/led-matrix.tsx`: canvas render + hit-test; uses `indexData` for visual cells and WS LED index.
- `frontend/src/helpers.ts`: `rotateArray(matrix, rotations)`; quarter-turns, mod 4.
- `src/plugins/DrawPlugin.cpp`: Draw WS writes direct to `Screen` buffer (`led`, `screen`, `clear`, `persist`, `load`).
- `src/plugins/DDPPlugin.cpp`: UDP DDP live pixel input, port 4048.
- `src/plugins/ArtNet.cpp`: Art-Net/DMX live input, universe configurable by WS `artnet`.
- `src/webgui.cpp`: generated embedded gzipped web UI; normally do not hand-edit.

## Current firmware facts

In `platformio.ini`:

```ini
[platformio]
default_envs = esp32c3

[env:esp32c3]
extends = env:esp32-base
board = esp32-c3-devkitm-1
build_flags =
	${env:esp32-base.build_flags}
	-DARDUINO_USB_CDC_ON_BOOT=1
	-DARDUINO_USB_MODE=1
```

In `include/constants.h` current C3 pins:

```cpp
#define PANEL_TEST_MODE 0

#if defined(CONFIG_IDF_TARGET_ESP32C3)
#define PIN_ENABLE 0
#define PIN_DATA   6
#define PIN_CLOCK  4
#define PIN_LATCH  7
#define PIN_BUTTON 5
#define PIN_PRESENCE -1
#endif
```

- `PIN_PRESENCE` currently disabled (`-1`) even though LD2410 OUT wiring target is GPIO10.
- When implementing presence, set C3 `PIN_PRESENCE 10`.
- `src/main.cpp` already guards presence init with `#if PIN_PRESENCE >= 0`.
- If LD2410 OUT unstable, test `INPUT_PULLDOWN`, but do not assume; first log/meter real OUT behavior.
- Weather default currently user-specific: `WEATHER_LOCATION "31311 Uetze Niedersachsen Deutschland"`.

In `src/screen.cpp`:

```cpp
#define TIMER_INTERVAL_US 200
#define GRAY_LEVELS 64
#define SCREEN_SPI_FREQUENCY 10000000
```

- SPI 10 MHz works with level shifter.
- Tests 1 MHz / 4 MHz were not useful; low SPI can break/stress webserver because timer interval 200 us and SPI transfer of 32 bytes gets too slow.
- Display refresh writes 32 bytes every 200 us timer tick.
- ESP32-C3 is single-core; WiFi/server and display timing can interact.

## Debug/test mode

- Barebones panel test exists behind `PANEL_TEST_MODE`.
- `PANEL_TEST_MODE 0`: normal WiFi/webserver/plugins.
- `PANEL_TEST_MODE 1`: no WiFi/webserver/plugins; full panel blink + 5-pixel snake.
- Test mode code is in `src/main.cpp` under `#if PANEL_TEST_MODE`.
- Keep disabled unless user asks hardware panel test.

## Build commands

PlatformIO executable:

```powershell
C:\Users\Niklas\.platformio\penv\Scripts\platformio.exe
```

Build:

```powershell
C:\Users\Niklas\.platformio\penv\Scripts\platformio.exe run -e esp32c3
```

Erase NVS/flash if WiFi credentials/config must reset:

```powershell
C:\Users\Niklas\.platformio\penv\Scripts\platformio.exe run -e esp32c3 -t erase
```

Sandbox note:

- PlatformIO build from Codex sandbox usually fails first with permission denied on `C:\Users\Niklas\.platformio\platforms.lock`.
- Rerun same build with escalation. This is normal: PlatformIO writes global cache/lock under user home.
- Do not use bare `pio`; may not be in PATH. Use full path above.

## Frontend / web GUI build

- Frontend is Solid/Vite under `frontend/`.
- Package manager: `pnpm@10.27.0`.
- Bare `pnpm` may not be in PATH; use `corepack pnpm ...`.
- First build may need:

```powershell
cd frontend
corepack pnpm install --frozen-lockfile
corepack pnpm run build
```

- `pnpm run build` = `vite build && node compress.mjs`.
- `compress.mjs` rewrites `../src/webgui.cpp` from `frontend/dist/index.html`.
- After frontend changes shipped on device, rebuild so `src/webgui.cpp` matches.
- `dist/`, `node_modules/` ignored.
- `tsc -p tsconfig.app.json --noEmit` currently fails pre-existing: `src/types.ts` enum + `erasableSyntaxOnly`. Vite build succeeds.

## Draw plugin rotation facts

- Global panel rotation = `Screen.currentRotation`; menu rotate buttons send WS `event:"rotate"` and rotate/persist panel output.
- Frontend preview/hit-test uses `indexData` passed to `LedMatrix`.
- `LedMatrix` renders `data[mappedIndex]` and sends `led { index: mappedIndex, status }`.
- DrawPlugin `led` event calls `Screen.setPixelAtIndex(index,status)` directly; no server-side plugin transform.
- 2026-06-19 fix: Draw canvas needed +180 deg independent of panel rotation.
- Implemented in `frontend/src/app.tsx`: `DRAW_PLUGIN_ROTATION_OFFSET = 2`, mapping `rotateArray(store.indexMatrix, store.rotation + DRAW_PLUGIN_ROTATION_OFFSET)`.
- Do not fix this in `Screen.currentRotation` or `src/screen.cpp`; that changes all plugins/panel, not only Draw canvas.

## Draw plugin default image facts

- Draw startup: `DrawPlugin::setup()` delays 50 ms, `Screen.clear()`, then `Screen.loadFromStorage()`, then `sendInfo()`.
- Draw has no hardcoded default image currently. Its default = whatever 256-byte screen buffer was saved in NVS/Preferences under `led-wall` key `data`.
- Web UI Import path already supports PNG/image import:
  - `frontend/src/helpers.ts` loads chosen image into 16x16 `OffscreenCanvas`.
  - Converts pixels by RGB sum threshold `BRIGHTNESS_THRESHOLD = 383`.
  - Dark pixels => `1`, light pixels => `0`.
  - `frontend/src/app.tsx` sends WS `event:"screen"` with `data:number[256]`.
  - `DrawPlugin` handles `screen` via `Screen.setRenderBuffer(buffer)`; non-gray path maps `1` to `255`, `0` to `0`.
  - UI Save sends `event:"persist"`; `DrawPlugin` calls `Screen.persist()`.
- Easiest user workflow for default Draw image: select Draw, import 16x16 PNG, Save. Firmware then loads it every Draw start.
- If asked for firmware-baked Draw default:
  - add generated `uint8_t defaultDrawImage[TOTAL_PIXELS]` header from 16x16 PNG.
  - in `DrawPlugin::setup()`, load default only when storage marker missing, then `Screen.persist()`.
  - Need marker, e.g. Preferences bool `draw-default-init`; cannot infer uninitialized from all-black image.
  - Keep PNG converter offline/build-time, not on ESP unless user explicitly wants runtime image decoding.

## DDP / ArtNet facts

- DDPPlugin:
  - Requires `AsyncUDP.h`; guarded by `ASYNC_UDP_ENABLED`.
  - Listens UDP port `4048`.
  - Expects DDP-ish packet length >=10, skips 10-byte header.
  - Treats payload as RGB triples.
  - Brightness = average RGB.
  - If exactly 1 RGB pixel, fills whole panel with that brightness.
  - Else maps sequential RGB triples to `Screen.setPixelAtIndex(i, brightness > 4, brightness)`.
  - Good for pixel streaming controllers.
- ArtNetPlugin:
  - Uses bundled `lib/ArtnetWifi`.
  - `setup()` calls `artnet.begin()`, sets DMX callback, default universe 1.
  - `loop()` must call `artnet.read()`.
  - DMX callback accepts universe 0 or configured outgoing universe.
  - Maps `data[i]` directly to pixel i brightness/on threshold >4.
  - WS event `artnet` can change universe.
  - Good for DMX/lighting software; grayscale only here, not RGB.

## Plugin ID display behavior

- 2026-06-19 implemented in `src/PluginManager.cpp`.
- `renderPluginId()` now:
  - shows selected plugin ID for `PLUGIN_ID_DISPLAY_MS = 2000`.
  - then `Screen.clear()`.
  - waits `PLUGIN_ID_BLANK_REFRESH_MS = 75` so timer renders blank panel before plugin setup/loop.
- `currentStatus = LOADING` still blocks active plugin loop during ID + blank refresh.
- Scheduler behavior unchanged: `renderPluginId()` returns early when `Scheduler.isActive`.

## NVS/persistence

- Flashing firmware does not reset WiFi credentials.
- WiFiManager credentials, brightness, plugin, draw image, rotation/config live in NVS/Preferences.
- Need erase target to clear.

## Clock / NTP facts

- Clock issue #231 root: clock plugins blank because `getLocalTime()` returns false when NTP unsynced.
- Relevant upstream/fork context: NemoN compare `ph1p/main...NemoN/main` fixed by NTP fallback servers + initial wait + plugin error display. Do not cherry-pick full fork; it contains lots unrelated DDP/ArtNet/tooling.
- Current local robust fix:
  - `Config::applyTimeConfig(bool waitForSync=false)` in `src/config.cpp`.
  - Uses persistent static `String appliedTzInfo/appliedNtpServer`; important because Arduino ESP32 `configTzTime()` stores SNTP server pointers via `sntp_setservername`, so avoid temporary `String(...).c_str()` lifetime risk.
  - Calls `configTzTime(tz, configured_ntp, "pool.ntp.org", "time.google.com")`.
  - `src/main.cpp` calls `config.applyTimeConfig(true)` after WiFi connect; waits up to 20*500ms for first sync and logs OK/FAILED.
  - `src/webhandler.cpp` calls `config.applyTimeConfig()` after `/api/config` save/reset so NTP/TZ changes apply without reboot.
- Clock plugins changed:
  - `ClockPlugin`, `BigClockPlugin`, `TickingClockPlugin`, `PongClockPlugin` use `getLocalTime(&timeinfo, 100)` in loop.
  - On first NTP failure draw centered X; on recovery clear previous digit caches and force redraw.
  - `PongClockPlugin::setup()` uses `getLocalTime(&timeinfo, 2000)`.
  - `PongClockPlugin::pong_predict_y()` has max 100 iterations to avoid runaway loop.
- If clock still X after flash: check serial `[NTP]` logs, WiFi/DNS/UDP123, stored config values. Web config should be:
  - NTP `de.pool.ntp.org`
  - TZ `CET-1CEST,M3.5.0,M10.5.0/3`
- Because Preferences override firmware defaults, old bad config persists until web reset/save or erase.

## Weather facts

- Weather plugin currently still uses `wttr.in`, not NemoN Open-Meteo rewrite.
- Current local fixes:
  - default location in `include/constants.h`: `31311 Uetze Niedersachsen Deutschland`.
  - `WeatherPlugin::encodeLocation()` URL-encodes spaces/special chars before `https://wttr.in/<location>?format=j2&lang=en`.
- User reported Weather works after these changes.
- If changing weather location and old Preferences exist, set via web config or reset; firmware default alone may not apply.
- Avoid pulling NemoN weather rewrite unless explicitly wanted; it changes provider/model and adds much larger diff.

## Current generated web UI state

- If `frontend/src/app.tsx` changes, generated `src/webgui.cpp` usually changes too.
- Current dirty-tree after recent work may include `frontend/src/app.tsx` + `src/webgui.cpp`; treat `src/webgui.cpp` as generated but commit it when shipping UI firmware.
- Do not hand-edit `src/webgui.cpp`; edit frontend then run build/compress.

## Known hardware findings

- ESP32-C3 direct 3.3V signals to 5V panel caused flicker/wrong pixels.
- SN74AHCT125N at 5V solved issue.
- 84 nF cap at LS VCC/GND acceptable.
- Separate ESP power supply test caused panel off/weirdness; prefer common 5V/GND setup. If separate supplies ever used: common GND mandatory; avoid blindly paralleling 5V rails.

## Button findings 2026-06-19

- Current user wiring: original panel button wires cut/replaced; button is isolated direct between ESP GPIO5 and GND.
- This is OK for active-low input. No external resistor normally needed if internal pullup active.
- Never pull ESP32-C3 button GPIO to 5V. If external pullup needed, use ~10k to 3.3V only.
- Expected meter test powered: GPIO5 unpressed ~=3.3V, pressed ~=0V. Unpowered continuity: pressed GPIO5-GND near short.
- Upstream issue #79 uses existing board contact / C6 pad as convenience path to original button/GND. Not required if button is fully rewired direct GPIO5-GND.
- Code path checked: `PIN_BUTTON 5`, `BfButton(..., true, LOW)`, `btn.read()` in loop, single press -> `pluginManager.activateNextPlugin()`.
- ButtonFever sets `INPUT_PULLUP` in global `BfButton` constructor. If button dead, first robust diagnostic/fix: explicitly `pinMode(PIN_BUTTON, INPUT_PULLUP)` in `baseSetup()` and Serial-log `digitalRead(PIN_BUTTON)`.
- ButtonFever single press is delayed by double-press timeout because `.onDoublePress(...)` is registered; short press triggers after release + ~300 ms, not instantly.
- Arduino ESP32-C3 variant for current board maps pin `5` to GPIO5/MISO/A5; `SPI.begin(PIN_CLOCK, -1, PIN_DATA, -1)` disables MISO, so no direct SPI MISO conflict expected.

## Board alternatives user owns

- ESP32 S2 mini D1
- ESP32 C3
- ESP32 WROOM 32D
- Wemos D1 mini

Prior suggested order if changing board:

1. ESP32 WROOM 32D best fit / dual-core / closer upstream.
2. ESP32-C3 current works with level shifter.
3. ESP32-S2 mini D1 single-core, not clearly better.
4. Wemos D1 mini ESP8266 fallback only.

## Roadmap high-level

- Splashscreen during boot, at least 5 s.
- WiFiManager icon while config portal active.
- Plugin-ID display behavior done in code and `Plan.md`.
- Fork item marked done in `Plan.md`; remote config/remote pattern ideas still planned.
- LD2410 OUT presence low priority.

## Presence implementation notes

Low priority. For future implementation:

- Use only digital `OUT`, no UART.
- Set `PIN_PRESENCE 10` for C3.
- Add small module, not random loop code:
  - `include/presence.h`
  - `src/presence.cpp`
  - `Presence.begin()`
  - `Presence.update()`
  - `Presence.isPresent()`
  - `Presence.lastChangedAt()`
- Add 200-500 ms stable-state filter.
- First step only Serial/status reporting.
- Later optional web status JSON.
- Do not auto-disable display/plugin scheduler before user explicitly asks.
- Check whether OUT HIGH means presence; some modules/configs can invert.
- If LD2410 OUT is 5V, protect ESP input via divider/level shifting. Need measure/confirm.

## Editing guidance for this repo

- Preserve user hardware-specific C3 pin block.
- Do not remove `PANEL_TEST_MODE` unless user asks; it is useful debug tool.
- Do not revert uncommitted user changes. Worktree commonly dirty.
- Prefer minimal diff.
- Use `apply_patch` for manual edits.
- Keep docs ASCII-only unless existing file requires otherwise. Current notes use `ue/ae/oe` style in docs.
- If changing display timing, consider 200 us refresh + 32-byte SPI transfer + C3 single-core WiFi impact.
- If changing `Screen.setup()` order, ensure timer not initialized twice.
- If adding splash, remember current ESP32 `Screen.setup()` happens in drawing task after `baseSetup()`, too late for WiFiManager splash unless refactored.
