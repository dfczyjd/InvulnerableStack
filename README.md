# InvulnerableStack

Библиотека для работы с "неуязвимым" стеком с 3 уровнями защиты от повреждений данных: проверка целостности, канарейки и хеширование структуры.
Выполнена в рамках курса ИСП РАН "Компиляторные технологии" 2020-2021 учебного года.

## Описание
Библиотека позволяет использовать стек для стандартных числовых типов. Для подключения стека типа TYPE следует подключить файл InvulnerableStack.h следующим образом:  
    \#define STACK_TYPE TYPE  
    \#include "InvulnerableStack.h"  
    \#undef STACK_TYPE  

По умолчанию стек работает без уровней защиты. Для установки требуемых уровней требуется до подключения файла InvulnerableStack.h указать следующее:  
    \#include "SecurityLevels.h"  
    \#define SECURITY_LEVEL LEVEL  
где вместо LEVEL следует подставить нужные значения констант, описанные в SecurityLevels.h, разделяя их оператором "|" (побитовое "ИЛИ").

## Сборка
Для сборки проекта необходимо запустить систему сборки CMake для нужного компилятора в корневой директории проекта, затем скомпилировать его.
Результатом будет исполняемый файл test.exe

## Запуск
Для тестирования программы необходимо запустить исполняемый файл test.exe. Друшие файлы в тестировании не задействованы.

## Документация
Документация проекта может быть сгенерирована автоматически с помощью инструмена DoxyGen и конфигурационного файла Doxyfile.config

## Структура проекта
 * src -- файлы основного проекта
   * InvulnerableStack.h -- функции самого стека.
   * PrintItem.h -- функции вывода элементов стека.
   * SecurityLevels.h -- маски для уровней защиты.
 * tests -- файлы тестовой библиотеки и сами тесты
   * mytest.c, mytest.h -- тестовая библиотека
   * mytest_tests.c -- тесты и функция main для тестового проекта
