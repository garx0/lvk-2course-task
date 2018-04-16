#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include "./src/pugixml.hpp"

#include "system.h"
#include "funcs.h"
#include "exceptions.h"

using namespace std;

int randNum(int a, int b)
{
	if(a == b) return a;
	return a + rand() % (b - a);
}

double randCoef(int divisions)
{
	if(divisions <= 0) throw Exc(Exc::BAD_ARGS);
	double res = randNum(0, divisions);
	res /= divisions;
	return res;
}

double genCostCurve(double rel, double slope)
{
	if(rel < 0.0 || rel > 1.0) throw Exc(Exc::BAD_ARGS);
	if(slope >= 1.0 || slope <= -1.0) throw Exc(Exc::BAD_ARGS);
	if(rel == 0.0) return 0.0;
	if(rel == 1.0) return 1.0;
	if(slope == 0.0) return rel;
	double a = abs(slope);
	double coef;
	coef = ( 1/(1/a - rel) - a ) / ( 1/(1/a - 1) - a );
	//coef in [0,1]
	if(coef >= 1.0) return 1.0;
	if(coef <= 0.0) return 0.0;
	return slope > 0 ? coef : 1 - coef;
}

double genCost(double rel, double slope, double cost90, double randomness)
{
	if(rel < 0.0 || rel > 1.0 || slope >= 1.0 || slope <= -1.0 ||
			cost90 < 0.0 || randomness < 0.0 || randomness > 1.0) {
		throw Exc(Exc::BAD_ARGS);
	}
	if(cost90 == 0.0) return 0.0;
	double cost100 = cost90 / genCostCurve(0.9, slope);
	//cost100 такой, что: genCostCurve(0.9, slope) * cost100 = cost90;
	double dev = randomness * (randCoef() - 0.5);
	double curveNoised = genCostCurve(rel, slope) + dev;
	if(curveNoised < 0) curveNoised = -curveNoised;
	return cost100 * curveNoised;
}

void sysGen(System& system, int nModules, int nSoftVersions, 
	int nHardVersions, double limitCost, double minRel, double cost90,
	double costRandomness, double costRelSlope)
{
	if(nModules <= 0 || nSoftVersions <= 0 || nHardVersions <= 0 ||
			(limitCost < 0.0 && limitCost != -1.0) ||
			minRel < 0.0 || minRel > 1.0 || cost90 < 0.0 ||
			costRandomness < 0.0 || costRandomness > 1.0 || 
			costRelSlope < -1.0 || costRelSlope > 1.0) {
		throw Exc(Exc::BAD_ARGS);
	}
	system.clear();
	double rel, cost;
	for(int i = 1; i <= nModules; i++) {
		system.pushBackEmptyModule();
    	for(int k = 0; k < 2; k++) {
			Ware::Type wareType = Ware::intToType(k);
			int nWare = wareType ==
				Ware::SW ? nSoftVersions : nHardVersions;
			for(int j = 1; j <= nWare; j++) {
				rel = minRel + (1 - minRel) * randCoef();
				cost = genCost(rel, costRelSlope, cost90, costRandomness);
				system.pushBackWare(i, wareType, rel, cost);
			}
		}
	}
	if(limitCost >= 0.0) {
		system.limitCost() = limitCost;
	} else {
		system.limitCost() = sysAvgCost(system) * 3;
	}
}

void sysReadFromXml(System& system, const char* filename)
{
	system.clear();
	pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename);
    if(!result)
		throw PugiXmlExc(result);
    pugi::xml_node sysNode = doc;
    sysNode = sysNode.child("system");
    system.limitCost() = stod( sysNode.attribute("limitcost").value() );
    int i = 1;
    for(pugi::xml_node iNode = sysNode.child("module"); 
			iNode;
			iNode = iNode.next_sibling("module"), i++ )
	{
		system.pushBackEmptyModule();
		for(int k = 0; k < 2; k++) {
			Ware::Type wareType = Ware::intToType(k);
			const char* wareTypeStr = k == 0 ? "sw" : "hw";
			for(pugi::xml_node jNode = iNode.child(wareTypeStr);
				jNode;
				jNode = jNode.next_sibling(wareTypeStr) )
			{
				string str1 = jNode.attribute("rel").value();
				double rel = stod(str1);
				string str2 = jNode.attribute("cost").value();
				double cost = stod(str2);
				system.pushBackWare(i, wareType, rel, cost);
			}
		}
	}	
}

