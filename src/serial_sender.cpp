// serial_sender.cpp
#include <wiringPi.h> //for delay()
#include <wiringSerial.h>

#include <sys/types.h>
//#include <sys/socket.h>
#include <sys/ioctl.h>
//#include <net/if.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <ifaddrs.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>
#include <thread>
#include "serial_sender.h"
//#include "canvas/canvas_manager.h"
//#include "canvas/canvas.h"

using namespace std;
using namespace Beamertool;

SerialSender::SerialSender(char * serialPortName, int serialPortBaud, int dmxChOut, int led_gpio_pin) {

    // save parameters
    this->serialPortName = serialPortName; //"/dev/ttyAMA0" onboard "/dev/ttyUSB0" usb
    this->serialPortBaud = serialPortBaud;
    this->dmxChOut = dmxChOut;
    this->led_gpio_pin = led_gpio_pin; //<0 -> deaktiviert
    this->led_status = 0;
    this->led_packet_counter = 0;

    // Init Serial Port
    //this->fd = serialOpen (this->serialPortName,this->serialPortBaud);
    this->fd = serialOpen ("/dev/ttyUSB0",9600);
    if (this->fd < 0) {
        printf ("init_net: Kann seriellen Port nicht öffnen ...(%s)\n", strerror(errno));
        exit (EXIT_FAILURE);
    }

    // check for root
    this->is_root = false;
    if (getuid() == 0) {
        this->is_root = true;
    }

    // init LED
    //initLED();
    //setLED(1);

    // Start net listener
    //memset(&this->client_address, '\0', sizeof(this->client_address));
    this->quit = false;
    cyclic_sender_thread = thread(&SerialSender::cyclicSender, this);

}

SerialSender::~SerialSender() {
    // stop listening
    this->quit = true;
    cyclic_sender_thread.join();
    //close serial Port
    serialClose (this->fd);
    
    // poweroff led
    //setLED(0);
    //quitLED();
}

void SerialSender::cyclicSender() {
	while (!this->quit)
    {
		
		//Byte String zusammenbauen
		//Beispiel:  "1c127w2c127w3c127w4c127w\n"
		//Erklärung: 1c -> Kanal 1, 127w -> Wert von diesem Kanal auf 127 setzen
		//           der Empfänger (Arduino) übernimmt einen Wert immer mit dem 'w'
		//           am Ende \n um die Reflektion vom Arduino im Serial Monitor besser beobachten zu können
		//***TODO***
		sprintf(this->sendString,"\0");
		for (int i=0; i<this->dmxChOut; i++)
		{
			sprintf(this->sendString,"%s%ic%iw",this->sendString,1+i,this->dmx[i]); //Startadresse für DMX Out hardcoded = 1 ***MAGIC***
		}
		sprintf(this->sendString,"%s\n",this->sendString);
	
		//Bytes verschicken
		/*
		if (sendCount == 0)
		{
			serialPrintf(this->fd, "1c127w2c127w3c127w4c127w\n"); 
			sendCount++;
		}
		else
		{
			serialPrintf(this->fd, "1c120w2c120w3c120w4c120w\n"); 
			sendCount = 0;
		}
		sleep(1); //test
		*/
		//serialPrintf(this->fd, this->sendString); 
		serialPuts(this->fd, this->sendString); 
		//delay (100); //9600 bit/s : (130*8) bit -> 9,2Hz ~10 Hz -> dabei werden irgendwo trotzdem bytes verschluckt
		delay (200); //5Hz läuft stabil
		//bei 115200 sollte es 12x schneller gehen
	}
}

void SerialSender::setValues(int values[16]) {
	//Werte übernehmen
	for (int i=0; i<16; i++)
	{
		this->dmx[i] = values[i];
	}
}

void SerialSender::initLED() {
    if (this->is_root && this->led_gpio_pin >= 0) {
        FILE *ptr;

        ptr = fopen("/sys/class/gpio/export", "w");
        fprintf(ptr, "%d", this->led_gpio_pin);	// enable GPIO-Port	
        fclose(ptr);

        char gpio[50];
        sprintf(gpio, "/sys/class/gpio/gpio%d/direction", this->led_gpio_pin);
        ptr = fopen(gpio, "w");
        fprintf(ptr, "out");  // set Port to output
        fclose(ptr);
    }
}

void SerialSender::setLED(int value) {
    if (this->is_root && this->led_gpio_pin >= 0) {
        int set_value;
        if (value >= 1) {
            set_value = 1;
            this->led_status = 1;
        } else {
            set_value = 0;
            this->led_status = 0;
        }

        FILE *ledptr;
        char gpio[50];
        sprintf(gpio, "/sys/class/gpio/gpio%d/value", this->led_gpio_pin);
        ledptr = fopen(gpio, "w");
        fprintf(ledptr, "%d", set_value);
        fclose(ledptr);
    }
}

void SerialSender::quitLED() {
    if (this->is_root && this->led_gpio_pin >= 0) {
        FILE *ptr;
        ptr = fopen("/sys/class/gpio/unexport", "w");
        fprintf(ptr, "%d", this->led_gpio_pin);
        fclose(ptr);
    }
}
