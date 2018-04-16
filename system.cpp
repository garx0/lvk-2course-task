#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>

#include "system.h"
#include "exceptions.h"

using namespace std;

System::Module::Module(const Module& module) {
	//cout << "KK mod" << endl; //DEBUG
	softVersions = module.softVersions;
	hardVersions = module.hardVersions;
	curSoftVersionNo = module.curSoftVersionNo;
	curHardVersionNo = module.curHardVersionNo;
	//cout << "/KK mod" << endl;
}

System::System(const System& system)
{
	//cout << "KK sys" << endl; //DEBUG
	modules = system.modules;
	limitCost_ = system.limitCost_;
	//cout << "/KK sys" << endl; //DEBUG
}

System& System::operator=(const System& system)
{
	clear();
	modules = system.modules;
	limitCost_ = system.limitCost_;
	return *this;
}

/*
static Ware::Type Ware::intToType(int num)
{
	switch(num) {
		case 0: return SW;
		case 1: return HW;
		default: throw Exc(Exc::BAD_ARGS);
	}
}
*/

int System::getNModules() const
{
	return modules.size();
}

int System::getNWare(int moduleNo, Ware::Type wareType) const
{
	switch(wareType) {
		case Ware::SW:
			return modules[moduleNo - 1].softVersions.size();
		case Ware::HW:
			return modules[moduleNo - 1].hardVersions.size();
	}
}

const Ware& System::getWare(int moduleNo, 
		Ware::Type wareType, int versionNo) const
{
	switch(wareType) {
		case Ware::SW:
			return modules[moduleNo - 1].softVersions[versionNo - 1];
		case Ware::HW:
			return modules[moduleNo - 1].hardVersions[versionNo - 1];
	}
}

void System::pushBackEmptyModule()
{
	modules.push_back(Module());
}

void System::pushBackWare(int moduleNo, Ware::Type wareType, 
		double rel, double cost, int num)
{
	if(num == 0) {
		num = getNWare(moduleNo, wareType) + 1;
	}
	switch(wareType) {
		case Ware::SW:
			modules[moduleNo - 1].softVersions.push_back(
				Software(rel, cost, num) );
			break;
		case Ware::HW:
			modules[moduleNo - 1].hardVersions.push_back(
				Hardware(rel, cost, num) );
			break;
	}
}

int& System::curWareNo(int moduleNo, Ware::Type wareType)
{
	switch(wareType) {
		case Ware::SW:
			return modules[moduleNo - 1].curSoftVersionNo;
		case Ware::HW:
			return modules[moduleNo - 1].curHardVersionNo;
	}
}

int System::curWareNo(int moduleNo, Ware::Type wareType) const
{
	switch(wareType) {
		case Ware::SW:
			return modules[moduleNo - 1].curSoftVersionNo;
		case Ware::HW:
			return modules[moduleNo - 1].curHardVersionNo;
	}
}

const Ware& System::getCurWare(int moduleNo, 
		Ware::Type wareType) const
{
	return getWare(moduleNo, wareType, 
		curWareNo(moduleNo, wareType) );
}
	
double System::getRel() const
{
	int i;
	double rel = 1.0;
	int nModules = getNModules();
	for(i = 1; i <= nModules; i++) {
		rel *= getCurWare(i, Ware::SW).rel *
			getCurWare(i, Ware::HW).rel;
	}
	return rel;
}

double System::getCost() const
{
	int i;
	double cost = 0.0;
	int nModules = getNModules();
	for(i = 1; i <= nModules; i++) {
		cost += getCurWare(i, Ware::SW).cost +
			getCurWare(i, Ware::HW).cost;
	}
	return cost;
}

double System::getModuleRel(int moduleNo) const
{
	return getCurWare(moduleNo, Ware::SW).rel *
		getCurWare(moduleNo, Ware::HW).rel;
}

double System::getModuleCost(int moduleNo) const
{
	return getCurWare(moduleNo, Ware::SW).cost +
		getCurWare(moduleNo, Ware::HW).cost;
}

double System::limitCost() const
{
	return limitCost_;
}

double& System::limitCost()
{
	return limitCost_;
}
	
void System::clear()
{
	int n = modules.size();
	for(int i = 0; i < n; i++) {
		modules[i].softVersions.clear();
		modules[i].hardVersions.clear();
	}
	modules.clear();
}

