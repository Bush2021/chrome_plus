/*
 * Test for the RandGen.inl
 */

#include "UnitTest.h"

#include <KNSoft/NDK/Package/RandGen.inl>

TEST_FUNC(RandGen)
{
    USHORT u16;
    UINT u32;
    UINT64 u64;
    size_t uSizeT;
    BYTE Buff3[3];
    BYTE Buff8[8];
    BYTE Buff12[12];

    TEST_OK(Rand_HW16(&u16));
    TEST_OK(Rand_HW32(&u32));
    TEST_OK(Rand_HW64(&u64));
    TEST_OK(Rand_HWSizeT(&uSizeT));
    u16 = Rand_SW16();
    u32 = Rand_SW32();
    u64 = Rand_SW64();
    uSizeT = Rand_SWSizeT();
    u16 = Rand_16();
    u32 = Rand_32();
    u64 = Rand_64();
    uSizeT = Rand_SizeT();
    TEST_OK(Rand_HWBuffer(Buff3, sizeof(Buff3)));
    TEST_OK(Rand_HWBuffer(Buff8, sizeof(Buff8)));
    TEST_OK(Rand_HWBuffer(Buff12, sizeof(Buff12)));
    Rand_SWBuffer(Buff3, sizeof(Buff3));
    Rand_SWBuffer(Buff8, sizeof(Buff8));
    Rand_SWBuffer(Buff12, sizeof(Buff12));
}
