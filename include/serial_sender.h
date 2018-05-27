// serial_sender.h

#ifndef SERIAL_SENDER_H
#define SERIAL_SENDER_H

//#include <netinet/in.h>
#include <string>
#include <vector>
#include <thread>
#include "canvas/canvas_classes.h"

#define LED_SWITCH_AFTER_PACKETS_NUM 2  // change LED every _NUM_ packets

namespace Beamertool {

    class SerialSender {
    
    public:
        /**
         * standard constructor
         * @param serialPortName
         * @param led_gpio_pin      GPIO Pin for Packet receive status LED
         */
        SerialSender(char * serialPortName, int serialPortBaud, int dmxChOut, int led_gpio_pin);
        
        /**
         * destructor
         */
        ~SerialSender();
        
         /**
         * set dmx values to send from external function
         */
        void setValues(int values[16]);
    
    private:
    
         /**
         * send serial packets cyclic as fast as the baudrate allows
         */
        void cyclicSender();

        /**
         * init the LED output (if root)
         */
        void initLED();
        
        /**
         * set the led on or of (if root)
         * @param value 0=led off; 1=led on
         */
        void setLED(int value);
        
        /**
         * reset the LED output (if root)
         */
        void quitLED();
        
        
        std::thread cyclic_sender_thread;           // cyclic sender thread
        bool quit;                                  // shall quit status
   
        int dmx[16] = {0};                          // dmx values
        int fd;                                     // handle to serial port
        char sendString[130];
        int dmxChOut;
        
        char * serialPortName;		                // Port Name 
		int serialPortBaud;							// Baudrate
        int led_gpio_pin;                           // Packet receive status LED pin
        int led_status;                             // LED status (on/off)
        int led_packet_counter;                     // packets received since last LED change
        bool is_root;                               // program runs with root privileges flag
        int sendCount = 0;							// DEBUG
    };
}

#endif // SERIAL_SENDER_H
