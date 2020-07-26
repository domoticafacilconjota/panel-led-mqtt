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

#include <Arduino.h>                              // Arduino
#include <WiFiManager.h>                          // Configuración WiFi
#include <SmartMatrix3.h>                         // Librería para controlar el panel LED
#include <PubSubClient.h>                         // Cliente MQTT 0.8.2
#include <ArduinoJson.h>                          // Arduino JSON parser 6.15.1
#include <TimeLib.h>					                    // Control del tiempo sin RTC
#include <NtpClientLib.h>				                  // Cliente NTP 3.0.2-beta

#include <constants.h>                            // Cabecera de constantes
#include <utils.h>                                // Cabecera de utilidades
#include <mqtt.h>                                 // Cabecera MQTT

#include <gimpbitmap.h>                           // Cabecera GIMP

/** LISTADO DE BITMAPS **/
#include "img_yt.c"
#include "img_df.c"

#define configTIMER_TASK_STACK_DEPTH 4096         // La cosa está muy mala

#define GPIOPINOUT ESP32_FORUM_PINOUT_WITH_LATCH  // ESP32 sin controladora
#define COLOR_DEPTH 24                            // Profundidad de color

const uint8_t kMatrixWidth = 64;                  // Número de LEDs a lo ancho
const uint8_t kMatrixHeight = 32;                 // Número de LEDs a lo alto
const uint8_t kRefreshDepth = 36;                 // Profundidad para refrescar
const uint8_t kDmaBufferRows = 4;                 // Número de filas en el buffer DMA

// El panel recomendado LINK utiliza un refresco de 16
const uint8_t kPanelType = SMARTMATRIX_HUB75_32ROW_MOD16SCAN;

// No vamos a utilizar opciones por ahora.
const uint8_t kMatrixOptions = (SMARTMATRIX_OPTIONS_NONE); 
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);
const uint8_t kScrollingLayerOptions = (SM_SCROLLING_OPTIONS_NONE);
const uint8_t kIndexedLayerOptions = (SM_INDEXED_OPTIONS_NONE);

// Configuramos los parámetros
SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);
SMARTMATRIX_ALLOCATE_SCROLLING_LAYER(scrollingLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kScrollingLayerOptions);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(indexedLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);

// Inicializamos variables que posteriormente podrán ser modificadas mediante MQTT
int defaultBrightness = (10*255) / 100;
int defaultScrollOffset = 6;

// Variables de configuración para el AP WiFi
char host[] = "DF";
char separator[] = "_";
char SSID[18];
byte mac[6];
char macFull[6];

// Variable para guardar el bitmap de forma temporal.
uint8_t bitmapArray[32*32*3+1];
#define BITMAP_WIDTH 32
#define BITMAP_HEIGHT 32
#define RRR 0
#define GGG 1
#define BBB 2

// Función para dibujar los bitmaps
void drawBitmap(int16_t x, int16_t y, const gimp32x32bitmap* bitmap) {
  for(unsigned int i=0; i < bitmap->height; i++) {
    for(unsigned int j=0; j < bitmap->width; j++) {

      rgb24 pixel = { bitmap->pixel_data[(i*bitmap->width + j)*3 + 0],
                      bitmap->pixel_data[(i*bitmap->width + j)*3 + 1],
                      bitmap->pixel_data[(i*bitmap->width + j)*3 + 2] };

      backgroundLayer.drawPixel(x + j, y + i, pixel);

      // if (x + j <= 4) {
      // Serial.print(x + j);
      // Serial.print(" ");
      // Serial.print(y + i);
      // Serial.print(" ");
      // Serial.print(bitmap->pixel_data[(i*bitmap->width + j)*3 + 0]);
      // Serial.print(" ");
      // Serial.print(bitmap->pixel_data[(i*bitmap->width + j)*3 + 1]);
      // Serial.print(" ");
      // Serial.println(bitmap->pixel_data[(i*bitmap->width + j)*3 + 2]);
      // }


      
    }
  }
}

// Función para dibujar los bitmaps
void drawBitmapMqtt(int16_t aX, int16_t aY, const char* temp) {
  int led = 0;
  // Creamos una copia temporal.
  char* otaImage = (char*) temp;
  // Si la imagen enviada OTA no ha sido procesada, lo hacemos
  if (otaImage) {
    char *token;

    while((token = strsep(&otaImage, ","))) {
      bitmapArray[led] = strtoul(token, nullptr, 0);

      led++;
    }
  }

  for (int y=0; y<BITMAP_HEIGHT; y++) {
    for (int x=0; x<BITMAP_WIDTH; x++) {
      rgb24 pixel = {bitmapArray[(y*BITMAP_WIDTH + x)*3 + RRR],
                  bitmapArray[(y*BITMAP_WIDTH + x)*3 + GGG],
                  bitmapArray[(y*BITMAP_WIDTH + x)*3 + BBB]};
      
      backgroundLayer.drawPixel(aX + x, aY + y, pixel);
    }
  }  
}

