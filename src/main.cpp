#include <Arduino.h>                              // Arduino
#include <SmartMatrix3.h>                         // Librería para controlar el panel LED

#include <constants.h>                            // Cabecera de constantes
#include <utils.h>                                // Cabecera de utilidades

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

// Función para dibujar los bitmaps
void drawBitmap(int16_t x, int16_t y, const gimp32x32bitmap* bitmap) {
  for(unsigned int i=0; i < bitmap->height; i++) {
    for(unsigned int j=0; j < bitmap->width; j++) {
      rgb24 pixel = { bitmap->pixel_data[(i*bitmap->width + j)*3 + 0],
                      bitmap->pixel_data[(i*bitmap->width + j)*3 + 1],
                      bitmap->pixel_data[(i*bitmap->width + j)*3 + 2] };

      backgroundLayer.drawPixel(x + j, y + i, pixel);
    }
  }
}


void setup() {
  // Don Quijote de la Mancha
  Serial.begin(115200);

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
  scrollingLayer.start("Has conectado un cable mal... noooo es broma, pero Jota es super guapo y lo sabes", 1);  

  delay(15000);

  // Paramos el desplazamiento del texto
  scrollingLayer.stop();
}

void loop() {
  // Nos aseguramos de que el fondo sea negro
  backgroundLayer.fillScreen(NEGRO);

  // Dibujamos el logo de Domótica Fácil con Jota
  drawBitmap(32,0,&df);

  // Preparamos el texto a mostrar al lado del logo
  char domotica[] = "Domótica";     // Importante para mostrar acentos
  char facil[] = "Fácil";           // Importante para mostrar acentos
  utf8ascii(domotica);
  utf8ascii(facil);

  // Esto es magia
  backgroundLayer.setFont(font3x5);
  backgroundLayer.drawString(alignToCenter(32, 4, 8), 2, BLANCO, domotica);
  backgroundLayer.drawString(alignToCenter(32, 4, 5), 8, BLANCO, facil);
  backgroundLayer.drawString(alignToCenter(32, 4, 8), 14, BLANCO, "con Jota");
  backgroundLayer.drawString(alignToCenter(32, 4, 2), 20, BLANCO, "en");
  backgroundLayer.drawString(alignToCenter(32, 4, 7), 26, BLANCO, "YouTube");
  backgroundLayer.swapBuffers();

  delay(5000);
}