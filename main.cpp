#include "I2C.h"
#include "ThisThread.h"
#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include <cstring>

#define TIEMPO_MUESTREO 1s       // Definición del tiempo de muestreo
#define TMP102_ADDRESS 0x90      // Dirección del sensor TMP102

// Inicialización de puertos y variables
BufferedSerial serial(USBTX, USBRX);
I2C i2c(D14, D15);
Adafruit_SSD1306_I2c oled(i2c, D0);
AnalogIn ain(A0);                // Pin del potenciómetro
char comando[3] = {0x01, 0x60, 0xA0}, data[2], men[40]; // Variables para el comando, datos y mensaje

// Función para inicializar la pantalla OLED
void initOLED() {
    oled.begin();                         // Inicia la pantalla OLED
    oled.setTextSize(1);                  // Configura el tamaño del texto
    oled.setTextColor(1);                 // Establece el color del texto
    oled.display();                        // Muestra los cambios
    ThisThread::sleep_for(3000ms);       // Espera 3 segundos
    oled.clearDisplay();                  // Limpia la pantalla
    oled.display();                        // Muestra la pantalla limpia
}

// Función para mostrar un valor en el OLED y en el puerto serial
void displayValue(const char* label, float value) {
    // Formatea el mensaje con la etiqueta y el valor
    sprintf(men, "%s\n\r %01u.%04u\n\r", label, int(value), int((value - int(value)) * 10000));
    oled.clearDisplay();                  // Limpia la pantalla
    oled.setTextCursor(0, 2);            // Establece el cursor para el texto
    oled.printf(men);                     // Imprime el mensaje en la pantalla
    oled.display();                       // Muestra el mensaje
    serial.write(men, strlen(men));      // Envía el mensaje por el puerto serial
}

// Función para leer el voltaje del potenciómetro
float readVoltage() { 
    return ain * 3.3;                   
}

// Función para leer la temperatura del sensor TMP102
float readTemperature() {
    // Envía el comando para leer el registro de temperatura
    i2c.write(TMP102_ADDRESS, (char[]){0}, 1);
    i2c.read(TMP102_ADDRESS, data, 2);   // Lee 2 bytes de datos
    return ((data[0] << 4) | (data[1] >> 4)) * 0.0625; // Convierte los datos leídos a temperatura
}

int main() {
    initOLED(); // Inicializa el OLED
    serial.write("Arranque del programa\n\r", 26); // Envia mensaje de arranque
    i2c.write(TMP102_ADDRESS, comando, 3); // Inicia el sensor TMP102

    while (true) {
        // Lee y muestra el voltaje y la temperatura en un bucle infinito
        displayValue("El voltaje es:", readVoltage());
        displayValue("La Temperatura es:", readTemperature());
        ThisThread::sleep_for(TIEMPO_MUESTREO); // Espera el tiempo de muestreo definido
    }
}