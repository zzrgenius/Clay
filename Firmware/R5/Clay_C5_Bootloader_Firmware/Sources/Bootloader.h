#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "Drivers/ESP8266.h"

//defines
#define APPLICATION_KEY_VALUE  0xA5A5A5A5U // The value that gets written into the shared variable.
#define BOOTLOADER_KEY_VALUE   0x00000000U
#define BOOT_START_ADDR        0x00000000U

#define FIRMWARE_SERVER_ADDRESS "107.170.180.158"
#define FIRMWARE_SERVER_PORT 3000

#define DEFAULT_FIRMWARE_CHECKSUM 0x0000
#define DEFAULT_FIRMWARE_SIZE 0 // The total size of the firmware to retrieve from the server.
#define FIRMWARE_BLOCK_SIZE 512 // The number of bytes to request from the firmware server in a single request.

#define CRC16 0x8005

typedef struct shared_bootloader_data
{
    uint32_t ApplicationKey; // Set in bootloader and application. Indicates whether the bootloader was started automatically on boot, or jumped to from the main application firmware.
    bool UpdateApplication; // Set in application. Indicates to the bootloader to perform an update.
    bool ApplicationUpdateAvailable; // Set in bootloader. Set to true if remote server contains different firmware.
    uint16_t pad;
} shared_bootloader_data;

extern shared_bootloader_data SharedData;

extern uint8_t Initialize_Bootloader ();

extern bool Has_User_Requested_Update ();

extern uint8_t Verify_Firmware ();

extern uint8_t Has_Latest_Firmware ();

extern uint16_t Calculate_Checksum_On_Bytes (const uint8_t *data, uint32_t size);

extern uint8_t Verify_Firmware_Bytes (const uint8_t *bytes, uint32_t length, uint16_t expected_checksum);

extern uint8_t Write_Firmware_Bytes (uint32_t address, const uint8_t *bytes, uint32_t length);

extern uint8_t Update_Firmware ();

extern void Jump_To_Application ();

extern void Disable_Interrupts ();

#endif /* BOOTLOADER_H */
