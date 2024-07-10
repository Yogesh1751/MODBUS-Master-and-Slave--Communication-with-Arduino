# 1. Slave Code Working 

## 1.1 For Arduino 1
    This code is an implementation of a Modbus slave device using the ModbusRtu library in Arduino. Here’s a detailed explanation of each part of the code:
    
  ### Includes and Definitions
    ```cpp
    #include <ModbusRtu.h>  // Include the Modbus RTU library
    
    #define ID 1  // Define the Modbus slave ID as 1
    ```
    
   ### Object Declaration
    ```cpp
    Modbus slave(ID, Serial, 2);  // Create a Modbus slave object with ID 1, using the Serial port and pin 2 for TX Enable
    ```
    - `Modbus slave(ID, Serial, 2);`: This line declares a Modbus slave object. The parameters are:
      - `ID`: The slave ID, which is set to 1.
      - `Serial`: The hardware serial port used for communication.
      - `2`: The pin number used for TX Enable, which controls the transmission of data.
    
   ### Variable Declaration
    ```cpp
    uint16_t au16data;  // Declare a 16-bit unsigned integer to hold the analog value to be sent to the master
    ```
    - `uint16_t au16data;`: This declares a variable to store the analog value that will be sent to the Modbus master.
    
  ### Setup Function
    ```cpp
    void setup() {
      Serial.begin(115200);  // Initialize serial communication at 115200 baud rate
      Serial.println("Slave setup complete");  // Print a setup completion message
    
      slave.start();  // Start Modbus communication
    }
    ```
    - `Serial.begin(115200);`: Initializes the serial communication with a baud rate of 115200.
    - `Serial.println("Slave setup complete");`: Prints a message to the serial monitor indicating that the setup is complete.
    - `slave.start();`: Starts the Modbus communication.
    
   ### Loop Function
    ```cpp
    void loop() {
      au16data = random(0, 1024);  // Assign a random value between 0 and 1023 to au16data
    
      slave.poll(&au16data, 1);  // Send the value of au16data to the master
    
      Serial.print("Sent Data to Master: ");  // Print a message to the serial monitor
      Serial.println(au16data);  // Print the value of au16data to the serial monitor
    
      delay(750);  // Wait for 750 milliseconds before repeating
    }
    ```
    - `au16data = random(0, 1024);`: Assigns a random integer value between 0 and 1023 to `au16data`.
    - `slave.poll(&au16data, 1);`: Sends the value of `au16data` to the Modbus master. The `poll` function takes two parameters:
      - `&au16data`: The address of the data to be sent.
      - `1`: The number of registers to send (in this case, one register).
    - `Serial.print("Sent Data to Master: ");` and `Serial.println(au16data);`: These lines print the value of `au16data` to the serial monitor for debugging purposes.
    - `delay(750);`: Introduces a delay of 750 milliseconds before the loop repeats.
    
   ### Summary
    This code sets up a Modbus slave device that generates a random analog value between 0 and 1023, sends it to a Modbus master, and prints the value to the serial monitor every 750 milliseconds. The Modbus          communication is handled by the `ModbusRtu` library, which facilitates communication between the slave and master devices.

## 1.2 For Arduino 2

   ### Includes and Definitions
    ```cpp
    #include <ModbusRtu.h>  // Include the Modbus RTU library
    
    #define ID 2  // Define the Modbus slave ID as 2
    ```

    In 2 Arduino Only Id of Slave Device is change , other part is same.


# 2. Master For 2 Slave Arduino

This code is for an Arduino-based system that communicates with two Modbus slave devices using the Modbus RTU protocol over RS-485. The master device (Arduino) reads data from the two slaves and prints the received data to the Serial Monitor. Here's a detailed explanation of the code:

### Header Files and Global Variables

```cpp
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
```

- `au16data_slave1` and `au16data_slave2` are arrays used to store data read from the Modbus slaves.
- `u8state` is used to keep track of the current state of the state machine.
- `master` is an instance of the `Modbus` class, configured to use RS-485 with the Serial interface and pin 2 for direction control.
- `telegram` is a structure used to define Modbus queries.
- `u32wait` is a variable to manage timing.

### Setup Function

```cpp
void setup() {
  Serial.begin(19200); // Baud rate at 19200
  master.start();
  master.setTimeOut(2000); // If there is no answer in 2000 ms, roll over
  u32wait = millis() + 1000;
  u8state = 0; 
}
```

- Initializes the Serial communication at 19200 baud rate.
- Starts the Modbus master.
- Sets a timeout of 2000 ms for Modbus communication.
- Initializes the `u32wait` variable for the initial delay.
- Sets the initial state to 0.

