#ifndef SCANCONST_H
#define SCANCONST_H

#include <QString>

static char RX_PREFIX = 0xAD;
static char TX_PREFIX = 0xAC;
static char DEV_ADDRESS = 0x56;

static char bootloaderDevId[] = { 'B', 'O', 'O', 'T' };
static char bootloaderDeviceProtocolVersion[] = { 0, 2 };
static char scriptDevId[] = { 'P', 'A', 'W', 'N' };
static char scriptDeviceProtocolVersion[] = { 0, 2 };

typedef struct Frame frame_t;
typedef struct WriteState writeState_t;

typedef enum Cmd {
    S_INFO        = 0xE1,
    S_MODE        = 0xE2,
    S_FLASH       = 0xE3,
    S_WRITE       = 0xE4,
    S_SIGNATURE   = 0xE5,
    S_CONSOLE_LOG = 0xED,
    B_INFO        = 0xF1,
    B_MODE        = 0xF2,
    B_FLASH       = 0xF3,
    B_WRITE       = 0xF4,
    B_SIGNATURE   = 0xF5
} cmd_t;

typedef enum Mode {
    STATES      = 0x00,
    STOP        = 0x01,
    START       = 0x02,
    STOP_WRITE  = 0x81,
    START_WRITE = 0x82,
    MODE_BOOT   = 0x0B,
    MODE_APP   = 0x0A
} mode_t;

typedef enum FlashRequest
{
    STATE   = 0x01,
    UNLOC   = 0x02,
    ERASE   = 0x03,
    CRC     = 0x04,
    JUMP    = 0x05
} flashRequest_t;

typedef enum FlashResponse
{
    LOCKED      = 0x01,
    UNLOCKED    = 0x02,
    ERASED      = 0x03,
    ERASE_ERROR = 0x43,
    ERASE_BUSY  = 0x83,
    CRC_OK      = 0x04,
    CRC_ERROR   = 0x44,
    CRC_BUSY    = 0x84,
    JUMP_OK     = 0x05,
    JUMP_DENIED = 0x25,
    JUMP_ERROR  = 0x45,
    JUMP_BUSY   = 0x85,
} flashResponse_t;

typedef enum WriteResponse
{
    SUCCESS         = 0x01,
    ERROR_ADDRESS   = 0x02,
    ERROR_WRITE     = 0x03,
    ERROR_ERASE     = 0x04,
    ERROR_SYGNATURE = 0x05,
    BUSY            = 0x06,
} writeResponse_t;

typedef enum WriteStatus
{
    W_STARTING        = 0x01,
    W_CONNECTED       = 0x02,
    W_WRITING         = 0x03,
    W_SUCCESS         = 0x04,
    W_ERROR           = 0x05,
} writeStatus_t;

struct Frame
{
    char prefix;
    char address;
    cmd_t cmd;
    char size;
    char data[128];
    unsigned char crc;
    int index = 0;
};


struct WriteState
{
    writeStatus_t status;
    uint32_t progress;
    uint32_t size;
    QString message;
    QString console;
};


class ScanConst
{
public:
    static unsigned char getCrc(unsigned char crc, unsigned char b)
    {
        const unsigned char hash_table[256] =
          {0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
          157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
          35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
          190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
          70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
          219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
          101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
          248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
          140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
          17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
          175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
          50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
          202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
          87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
          233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
          116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53};

        return hash_table[crc ^ b];
    }

};

#endif // SCANCONST_H
