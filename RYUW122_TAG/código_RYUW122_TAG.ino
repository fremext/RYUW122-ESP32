#include <HardwareSerial.h>

HardwareSerial mySerial(2); // Definir objeto para comunicación serial hardware en el puerto 2
#define NRST 23

void setup() {
  pinMode(NRST, OUTPUT);
  Serial.begin(115200);
  
  // Esperar a que el puerto serial se conecte. Solo necesario para USB nativo
  while (!Serial) {
    ; 
  }
  
  // Inicializar comunicación serial para el módulo RYUW122
  mySerial.begin(115200, SERIAL_8N1, 16, 17); // Establecer velocidad de baudios y pines para ESP32

  HW_RESET(); // Realizar reseteo de hardware
  delay(1000); 
  
  // Configuración como Tag
  sendATCommand("AT+MODE=0\r\n"); // Establecer el modo como Tag
  readResponse(); // Leer la respuesta del módulo
  delay(1000);
  
  sendATCommand("AT+NETWORKID=REYAX123\r\n"); // Establecer ID de red
  readResponse(); // Leer la respuesta del módulo
  delay(1000);

  sendATCommand("AT+ADDRESS=DAVID123\r\n"); // Establecer dirección del dispositivo
  readResponse(); // Leer la respuesta del módulo
  delay(1000);

  sendATCommand("AT+CPIN=FABC0002EEDCAA90FABC0002EEDCAA90\r\n"); // Establecer clave de encriptación
  readResponse(); // Leer la respuesta del módulo
  delay(1000);
}

void loop() {
  // Transmitir datos al Anchor
  sendATCommand("AT+TAG_SEND=S,HELLO\r\n");
  readResponse(); // Leer la respuesta del módulo
  delay(250); // Intervalo entre transmisiones, ajustar según sea necesario
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
  }
}

// Función para realizar un reseteo de hardware
void HW_RESET(){
      digitalWrite(NRST, LOW ); // Establecer pin NRST en bajo
      delay(500); // Esperar 500 milisegundos
      digitalWrite(NRST, HIGH); // Establecer pin NRST en alto
}