void sysSaveToXml(const System& system, const char* filename)
{
	pugi::xml_document doc;
    pugi::xml_node sysNode = doc.append_child("system");
	char buf[32];
    sprintf(buf, "%.3g", system.limitCost() );
    sysNode.append_attribute("limitcost") = buf;
    int nModules = system.getNModules();
    for(int i = 1; i <= nModules; i++) {
		pugi::xml_node moduleNode = sysNode.append_child("module");
		moduleNode.append_attribute("num") = i;
		for(int k = 0; k < 2; k++) {
			Ware::Type wareType = Ware::intToType(k);
			const char* wareTypeStr = 
				wareType == Ware::SW ? "sw" : "hw";
			int nWare = system.getNWare(i, wareType);
			for(int j = 1; j <= nWare; j++) {
				pugi::xml_node wareNode = 
					moduleNode.append_child(wareTypeStr);
				Ware ware = 
					system.getWare(i, wareType, j);
				wareNode.append_attribute("num") = ware.num;
				sprintf(buf, "%.3lf", ware.rel);
				wareNode.append_attribute("rel") = buf;
				sprintf(buf, "%.3g", ware.cost);
				wareNode.append_attribute("cost") = buf;
			}
		}
	}
    doc.save_file(filename);
}

void sysConfigSaveToXml(const System& system, int iter, const char* filename)
{
	if(iter > 0) {
		pugi::xml_document doc;
		pugi::xml_node sysNode = doc.append_child("system");
		char buf[32];
		sprintf(buf, "%.3g", system.limitCost() );
		sysNode.append_attribute("limitcost") = buf;
		sprintf(buf, "%.3lf", system.getRel() );
		sysNode.append_attribute("rel") = buf;
		sprintf(buf, "%.3g", system.getCost() );
		sysNode.append_attribute("cost") = buf;
		sysNode.append_attribute("iteration") = iter; 
		int nModules = system.getNModules();
		for(int i = 1; i <= nModules; i++) {
			pugi::xml_node moduleNode = sysNode.append_child("module");
			moduleNode.append_attribute("num") = i;
			for(int k = 0; k < 2; k++) {
				Ware::Type wareType = Ware::intToType(k);
				const char* wareTypeStr = 
					wareType == Ware::SW ? "sw" : "hw";
				pugi::xml_node wareNode = 
						moduleNode.append_child(wareTypeStr);
				Ware ware =
					system.getCurWare(i, wareType);
				wareNode.append_attribute("num") = ware.num;
				sprintf(buf, "%.3lf", ware.rel);
				wareNode.append_attribute("rel") = buf;
				sprintf(buf, "%.3g", ware.cost);
				wareNode.append_attribute("cost") = buf; 
			}
		}
		doc.save_file(filename);
	} else {
		char buf[256];
		if(iter == 0) {
			sprintf(buf, 
				"Algorithm didn't find configuration with cost <= limitcost");
		} else if(iter == -1) {
			sprintf(buf, 
				"Error: Incorrect input for the algorithm");
		} else
			throw Exc(Exc::BAD_ARGS);
		pugi::xml_document doc;
		pugi::xml_node sysNode = doc.append_child("system");
		char buf2[64];
		sprintf(buf2, "%.3g", system.limitCost() );
		sysNode.append_attribute("limitcost") = buf2;
		sysNode.append_attribute("iteration") = iter;
		sysNode.append_child(pugi::node_pcdata).set_value(buf);
		doc.save_file(filename);
	}
}

bool sysNumerIsCorrect(const System& system)
{
	int nModules = system.getNModules();
	for(int i = 1; i <= nModules; i++) {
		for(int k = 0; k < 2; k++) {
			Ware::Type wareType = Ware::intToType(k);
			int nWare = system.getNWare(i, wareType);
			for(int j = 1; j <= nWare; j++) {
				if(system.getWare(i, wareType, j).num != j)
					return false;
			}
		}
	}
	return true;
}
	
