#ifndef CONSTANTS_H
#define CONSTANTS_H
// Исполнено
#define UVK20_RES_OK            0x80000000
// Исполнено: СПО записано
#define UVK20_RES_OK_SPOLOAD    0x80000001
// Исполнено: СПО стерто
#define UVK20_RES_OK_SPOERASE   0x80000002
// Исполнено: СИ записано
#define UVK20_RES_OK_SILOAD     0x80000003
// Исполнено: СИ стерто
#define UVK20_RES_OK_SIERASE    0x80000004
// Ошибка: контрольной суммы
#define UVK20_RES_FAIL_CS       0x00010000
// Ошибка: неверная команда
#define UVK20_RES_FAIL_CMND     0x00020000
// Ошибка: неверно заполненное поле
#define UVK20_RES_FAIL_POLE     0x00040000
// Ошибка: чтение ГСЧ
#define UVK20_RES_FAIL_GS4      0x00080000
// Ошибка: переполнение
#define UVK20_RES_FAIL_OVERF    0x00100000
// Ошибка: загрузки СПО
#define UVK20_RES_FAIL_SPOLOAD  0x00200000
// Ошибка: стирание СИ
#define UVK20_RES_FAIL_SIERASE  0x00400000
// Ошибка: стирание СПО
#define UVK20_RES_FAIL_SPOERASE 0x00800000
// Ошибка: отсутствует СИ
#define UVK20_RES_FAIL_SINO     0x01000000
// Ошибка: СИ существует
#define UVK20_RES_FAIL_SIYES    0x02000000
// Ошибка: неверная конфигурация
#define UVK20_RES_FAIL_GCONFNO  0x04000000
// Ошибка: запись конфигурации
#define UVK20_RES_FAIL_GCONF    0x08000000

#define SIZE_FOR_CONTROSUM_TS1  4

#define SIZE_FOR_SUB_FORCHECKTS2  8

#endif // CONSTANTS_H
