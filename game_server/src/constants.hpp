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
#define BLOCK_FULL (BLOCK_SIZE + 2 * BLOCK_PADDING)

/* Flag related constants
 */
#define FLAG_TYPE uint32_t
#define NO_LAST_MASK NO_31_MASK
#define JUST_LAST_MASK JUST_31_MASK
#define NO_31_MASK 0xfffffffe
#define NO_30_MASK 0xfffffffd
#define NO_29_MASK 0xfffffffb
#define JUST_31_MASK 1
#define JUST_30_MASK 2
#define JUST_29_MASK 4
#define JUST_28_MASK 8
#define FULL_MASK 0xffffffff

/* Game limit related constants
 */
#define MINX -1000
#define MINY -1000
#define MAXX 1000
#define MAXY 1000
#define DEFAULT_GTC 1
#define DEFAULT_WAIT_TIME 30

#endif
