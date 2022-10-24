#ifndef IHEXLOAD_H
#define IHEXLOAD_H

#include <stdint.h>
#include <stdbool.h>

void vdu_sys_hexload(void);

typedef uint_least32_t ihex_address_t;
typedef uint_least16_t ihex_segment_t;
typedef int ihex_count_t;

#define IHEX_LINE_MAX_LENGTH 255
#define IHEX_LINEAR_ADDRESS(ihex) ((ihex)->address + (((ihex_address_t)((ihex)->segment)) << 4))
#define IHEX_BYTE_ADDRESS(ihex, byte_index) ((((ihex)->address + (byte_index)) & 0xFFFFU) + (((ihex_address_t)((ihex)->segment)) << 4))
#define IHEX_NEWLINE_STRING "\n"

enum ihex_flags {
    IHEX_FLAG_ADDRESS_OVERFLOW = 0x80   // 16-bit address overflow
};
typedef uint8_t ihex_flags_t;

typedef struct ihex_state {
    ihex_address_t  address;
    ihex_address_t  next_address;                      // modified: to verify padding requirements between records
    ihex_segment_t  segment;
    ihex_flags_t    flags;
    uint8_t         line_length;
    uint8_t         length;
    uint32_t        total;                            // modified: to track number of bytes sent total
    uint8_t         data[IHEX_LINE_MAX_LENGTH + 1];
    bool     checksum_error;
    bool     length_error;
} kk_ihex_t;

enum ihex_record_type {
    IHEX_DATA_RECORD,
    IHEX_END_OF_FILE_RECORD,
    IHEX_EXTENDED_SEGMENT_ADDRESS_RECORD,
    IHEX_START_SEGMENT_ADDRESS_RECORD,
    IHEX_EXTENDED_LINEAR_ADDRESS_RECORD,
    IHEX_START_LINEAR_ADDRESS_RECORD
};
typedef uint8_t ihex_record_type_t;


// Begin reading at address 0
void ihex_begin_read(struct ihex_state *ihex);

// Read a single character
void ihex_read_byte(struct ihex_state *ihex, char chr);

// Read `count` bytes from `data`
void ihex_read_bytes(struct ihex_state * ihex,
                     const char * data,
                     ihex_count_t count);

// End reading (may call `ihex_data_read` if there is data waiting)
void ihex_end_read(struct ihex_state *ihex);

extern bool ihex_data_read(struct ihex_state *ihex,
                                  ihex_record_type_t type,
                                  bool checksum_mismatch);

// Begin reading at `segment`; this is required only if the initial segment
// is not specified in the input data and it is non-zero.
//
void ihex_read_at_segment(struct ihex_state *ihex, ihex_segment_t segment);
#endif
