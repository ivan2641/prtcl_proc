#include "baru_pnk_port_tlm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define BUF_SIZE 133

/// @brief Вывод в консоль результата теста.
/// @param res 0 - результат неуспешный, 1 - успех
void print_test_result(size_t res)
{
    res == 1 ? printf("Test passed\n") : printf("Test failed\n");
}

int test_pnk_tlm(const char *buf, size_t length)
{
    pnk_port_handler_t handler;
    init_pnk_port_handler(&handler);
    pnk_tlm_t tlm;
    int result;
    for (size_t i = 0; i < length; i++) {
        result = parse_pnk_port_tlm(&handler, buf[i], &tlm);
        if (result < 0)
            break;
    }
    return result;
}

/// @brief Генератор случайного корректного тестового сообщения
void generator_correct_tlm_message(char *buf)
{
    srand(time(NULL));
    buf[0] = 0xFC; // Мусор
    buf[1] = 0x10; // DLE
    buf[2] = 0xFF; // ID
    buf[3] = 127; // Длина поля данных в байтах

    for (int i = 4; i < 130; i++) {
        char val = rand() % 255;
        if (val == 16) {
            buf[i] = val + 1;
        } else {
            buf[i] = val;
        }
    }
    uint16_t crc = pnk_utils_calc_crc16(&buf[2], 132 - 3);
    buf[131] = (uint8_t)(crc & 0xFF);
    buf[132] = (uint8_t)(crc >> 8);
}

/// @brief Генератор тестового сообщения с DLE в середине
void generator_for_double_dle_tlm_message(char *buf)
{
    buf[0] = 0xFC; // Мусор
    buf[1] = 0x10; // DLE
    buf[2] = 0xFF; // ID
    buf[3] = 127; // Длина поля данных в байтах

    for (int i = 4; i < 130; i++) {
        buf[i] = i;
    }
    uint16_t crc = pnk_utils_calc_crc16(&buf[2], 132 - 3);
    buf[131] = (uint8_t)(crc & 0xFF);
    buf[132] = (uint8_t)(crc >> 8);
    /*После расчета КС задваиваем DLE в середине*/
    for (int i = BUF_SIZE + 1; i > 16; i--) {
        buf[i] = buf[i - 1];
    }
    buf[17] = 0x10;
}

/// @brief Корректное сообщение
void test_correct_tlm_message()
{
    char tlm_buf[BUF_SIZE];
    generator_correct_tlm_message(tlm_buf);
    int result = test_pnk_tlm(tlm_buf, BUF_SIZE);
    print_test_result(result);
}

/// @brief некорретная кс
void test_uncorrect_crc_tlm_message()
{
    char tlm_buf[BUF_SIZE];
    generator_correct_tlm_message(tlm_buf);
    tlm_buf[BUF_SIZE - 1] += 1;
    int result = test_pnk_tlm(tlm_buf, BUF_SIZE);
    result == -2 ? print_test_result(1) : print_test_result(0);
}

/// @brief В середине однократно встречается DLE = 0x10
void test_uncorrect_dle_tlm_message()
{
    char tlm_buf[BUF_SIZE];
    generator_correct_tlm_message(tlm_buf);
    tlm_buf[64] = 0x10;
    int result = test_pnk_tlm(tlm_buf, BUF_SIZE);
    result == -1 ? print_test_result(1) : print_test_result(0);
}

/// @brief В середине задваивается DLE = 0x10
void test_correct_dle_tlm_message()
{
    char tlm_buf[BUF_SIZE + 1];
    generator_for_double_dle_tlm_message(tlm_buf);
    int result = test_pnk_tlm(tlm_buf, BUF_SIZE + 1);
    result == 1 ? print_test_result(1) : print_test_result(0);
}

/// @brief Значение контрольной суммы содержит DLE
void test_crc_with_dle()
{
    char tlm_buf[]
        = {0xFC, 0x10, 0xFF, 0x7F, 0x17, 0x68, 0xB2, 0x0C, 0xDD, 0x46, 0xF4, 0x4E, 0x84, 0x57, 0xB5, 0xA5, 0x08, 0x4A, 0xBF, 0x5B,
           0x92, 0x39, 0xE7, 0xC0, 0x79, 0x6F, 0x98, 0x17, 0xA1, 0x40, 0x67, 0xD3, 0x3D, 0x83, 0x66, 0x54, 0xEB, 0x19, 0xDF, 0x4A,
           0xDE, 0xD5, 0x98, 0xE3, 0x2D, 0xCD, 0x89, 0xB5, 0x19, 0xC8, 0x11, 0x2B, 0x02, 0x78, 0xEC, 0x7C, 0x67, 0x05, 0x13, 0x0A,
           0x46, 0x7A, 0x5D, 0x83, 0x7D, 0xC3, 0x57, 0xE9, 0x5C, 0x37, 0x34, 0x3B, 0x0D, 0x4C, 0x9E, 0xBA, 0x1A, 0x28, 0x70, 0xB2,
           0xF1, 0x01, 0xDE, 0xF3, 0x79, 0x4B, 0xEF, 0x60, 0x50, 0x83, 0x6A, 0x96, 0xFD, 0x47, 0x1A, 0xFB, 0x0B, 0xF0, 0xE5, 0xE6,
           0x29, 0x99, 0xA2, 0xB5, 0xE5, 0x41, 0xEF, 0x00, 0x6A, 0xDF, 0x33, 0xDB, 0xE0, 0x91, 0x4F, 0xD9, 0xDC, 0xBF, 0x3B, 0x2D,
           0x43, 0x25, 0xC4, 0xC0, 0x6D, 0x5E, 0xBC, 0xF7, 0xCF, 0x22, 0x71, 0xC2, 0x10, 0x10};

    int result = test_pnk_tlm(tlm_buf, BUF_SIZE + 1);
    result == 1 ? print_test_result(1) : print_test_result(0);
}

int main()
{
    test_correct_tlm_message();
    test_uncorrect_crc_tlm_message();
    test_uncorrect_dle_tlm_message();
    test_correct_dle_tlm_message();
    test_crc_with_dle();

    return 0;
}
