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