void System::printTest() const //DEBUG
{
	cout << "limitcost = " << limitCost_ << endl;
	int n = modules.size();
	for(int i = 0; i < n; i++) {
		cout << "module " << i << ":" << endl;
		int nSoft = modules[i].softVersions.size();
		for(int j = 0; j < nSoft; j++) {
			cout << "	soft " << j << ": rel = " <<
				modules[i].softVersions[j].rel << ", cost = " <<
				modules[i].softVersions[j].cost << ", num = " <<
				modules[i].softVersions[j].num << endl;
		}
		int nHard = modules[i].hardVersions.size();
		for(int j = 0; j < nHard; j++) {
			cout << "	hard " << j << ": rel = " <<
				modules[i].hardVersions[j].rel << ", cost = " <<
				modules[i].hardVersions[j].cost << ", num = " <<
				modules[i].hardVersions[j].num << endl;
		}
	}
}

void sortVersions(System& system, int variant)
//на получившейся системе могут некорректно работать,
//например, алгоритмы поиска оптимальной комбинации,
//т.к. в ней не совпадают порядковые номера версий
//(в плане интерфейса) и поля num версий
{
	//System system1 = system; //DEBUG
	
 	int nModules = system.getNModules();
	for(int i = 0; i < nModules; i++) {
		switch(variant) {
			case 1: 
				sort(system.modules[i].softVersions.begin(),
					system.modules[i].softVersions.end(), cmpLess1);
				sort(system.modules[i].hardVersions.begin(),
					system.modules[i].hardVersions.end(), cmpLess1);
				break;
			case 2:
				sort(system.modules[i].softVersions.begin(),
					system.modules[i].softVersions.end(), cmpLess2);
				sort(system.modules[i].hardVersions.begin(),
					system.modules[i].hardVersions.end(), cmpLess2);
				break;
			case 3:
				sort(system.modules[i].softVersions.begin(),
					system.modules[i].softVersions.end(), cmpLess3);
				sort(system.modules[i].hardVersions.begin(),
					system.modules[i].hardVersions.end(), cmpLess3);
				break;
			case 4:
				sort(system.modules[i].softVersions.begin(),
					system.modules[i].softVersions.end(), cmpLess4);
				sort(system.modules[i].hardVersions.begin(),
					system.modules[i].hardVersions.end(), cmpLess4);
				break;
			default:
				throw Exc(Exc::BAD_ARGS);
		}
	}
}

bool cmpLess1(const Ware& ware1, const Ware& ware2)
{
	return ware1.rel < ware2.rel;
}

bool cmpLess2(const Ware& ware1, const Ware& ware2)
{
	if(ware1.rel != ware2.rel) {
		return ware1.rel < ware2.rel;
	} else {
		return ware1.cost < ware2.cost;
	}
}

bool cmpLess3(const Ware& ware1, const Ware& ware2)
{
	return ware1.cost < ware2.cost;
}

bool cmpLess4_(const Ware& ware1, const Ware& ware2, 
	double relDiffThres, double costRelationThres)
//если надежности сильно различаются, выбираем в пользу надежности.
//если надежности не сильно различаются, а стоимости - сильно, то
//выбираем в пользу цены.
{
	if(abs(ware1.rel - ware2.rel) >= relDiffThres) {
		return ware1.rel < ware2.rel;
	} else {
		double costRelation = ware1.cost / ware2.cost;
		if(costRelation >= 1.0) {
			costRelation = 1 / costRelation;
		}
		//мультипликативный аналог ( -|num| ) числа costRelation
		if(costRelation < costRelationThres) {
			return ware1.cost < ware2.cost;
		} else {
			return ware1.rel < ware2.rel;
		}
	}
}

bool cmpLess4(const Ware& ware1, const Ware& ware2)
{
	return cmpLess4_(ware1, ware2, 0.01, 0.9);
}

bool cmpLess(const Ware& ware1, const Ware& ware2, int variant)
{
	switch(variant) {
		case 1: return cmpLess1(ware1, ware2);
		case 2: return cmpLess2(ware1, ware2);
		case 3: return cmpLess3(ware1, ware2);
		case 4: return cmpLess4(ware1, ware2);
		default: throw Exc(Exc::BAD_ARGS);
	}
}