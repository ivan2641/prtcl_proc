#include "baru_pnk_port_gnss.h"

const int UNKNOWN_ALTITUDE = 0x7FFFC;
const int UNKNOWN_ALT_RATE = 0x7FFD;
const int UNKNOWN_VELOCITY = 0xFFFE;

int __pnk_port_put_gnss_coord__(const void *arr, size_t len, pnk_gnss_t *gnss)
{

    /* TBD: return not just int as type, should be created special enum*/
    if (!arr) {
        return -1;
    }

    /*Переложить arr->gnss.А есть ли смысл ?*/

    return 0;
}

int __pnk_port_get_gnss_coord(char *msg, size_t *len, const pnk_gnss_t *gnss)
{
    const size_t GNSS_DATA_LEN = 23;
    const size_t GNSS_PACKET_LEN = GNSS_DATA_LEN + 5;

    /* TBD:return not just int as type, should be created special enum*/
    if (!gnss) {
        return -1;
    }

    if (*len < GNSS_PACKET_LEN) {
        return -2;
    }

    const size_t SHIFT = 8;

    msg[0] = 0x10;
    msg[1] = 0xFE;
    msg[2] = GNSS_DATA_LEN;

    uint32_t utc = gnss->utc;
    msg[3] = (char)(utc & 0xFF);
    utc = utc >> SHIFT;
    msg[4] = (char)(utc & 0xFF);
    utc = utc >> SHIFT;
    msg[5] = (char)(utc & 0xFF);
    utc = utc >> SHIFT;
    msg[6] = (char)(utc & 0xFF);

    int32_t lat = gnss->lat;
    msg[7] = (char)(lat & 0xFF);
    lat = lat >> SHIFT;
    msg[8] = (char)(lat & 0xFF);
    lat = lat >> SHIFT;
    msg[9] = (char)(lat & 0xFF);
    lat = lat >> SHIFT;
    msg[10] = (char)(lat & 0xFF);

    int32_t lon = gnss->lon;
    msg[11] = (char)(lon & 0xFF);
    lon = lon >> SHIFT;
    msg[12] = (char)(lon & 0xFF);
    lon = lon >> SHIFT;
    msg[13] = (char)(lon & 0xFF);
    lon = lon >> SHIFT;
    msg[14] = (char)(lon & 0xFF);

    int32_t alt = gnss->alt != UNKNOWN_ALTITUDE ? gnss->alt : 0;
    msg[15] = (char)(alt & 0xFF);
    alt = alt >> SHIFT;
    msg[16] = (char)(alt & 0xFF);
    alt = alt >> SHIFT;
    msg[17] = (char)(alt & 0xFF);
    alt = alt >> SHIFT;
    msg[18] = (char)(alt & 0xFF);

    int16_t alt_rate = gnss->alt_rate != UNKNOWN_ALT_RATE ? gnss->alt_rate : 0;
    msg[19] = (char)(alt_rate & 0xFF);
    alt_rate = alt_rate >> SHIFT;
    msg[20] = (char)(alt_rate & 0xFF);

    uint16_t w_speed = gnss->w_speed != UNKNOWN_VELOCITY ? gnss->w_speed : 0;
    msg[21] = (char)(w_speed & 0xFF);
    w_speed = w_speed >> SHIFT;
    msg[22] = (char)(w_speed & 0xFF);

    uint16_t w_direct = gnss->w_direct != UNKNOWN_VELOCITY ? gnss->w_direct : 0;
    msg[23] = (char)(w_direct & 0xFF);
    w_direct = w_direct >> SHIFT;
    msg[24] = (char)(w_direct & 0xFF);

    msg[25] = gnss->sat_num;

    uint16_t crc = pnk_utils_calc_crc16(&msg[1], GNSS_DATA_LEN - 3);
    msg[26] = (char)(crc & 0xFF);
    crc = crc >> 8;
    msg[27] = (char)(crc & 0xFF);

    *len = GNSS_PACKET_LEN;

    return 0;
}