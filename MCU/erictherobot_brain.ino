/*
 
This is the firmware for Eric the Robot's brain.
It receives serial control commands and behaves accordingly.

*/

#include <Servo.h> 

//#include <SoftwareSerial.h>

//SoftwareSerial mySerial(5, 6); // RX, TX

/* Configuration section */

#define MAX_CMD_LENGTH 5     // Max length of accepted commands (including any arguments)
#define CMD_TERM_CHAR '.'    // Character that marks the end of a command
                             // (Not using a newline so we don't have to worry 
                             // about CR/LF stuff when debugging from a terminal)
#define SERIAL_BAUDRATE 9600 

#define MOTOR1_PIN 10
#define MOTOR1_CENTER 1643 // 1641
#define MOTOR2_PIN 9
#define MOTOR2_CENTER 1664 // 1662
#define MOTOR_BAND 141 // max absolute distance from the center that
                       // still changes the speed

#define LEFT_MOTOR MOTOR2
#define RIGHT_MOTOR MOTOR1

#define DEBUG_LED_PIN 13

/* Some macro magic */

#define MACRO_CONCAT(x,y) x##y       // This extra level of indirection is needed
                                     // to make the preprocessor evaluation work out
#define MOTOR_PIN(m) MACRO_CONCAT(m,_PIN)
#define MOTOR_CENTER(m) MACRO_CONCAT(m,_CENTER)

/* Initialization routine */

Servo motor_left, motor_right;

void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    motor_left.attach(MOTOR_PIN(LEFT_MOTOR));
    motor_right.attach(MOTOR_PIN(RIGHT_MOTOR));
    motor_left.writeMicroseconds(MOTOR_CENTER(LEFT_MOTOR));
    motor_right.writeMicroseconds  (MOTOR_CENTER(RIGHT_MOTOR));
    pinMode(DEBUG_LED_PIN, OUTPUT);
}

/* Main control loop */
void loop() {
  unsigned char cmd_buf[MAX_CMD_LENGTH+1]; // +1 to account for termination character
  unsigned char cmd_idx, i, cin;
  
  // Let everyone know we're ready
  Serial.println("Hello!");

  while (1) {
    // Display a prompt
    Serial.print("\r\n> ");
    
    cmd_idx = 0;
    
    /* Read in a command */
    do {
      while (!Serial.available()); // wait for input
   
      cin = Serial.read();
        
      cmd_buf[cmd_idx] = cin; // save 1 byte of command
      cmd_idx++;
      
      if (cmd_idx == MAX_CMD_LENGTH) {
        Serial.println("\r\ntoo long");
        if (cin != CMD_TERM_CHAR)
          while (Serial.read() != CMD_TERM_CHAR); // wait for term char to start over
        cmd_idx = 0;
        break;
      }
      
    } while (cin != CMD_TERM_CHAR); // terminate command on term char
    
    /* Process the command */
    if (cmd_idx > 1) {
      /* Echo it back, for debugging */
      Serial.print("\r\nOK: ");
      for(i = 0; i < cmd_idx; i++) {
        Serial.write(cmd_buf[i]);
      }
      
      switch(cmd_buf[0]) {
        case 'L': /* Control debug LED */ 
          if (cmd_buf[1] == '1') {
            digitalWrite(DEBUG_LED_PIN, HIGH);   // set the LED on
          } else if (cmd_buf[1] == '0') {
            digitalWrite(DEBUG_LED_PIN, LOW);   // set the LED off
          }
          break;
        case 'V': /* Set motor speeds */ 
          // Args: 1 byte left speed, 1 byte right speed  (128 is stopped)
          motor_left.writeMicroseconds(map(255-cmd_buf[1], 0, 255, // invert so motion is relative to front of robot
            MOTOR_CENTER(LEFT_MOTOR) - MOTOR_BAND, MOTOR_CENTER(LEFT_MOTOR) + MOTOR_BAND));
          motor_right.writeMicroseconds(map(cmd_buf[2], 0, 255, 
            MOTOR_CENTER(RIGHT_MOTOR) - MOTOR_BAND, MOTOR_CENTER(RIGHT_MOTOR) + MOTOR_BAND));
          break;
      }
    }
  }
}



