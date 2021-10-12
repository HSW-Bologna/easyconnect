#ifndef ADDRESS_MAP_H_INCLUDED
#define ADDRESS_MAP_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>


#define ADDRESS_MAP_MAX_COUNT      256
#define ADDRESS_MAP_UNIT_SIZE      sizeof(uint64_t)
#define ADDRESS_MAP_UNIT_SIZE_BITS (ADDRESS_MAP_UNIT_SIZE * 8)
#define ADDRESS_MAP_BYTES          (ADDRESS_MAP_MAX_COUNT / ADDRESS_MAP_UNIT_SIZE)


typedef struct {
    uint64_t map[ADDRESS_MAP_BYTES];
} address_map_t;


void address_map_set_bit(address_map_t *map, size_t bit);
int  address_map_is_bit_set(address_map_t const *map, size_t bit);
int  address_map_find(address_map_t const *map, size_t start, size_t *found);


#endif