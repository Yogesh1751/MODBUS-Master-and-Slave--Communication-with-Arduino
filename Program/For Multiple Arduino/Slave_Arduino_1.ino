#include <ModbusRtu.h>

#define ID   1

Modbus slave(ID, Serial, 2); // Modbus object declaration for slave (Pin 2 for TX Enable)

uint16_t au16data; // Analog value to be sent to the master

void setup() {
  Serial.begin(115200); // Start serial communication
  Serial.println("Slave setup complete");

  slave.start(); // Start Modbus communication
}

void loop() {
  // Assign random analog value
  au16data = random(0, 1024);  // Random value between 0 and 1023

  // Send analog value to master
  slave.poll(&au16data, 1);
  
  // Print the data sent to the master
  Serial.print("Sent Data to Master: ");
  Serial.println(au16data);

  delay(750); // Delay before sending again (adjust as needed)
}
