#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include "system.h"

using namespace std;

int randNum(int a, int b);
// Возвращает случайное целое число из [a, b]

double randCoef(int divisions = 16384);
/* Возвращает случайное вещ. число из [0, 1]
 *     с "глубиной значений"  divisions
 *     (divisions - число различных значений, которые может
 *     возвратить эта функция).
 */

double genCostCurve(double rel, double slope);
/* Выдает при определенном значении крутизны slope 
 *     значение функции curve(rel): [0,1] -> [0,1]
 * curve(rel) (нестрого) монотонна, задает гиперболическую кривую,
 *     описывающую, как растет цена с ростом надежности.
 * 
/* Допустимые значения slope - интервал (-1, 1)
 * 
 * slope = 0    => линейная зависимость;
 * slope -> 1   => гиперболическая зависимость, выпукл вниз,
 *     крутизна в окрестности rel = 1 растет;
 * slope -> -1  => гиперболическая зависимость, выпукл вверх,
 *     крутизна в окрестности rel = 0 растет;
 * Вид кривой при разных значениях slope:
 *     https://www.desmos.com/calculator/r7wtvsxe86
 */

double genCost(double rel, double slope, double cost90, double randomness);
/* Генерирует значение стоимости оборудования по его надежности,
 *     используя зависимость из комментария к функции genCostCurve с "зашумлением",
 *     определенным значением randomness.
 * Т.к. curve(rel) - число из [0, 1], для получения стоимости будем домножать
 *     значение curve(rel) на некое число, равное стоимости ПО/О с надежностью 1.0
 * Это число можно также вычислить через стоимость ПО/О с надежностью 0.9:
 * Параметр cost90 - стоимость ПО/О с надежностью 0.9 при randomness = 0;
 * Допустимые значения:
 *     randomness in [0, 1]
 *     slope in (-1, 1)
 *     cost90 >= 0
 */

void sysGen(System& system, int nModules, int nSoftVersions, 
	int nHardVersions, double limitCost = -1.0, double minRel = 0.8, double cost90 = 15.0, 
	double costRandomness = 0.2, double costRelSlope = 0.97);
/* Генерирует систему с указанными: 
 *     - количеством модулей,
 *     - количеством версий ПО,
 *     - количеством версий оборудования,
 *     - ограничением на стоимость 
 *         (если не указано или указано -1.0, ограничение будет сгенерировано
 *         автоматически, чтобы алгоритмы поиска оптимальной конфигурации
 *         выдал конфигурацию с адекватной надежностью),
 *     - минимальной допустимой надежности версии ПО/О,
 *     - примерной стоимостью ПО/О с надежностью 0.9,
 *     - степенью рандомизации (более-менее реалистичные значения - в [0, 0.2]),
 *     - значением крутизны для кривой, описывающей зависимость стоимости
 *         от надежности (см. комментарий к функции genCostCurve)
 * 
 *     randomness in [0, 1]
 *     slope in (-1, 1)
 *     cost90 >= 0
 */

void sysReadFromXml(System& system, const char* filename);
/* Считывает данные о всех версиях ПО/О системы из xml-файла.
 * 
/* (!) Предполагается, что в xml-файле все модули и версии нумеруются
 *     непрерывно монотонно, начиная с 1.
 * Иначе нумерация в файле и в представлении класса System не будут
 *     совпадать.
 */

void sysSaveToXml(const System& system, const char* filename);
// Сохраняет данные о всех версиях ПО/О системы в xml-файл.

void sysConfigSaveToXml(const System& system, int iter, const char* filename);
/* Сохраняет данные о конфигурации системы и количестве итераций алгоритма 
 *     поиска оптимальной конфигурации системы в xml-файл.
 * Если iter == 0, конфигурация не сохраняется, а сохраняется
 *     только ограничение на стоимость системы и сообщение о неудаче
 *     алгоритма.
 */
 
bool sysNumerIsCorrect(const System& system);
/* Выясняет, совпадают ли в системе порядковые номера версий
 *     (в плане интерфейса) и поля num версий.
 */

int findOptGenerous(System& system, int variant = 1);
/* Щедрый алгоритм нахождения оптимальной конфигурации системы при
 *     данном ограничении на стоимость.
 * Параметр variant из [1, 4] задаёт одну из вариаций алгоритма, 
 *     основанную на функции сравнении ПО/О cmpLess.
 * Возвращает число итераций
 *     (0 означает неудачу алгоритма,
 *     -1 означает, что алгоритм не выполнился, т.к. не выполнено 
 *     условие ниже).
 * (!) Во входной системе должна быть корректная нумерация, т.е. должны
 *     совпадать порядковые номера (в плане интерфейса) и поля num
 *     версий, иначе алгоритм откажется выполняться.
 * Если система создана с помощью интерфейса класса System, то нумерация
 *     корректна и алгоритм работает корректно.
 * Если система прошла через функцию sortVersions, нумерация будет
 *     некорректна, и алгоритм не выполнится.
 */

int findOptGenerousBwd(System& system, int variant = 1);
/* Щедрый алгоритм нахождения оптимальной конфигурации системы при
 *     данном ограничении на стоимость с обратным проходом
 *     (также в 4 вариациях, см. комментарий к предыдущей функции).
 * Отличается от findOptGenerous только тем, что алгоритм движется
 *     не от "худшей" комбинации вверх, а от "лучшей" вниз.
 */

void findOptBrute_(System& system, int firstModuleNo, int lastModuleNo);
/* Находит оптимальную конфигурацию системы при заданном ограничении на стоимость
 *     путем перебора комбинаций модулей с номерами в заданных пределах, 
 *     при фиксированных конфигурациях остальных модулей системы.
 * Вспомогательная функция для алгоритма полного перебора findOptBrute.
 */

int findOptBrute(System& system);
/* Алгоритм нахождения оптимальной конфигурации системы при
 *     данном ограничении на стоимость путём полного перебора
 *     всех конфигураций системы.
 */
 
void sysRndConfig(System& system);
// Генерация случайной комбинации системы.

double sysAvgCost(const System& system, int nTests = 1000);
/* Вычисление примерной средней стоимости системы путем
 *     перебора нескольких случайных конфигураций системы (в количестве
 *     nTests) и вычисление среднего арифметического их стоимостей.
 * Чем больше nTests, тем больше вычислений нужно для вычисления
 *     значения функции.
 */
