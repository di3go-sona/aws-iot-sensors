/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       Test application for GNRC LoRaWAN
 *
 * @author      José Ignacio Alamos <jose.alamos@haw-hamburg.de>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thread.h"
#include "xtimer.h"
#include "shell.h"
#include "shell_commands.h"
#include <random.h>


#include "board.h"

#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"

#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc/netreg.h"

#define LORAWAN_PORT (2U)

#define INTERVAL 10 

int get_humidity(void)
{
  int upper = 100;
  int lower = 0;
  int r_num = (int) random_uint32();
  int humidity = ((r_num*1000) % (upper - lower + 1)) + lower;

  return humidity;
}

int get_temperature(void)
{
  int upper = 50;
  int lower = -50;
  int r_num = (int) random_uint32();
  int temperature = ((r_num*1000) % (upper - lower + 1)) + lower;

  return temperature;
}

int get_rain(void)
{
  int upper = 50;
  int lower = 0;
  int r_num = (int) random_uint32();
  int temperature = ((r_num*1000) % (upper - lower + 1)) + lower;

  return temperature;
}

int get_wind(void)
{
  int upper = 100;
  int lower = 0;
  int r_num = (int) random_uint32();
  int temperature = ((r_num*1000) % (upper - lower + 1)) + lower;

  return temperature;
}

int get_wind_dir(void)
{
  int upper = 360;
  int lower = 0;
  int r_num = (int) random_uint32();
  int temperature = ((r_num*1000) % (upper - lower + 1)) + lower;

  return temperature;
}



static void _tx_usage(void)
{
    puts("usage: send <if_pid> <payload> [port]");
}
static void _rand_tx_usage(void)
{
    puts("usage: send <if_pid> [port]");
}

int tx_cmd(int argc, char **argv)
{
    gnrc_pktsnip_t *pkt;
    uint8_t port = LORAWAN_PORT; /* Default: 2 */
    int interface;


    if(argc < 3) {
        _tx_usage();
        return 1;
    }

    interface = atoi(argv[1]);
    /* handle optional parameters */
    if (argc > 3) {
        port = atoi(argv[3]);
        if (port == 0 || port >= 224) {
            printf("error: invalid port given '%d', "
                   "port can only be between 1 and 223\n", port);
            return 1;
        }
    }

    pkt = gnrc_pktbuf_add(NULL, argv[2], strlen(argv[2]), GNRC_NETTYPE_UNDEF);

    /* register for returned packet status */
    if (gnrc_neterr_reg(pkt) != 0) {
        puts("Can not register for error reporting");
        return 0;
    }

    gnrc_netapi_set(interface, NETOPT_LORAWAN_TX_PORT, 0, &port, sizeof(port));
    gnrc_netif_send(gnrc_netif_get_by_pid(interface), pkt);

    msg_t msg;
    /* wait for packet status and check */
    msg_receive(&msg);
    if ((msg.type != GNRC_NETERR_MSG_TYPE) ||
        (msg.content.value != GNRC_NETERR_SUCCESS)) {
        puts("Error sending packet (not joined?)");
    }
    else {
        puts("Successfully sent packet");
    }
    return 0;
}

int rand_tx_cmd(int argc, char **argv)
{
  int temperature, humidity, rain, wind, wind_dir;
  char buf[100];
  char* args[4];


  // Check if the interface id was set, if not exit
  if ( argc < 2 ){
    _rand_tx_usage();
    return 1;
  }

  while(1){
     // Get Random Measurement
    temperature = get_temperature();
    humidity = get_humidity();
    rain = get_rain();
    wind = get_wind();
    wind_dir = get_wind_dir();

    // Create a string buffer that will be sent with LoRa
    // Set buffer to all zeros  
    memset((char*)&buf,0,100);
    // Write Measurement to buffer 
    snprintf( (char*)&buf,100, "%d, %d, %d, %d, %d \n",temperature, humidity, rain, wind, wind_dir);
    // Instead of sending the message I will do something quite ugly 
    // but highly functional, here I'm simply calling a command that 
    // will perform all the operations required to send the message

    args[0] = "tx";
    args[1] = argv[1];
    args[2] = (char*)&buf;
    args[3] = NULL;
    tx_cmd(3,args);
    
    // Sleep 
    xtimer_sleep((uint32_t) INTERVAL);
  }

  return 0;
}

static const shell_command_t shell_commands[] = {
    { "send",       "Send LoRaWAN data",     tx_cmd},
    { "rand_send",       "Send random sensors LoRaWAN data",     rand_tx_cmd},
    { NULL, NULL, NULL }
};


int main(void)
{
    /* start the shell */
    puts("Initialization successful - starting the shell now");
    gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(LORAWAN_PORT,
                                                          gnrc_pktdump_pid);
    gnrc_netreg_register(GNRC_NETTYPE_LORAWAN, &dump);
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