// Llamada de control para el modo de configuración
void configModeCallback (WiFiManager *myWiFiManager) {
    
    // Paramos el desplazamiento del texto y configuramos un nuevo texto
    scrollingLayer.stop();
    scrollingLayer.setColor(BLANCO);
    scrollingLayer.setMode(wrapForward);
    scrollingLayer.setSpeed(80);
    scrollingLayer.setFont(font6x10);
    char noConnection[] = "Imposible conectar. Creando punto de acceso para realizar la configuración. Es el siguiente:";     // Importante para mostrar acentos
    utf8ascii(noConnection);
    scrollingLayer.start(noConnection, -1);

    // Mostramos el SSDI justo debajo del texto que se desplaza
    backgroundLayer.setFont(font5x7);
    backgroundLayer.drawString(0, 21, AZUL_CLARO, SSID);
    backgroundLayer.swapBuffers();
}

// JSON
StaticJsonDocument<3500> document;

void setup() {
  // Don Quijote de la Mancha
  Serial.begin(115200);

  // Configuración de la WiFi AP / STA
	WiFiManager wifiManager;
	WiFi.macAddress(mac);
	sprintf(macFull, "%d", ESP.getEfuseMac());
  // Concatenamos los parámetros necesarios para formar el SSID
	strcat(SSID, host);
	strcat(SSID, separator);
	strcat(SSID, macFull);

  // Configuramos las capas que posteriormente vamos a utilizar
  matrix.addLayer(&backgroundLayer); 
  matrix.addLayer(&scrollingLayer); 
  matrix.addLayer(&indexedLayer);
  // Pintemos 
  matrix.begin();
  // Pongamos un brillo inicial bajo por si conectamos todo directamente a un USB
  matrix.setBrightness(defaultBrightness);
  // Configuremos el margen para los textos que se desplazan
  scrollingLayer.setOffsetFromTop(defaultScrollOffset);
  // Para ver los colores de forma Chachi Piruli, activemos la corrección de color
  backgroundLayer.enableColorCorrection(true);


  // Texto de ayuda
  scrollingLayer.setColor(BLANCO);
  scrollingLayer.setMode(wrapForward);
  scrollingLayer.setSpeed(60);
  scrollingLayer.setFont(font6x10);
  scrollingLayer.start("Intentando conectar a la WiFi", -1);

  ////////////////////////
  // WiFi everywhere :D //
  ////////////////////////

  Serial.println("Conectando a la WiFi...");
	// Configuramos el nombre para el SSID. Debe ser diferente para cada dispositivo
	WiFi.setHostname(SSID);
	WiFi.setAutoReconnect(true);

  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setTimeout(1); // Cambiar a 180
  wifiManager.setCleanConnect(true);

  	if (wifiManager.autoConnect(SSID)) {
		// En el caso de que la conexión sea un éxito.
    // TODO

	} else {
		// En el caso de que la conexión NO sea un éxito.

    // TODO
	}

  // Paramos el desplazamiento del texto
  scrollingLayer.stop();

  // MQTT
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASS);
  client.setBufferSize(30000);
  client.setCallback(mqttCallback);

  if (client.connected()) {
      Serial.println("Conectado a MQTT");
      // Publicamos los asuntos necesarios
      client.publish(SUBSCRIBERS_TOPIC, subscribersInScreen);
      // Nos suscribimos a los asuntos necesario. También te deberías suscribir a https://www.youtube.com/domoticafacilconjota
      client.subscribe(STATUS_TOPIC);
      client.subscribe(SUBSCRIBERS_TOPIC);
      client.subscribe(OTA_DATA_STD);

  }
	
  // Ahora que ya está todo configuramos. Intentamos sincronizar con un servidor NTP
	NTP.onNTPSyncEvent([](NTPSyncEvent_t event) {
		ntpEvent = event;
		ntpEventTriggered = true;
	});

}

