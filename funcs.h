#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include "system.h"

using namespace std;

int randNum(int a, int b);

double randCoef(int divisions = 16384);

double genCostCurve(double rel, double slope);
//slope in (-1, 1), slope != +-1
//slope = 0    => линейная зависимость
//slope -> 1   => гиперболическая зависимость, выпукл вниз,
//	крутизна в окрестности rel = 1 растет
//slope -> -1  => гиперболическая зависимость, выпукл вверх,
//	крутизна в окрестности rel = 0 растет

double genCost(double rel, double slope, double cost90, double randomness);
//cost90 - стоимость ПО/оборуд. с надежностью 0.9 при randomness = 0;

void sysGen(System& system, int nModules, int nSoftVersions, 
	int nHardVersions, double limitCost, double minRel = 0.8, double cost90 = 15.0, 
	double costRandomness = 0.2, double costRelSlope = 0.97);

void sysReadFromXml(System& system, const char* filename);

void sysSaveToXml(const System& system, const char* filename);
//сохраняет данные о системе и всех версиях оборудования

void sysCombSaveToXml(const System& system, int iter, const char* filename);

int findOptGenerous(System& system, int variant = 1);
//returns number of iterations
//во входной системе должны совпадать порядковые номера версий
//(в плане интерфейса) и поля num версий, иначе алгоритм работает
//некорректно
//этим условиям удовлетворяет любая система, созданная с помощью
//интерфейса класса System.
//не удовлетворяет - система, прошедшая через функцию sortVersions

void findOptGreedy_(System& system, int firstModuleNo, int lastModuleNo);
//найти оптимальную комбинацию путем перебора комбинаций модулей
//с номерами в заданных пределах, при фиксированных комбинациях
//остальных модулей системы

void findOptGreedy(System& system);

