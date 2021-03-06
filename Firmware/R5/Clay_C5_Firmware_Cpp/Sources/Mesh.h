/*
 * Mesh.h
 *
 *  Created on: Nov 19, 2015
 *      Author: Ben
 */

#ifndef MESH_H_
#define MESH_H_

#include <stdint.h>
#include "PE_Types.h"

#define MESH_MAX_NODES 10
//
#define MESH_RESERVED_ADDRESS 254
//#define MESH_STARTUP_ADDRESS 253
#define MESH_STARTUP_ADDRESS 3
//
#define MESH_ALIVE_PERIOD_MS            5000
#define MESH_NODE_DISCONNECT_TIMEOUT_MS (3 * MESH_ALIVE_PERIOD_MS)
//
//enum for now
typedef enum
{
    MESH_CMD_CHANGE_MESH_MODE = 0x00,
    MESH_CMD_UPDATE_IMU_DATA = 0x01,
    MESH_CMD_ADDRESS_CLAIM_MSG = 0x02,
    MESH_CMD_MAX = 0x03   //max mesh command value.
} MeshCommandIndex;

#define MESH_CMD_TERMINATION            0xEE

//
#define ADDRESS_1                       1
#define ADDRESS_2                       0
#define ADDRESS_3                       0
#if !(ADDRESS_1 || ADDRESS_2 || ADDRESS_3)
#error must define address
#endif
//
typedef void (*cmd_func)(uint8_t * data, uint8_t len);

typedef enum
{
    _250uS = 0,
    _500uS = 1,
    _750uS = 2,
    _1000uS = 3,
    _1250uS = 4,
    _1500uS = 5,
    _1750uS = 6,
    _2000uS = 7,
    _2250uS = 8,
    _2500uS = 9,
    _2750uS = 10,
    _3000uS = 11,
    _3250uS = 12,
    _3500uS = 13,
    _3750uS = 14,
    _4000uS = 15
} mesh_HW_retry_interval;

typedef enum
{
    _1Mbps = 0,
    _2Mbps = 1
} mesh_radio_datarate;

typedef enum
{
    _18dBm = 0,
    _12dBm = 1,
    _6dBm = 2,
    _0dBm = 3
} mesh_radio_tx_attenuation;

typedef struct
{
    uint8_t cmd;
    cmd_func function;
} mesh_command;

extern bool mesh_rx_enabled;
extern bool mesh_messages_available;
extern LDD_TDeviceData *SPI_DeviceData;

extern mesh_command commands[];
extern uint32_t command_count;

extern bool Enable_Mesh();
extern bool Start_Mesh();
extern void Stop_Mesh();
extern void Pause_Mesh();
extern void Resume_Mesh();
extern void Reset_Mesh();

extern void mesh_init();
extern void mesh_register_callback(MeshCommandIndex command, cmd_func function);
extern void mesh_process_commands(void);
extern uint8_t mesh_rx(void * data, uint8_t * dataLength, uint8_t * source);
extern uint8_t mesh_tx(void * data, uint32_t dataLength, uint8_t destination);
extern uint8_t mesh_broadcast(void * data, uint32_t dataLength);
extern void mesh_discover_nodes_and_get_address();
extern uint8_t get_first_node();
extern uint8_t get_last_node();
extern uint8_t get_next_node(uint8_t startAddr);
extern void mesh_irq_handler(void);
extern bool set_RH_retry_count(uint8_t retries);
extern bool set_RH_timeout(uint16_t timeout_ms);
extern bool set_hw_retry_count(uint8_t retransmitCount);
extern bool set_hw_retry_delay(mesh_HW_retry_interval retransmitDelay);
extern void re_init_mesh_retries();

#endif /* MESH_H_ */
