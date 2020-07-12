#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <NtpClientLib.h>

int countDigit(long long n);
int alignToCenter (int totalWidth, int fontWidth, int digits);

byte utf8ascii(byte ascii);
String utf8ascii(String stringToConvert);
void utf8ascii(char* charToConvert);


#endif