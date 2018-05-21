// serial_sender.cpp

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>
#include <thread>
#include "artnet_receiver.h"
#include "canvas/canvas_manager.h"
#include "canvas/canvas.h"

using namespace std;
using namespace Beamertool;

SerialSender::SerialSender(CanvasManager * screen, int canvases_id, unsigned int universe, unsigned int subnet, int dmx_start, float scale_multiplier, int led_gpio_pin) {

    // save parameters
    this->screen = screen;
    this->canvases_id = canvases_id;
    this->artnet_universe = universe;
    this->artnet_subnet = subnet;
    this->dmx_start = dmx_start;
    this->scale_multiplier = scale_multiplier;
    this->led_gpio_pin = led_gpio_pin;
    this->led_status = 0;
    this->led_packet_counter = 0;

    // Init Socket
    this->s = socket (AF_INET, SOCK_DGRAM , IPPROTO_UDP);
    if (this->s < 0) {
        printf ("init_net: Kann seriellen Port nicht öffnen ...(%s)\n", strerror(errno));
        exit (EXIT_FAILURE);
    }

    // check for root
    this->is_root = false;
    if (getuid() == 0) {
        this->is_root = true;
    }

    // init LED
    initLED();
    setLED(1);

    // Start net listener
    memset(&this->client_address, '\0', sizeof(this->client_address));
    this->quit = false;
    cyclic_sender_thread = thread(&SerialSender::cyclicSender, this);

}

SerialSender::cyclicSender() {
	
	//Bytes verschicken
}

SerialSender::setValues() {
	
	//Werte übernehmen
}
