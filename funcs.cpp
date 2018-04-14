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
	if(divisions <= 0) throw Exc(Exc::Type::BAD_ARGS);
	double res = randNum(0, divisions);
	res /= divisions;
	return res;
}

double genCostCurve(double rel, double slope)
//slope in (-1, 1), slope != +-1
//slope = 0    => линейная зависимость
//slope -> 1   => гиперболическая зависимость, выпукл вниз,
//	крутизна в окрестности rel = 1 растет
//slope -> -1  => гиперболическая зависимость, выпукл вверх,
//	крутизна в окрестности rel = 0 растет
{
	if(rel < 0.0 || rel > 1.0) throw Exc(Exc::Type::BAD_ARGS);
	if(slope >= 1.0 || slope <= -1.0) throw Exc(Exc::Type::BAD_ARGS);
	if(rel == 0.0) return 0.0;
	if(rel == 1.0) return 1.0;
	if(slope == 0.0) return rel;
	double a = abs(slope);
	double coef;
	coef = ( 1/(1/a - rel) - a ) / ( 1/(1/a - 1) - a );
	//V проверить, то ли получилось
	//coef in [0,1]
	if(coef >= 1.0) {cout << "coef >= 1" << endl; return 1.0;}
	if(coef <= 0.0) {cout << "coef <= 1" << endl; return 0.0;}
	return slope > 0 ? coef : 1 - coef;
}

double genCost(double rel, double slope, double cost90, double randomness)
//cost90 - стоимость ПО/оборуд. с надежностью 0.9 при randomness = 0;
{
	if(rel < 0.0 || rel > 1.0 || slope >= 1.0 || slope <= -1.0 ||
			cost90 < 0.0 || randomness < 0.0 || randomness > 1.0) {
		throw Exc(Exc::Type::BAD_ARGS);
	}
	if(cost90 == 0.0) return 0.0;
	double cost100 = cost90 / genCostCurve(0.9, slope);
	//cost100 is such that: genCostCurve(0.9, slope) * cost100 = cost90;
	double dev = randomness * (randCoef() - 0.5);
	double curveNoised = genCostCurve(rel, slope) + dev;
	if(curveNoised < 0) curveNoised = -curveNoised;
	return cost100 * curveNoised;
}

void sysGen(System& system, int nModules, int nSoftVersions, 
	int nHardVersions, double limitCost, double minRel, double cost90,
	double costRandomness, double costRelSlope)
{
	system.clear();
	system.limitCost() = limitCost;
	double rel, cost;
	if(nModules <= 0 || nSoftVersions <= 0 || nHardVersions <= 0 ||
			minRel < 0.0 || minRel > 1.0 || cost90 < 0.0 ||
			costRandomness < 0.0 || costRandomness > 1.0 || 
			costRelSlope < -1.0 || costRelSlope > 1.0) {
		throw Exc(Exc::Type::BAD_ARGS);
	} 
	for(int i = 1; i <= nModules; i++) {
		system.pushBackEmptyModule();
    	for(int k = 0; k < 2; k++) {
			Ware::Type wareType = Ware::intToType(k);
			int nWare = wareType ==
				Ware::Type::SW ? nSoftVersions : nHardVersions;
			//nWare += randNum(-2, 2); if(nWare <= 0) {nWare = 1;}
			for(int j = 1; j < nWare; j++) {
				rel = minRel + (1 - minRel) * randCoef();
				cost = genCost(rel, costRelSlope, cost90, costRandomness);
				system.pushBackWareVersion(i, wareType, rel, cost);
			}
		}
	}
}

void sysReadFromXml(System& system, const char* filename)
{
//предполагается, что в xml-файле все модули и версии нумеруются
//непрерывно монотонно, начиная с 1
	system.clear();
	pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename);
    //cout << "Load result: " << result.description() << endl; //DEBUG
    if(!result)
		throw PugiXmlExc(result);
    pugi::xml_node sysNode = doc;
    sysNode = sysNode.child("system");
    system.limitCost() = stod( sysNode.attribute("limitcost").value() );
    //cout << "sys limitCost = " << system.limitCost() << endl; //DEBUG
    int i = 1;
    for(pugi::xml_node iNode = sysNode.child("module"); 
			iNode;
			iNode = iNode.next_sibling("module"), i++ )
	{
		system.pushBackEmptyModule();
		//cout << "in i-cycle\n"; //DEBUG
		for(int k = 0; k < 2; k++) {
			Ware::Type wareType = Ware::intToType(k);
			const char* wareTypeStr = k == 0 ? "sw" : "hw"; // V replace with static
			for(pugi::xml_node jNode = iNode.child(wareTypeStr);
				jNode;
				jNode = jNode.next_sibling(wareTypeStr) )
			{
				//cout << "in j-cycle1\n"; //DEBUG
				string str1 = jNode.attribute("rel").value();
				double rel = stod(str1);
				string str2 = jNode.attribute("cost").value();
				double cost = stod(str2);
				system.pushBackWareVersion(i, wareType, rel, cost);
			}
		}
	}	
}

