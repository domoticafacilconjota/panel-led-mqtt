/*
  Copyright (C) 2020  Domótica Fácil con Jota en YouTube

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>
#include <password.h>

// TODO Este es el fichero de conteo de resets
#define POWER_CYCLE_FILE "/power.json"

// Servidor NTP Global
#define NTP_SERVER "es.pool.ntp.org"

#define MQTT_CLIENT "panel-led-mqtt"
#define STATUS_TOPIC "plm/status"
#define SUBSCRIBERS_TOPIC "plm/subscribers"
#define OTA_DATA_STD "plm/ota_data"


// Colores
#define NEGRO           {0x00,0x00,0x00}
#define BLANCO          {0xFF,0xFF,0xFF}
#define AZUL            {0x00,0x00,0xFF}
#define AMARILLO        {0xFF,0xFF,0x00}
#define VERDE           {0x00,0xFF,0x00}
#define MAGENTA         {0xFF,0x00,0xFF}
#define ROJO            {0xFF,0x00,0x00}
#define CIAN            {0x00,0xFF,0xFF}

#define GRIS            {0x80,0x80,0x80}
#define GRIS_CLARO      {0xC0,0xC0,0xC0}
#define GRIS_PALIDO     {0xE0,0xE0,0xE0}
#define GRIS_OSCURO     {0x40,0x40,0x40}

#define AZUL_OSCURO     {0x00,0x00,0x80}
#define VERDE_OSCURO    {0x00,0x80,0x00}
#define ROJO_OSCURO     {0x80,0x00,0x00}
#define AZUL_CLARO      {0x80,0xC0,0xFF}
#define VERDE_CLARO     {0x80,0xFF,0x80}
#define ROJO_CLARO      {0xFF,0xC0,0xFF}
#define ROSA            {0xFF,0xAF,0xAF}
#define MARRON          {0x60,0x30,0x00}
#define NARANJA         {0xFF,0x80,0x00}
#define PURPURA         {0xC0,0x00,0xFF}
#define LIMA            {0x80,0xFF,0x00}

// Modos
const int NO_ONE = 0;
const int WELCOME = 1;
const int YOUTUBE = 2;
const int TWITTER = 3;
const int CUSTOM = 4;

#endif