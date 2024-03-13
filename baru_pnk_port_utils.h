#ifndef __BARU_PNK_PORT_UTILS_H__
#define __BARU_PNK_PORT_UTILS_H__

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

int16_t pnk_utils_calc_crc16(const char *msg, size_t len);


#ifdef __cplusplus
}
#endif

#endif
