#include "baru_pnk_port_tlm.h"

const int PNK_TELEMETRY_LENGTH = 98;
const unsigned char DLE = 0x10;
const unsigned char ID_PNK_BARU = 0xFF;

int fill_telemetry(pnk_tlm_t *tlm, pnk_port_handler_t *handler)
{
    int ret_val = 0;
    char *buf = handler->buffer;
    uint16_t crc_in = pnk_utils_calc_crc16(&buf[1], PACKET_PNK_TELEMETRY_LENGTH - 3);
    uint8_t high_byte = buf[PACKET_PNK_TELEMETRY_LENGTH - 1];
    uint8_t low_byte = buf[PACKET_PNK_TELEMETRY_LENGTH - 2];
    uint16_t crc_out = ((uint16_t)high_byte << 8) | (uint16_t)low_byte;

    if (crc_in != crc_out) {
        ret_val = -2;
    } else {
        memcpy(tlm->data, &buf[3], W_PNK_TELEMETRY_LENGTH);
        ret_val = 1;
    }
    init_pnk_port_handler(handler);
    return ret_val;
}

void init_pnk_port_handler(pnk_port_handler_t *handler)
{
    handler->state = STATE_WAITING_DLE;
    memset(handler->buffer, 0, PACKET_PNK_TELEMETRY_LENGTH);
    handler->word_count = 0;
    handler->previous = 0;
}

int parse_pnk_port_tlm(pnk_port_handler_t *handler, uint8_t val, pnk_tlm_t *tlm)
{
    int ret_val = 0;

    if (PACKET_PNK_TELEMETRY_LENGTH <= handler->word_count) {
        init_pnk_port_handler(handler);
    }
    char *buf = handler->buffer;
    switch (handler->state) {
    case STATE_WAITING_DLE:
        if (val == DLE && handler->previous != DLE) {
            handler->state = STATE_ID;
            buf[handler->word_count++] = val;
        }
        break;
    case STATE_ID:
        if (val == ID_PNK_BARU) {
            handler->state = STATE_LENGTH;
            buf[handler->word_count++] = val;
        } else {
            init_pnk_port_handler(handler);
        }
        break;
    case STATE_LENGTH:
        if (val == DLE && handler->previous == ID_PNK_BARU) {
            break;
        } else if ((val != DLE && handler->previous == DLE)) {
            ret_val = -1;
            init_pnk_port_handler(handler);
        } else {
            buf[handler->word_count++] = val;
            handler->state = STATE_MESSAGE;
        }
        break;
    case STATE_MESSAGE:
        if (val == DLE && handler->previous != DLE) {
            handler->state = STATE_DLE;
            break;
        }
        buf[handler->word_count++] = val;
        if (buf[2] == handler->word_count - 5) {
            ret_val = fill_telemetry(tlm, handler);
        }
        break;
    case STATE_DLE:
        if (val != DLE) {
            ret_val = -1;
            init_pnk_port_handler(handler);
        } else {
            buf[handler->word_count++] = val;
            handler->state = STATE_MESSAGE;
            if (buf[2] == handler->word_count - 5) {
                ret_val = fill_telemetry(tlm, handler);
            }
        }
        break;
    default:
        break;
    }
    handler->previous = val;
    return ret_val;
}

int __pnk_port_get_tlm__(char *msg, size_t *len, const pnk_tlm_t *tlm)
{
    if (!tlm) {
        return -1;
    }

    memcpy(msg, tlm->data, 60);
    memcpy(&msg[60], &tlm->data[82], PNK_TELEMETRY_LENGTH - 60);
    *len = PNK_TELEMETRY_LENGTH;

    return 0;
}

//========================================================================
int16_t temp_exhaust_gases(const pnk_tlm_t *tlm)
{
    int16_t result = 0;
    result = (result << 8) + (int16_t)(tlm->data[1] & 0xFF);
    result = result + (int16_t)(tlm->data[2] & 0xFF);
    return result;
}

int16_t bar_alt(const pnk_tlm_t *tlm)
{
    int16_t result = 0;
    result = (result << 8) + (int16_t)(tlm->data[3] & 0xFF);
    result = result + (int16_t)(tlm->data[4] & 0xFF);
    return result;
}

uint16_t geom_alt(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[5] & 0xFF);
    result = result + (uint16_t)(tlm->data[6] & 0xFF);
    return result;
}

