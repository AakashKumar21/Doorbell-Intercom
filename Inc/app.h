#include "main.h"
#include <stdio.h>
#include "mcp4725.h"

enum StatusFlag{
    NoFlag,
    SRingFlag,
    LRingFlag
};

enum PacketType{
    AudioType,
    SRingType,
    LRingType
};

enum RingType{
    Short,
    Long
};

void app_setup();