#ifndef __BARU_TELEMETRY_PORT_TLM_H__
#define __BARU_TELEMETRY_PORT_TLM_H__

#include "baru_pnk_port_utils.h"


#ifdef __cplusplus
extern "C" {
#endif

/*Длина всего пакета*/
#define PACKET_PNK_TELEMETRY_LENGTH 132
/*Длина телеметрии*/
#define W_PNK_TELEMETRY_LENGTH (PACKET_PNK_TELEMETRY_LENGTH - 5)

/// @brief Вспомогательная структура, в которой хранится телеметрия целиком
typedef struct pnk_tlm_t {
    char data[W_PNK_TELEMETRY_LENGTH];
} pnk_tlm_t;

/// @brief STATE_WAITING_DLE - ожидание DLE,
/// STATE_ID - чтение ID,
/// STATE_LENGTH - чтение длины телеметрии,
/// STATE_MESSAGE - чтение данных телеметрии и КС,
/// STATE_LENGTH - чтение длины телеметрии,
/// STATE_DLE - получили одиночный DLE, возможна ошибка
typedef enum pnk_message_tlm_state
{
    STATE_WAITING_DLE = 0,
    STATE_ID,
    STATE_LENGTH,
    STATE_MESSAGE,
    STATE_DLE
} pnk_message_tlm_state;

/// @brief Обработчик для разбора телеметрии
typedef struct pnk_port_handler_t {
    pnk_message_tlm_state state;
    char buffer[PACKET_PNK_TELEMETRY_LENGTH];
    int word_count;
    uint8_t previous;
} pnk_port_handler_t;

/// @brief Функция инициализации структуры pnk_port_handler_t
void init_pnk_port_handler(pnk_port_handler_t *handler);

/// @brief Автомат состояний. Осуществляет разбор сообщений телеметрии из ПНК-163 в промежуточную структуру
/// @param handler Обработчик хранит состояние, буфер, счетчик слов, а также технологическую информацию
/// @param val Байт бинарного сообщения
/// @param tlm Формируемая структура телеметрии
/// @return -2 - ошибка КС, -1 - ошибка формирования структуры телеметрии, 0 - телеметрия ешё не заполнена, 1 - телеметрия
/// заполнена
int parse_pnk_port_tlm(pnk_port_handler_t *handler, uint8_t val, pnk_tlm_t *tlm);

/// @brief Функция упаковки необходимой телеметрии
/// @param msg destination
/// @param len length
/// @param tlm source
/// @return
int __pnk_port_get_tlm__(char *msg, size_t *len, const pnk_tlm_t *tlm);

#ifdef __cplusplus
}
#endif

#endif
