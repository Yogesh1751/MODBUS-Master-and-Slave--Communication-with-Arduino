
//Master Code 

#include <ModbusRtu.h>

// Data arrays for Modbus network sharing
uint16_t au16data_slave1[16];
uint16_t au16data_slave2[16];
uint8_t u8state;

// Modbus object declaration
Modbus master(0, Serial, 2); // RS-485, pin 2 controls direction

// Modbus query structure
modbus_t telegram;

unsigned long u32wait;

void setup() {
  Serial.begin(19200); // Baud rate at 19200
  master.start();
  master.setTimeOut(2000); // If there is no answer in 2000 ms, roll over
  u32wait = millis() + 1000;
  u8state = 0; 
}

void loop() {
  switch (u8state) {
    case 0: 
      if (millis() > u32wait) u8state++; // Wait state
      break;
    case 1: 
      telegram.u8id = 1; // Slave 1 address
      telegram.u8fct = 3; // Function code (registers read)
      telegram.u16RegAdd = 0; // Start address in slave
      telegram.u16CoilsNo = 4; // Number of elements (coils or registers) to read
      telegram.au16reg = au16data_slave1; // Pointer to a memory array in the Arduino

      master.query(telegram); // Send query to Slave 1
      Serial.println("Query sent to slave 1.");
      u8state++;
      delay(500);
      break;
      delay(500);
      
    case 2:
      master.poll(); // Check incoming messages
      if (master.getState() == COM_IDLE) {
        u8state = 3;
        u32wait = millis() + 100; 
        Serial.println("Response received from slave 1:");
        for (int i = 0; i < telegram.u16CoilsNo; i++) {
          Serial.print(au16data_slave1[i]);
          Serial.print(" ");
        }
        Serial.println();
      }
      
      delay(500);
      break;
      delay(500);
    case 3: 
      telegram.u8id = 2; // Slave 2 address
      telegram.u8fct = 3; // Function code (registers read)
      telegram.u16RegAdd = 0; // Start address in slave
      telegram.u16CoilsNo = 4; // Number of elements (coils or registers) to read
      telegram.au16reg = au16data_slave2; // Pointer to a memory array in the Arduino

      master.query(telegram); // Send query to Slave 2
      Serial.println("Query sent to slave 2.");
      u8state++;
      delay(500);
      break;
      delay(500);
    case 4:
      master.poll(); // Check incoming messages
      if (master.getState() == COM_IDLE) {
        u8state = 0;
        u32wait = millis() + 100; 
        Serial.println("Response received from slave 2:");
        for (int i = 0; i < telegram.u16CoilsNo; i++) {
          Serial.print(au16data_slave2[i]);
          Serial.print(" ");
        }
        Serial.println();
      }
      break;
  }
  //delay(500);
}