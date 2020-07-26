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

#include <PubSubClient.h>
#include <WiFi.h>
#include <constants.h>

unsigned long lastMsg = 0;
int value = 0;
char* subscribersInScreen = (char*)"00000";
char* currentSubscribers = (char*)"00000";
char* otaData;

boolean deserilize = false;

// MODOS
int currentMode = WELCOME;

// Llamada de vuelta sobre MQTT
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");

  // YOUTUBE
  if (strcmp(topic, SUBSCRIBERS_TOPIC) == 0) {
    payload[length] = '\0';
    subscribersInScreen = (char*)payload;

    if (strcmp(subscribersInScreen, currentSubscribers) == 0) {
      // TODO
    } else {
      currentSubscribers = subscribersInScreen;
    }
    currentMode = YOUTUBE;
  }

  // OTA IMAGE
  if (strcmp(topic, OTA_DATA_STD) == 0) {
    payload[length] = '\0';
    otaData = (char*)payload;

    deserilize = true;

    currentMode = CUSTOM;
  }
}

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void mqttReconnect() {
      Serial.print("mqttReconnect");

  // Reintentamos hasta que se conecte
  while (!client.connected()) {
    Serial.print("Intentando reconectar a MQTT");
    // Realizamos la conexión
    if (client.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASS)) {
      Serial.println("Conectado a MQTT");
      // Una vez nos hayamos conectado, publicamos
      client.publish(SUBSCRIBERS_TOPIC, subscribersInScreen);
      // También nos volvemos a suscribir
      client.subscribe(STATUS_TOPIC);
      client.subscribe(SUBSCRIBERS_TOPIC);
      client.subscribe(OTA_DATA_STD);
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" reintento en 5 segundos");
      delay(5000);
    }
  }
}