uint16_t starter_generator_speed(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[7] & 0xFF);
    result = result + (uint16_t)(tlm->data[8] & 0xFF);
    return result;
}

uint16_t true_air_speed(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[9] & 0xFF);
    result = result + (uint16_t)(tlm->data[10] & 0xFF);
    return result;
}

uint16_t course_angle(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[11] & 0xFF);
    result = result + (uint16_t)(tlm->data[12] & 0xFF);
    return result;
}

int16_t pitch_angle(const pnk_tlm_t *tlm)
{
    int16_t result = 0;
    result = (result << 8) + (int16_t)(tlm->data[13] & 0xFF);
    result = result + (int16_t)(tlm->data[14] & 0xFF);
    return result;
}

int16_t roll_angle(const pnk_tlm_t *tlm)
{
    int16_t result = 0;
    result = (result << 8) + (int16_t)(tlm->data[15] & 0xFF);
    result = result + (int16_t)(tlm->data[16] & 0xFF);
    return result;
}

int8_t tension_vec_x(const pnk_tlm_t *tlm)
{
    int8_t result = (int8_t)(tlm->data[17] & 0xFF);
    return result;
}

uint8_t control_voltage(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[18] & 0xFF);
    return result;
}

int8_t tension_vec_y(const pnk_tlm_t *tlm)
{
    int8_t result = (int8_t)(tlm->data[19] & 0xFF);
    return result;
}

uint8_t supply_voltage(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[20] & 0xFF);
    return result;
}

int8_t tension_vec_z(const pnk_tlm_t *tlm)
{
    int8_t result = (int8_t)(tlm->data[21] & 0xFF);
    return result;
}

uint8_t directed_voltage(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[22] & 0xFF);
    return result;
}

uint16_t speed_fuel_pump(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[23] & 0xFF);
    result = result + (uint16_t)(tlm->data[24] & 0xFF);
    return result;
}

int8_t rudder_angle(const pnk_tlm_t *tlm)
{
    int8_t result = (int8_t)(tlm->data[25] & 0xFF);
    return result;
}

int8_t elevator_angle(const pnk_tlm_t *tlm)
{
    int8_t result = (int8_t)(tlm->data[26] & 0xFF);
    return result;
}

int8_t left_aileron_angle(const pnk_tlm_t *tlm)
{
    int8_t result = (int8_t)(tlm->data[27] & 0xFF);
    return result;
}

int8_t right_aileron_angle(const pnk_tlm_t *tlm)
{
    int8_t result = (int8_t)(tlm->data[28] & 0xFF);
    return result;
}

uint16_t speed_oil_separator(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[29] & 0xFF);
    result = result + (uint16_t)(tlm->data[30] & 0xFF);
    return result;
}

uint16_t engine_speed(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[31] & 0xFF);
    result = result + (uint16_t)(tlm->data[32] & 0xFF);
    return result;
}

uint8_t detected_errors1(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[33] & 0xFF);
    return result;
}

uint8_t detected_errors2(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[34] & 0xFF);
    return result;
}

int8_t longitudal_overload(const pnk_tlm_t *tlm)
{
    int8_t result = (int8_t)(tlm->data[35] & 0xFF);
    return result;
}

int8_t normal_overload(const pnk_tlm_t *tlm)
{
    int8_t result = (int8_t)(tlm->data[36] & 0xFF);
    return result;
}

int8_t side_overload(const pnk_tlm_t *tlm)
{
    int8_t result = (int8_t)(tlm->data[37] & 0xFF);
    return result;
}

int8_t temp_intake_air(const pnk_tlm_t *tlm)
{
    int8_t result = (int8_t)(tlm->data[38] & 0xFF);
    return result;
}

uint8_t state_discrete_inputs1(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[39] & 0xFF);
    return result;
}

uint8_t state_discrete_inputs2(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[40] & 0xFF);
    return result;
}

int16_t roll_rate(const pnk_tlm_t *tlm)
{
    int16_t result = 0;
    result = (result << 8) + (int16_t)(tlm->data[41] & 0xFF);
    result = result + (int16_t)(tlm->data[42] & 0xFF);
    return result;
}

