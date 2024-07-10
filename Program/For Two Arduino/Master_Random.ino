#include <ModbusRtu.h>

uint16_t au16data[2]; // Array to store received data
uint8_t u8state; // State variable
uint8_t u8query; // Query variable

Modbus master(0, Serial, 2); // Modbus object declaration for master (Pin 2 for TX Enable)

modbus_t telegram; // Modbus telegram structure

unsigned long u32wait;

void setup() {
  telegram.u8id = 1; // Slave address
  telegram.u8fct = 3; // Function code (read holding registers)
  telegram.u16RegAdd = 0; // Start address in slave
  telegram.u16CoilsNo = 2; // Number of elements (registers) to read
  telegram.au16reg = au16data; // Pointer to data array

  Serial.begin(115200); // Start serial communication
  Serial.println("Master setup complete");

  master.start(); // Start Modbus communication
  master.setTimeOut(5000); // Set timeout
  u32wait = millis() + 1700; // Initial wait time
  u8state = u8query = 0; // Initialize state and query variables
}

void loop() {
  switch(u8state) {
    case 0: 
      if (millis() > u32wait) u8state++; // Wait state
      break;
    case 1: 
      master.query(telegram); // Send query
      u8state++;
      break;
    case 2:
      master.poll(); // Check incoming messages
      if (master.getState() == COM_IDLE) {
        u8state = 0; // Reset state
        u32wait = millis() + 1700; // Set next wait time
        
        // Print the data received from the slave
        Serial.print("Received Data: ");
        for (int i = 0; i < 1; i++) {
          Serial.print(au16data[i]);
          Serial.print(" ");
        }
        Serial.println();
        u8state = 0 ;
      }
      break;
  }
}