void loop() {
	// Intentar reconectarse si se pierde la WiFi
  while (WiFi.status() != WL_CONNECTED || WiFi.localIP() == IPAddress(0,0,0,0)) {
    WiFi.reconnect();
    Serial.println(F("Intentando reconectar a WiFi"));
    delay(10000);
  }

  client.loop();

  // Si se produce una desconexión del cliente MQTT, intentamos reconectarnos
  if (!client.connected()) {
    Serial.println("No conectado a MQTT. Intentando reconectar.");
    mqttReconnect();
  }

  // Nos hacemos cargo de sincronizar con el servidor NTP cada cierto tiempo.
	if (syncEventTriggered) {
		processSyncEvent(ntpEvent);
		syncEventTriggered = false;
	}

  time_t currentTime = now();

  // Configuramos todo lo necesario para dar la bienvenida
  if (currentMode == WELCOME) {
    backgroundLayer.fillScreen(NEGRO);

    drawBitmap(32,0,&df);

    char domotica[] = "Domótica";     // Importante para mostrar acentos
    char facil[] = "Fácil";           // Importante para mostrar acentos
    utf8ascii(domotica);
    utf8ascii(facil);


    backgroundLayer.setFont(font3x5);
    backgroundLayer.drawString(alignToCenter(32, 4, 8), 2, BLANCO, domotica);
    backgroundLayer.drawString(alignToCenter(32, 4, 5), 8, BLANCO, facil);
    backgroundLayer.drawString(alignToCenter(32, 4, 8), 14, BLANCO, "con Jota");
    backgroundLayer.drawString(alignToCenter(32, 4, 2), 20, BLANCO, "en");
    backgroundLayer.drawString(alignToCenter(32, 4, 7), 26, BLANCO, "YouTube");
    backgroundLayer.swapBuffers();

    currentMode = NO_ONE;
    delay(5000);
  }

  // Se solicita mostrar los datos de YouTube
  if (currentMode == YOUTUBE) {
    // Configuramos el fondo y la fuente
    backgroundLayer.fillScreen(NEGRO);
    backgroundLayer.setFont(gohufont11b);

    if (minute(currentTime) % 2 == 0) {
      // YouTube a la izquierda
      drawBitmap(0,0,&yt);
      // DF a la derecha
      drawBitmap(32,0,&df);

      backgroundLayer.drawString(alignToCenter(32, 6, strlen(subscribersInScreen)), 21, BLANCO, subscribersInScreen);
      backgroundLayer.swapBuffers();

    } else {
      // DF a la izquierda
      drawBitmap(0,0,&df);
      // YouTube a la derecha
      drawBitmap(32,0,&yt);

      backgroundLayer.drawString(32 + alignToCenter(32, 6, strlen(subscribersInScreen)), 21, BLANCO, subscribersInScreen);
      backgroundLayer.swapBuffers();

    }
  }

    // Se solicita mostrar los datos de YouTube
  if (currentMode == CUSTOM) {
    // ¿Problemas? Better Call Saul 
    DeserializationError error;

    // Sólo procedemos en el caso de que sea necesario
    if (deserilize) {
        error = deserializeJson(document, otaData);
        deserilize = false;
    }

    // Configuramos el fondo
    backgroundLayer.fillScreen(NEGRO);

    // En caso de error lo mostramos
    if (error) {
      Serial.print(F("deserializeJson() falló: "));
      Serial.println(error.c_str());
      return;
    }

    // Declaramos las constantes que utilizaremos para mostrar la información en el panel
    const char* titleMqtt = document["title"];
    const char* valueMqtt = document["value"];
    const char* imageMqtt = document["image"];

    if (minute(currentTime) % 2 == 0) {

      // Imagen a la izquierda
      if (titleMqtt && valueMqtt && imageMqtt) {
        drawBitmapMqtt(0, 0, imageMqtt);
        backgroundLayer.setFont(font3x5);
        backgroundLayer.drawString(alignToCenter(64, 4, strlen(titleMqtt)), 1, BLANCO, titleMqtt);
        backgroundLayer.setFont(font6x10);
        backgroundLayer.drawString(32 + alignToCenter(32, 6, strlen(valueMqtt)), 14, BLANCO, valueMqtt);

        backgroundLayer.swapBuffers();
      }
    } else {
      // DF a la izquierda

      // Imagen a la derecha
      if (titleMqtt && valueMqtt && imageMqtt) {
        drawBitmapMqtt(32, 0, imageMqtt);
        backgroundLayer.setFont(font3x5);
        backgroundLayer.drawString(alignToCenter(64, 4, strlen(titleMqtt)), 1, BLANCO, titleMqtt);
        backgroundLayer.setFont(font6x10);
        backgroundLayer.drawString(alignToCenter(32, 6, strlen(valueMqtt)), 14, BLANCO, valueMqtt);

        backgroundLayer.swapBuffers();      
      }
    }
  }

  delay(1000);

}
