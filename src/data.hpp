#pragma once
#include <SPI.h>
#include <SD.h>
#include <user.hpp>

void setup_sd();
void load_user();
void save_user();
