#pragma once

#include <Arduino.h>

namespace Splash
{
constexpr unsigned long BOOT_SPLASH_MIN_MS = 10000;

void showBootImage();
void waitForBootMinimum();
} // namespace Splash
