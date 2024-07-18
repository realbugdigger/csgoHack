#pragma once
#include <windows.h>
#include "csgoVector.h"

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}


class SDKEnt
{
public:
    union
    {
        DEFINE_MEMBER_N(SDKVec3, origin, 0x138);
        DEFINE_MEMBER_N(int, clientId, 0x64);
        DEFINE_MEMBER_N(SDKVec3, m_vecViewOffset, 0x108);
    };
}; //Size: 0x0284

class SDKEntListObj
{
public:
    class SDKEnt* ent; //0x0000
    char pad_0004[12]; //0x0004
}; //Size: 0x0010

class SDKEntList
{
public:
    SDKEntListObj entListObjs[32]; //0x0000
}; //Size: 0x0200

