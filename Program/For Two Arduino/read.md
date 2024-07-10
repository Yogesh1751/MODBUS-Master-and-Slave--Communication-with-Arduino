# 1. Slave Code Working 

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


# 2. Master Code Working 

            This code is an implementation of a Modbus master device using the ModbusRtu library in Arduino. It communicates with a Modbus slave to read holding registers. Here's a detailed explanation of each            part of the code:
        
   ### Includes and Definitions
        ```cpp
        #include <ModbusRtu.h>  // Include the Modbus RTU library
        
        uint16_t au16data[2]; // Array to store received data
        uint8_t u8state; // State variable
        uint8_t u8query; // Query variable
        ```
        - `uint16_t au16data[2];`: Array to store the data received from the slave.
        - `uint8_t u8state;`: Variable to keep track of the state of the master.
        - `uint8_t u8query;`: Variable to keep track of the query.
        
  ### Object Declaration
        ```cpp
        Modbus master(0, Serial, 2); // Modbus object declaration for master (Pin 2 for TX Enable)
        ```
        - `Modbus master(0, Serial, 2);`: This line declares a Modbus master object. The parameters are:
          - `0`: The master ID (not used, as the master does not have an ID).
          - `Serial`: The hardware serial port used for communication.
          - `2`: The pin number used for TX Enable, which controls the transmission of data.
        
  ### Telegram Structure Declaration
        ```cpp
        modbus_t telegram; // Modbus telegram structure
        unsigned long u32wait;
        ```
        - `modbus_t telegram;`: This declares a Modbus telegram structure, which is used to define the query sent to the slave.
        - `unsigned long u32wait;`: Variable to store the wait time.
        
   ### Setup Function
        ```cpp
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
        ```
        - `telegram.u8id = 1;`: Sets the slave address to 1.
        - `telegram.u8fct = 3;`: Sets the function code to 3 (read holding registers).
        - `telegram.u16RegAdd = 0;`: Sets the start address in the slave to 0.
        - `telegram.u16CoilsNo = 2;`: Sets the number of registers to read to 2.
        - `telegram.au16reg = au16data;`: Sets the pointer to the data array where the received data will be stored.
        - `Serial.begin(115200);`: Initializes serial communication with a baud rate of 115200.
        - `Serial.println("Master setup complete");`: Prints a setup completion message to the serial monitor.
        - `master.start();`: Starts the Modbus communication.
        - `master.setTimeOut(5000);`: Sets a timeout of 5000 milliseconds for the Modbus communication.
        - `u32wait = millis() + 1700;`: Sets an initial wait time of 1700 milliseconds.
        - `u8state = u8query = 0;`: Initializes the state and query variables to 0.
        
   ### Loop Function
        ```cpp
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
                for (int i = 0; i < 2; i++) {
                  Serial.print(au16data[i]);
                  Serial.print(" ");
                }
                Serial.println();
              }
              break;
          }
        }
        ```
        - `switch(u8state)`: This switch statement controls the state of the master.
        - `case 0`: Wait state.
          - `if (millis() > u32wait) u8state++;`: If the current time is greater than the wait time, move to the next state.
        - `case 1`: Send query state.
          - `master.query(telegram);`: Sends the query to the slave.
          - `u8state++;`: Move to the next state.
        - `case 2`: Check incoming messages state.
          - `master.poll();`: Checks for incoming messages from the slave.
          - `if (master.getState() == COM_IDLE)`: If the master is idle, process the received data.
            - `u8state = 0;`: Reset the state to 0.
            - `u32wait = millis() + 1700;`: Set the next wait time.
            - `Serial.print("Received Data: ");`: Print the received data to the serial monitor.
            - `for (int i = 0; i < 2; i++)`: Loop through the received data array and print each element.
              - `Serial.print(au16data[i]);`: Print the current element.
              - `Serial.print(" ");`: Print a space between elements.
            - `Serial.println();`: Print a new line.
        
   ### Summary
        This code sets up a Modbus master device that communicates with a Modbus slave to read holding registers. It uses a state machine to control the sequence of operations: waiting, sending a query, and              checking for incoming messages. The received data is printed to the serial monitor for debugging purposes. The Modbus communication is handled by the `ModbusRtu` library, which facilitates communication          between the master and slave devices.



### 3. Overview OF Both Master and Slave code

The two code snippets work together to establish communication between a Modbus master and a Modbus slave using the Modbus RTU protocol. The master requests data from the slave, and the slave responds with the requested data.

### Slave Code Explanation

```cpp
#include <ModbusRtu.h>

#define ID 1

Modbus slave(ID, Serial, 2); // Modbus object declaration for slave (Pin 2 for TX Enable)

uint16_t au16data; // Analog value to be sent to the master

void setup() {
  Serial.begin(115200); // Start serial communication
  Serial.println("Slave setup complete");

  slave.start(); // Start Modbus communication
}

void loop() {
  au16data = random(0, 1024);  // Assign random value between 0 and 1023

  slave.poll(&au16data, 1); // Send analog value to master
  
  Serial.print("Sent Data to Master: ");
  Serial.println(au16data);

  delay(750); // Delay before sending again
}
```

1. **Setup**:
   - The slave is assigned an ID of 1.
   - The serial communication is initialized at a baud rate of 115200.
   - Modbus communication is started.

2. **Loop**:
   - A random analog value is generated and assigned to `au16data`.
   - The `slave.poll(&au16data, 1);` function is called to handle Modbus communication and send the value to the master.
   - The value sent to the master is printed to the serial monitor.
   - A delay of 750 milliseconds is introduced before the next iteration.

### Master Code Explanation

```cpp
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
        for (int i = 0; i < 2; i++) {
          Serial.print(au16data[i]);
          Serial.print(" ");
        }
        Serial.println();
      }
      break;
  }
}
```

1. **Setup**:
   - The Modbus telegram structure is set up with the slave ID (1), function code (3 for reading holding registers), start address (0), number of registers to read (2), and the pointer to the data array (`au16data`).
   - Serial communication is initialized at a baud rate of 115200.Here you Can adjust the Baud Rate according to you.
   - Modbus communication is started.
   - A timeout of 5000 milliseconds is set for Modbus communication.
   - Initial wait time and state variables are set.

2. **Loop**:
   - A state machine controls the sequence of operations:
     - **State 0**: Waits until the current time exceeds the wait time, then transitions to the next state.
     - **State 1**: Sends the Modbus query to the slave using `master.query(telegram);` and transitions to the next state.
     - **State 2**: Polls for incoming messages using `master.poll();`. If the master is idle (`master.getState() == COM_IDLE`), it processes the received data:
       - Resets the state to 0.
       - Sets the next wait time.
       - Prints the received data to the serial monitor.

### How the Master Receives Data from the Slave

1. **Query Transmission**:
   - The master sends a query to the slave requesting to read holding registers (function code 3).
   - The query includes the slave ID (1), the start address (0), and the number of registers to read (2).

2. **Slave Response**:
   - The slave, upon receiving the query, processes it and retrieves the requested data (in this case, a random value stored in `au16data`).
   - The slave sends the data back to the master.

3. **Data Reception and Processing**:
   - The master, in its polling loop, checks for incoming messages.
   - When the master detects that it is idle and has received data, it processes the data.
   - The received data is stored in the `au16data` array and printed to the serial monitor.

This setup enables continuous communication between the master and the slave, with the master periodically requesting data and the slave responding with the requested values.
