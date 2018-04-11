#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "./src/pugixml.hpp"

using namespace std;

//test comment

class Exc {
public:
	enum class Type {
		UNKNOWN_ERR
	};
private:
	Type type;
public:
	Exc(Type a_type = Type::UNKNOWN_ERR): type(a_type) {}
};

class PugiXmlExc {
	pugi::xml_parse_result result;
public:
	PugiXmlExc(pugi::xml_parse_result a_result): result(a_result) {}
};

class System {
public:
	struct Ware {
		enum class Type { SW, HW };
		//static char* const wareTypeStr[] = {"sw", "hw"};
		static int typeToInt(Type type) {
			return static_cast<int>(type);
		}
		static Type intToType(int num) {
			if (num <= 0) {return Type::SW;} else {return Type::HW;}
		}
		friend System;
		double rel;
		double cost;
		int num;
		Ware(): rel(1.0), cost(0.0), num(0) {}
		Ware(double a_rel, double a_cost, int a_num): 
			rel(a_rel), cost(a_cost), num(a_num) {}
		bool operator< (const Ware& ware) {	return rel < ware.rel; }
		bool operator> (const Ware& ware) { return rel > ware.rel; }
		bool operator<= (const Ware& ware) { return rel <= ware.rel; }
		bool operator>= (const Ware& ware) { return rel >= ware.rel; }
		bool operator== (const Ware& ware) { return rel == ware.rel; }
		bool operator!= (const Ware& ware) { return rel != ware.rel; }
	};
	//const char* const Ware::wareTypeStr[] = {"sw", "hw"};
	struct Software : Ware {
		friend System;
		static const Type type = Type::SW;
		Software(): Ware() {}
		Software(double rel, double cost, int num):
			Ware(rel, cost, num) {}
	};
	struct Hardware : Ware {
		friend System;
		static const Type type = Type::HW;
		Hardware(): Ware() {}
		Hardware(double rel, double cost, int num): Ware(rel, cost, num) {}
	};
private:
	class Module {
		friend System;
		vector<Software> softVersions;
		vector<Hardware> hardVersions;
		int curSoftVersionNo;
		int curHardVersionNo;
		friend void sortVersions(System& system);
	public:
		Module(): curSoftVersionNo(0), curHardVersionNo(0) {}
		Module(const Module& module);
	};
private:
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
	int getNModules() const {return modules.size();}
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
	double getRel();
	double getCost();
	const double& limitCost() const {return limitCost_;}
	double& limitCost() {return limitCost_;}
	void printTest() const; //DEBUG
};

//using namespace System::Ware;

System::Module::Module(const Module& module) {
	softVersions = module.softVersions;
	hardVersions = module.hardVersions;
	curSoftVersionNo = module.curSoftVersionNo;
	curHardVersionNo = module.curHardVersionNo;
}

System::System(const System& system)
{
	modules = system.modules;
	limitCost_ = system.limitCost_;
	cout << "KK\n"; //DEBUG
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

const System::Ware& System::getWareVersion(int moduleNo, 
		Ware::Type wareType, int versionNo) const
{
	switch(wareType) {
		case System::Ware::Type::SW:
			return modules[moduleNo - 1].softVersions[versionNo - 1];
		case System::Ware::Type::HW:
			return modules[moduleNo - 1].hardVersions[versionNo - 1];
	}
}

void System::pushBackEmptyModule()
{
	modules.push_back(Module());
	//cout << "pushed back empty module" << endl; //DEBUG
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

const System::Ware& System::getCurWareVersion(int moduleNo, 
		Ware::Type wareType) const
{
	return getWareVersion(moduleNo, wareType, 
		curWareVersionNo(moduleNo, wareType) );
}
	
double System::getRel()
{
	int i;
	double rel = 1.0;
	int nModules = getNModules();
	for(i = 0; i < nModules; i++) {
		rel *= getCurWareVersion(i, Ware::Type::SW).rel *
			getCurWareVersion(i, Ware::Type::HW).rel;
	}
	return rel;
}

double System::getCost()
{
	int i;
	double cost = 0.0;
	int nModules = getNModules();
	for(i = 0; i < nModules; i++) {
		cost += getCurWareVersion(i, Ware::Type::SW).cost +
			getCurWareVersion(i, Ware::Type::HW).cost;
	}
	return cost;
}

void sysGenFromXml(System& system, const char* filename)
{
//предполагается, что в xml-файле все модули и версии нумеруются
//непрерывно монотонно, начиная с 1
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
			System::Ware::Type wareType = System::Ware::intToType(k);
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

void sysSaveToXml(System& system, const char* filename)
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
			System::Ware::Type wareType = System::Ware::intToType(k);
			const char* wareTypeStr = k == 0 ? "sw" : "hw"; //V replace with static
			int nWare = system.getNWareVersions(i, wareType);
			for(int j = 1; j <= nWare; j++) {
				pugi::xml_node wareNode = 
					moduleNode.append_child(wareTypeStr);
				wareNode.append_attribute("num") = 
					system.getWareVersion(i, wareType, j).num;
				sprintf(buf, "%.3lf", 
					system.getWareVersion(i, wareType, j).rel);
				wareNode.append_attribute("rel") = buf;
				sprintf(buf, "%.3g", 
					system.getWareVersion(i, wareType, j).cost);
				wareNode.append_attribute("cost") = buf;
			}
		}
	}
    //doc.print(cout); //DEBUG
    doc.save_file(filename);
}

