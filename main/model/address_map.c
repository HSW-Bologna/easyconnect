#include <stdlib.h>
#include "address_map.h"

#define INDEX(bit)    (bit / ADDRESS_MAP_UNIT_SIZE_BITS)
#define BITSHIFT(bit) (bit % ADDRESS_MAP_UNIT_SIZE_BITS)
#define BITMAP(bit)   (1ULL << BITSHIFT(bit))


void address_map_set_bit(address_map_t *map, size_t bit) {
    map->map[INDEX(bit)] |= BITMAP(bit);
}


int address_map_find(address_map_t const *map, size_t start, size_t *found) {
    for (size_t i = start; i < ADDRESS_MAP_MAX_COUNT; i++) {
        if ((map->map[INDEX(i)] & BITMAP(i)) > 0) {
            *found = i;
            return 1;
        }
    }

    return 0;
}


int address_map_is_bit_set(address_map_t const *map, size_t bit) {
    return (map->map[INDEX(bit)] & BITMAP(bit)) > 0;
}