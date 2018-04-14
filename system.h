#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>

using namespace std;

struct Ware {
	enum class Type { SW, HW };
	//static char* const wareTypeStr[] = {"sw", "hw"};
	static int typeToInt(Type type) {
		return static_cast<int>(type);
	}
	static Type intToType(int num) {
		if (num <= 0) {return Type::SW;} else {return Type::HW;}
	}
	double rel;
	double cost;
	int num;
	Ware(): rel(1.0), cost(0.0), num(1) {}
	Ware(double a_rel, double a_cost, int a_num): 
		rel(a_rel), cost(a_cost), num(a_num) {}
	bool operator< (const Ware& ware) const
		{ return rel < ware.rel; }
	bool operator> (const Ware& ware) const
		{ return rel > ware.rel; }
	bool operator<= (const Ware& ware) const
		{ return rel <= ware.rel; }
	bool operator>= (const Ware& ware) const
		{ return rel >= ware.rel; }
	bool operator== (const Ware& ware) const
		{ return rel == ware.rel; }
	bool operator!= (const Ware& ware) const
		{ return rel != ware.rel; }
};

struct Software : Ware {
	static const Type type = Type::SW;
	Software(): Ware() {}
	Software(double rel, double cost, int num):
		Ware(rel, cost, num) {}
};
struct Hardware : Ware {
	static const Type type = Type::HW;
	Hardware(): Ware() {}
	Hardware(double rel, double cost, int num): 
		Ware(rel, cost, num) {}
};

class System {
	class Module {
		friend System;
		vector<Software> softVersions;
		vector<Hardware> hardVersions;
		int curSoftVersionNo;
		int curHardVersionNo;
		//при генерации системы поле num класса Ware совпадает с 
		//порядковым (в плане интерфейса) номером ПО/оборудования 
		//в данном модуле.
		//если переставить элементы в векторе из версий ПО или оборуд.,
		//то настоящие порядковые номера будут содержаться только в
		//полях num этих версий
		friend void sortVersions(System& system);
	public:
		Module(): softVersions(), hardVersions(), curSoftVersionNo(1), curHardVersionNo(1) {}
		Module(const Module& module);
		~Module() { 
			//cout << "D mod" << endl; //DEBUG
			softVersions.clear(); hardVersions.clear(); 
			//cout << "/D mod" << endl; //DEBUG
		}
	};

	vector<Module> modules;
	double limitCost_;
	//в реализации:
	//	в векторах внутри Module и System нумерация идет от 0,
	// 	в полях cur~~~~VersionNo_ - от 1
	//в интерфейсе:
	//	нумерация версий и модулей идет от 1
	friend void sortVersions(System& system);
public:
	System(): limitCost_(0) {}
	System(const System& system);
	~System() { 
		//cout << "D sys" << endl; //DEBUG
		modules.clear(); 
		//cout << "/D sys" << endl; //DEBUG
	}
	int getNModules() const;
	int getNWareVersions(int moduleNo, Ware::Type wareType) const;
	const Ware& getWareVersion(int moduleNo, Ware::Type wareType, 
		int versionNo) const;
	void pushBackEmptyModule();
	void pushBackWareVersion(int moduleNo, Ware::Type wareType, 
		double rel, double cost, int num = 0);
	int& curWareVersionNo(int moduleNo, Ware::Type wareType);
	int curWareVersionNo(int moduleNo, Ware::Type wareType) const;
	const Ware& getCurWareVersion(int moduleNo, 
		Ware::Type wareType) const;
	double getRel() const;
	double getCost() const;
	double getModuleRel(int moduleNo) const;
	double getModuleCost(int moduleNo) const;
	double limitCost() const;
	double& limitCost();
	void clear();
	void printTest() const; //DEBUG
};