void sysSaveToXml(const System& system, const char* filename)
{
//сохраняет данные о системе и всех версиях оборудования
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
				wareType == Ware::Type::SW ? "sw" : "hw"; //V replace with static
			int nWare = system.getNWareVersions(i, wareType);
			for(int j = 1; j <= nWare; j++) {
				pugi::xml_node wareNode = 
					moduleNode.append_child(wareTypeStr);
				Ware ware = 
					system.getWareVersion(i, wareType, j);
				wareNode.append_attribute("num") = ware.num;
				sprintf(buf, "%.3lf", ware.rel);
				wareNode.append_attribute("rel") = buf;
				sprintf(buf, "%.3g", ware.cost);
				wareNode.append_attribute("cost") = buf;
			}
		}
	}
    //doc.print(cout); //DEBUG
    doc.save_file(filename);
}

void sysCombSaveToXml(const System& system, int iter, const char* filename)
{
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
				wareType == Ware::Type::SW ? "sw" : "hw"; //V...
			pugi::xml_node wareNode = 
					moduleNode.append_child(wareTypeStr);
			Ware ware =
				system.getCurWareVersion(i, wareType);
			wareNode.append_attribute("num") = ware.num;
			sprintf(buf, "%.3lf", ware.rel);
			wareNode.append_attribute("rel") = buf;
			sprintf(buf, "%.3g", ware.cost);
			wareNode.append_attribute("cost") = buf; 
		}
	}
	doc.save_file(filename);
}

int findOptGenerous(System& system)
//returns number of iterations
//во входной системе должны совпадать порядковые номера версий
//(в плане интерфейса) и поля num версий, иначе алгоритм работает
//некорректно
//этим условиям удовлетворяет любая система, созданная с помощью
//интерфейса класса System.
//не удовлетворяет - система, прошедшая через функцию sortVersions
{
/*
 * a < b === должен ли a идти перед b
 * comp = operator<=
 */
	double cost0, cost;
	double limitCost = system.limitCost();
	System system1 = system;
	sortVersions(system1);
	int nModules = system.getNModules();
	for(int i = 1; i <= nModules; i++) {
		for(int k = 0; k < 2; k++) {
			Ware::Type wareType = Ware::intToType(k);
			system1.curWareVersionNo(i, wareType) = 1;
				//1, т.к. берем наименее надежную
				//пока храним в качестве curWareVersionNo
				//порядковый номер версии в отсортированном массиве
		}
	}
	cost0 = system1.getCost();
	double minRel;
	int minRelModuleNo;
	Ware::Type minRelWareType;
	int minRelWareNo;
	int iter = 1;
	bool ok = cost0 <= limitCost;
	while(ok) {
		cost = cost0;
		double testRel;
		minRel = 2.0;
		for(int i = 1; i <= nModules; i++) {
			for(int k = 0; k < 2; k++) {
				Ware::Type wareType = 
					Ware::intToType(k);
				int wareNo = system1.curWareVersionNo(i, wareType);
				testRel = 
					system1.getWareVersion(i, wareType, wareNo).rel;
				if(testRel < minRel && 
					wareNo < system1.getNWareVersions(i, wareType) )
				{
					minRel = testRel;
					minRelModuleNo = i;
					minRelWareType = wareType;
					minRelWareNo = wareNo;
				}
			}
		}
		if(minRel > 1.0) break;
		cost0 += - system1.getWareVersion(minRelModuleNo,
			minRelWareType, minRelWareNo).cost
				+ system1.getWareVersion(minRelModuleNo,
			minRelWareType, minRelWareNo + 1).cost;
		//стоимость системы после замены версии ПО/оборуд.
		//в одном из модулей
		ok = cost0 <= limitCost;
		if (ok) {
			system1.curWareVersionNo(minRelModuleNo, minRelWareType)++;
			iter++;
		}
	}
	for(int i = 1; i <= nModules; i++) {
		for(int k = 0; k < 2; k++) {
			Ware::Type wareType = Ware::intToType(k);
			int sortedCurWareNo = system1.curWareVersionNo(i, wareType);
			system.curWareVersionNo(i, wareType) = 
				system1.getCurWareVersion(i, wareType).num;
		}
	}
	return iter;
}

