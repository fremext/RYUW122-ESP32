#include <HardwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

HardwareSerial mySerial(2); // Definir objeto para comunicación serial hardware en el puerto 2
#define NRST 23

int previousDistance = -1; // Variable para almacenar la distancia anterior

void setup() {
  pinMode(NRST, OUTPUT);
  Serial.begin(115200);
  
  // Inicializar la pantalla OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("No se pudo iniciar la pantalla OLED"));
    while (true);
  }

  display.clearDisplay(); // Limpiar la pantalla
  display.setTextSize(2); // Establecer tamaño del texto
  display.setTextColor(SSD1306_WHITE); // Establecer color del texto
  display.setCursor(15, 20); // Establecer posición del cursor
  display.println("@fremext"); // Texto a mostrar en la pantalla OLED
  display.display(); // Mostrar en la pantalla
  delay(2000); // Esperar 2 segundos

  // Esperar a que el puerto serial se conecte. Solo necesario para USB nativo
  while (!Serial) {
    ; 
  }
  
  // Inicializar comunicación serial para el módulo RYUW122
  mySerial.begin(115200, SERIAL_8N1, 16, 17); // Establecer velocidad de baudios y pines para ESP32

  HW_RESET(); // Realizar reseteo de hardware
  delay(1000); 
  
  // Configuración como Anchor
  sendATCommand("AT+MODE=1\r\n"); // Establecer el modo como Anchor
  readResponse(); // Leer la respuesta del módulo
  delay(1000);
  
  sendATCommand("AT+NETWORKID=REYAX123\r\n"); // Establecer ID de red
  readResponse(); // Leer la respuesta del módulo
  delay(1000);

  sendATCommand("AT+ADDRESS=REYAX003\r\n"); // Establecer dirección del dispositivo
  readResponse(); // Leer la respuesta del módulo
  delay(1000);

  sendATCommand("AT+CPIN=FABC0002EEDCAA90FABC0002EEDCAA90\r\n"); // Establecer clave de encriptación
  readResponse(); // Leer la respuesta del módulo
  delay(1000);
}

void loop() {
  // Transmitir datos al Tag y solicitar distancia
  sendATCommand("AT+ANCHOR_SEND=DAVID123,4,TEST\r\n");
  readResponse(); // Leer la respuesta del módulo
  delay(250); // Intervalo entre mediciones, ajustar según sea necesario
}

// Función para enviar comandos AT al módulo RYUW122
void sendATCommand(String command) {
  mySerial.print(command); // Enviar el comando a RYUW122
  delay(250); // Esperar antes de enviar el siguiente comando
}

// Función para leer la respuesta del módulo RYUW122
void readResponse() {
  long startTime = millis(); // Guardar el tiempo de inicio
  while (!mySerial.available() && millis() - startTime < 250) { 
    // Esperar la respuesta durante un máximo de 250 milisegundos
  }
  while (mySerial.available()) {
    Serial.write(mySerial.read()); // Eco de la respuesta en el monitor serie
    
    // Mostrar la distancia en la pantalla OLED
    display.clearDisplay(); // Limpiar la pantalla
    display.setTextSize(2); // Establecer tamaño del texto
    display.setTextColor(SSD1306_WHITE); // Establecer color del texto
    display.setCursor(50, 50); // Establecer posición del cursor
    display.println("Cm"); // Texto a mostrar en la pantalla OLED
    display.setTextSize(4); // Establecer tamaño del texto
    display.setCursor(30, 5); // Establecer posición del cursor
    String response = mySerial.readStringUntil('\n'); // Leer la respuesta del módulo
    int newDistance = response.substring(response.lastIndexOf(',') + 1).toInt(); // Obtener la distancia
    if (newDistance != previousDistance) { // Verificar si la distancia ha cambiado
      display.println(newDistance); // Mostrar la nueva distancia en la pantalla OLED
      previousDistance = newDistance; // Guardar la nueva distancia como la anterior
    }
    display.display(); // Mostrar en la pantalla OLED
  }
}

// Función para realizar un reseteo de hardware
void HW_RESET(){
      digitalWrite(NRST, LOW ); // Establecer pin NRST en bajo
      delay(500); // Esperar 500 milisegundos
      digitalWrite(NRST, HIGH); // Establecer pin NRST en alto
}
