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

/*
static Ware::Type Ware::intToType(int num)
{
	switch(num) {
		case 0: return Type::SW;
		case 1: return Type::HW;
		default: throw Exc(Exc::Type::BAD_ARGS);
	}
}
*/

int System::getNModules() const
{
	return modules.size();
}

int System::getNWareVersions(int moduleNo, Ware::Type wareType) const
{
	switch(wareType) {
		case Ware::Type::SW:
			return modules[moduleNo - 1].softVersions.size();
		case Ware::Type::HW:
			return modules[moduleNo - 1].hardVersions.size();
	}
}

const Ware& System::getWareVersion(int moduleNo, 
		Ware::Type wareType, int versionNo) const
{
	switch(wareType) {
		case Ware::Type::SW:
			return modules[moduleNo - 1].softVersions[versionNo - 1];
		case Ware::Type::HW:
			return modules[moduleNo - 1].hardVersions[versionNo - 1];
	}
}

void System::pushBackEmptyModule()
{
	modules.push_back(Module());
}

void System::pushBackWareVersion(int moduleNo, Ware::Type wareType, 
		double rel, double cost, int num)
{
	if(num == 0) {
		num = getNWareVersions(moduleNo, wareType) + 1;
	}
	switch(wareType) {
		case Ware::Type::SW:
			modules[moduleNo - 1].softVersions.push_back(
				Software(rel, cost, num) );
			break;
		case Ware::Type::HW:
			modules[moduleNo - 1].hardVersions.push_back(
				Hardware(rel, cost, num) );
			break;
	}
}

int& System::curWareVersionNo(int moduleNo, Ware::Type wareType)
{
	switch(wareType) {
		case Ware::Type::SW:
			return modules[moduleNo - 1].curSoftVersionNo;
		case Ware::Type::HW:
			return modules[moduleNo - 1].curHardVersionNo;
	}
}

int System::curWareVersionNo(int moduleNo, Ware::Type wareType) const
{
	switch(wareType) {
		case Ware::Type::SW:
			return modules[moduleNo - 1].curSoftVersionNo;
		case Ware::Type::HW:
			return modules[moduleNo - 1].curHardVersionNo;
	}
}

const Ware& System::getCurWareVersion(int moduleNo, 
		Ware::Type wareType) const
{
	return getWareVersion(moduleNo, wareType, 
		curWareVersionNo(moduleNo, wareType) );
}
	
double System::getRel() const
{
	int i;
	double rel = 1.0;
	int nModules = getNModules();
	for(i = 1; i <= nModules; i++) {
		rel *= getCurWareVersion(i, Ware::Type::SW).rel *
			getCurWareVersion(i, Ware::Type::HW).rel;
	}
	return rel;
}

double System::getCost() const
{
	int i;
	double cost = 0.0;
	int nModules = getNModules();
	for(i = 1; i <= nModules; i++) {
		cost += getCurWareVersion(i, Ware::Type::SW).cost +
			getCurWareVersion(i, Ware::Type::HW).cost;
	}
	return cost;
}

double System::getModuleRel(int moduleNo) const
{
	return getCurWareVersion(moduleNo, Ware::Type::SW).rel *
		getCurWareVersion(moduleNo, Ware::Type::HW).rel;
}

double System::getModuleCost(int moduleNo) const
{
	return getCurWareVersion(moduleNo, Ware::Type::SW).cost +
		getCurWareVersion(moduleNo, Ware::Type::HW).cost;
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
			default:
				throw Exc(Exc::Type::BAD_ARGS);
		}
	}
}

bool cmpLess1(const Ware& ware1, const Ware& ware2)
{
	return ware1.rel < ware2.rel;
}

bool cmpLess2(const Ware& ware1, const Ware& ware2)
{
	static double eps = 0.01;
	if(abs(ware1.rel - ware2.rel) >= eps) {
		return ware1.rel < ware2.rel;
	} else {
		return ware1.cost < ware2.cost;
	}
}

bool cmpLess(const Ware& ware1, const Ware& ware2, int variant)
{
	switch(variant) {
		case 1: return cmpLess1(ware1, ware2);
		case 2: return cmpLess2(ware1, ware2);
		default: throw Exc(Exc::Type::BAD_ARGS);
	}
}