void findOptGreedy_(System& system, int firstModuleNo, int lastModuleNo)
//найти оптимальную комбинацию путем перебора комбинаций модулей
//с номерами в заданных пределах, при фиксированных комбинациях
//остальных модулей системы
{
	int nModules = system.getNModules();
	if(firstModuleNo <= 0 || lastModuleNo <= 0) {
		throw Exc(Exc::Type::BAD_ARGS);
	}
	int nModulesPart = lastModuleNo - firstModuleNo + 1;
	if(nModulesPart > nModules || nModulesPart <= 0) {
		throw Exc(Exc::Type::BAD_ARGS);
	}
	int nSoftVersions = system.getNWareVersions(firstModuleNo, 
		Ware::Type::SW);
	int nHardVersions = system.getNWareVersions(firstModuleNo, 
		Ware::Type::HW);
	double limitCost = system.limitCost();
	double cost;
	double rel;
	double maxRel = 0.0;
	System bestComb = system;
	if(nModulesPart == 1) {
		system.curWareVersionNo(firstModuleNo, 
			Ware::Type::SW) = 1;
		system.curWareVersionNo(firstModuleNo, 
			Ware::Type::HW) = 1;
		cost = system.getCost();
		rel = system.getRel();
		for(int i = 1; i <= nSoftVersions; i++) {
			for(int j = 1; j <= nHardVersions; j++) {
				if(i == 1 && j == 1) continue;
				system.curWareVersionNo(firstModuleNo, 
					Ware::Type::SW) = i;
				system.curWareVersionNo(firstModuleNo, 
					Ware::Type::HW) = j;
				//пересчитываем cost, rel системы после смены j или i, j
				if(j > 1) {
					cost += system.getWareVersion(firstModuleNo, 
							Ware::Type::HW, j).cost
						- system.getWareVersion(firstModuleNo, 
							Ware::Type::HW, j - 1).cost;
					rel *= system.getWareVersion(firstModuleNo, 
							Ware::Type::HW, j).rel 
						/ system.getWareVersion(firstModuleNo, 
							Ware::Type::HW, j - 1).rel;
				} else {
					cost += system.getWareVersion(firstModuleNo, 
							Ware::Type::SW, i).cost
						+ system.getWareVersion(firstModuleNo, 
							Ware::Type::HW, 1).cost
						- system.getWareVersion(firstModuleNo, 
							Ware::Type::SW, i - 1).cost
						- system.getWareVersion(firstModuleNo, 
							Ware::Type::HW, nHardVersions).cost;
					rel *= system.getWareVersion(firstModuleNo, 
							Ware::Type::SW, i).rel
						* system.getWareVersion(firstModuleNo, 
							Ware::Type::HW, 1).rel
						/ system.getWareVersion(firstModuleNo, 
							Ware::Type::SW, i - 1).rel
						/ system.getWareVersion(firstModuleNo, 
							Ware::Type::HW, nHardVersions).rel;
				}
				if(rel > maxRel && cost <= limitCost) {
					maxRel = rel;
					bestComb = system;
				}
			}
		}
		system = bestComb;
	} else {
		//nModulesPart > 1
		for(int i = 1; i <= nSoftVersions; i++) {
			for(int j = 1; j <= nHardVersions; j++) {
				system.curWareVersionNo(firstModuleNo, 
					Ware::Type::SW) = i;
				system.curWareVersionNo(firstModuleNo, 
					Ware::Type::HW) = j;
				findOptGreedy_(system, firstModuleNo + 1, lastModuleNo);
				cost = system.getCost();
				rel = system.getRel();
				if(rel > maxRel && cost <= limitCost) {
					maxRel = rel;
					bestComb = system;
				}
			}
		}
		system = bestComb;
	}		
}

void findOptGreedy(System& system)
{
	findOptGreedy_( system, 1, system.getNModules() );
}