int findOptGenerous(System& system, int variant)
{
	if(!sysNumerIsCorrect) {
		return -1;
	}
	double cost0, cost;
	double limitCost = system.limitCost();
	System system1 = system;
	sortVersions(system1, variant);
	/* Сортируем вектор по возрастанию относительно соотв. функции
	 *     сравнения на "меньше".
	 */
	int nModules = system.getNModules();
	for(int i = 1; i <= nModules; i++) {
		for(int k = 0; k < 2; k++) {
			Ware::Type wareType = Ware::intToType(k);
			system1.curWareNo(i, wareType) = 1;
			/* 1, т.к. берем наименее надежную.
			 * Пока храним в качестве curWareNo порядковый номер
			 *     версии ПО/О в отсортированном векторе
			 *     (но нумерация от 1, а не как в векторе).
			 * Исходные номера версий находятся в полях num
			 *     версий ПО/О.
			 */
		}
	}
	cost0 = system1.getCost();
	Ware minWare;
	int minWareModuleNo;
	Ware::Type minWareType;
	int minWareNo;
	bool ok = cost0 <= limitCost;
	if(!ok) return 0;
	int iter = 1;
	int cmpVar = variant != 4 ? variant : 2;
	/* в случае 4 варианта ищем, какую версию менять на следующую в 
	 * отсортированном массиве, через функцию сравнения-2, а не 4
	 */
	while(ok) {
		cost = cost0;
		Ware testWare;
		minWare = Ware(2.0, limitCost + 1.0);
		bool minWareHasChanged = false;
		for(int i = 1; i <= nModules; i++) {
			for(int k = 0; k < 2; k++) {
				Ware::Type wareType = 
					Ware::intToType(k);
				int wareNo = system1.curWareNo(i, wareType);
				testWare = 
					system1.getWare(i, wareType, wareNo);
				if( cmpLess(testWare, minWare, cmpVar) && 
					wareNo < system1.getNWare(i, wareType) )
				{
					minWareHasChanged = true;
					minWare = testWare;
					minWareModuleNo = i;
					minWareType = wareType;
					minWareNo = wareNo;
				}
			}
		}
		if(!minWareHasChanged) break;
		cost0 += - system1.getWare(minWareModuleNo,
			minWareType, minWareNo).cost
				+ system1.getWare(minWareModuleNo,
			minWareType, minWareNo + 1).cost;
		/* Пересчитываем стоимость системы после замены версии ПО/О
		 *     в одном из модулей
		 */
		ok = cost0 <= limitCost;
		if (ok) {
			system1.curWareNo(minWareModuleNo, minWareType)++;
			iter++;
		}
	}
	for(int i = 1; i <= nModules; i++) {
		// Устанавливаем конфигурацию системы system по конфигурации
		//     "отсортированной" системы system1
		for(int k = 0; k < 2; k++) {
			Ware::Type wareType = Ware::intToType(k);
			int sortedCurWareNo = system1.curWareNo(i, wareType);
			system.curWareNo(i, wareType) = 
				system1.getCurWare(i, wareType).num;
		}
	}
	return iter;
}

int findOptGenerousBwd(System& system, int variant)
{
	if(!sysNumerIsCorrect) {
		return -1;
	}
	double cost0, cost;
	double limitCost = system.limitCost();
	System system1 = system;
	sortVersions(system1, variant);
	/* Сортируем вектор по возрастанию относительно соотв. функции
	 *     сравнения на "меньше".
	 */
	int nModules = system.getNModules();
	for(int i = 1; i <= nModules; i++) {
		for(int k = 0; k < 2; k++) {
			Ware::Type wareType = Ware::intToType(k);
			system1.curWareNo(i, wareType) = 
				system1.getNWare(i, wareType);
			/* NWare, т.к. берем наиболее надежную.
			 * Пока храним в качестве curWareNo порядковый номер
			 *     версии ПО/О в отсортированном векторе
			 *     (но нумерация от 1, а не как в векторе).
			 * Исходные номера версий находятся в полях num
			 *     версий ПО/О.
			 */
		}
	}
	cost0 = system1.getCost();
	Ware maxWare;
	int maxWareModuleNo;
	Ware::Type maxWareType;
	int maxWareNo;
	bool ok = cost0 <= limitCost;
	int iter = 1;
	int cmpVar = variant != 4 ? variant : 2;
	/* в случае 4 варианта ищем, какую версию менять на следующую в 
	 * отсортированном массиве, через функцию сравнения-2, а не 4
	 */
	while(!ok) {
		cost = cost0;
		Ware testWare;
		maxWare = Ware(-1.0, -1.0);
		bool maxWareHasChanged = false;
		for(int i = 1; i <= nModules; i++) {
			for(int k = 0; k < 2; k++) {
				Ware::Type wareType = 
					Ware::intToType(k);
				int wareNo = system1.curWareNo(i, wareType);
				testWare = 
					system1.getWare(i, wareType, wareNo);
				if( cmpLess(maxWare, testWare, cmpVar) && wareNo > 1 )
				{
					maxWareHasChanged = true;
					maxWare = testWare;
					maxWareModuleNo = i;
					maxWareType = wareType;
					maxWareNo = wareNo;
				}
			}
		}
		if(!maxWareHasChanged) break;
		cost0 += - system1.getWare(maxWareModuleNo,
			maxWareType, maxWareNo).cost
				+ system1.getWare(maxWareModuleNo,
			maxWareType, maxWareNo - 1).cost;
		/* Пересчитываем стоимость системы после замены версии ПО/О
		 *     в одном из модулей
		 */
		ok = cost0 <= limitCost;
		system1.curWareNo(maxWareModuleNo, maxWareType)--;
		iter++;
	}
	if(!ok) return 0;
	for(int i = 1; i <= nModules; i++) {
		// Устанавливаем конфигурацию системы system по конфигурации
		//     "отсортированной" системы system1
		for(int k = 0; k < 2; k++) {
			Ware::Type wareType = Ware::intToType(k);
			int sortedCurWareNo = system1.curWareNo(i, wareType);
			system.curWareNo(i, wareType) = 
				system1.getCurWare(i, wareType).num;
		}
	}
	return iter;
}

