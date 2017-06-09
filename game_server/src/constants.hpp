#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

/* Block related constants
 * Block padding defines the number of layers from the adjacent block that are
 * memorised within the current block
 */
#define CELL_TYPE uint8_t
#define BLOCK_PADDING 8
#define BLOCK_SIZE 1000
#define DEAD_CELL 0
#define LIVE_CELL 255
#define BITMAP_SIZE ((BLOCK_SIZE / 10) * (BLOCK_SIZE / 10))
#define BLOCK_FULL (BLOCK_SIZE + 2 * BLOCK_PADDING)

/* Block padding regions(used to optimise sync)
 */

#define BLOCK_NEIGHBOURING_REGIONS 8
#define P_REGION_NW 0
#define P_REGION_N 1
#define P_REGION_NE 2
#define P_REGION_W 3
#define P_REGION_E 4
#define P_REGION_SW 5
#define P_REGION_S 6
#define P_REGION_SE 7

/* Flag related constants
 */
#define FLAG_TYPE uint32_t
#define NO_LAST_MASK NO_31_MASK
#define JUST_LAST_MASK JUST_31_MASK
#define NO_31_MASK 0xfffffffe
#define NO_30_MASK 0xfffffffd
#define NO_29_MASK 0xfffffffb
#define NO_28_MASK 0xfffffff7
#define NO_27_MASK 0xffffffef
#define JUST_31_MASK 1
#define JUST_30_MASK 2
#define JUST_29_MASK 4
#define JUST_28_MASK 8
#define JUST_27_MASK 16
#define FULL_MASK 0xffffffff

/* Game limit related constants
 */
#define MINX -500
#define MINY -500
#define MAXX 499
#define MAXY 499
#define DEFAULT_GTC 1
#define DEFAULT_WAIT_TIME 30
#define LOG_MAX_BUFFER_DEFAULT 500
#define LOG_BUFFER_DELAY_DEFAULT 500

/* Database related constants
 */
#define DB_PORT 7778
#define EXPECT_READ 1
#define NO_READ 0
#define DB_MAX_BUF 2000

/* Math related constants
 */

#define FIXED_STRING "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define BASE_64_TABLE "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

/* Server related constants
 */
#define SV_DEF_PORT 7777
#define SV_MAX_PORT 65535
#define SV_MAX_LISTEN 5
#define SV_MAX_BUF 2000
#define SV_HTML_PATH "../client"
#define SV_HTTP_OK "HTTP/1.1 200 OK\n\n"
#define SV_HTTP_NOT_FOUND "HTTP/1.1 404 Not Found\n\n"
#define SV_HTTP_SWITCH "HTTP/1.1 101 Switching Protocols"
#define SV_HTTP_END "\r\n"

#endif
