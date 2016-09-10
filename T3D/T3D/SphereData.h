#pragma once

#include <stdint.h>

struct SphereDef {
	uint32_t depth;
	uint32_t index;
	uint32_t parent;
	float x, y, z;
	uint16_t code;
};

#define NODE 0
#define LEAF 1
#define NUMNODES 5461

extern const SphereDef SphereData_Array[NUMNODES];