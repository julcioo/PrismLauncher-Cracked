#pragma once

#include <cstdint>

extern "C" uint8_t __binary_bigtest_uncompr_start[];
extern "C" uint8_t __binary_bigtest_uncompr_end[];

extern "C" uint8_t __binary_littletest_uncompr_start[];
extern "C" uint8_t __binary_littletest_uncompr_end[];

extern "C" uint8_t __binary_errortest_eof1_start[];
extern "C" uint8_t __binary_errortest_eof1_end[];

extern "C" uint8_t __binary_errortest_eof2_start[];
extern "C" uint8_t __binary_errortest_eof2_end[];

extern "C" uint8_t __binary_errortest_noend_start[];
extern "C" uint8_t __binary_errortest_noend_end[];

extern "C" uint8_t __binary_errortest_neg_length_start[];
extern "C" uint8_t __binary_errortest_neg_length_end[];

extern "C" uint8_t __binary_toplevel_string_start[];
extern "C" uint8_t __binary_toplevel_string_end[];

extern "C" uint8_t __binary_bigtest_nbt_start[];
extern "C" uint8_t __binary_bigtest_nbt_end[];

extern "C" uint8_t __binary_bigtest_zlib_start[];
extern "C" uint8_t __binary_bigtest_zlib_end[];

extern "C" uint8_t __binary_bigtest_corrupt_nbt_start[];
extern "C" uint8_t __binary_bigtest_corrupt_nbt_end[];

extern "C" uint8_t __binary_bigtest_eof_nbt_start[];
extern "C" uint8_t __binary_bigtest_eof_nbt_end[];

extern "C" uint8_t __binary_trailing_data_zlib_start[];
extern "C" uint8_t __binary_trailing_data_zlib_end[];