### Loop Function

The loop function implements a state machine to manage Modbus queries and responses.

#### State 0: Wait State

```cpp
void loop() {
  switch (u8state) {
    case 0: 
      if (millis() > u32wait) u8state++; // Wait state
      break;
```

- In this state, the code waits until the current time (`millis()`) exceeds `u32wait`.

#### State 1: Send Query to Slave 1

```cpp
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
```

- Configures the Modbus query for slave 1 (address 1) to read 4 registers starting from address 0.
- Sends the query to slave 1.
- Prints a message indicating the query has been sent.
- Advances to the next state.

#### State 2: Poll for Response from Slave 1

```cpp
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
```

- Checks for incoming Modbus messages.
- If a response is received (`COM_IDLE` state), prints the data read from slave 1.
- Advances to the next state.

#### State 3: Send Query to Slave 2

```cpp
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
```

- Configures the Modbus query for slave 2 (address 2) to read 4 registers starting from address 0.
- Sends the query to slave 2.
- Prints a message indicating the query has been sent.
- Advances to the next state.

#### State 4: Poll for Response from Slave 2

```cpp
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
```

- Checks for incoming Modbus messages.
- If a response is received (`COM_IDLE` state), prints the data read from slave 2.
- Resets the state to 0 to start the process again.

### Delay ;

- If no data is receive from slave device 2 , then you need to adjust the Delay According to it .

### Summary

- The master device (Arduino) sends queries to two Modbus slaves to read registers.
- The responses from the slaves are printed to the Serial Monitor.
- The process repeats indefinitely in a loop, allowing continuous monitoring of the slave devices.


# 3. Working of Master and Slave Code

### Overview

The provided code sets up an Arduino as a Modbus slave device that generates and sends random analog values to a Modbus master. Here’s how the slave works and how the master receives data from it:

### Modbus Slave Code Explanation

#### Header Files and Constants

```cpp
#include <ModbusRtu.h>

#define ID   2
```

- Includes the Modbus RTU library.
- Defines the Modbus ID for the slave as 2.

#### Modbus Object and Global Variables

```cpp
Modbus slave(ID, Serial, 2); // Modbus object declaration for slave (Pin 2 for TX Enable)
uint16_t au16data; // Analog value to be sent to the master
```

- Creates a Modbus slave object with ID 2, using the Serial interface and pin 2 for TX enable.
- Declares a variable to hold the analog value to be sent to the master.

#### Setup Function

```cpp
void setup() {
  Serial.begin(115200); // Start serial communication
  Serial.println("Slave setup complete");

  slave.start(); // Start Modbus communication
}
```

- Initializes Serial communication at a baud rate of 115200.
- Prints a setup completion message to the Serial Monitor.
- Starts Modbus communication for the slave device.

#### Loop Function

```cpp
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
```

- Generates a random analog value between 0 and 1023.
- Sends the analog value to the master using the `slave.poll()` function.
- Prints the value sent to the master to the Serial Monitor.
- Waits for 750 milliseconds before repeating the process.

### How the Master Receives Data from Slaves

For the master to receive data from the slaves, it must be programmed to send Modbus requests to the slave devices and process their responses. Below is an example of how the master code might look and how it works with the slaves:

### Master Code Example

```cpp
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
}
```

### Explanation of Master Code

1. **Initialization**:
   - `Serial.begin(19200);` initializes the Serial communication at a baud rate of 19200.
   - `master.start();` starts the Modbus communication for the master device.
   - `master.setTimeOut(2000);` sets a timeout of 2000 ms for Modbus communication.
   - `u32wait = millis() + 1000;` sets an initial wait period.
   - `u8state = 0;` sets the initial state of the state machine.

2. **State Machine**:
   - **State 0**: Waits until the current time exceeds `u32wait`.
   - **State 1**: Sends a query to Slave 1 to read 4 registers starting from address 0. The data is stored in `au16data_slave1`.
   - **State 2**: Polls for a response from Slave 1. If a response is received, it prints the data and transitions to the next state.
   - **State 3**: Sends a query to Slave 2 to read 4 registers starting from address 0. The data is stored in `au16data_slave2`.
   - **State 4**: Polls for a response from Slave 2. If a response is received, it prints the data and transitions back to State 0 to repeat the process.

### Summary

- The **slave device** generates random analog values and makes them available for the master to read.
- The **master device** continuously sends read requests to the slaves and processes the responses.
- The master uses a state machine to manage communication with multiple slaves, ensuring data is read from each slave in turn.
- Received data is printed to the Serial Monitor for verification and monitoring.