void findOptBrute_(System& system, int firstModuleNo, int lastModuleNo)
{
	int nModules = system.getNModules();
	if(firstModuleNo <= 0 || lastModuleNo <= 0) {
		throw Exc(Exc::BAD_ARGS);
	}
	int nModulesPart = lastModuleNo - firstModuleNo + 1;
	//число модулей в заданном диапазоне
	if(nModulesPart > nModules || nModulesPart <= 0) {
		throw Exc(Exc::BAD_ARGS);
	}
	int nSoftVs = system.getNWare(firstModuleNo, 
		Ware::SW);
	int nHardVs = system.getNWare(firstModuleNo, 
		Ware::HW);
	double limitCost = system.limitCost();
	double cost;
	double rel;
	double maxRel = -1.0;
	System bestConfig = system;
	int& modNo = firstModuleNo;
	if(nModulesPart == 1) {
		for(int i = 1; i <= nSoftVs; i++) {
			for(int j = 1; j <= nHardVs; j++) {
				system.curWareNo(modNo, 
					Ware::SW) = i;
				system.curWareNo(modNo, 
					Ware::HW) = j;
				//пересчитываем cost, rel системы после смены j или i, j
				if(j > 1) {
					cost += system.getWare(modNo, Ware::HW, j).cost
						- system.getWare(modNo, Ware::HW, j - 1).cost;
					rel *= system.getWare(modNo, Ware::HW, j).rel 
						/ system.getWare(modNo, Ware::HW, j - 1).rel;
				} else if(i > 1) {
					cost += system.getWare(modNo, Ware::SW, i).cost
						+ system.getWare(modNo, Ware::HW, 1).cost
						- system.getWare(modNo, Ware::SW, i - 1).cost
						- system.getWare(modNo, Ware::HW, nHardVs).cost;
					rel *= system.getWare(modNo, Ware::SW, i).rel
						* system.getWare(modNo, Ware::HW, 1).rel
						/ system.getWare(modNo, Ware::SW, i - 1).rel
						/ system.getWare(modNo, Ware::HW, nHardVs).rel;
				} else {
					//i == 1, j == 1
					cost = system.getCost();
					rel = system.getRel();
				}
				if(rel > maxRel && cost <= limitCost) {
					maxRel = rel;
					bestConfig = system;
				}
			}
		}
		system = bestConfig;
	} else {
		//nModulesPart > 1
		for(int i = 1; i <= nSoftVs; i++) {
			for(int j = 1; j <= nHardVs; j++) {
				system.curWareNo(modNo, Ware::SW) = i;
				system.curWareNo(modNo, Ware::HW) = j;
				findOptBrute_(system, firstModuleNo + 1, lastModuleNo);
				cost = system.getCost();
				rel = system.getRel();
				if(rel > maxRel && cost <= limitCost) {
					maxRel = rel;
					bestConfig = system;
				}
			}
		}
		system = bestConfig;
	}		
}

int findOptBrute(System& system)
{
	findOptBrute_( system, 1, system.getNModules() );
	return system.getCost() <= system.limitCost() ? 1 : 0;
}

void sysRndConfig(System& system)
{
	int nModules = system.getNModules();
	for(int i = 1; i <= nModules; i++) {
		for(int k = 0; k < 2; k++) {
			Ware::Type wareType = Ware::intToType(k);
			int nWare = system.getNWare(i, wareType);
			system.curWareNo(i, wareType) = randNum(1, nWare);
		}
	}
}

double sysAvgCost(const System& system, int nTests)
{
	System system1 = system;
	sysRndConfig(system1);
	double avgCost = system1.getCost();
	for(int n = 1; n < nTests; n++) {
		sysRndConfig(system1);
		avgCost *= (double) n / (n + 1);
		avgCost += system1.getCost() / (n + 1);
	}
	return avgCost;
}		