int16_t yaw_rate(const pnk_tlm_t *tlm)
{
    int16_t result = 0;
    result = (result << 8) + (int16_t)(tlm->data[43] & 0xFF);
    result = result + (int16_t)(tlm->data[44] & 0xFF);
    return result;
}

int16_t pitch_rate(const pnk_tlm_t *tlm)
{
    int16_t result = 0;
    result = (result << 8) + (int16_t)(tlm->data[45] & 0xFF);
    result = result + (int16_t)(tlm->data[46] & 0xFF);
    return result;
}

uint16_t flight_time(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[47] & 0xFF);
    result = result + (uint16_t)(tlm->data[48] & 0xFF);
    return result;
}

uint8_t pnk_discrete_commands1(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[49] & 0xFF);
    return result;
}

uint8_t pnk_signals(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[50] & 0xFF);
    return result;
}

uint8_t pnk_discrete_commands2(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[51] & 0xFF);
    return result;
}

uint8_t pnk_discrete_commands3(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[52] & 0xFF);
    return result;
}

uint8_t pnk_discrete_commands4(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[53] & 0xFF);
    return result;
}

uint8_t flight_mode1(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[54] & 0xFF);
    return result;
}

uint8_t flight_mode2(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[55] & 0xFF);
    return result;
}

uint8_t flight_mode3(const pnk_tlm_t *tlm)
{
    uint8_t result = (uint8_t)(tlm->data[56] & 0xFF);
    return result;
}

uint32_t utc_coord(const pnk_tlm_t *tlm)
{
    uint32_t result = 0;
    result = (result << 8) + (uint32_t)(tlm->data[58] & 0xFF);
    result = (result << 8) + (uint32_t)(tlm->data[59] & 0xFF);
    result = (result << 8) + (uint32_t)(tlm->data[60] & 0xFF);
    result = result + (uint32_t)(tlm->data[61] & 0xFF);
    return result;
}

int32_t geom_alt_extra(const pnk_tlm_t *tlm)
{
    int32_t result = 0;
    result = (result << 8) + (int32_t)(tlm->data[62] & 0xFF);
    result = (result << 8) + (int32_t)(tlm->data[63] & 0xFF);
    result = (result << 8) + (int32_t)(tlm->data[64] & 0xFF);
    result = result + (int32_t)(tlm->data[65] & 0xFF);
    return result;
}

int32_t lat(const pnk_tlm_t *tlm)
{
    int32_t result = 0;
    result = (result << 8) + (int32_t)(tlm->data[66] & 0xFF);
    result = (result << 8) + (int32_t)(tlm->data[67] & 0xFF);
    result = (result << 8) + (int32_t)(tlm->data[68] & 0xFF);
    result = result + (int32_t)(tlm->data[69] & 0xFF);
    return result;
}

int32_t lon(const pnk_tlm_t *tlm)
{
    int32_t result = 0;
    result = (result << 8) + (int32_t)(tlm->data[70] & 0xFF);
    result = (result << 8) + (int32_t)(tlm->data[71] & 0xFF);
    result = (result << 8) + (int32_t)(tlm->data[72] & 0xFF);
    result = result + (int32_t)(tlm->data[73] & 0xFF);
    return result;
}

uint16_t ground_speed(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[74] & 0xFF);
    result = result + (uint16_t)(tlm->data[75] & 0xFF);
    return result;
}

uint16_t track_angle(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[76] & 0xFF);
    result = result + (uint16_t)(tlm->data[77] & 0xFF);
    return result;
}

uint16_t sat_number(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[78] & 0xFF);
    result = result + (uint16_t)(tlm->data[79] & 0xFF);
    return result;
}

uint16_t indicated_speed(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[80] & 0xFF);
    result = result + (uint16_t)(tlm->data[81] & 0xFF);
    return result;
}

uint16_t cwosg(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[82] & 0xFF);
    result = result + (uint16_t)(tlm->data[83] & 0xFF);
    return result;
}

uint16_t internal_temp_control_uint(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[84] & 0xFF);
    result = result + (uint16_t)(tlm->data[85] & 0xFF);
    return result;
}

uint16_t directed_flight_time(const pnk_tlm_t *tlm)
{
    uint16_t result = 0;
    result = (result << 8) + (uint16_t)(tlm->data[86] & 0xFF);
    result = result + (uint16_t)(tlm->data[87] & 0xFF);
    return result;
}
