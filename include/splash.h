#pragma once

#include <Arduino.h>

namespace Splash
{
constexpr unsigned long BOOT_SPLASH_MIN_MS = 5000;

void showBootImage();
void showWifiPortal();
void waitForBootMinimum();
} // namespace Splash