/*
int findMinGEWareNo(const System& system, double val, int moduleNo, int wareType)
{
//find ware version with minimum rel. of those which >= val
//in module(moduleNo)
	int nVers;
	if(wareType = hw) = system.getNHardVersions(moduleNo);
	double min = 2.0;
	double testVal;
	int ans = 0;
	for(int j = 1; j <= nVers; j++) {
		testVal = system.getHardVersion(moduleNo, j).rel;
		if(testVal < min && testVal >= val) {
			min = testVal;
			ans = j;
		}
	}
	return ans;
}
*/

void sortVersions(System& system)
{
	int nModules = system.getNModules();
	for(int i = 0; i < nModules; i++) {
		sort(system.modules[i].softVersions.begin(),
			system.modules[i].softVersions.end() );
		//третий арг. - operator< (по умолч.)
		sort(system.modules[i].hardVersions.begin(),
			system.modules[i].hardVersions.end() );
		//третий арг. - operator< (по умолч.)
	}
}

void findOptGenerous(System& system) 
{
/*
 * a < b === должен ли a идти перед b
 * comp = operator<=
 */
	double rel = 1;
	double cost0, cost;
	double limitCost = system.limitCost();
	System system1 = system;
	sortVersions(system1);
	int nModules = system.getNModules();
	for(int i = 1; i <= nModules; i++) {
		for(int k = 0; k < 2; k++) {
			System::Ware::Type wareType = System::Ware::intToType(k);
			system1.curWareVersionNo(i, wareType) = 1;
				//1, т.к. берем наименее надежную
				//пока храним в качестве curWareVersionNo
				//порядковый номер версии в отсортированном массиве
		}
	}
	cost0 = system1.getCost();
	double minRel = 2.0;
	int minRelModuleNo;
	System::Ware::Type minRelWareType;
	int minRelWareNo;
	while(cost0 < limitCost) {
		cost = cost0;
		double testRel;
		for(int i = 1; i <= nModules; i++) {
			for(k = 0; k < 2; k++) {
				System::Ware::Type wareType = 
					System::Ware::intToType(k);
				int wareNo = system1.curWareVersionNo(i, wareType);
				testRel = 
					system1.getWareVersion(i, wareType, wareNo).rel;
				if (testRel < minRel) {
					minRel = testRel;
					minRelModuleNo = i;
					minRelWareType = wareType;
					minRelWareNo = WareNo;
				}
			}
		}
		cost += - system1.getWareVersion(minRelModuleNo,
			minRelWareType, minRelWareNo)
				+ system1.getWareVersion(minRelModuleNo,
			minRelWareType, minRelWareNo + 1);
		
				
	}
		
		
}
	
int main(int argc, const char** argv)
{
	System system;
	sysGenFromXml(system, "example.xml");
	System system1 = system;
	sortVersions(system1);
	system.printTest();
	system1.printTest();
	sysSaveToXml(system, "out.xml");
	sysSaveToXml(system, "out2.xml");
	return 0;
}
