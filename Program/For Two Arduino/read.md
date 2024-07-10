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
