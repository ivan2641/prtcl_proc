#ifndef __BARU_PNK_PORT_GNSS_H__
#define __BARU_PNK_PORT_GNSS_H__

#include "baru_pnk_port_utils.h"


#ifdef __cplusplus
extern "C" {
#endif

/// @brief Вспомогательная структура
///
/// @field utc: время UTC от начала суток, к которому относятся координаты, lsb 1
/// @field lat: широта, -229…+229
/// @field lon: долгота, -230…+230
/// @field alt: геометрическая высота, фут, -1300… +129950
/// @field alt_rate: геометрическая вертикальная скорость, фут/мин, -31500… +31500
/// @field w_speed: путевая скорость, узлы, 0…11256
/// @field w_direct: путевой угол, 	0…4095
/// @field sat_num: количество спутников в уверенном приеме, шт, 0…16
typedef struct pnk_gnss_t {
    uint32_t utc;
    int32_t lat;
    int32_t lon;
    int32_t alt;
    int16_t alt_rate;
    uint16_t w_speed;
    uint16_t w_direct;
    uint8_t sat_num;
} pnk_gnss_t;

/// @brief Функция перекладки. Перекладывает из holy ghost в структуру pnk_gnss_t
/// @param arr holy ghost - //TBD: дописать позже. Понять откуда нужно переложить
/// @param size Длина массива telemetry
/// @param gnss вспомогательная структура
/// @return //TBD: to think what can be returned
int __pnk_port_put_gnss_coord__(const void *arr, size_t len, pnk_gnss_t *gnss);

/// @brief Функция выдачи из вспомогательной структуры pnk_gnss_t + расчет crc
/// @param msg Байтовый массив, в который необходимо сложить поля структуры
/// @param len Длина полученного массива
/// @param gnss вспомогательня структура
/// @return -1 - нулевая структура pnk_gnss_t, -2 - недостаточный размер буфера, 0 - успех
int __pnk_port_get_gnss_coord(char *msg, size_t *len, const pnk_gnss_t *gnss);

#ifdef __cplusplus
}
#endif

#endif
