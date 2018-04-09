#include <iostream>
#include <vector>
#include <string>

#include "./src/pugixml.hpp"

using namespace std;

/*
template<class T> class Array {
	T* ptr;
	int size;
public:
	Array(): size(0), ptr(nullptr) {}
	Array(int a_size);
	setSize(int a_size);
	//...
};

Array<T>::Array(int a_size) {
	if (a_size < 0) {
		throw false;
	}
	size = a_size;
	if (size == 0) {
		ptr = new T[a_size]();
	} else {
		ptr = nullptr;
	}
}

Array<T>::setSize(int a_size)
{
	if (a_size < 0) throw false;
	if (size == 0) {
		size = a_size;
		if(a_size != 0) {
			ptr = new T[a_size]();
		}
	} else {
		throw false;
	}
}
*/

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
		friend System;
		double rel;
		double cost;
		Ware(): rel(1.0), cost(0.0) {}
		Ware(double a_rel, double a_cost): rel(a_rel), cost(a_cost) {}
	};
	struct Hardware : Ware {
		friend System;
		Hardware(double rel, double cost): Ware(rel, cost) {}
	};
	struct Software : Ware {
		friend System;
		Software(double rel, double cost): Ware(rel, cost) {}
	};
private:
	class Module {
		friend System;
		vector<Hardware> hardVersions;
		vector<Software> softVersions;
		int curHardVersionNo_;
		int curSoftVersionNo_;
	};
private:
	vector<Module> modules;
	double limitCost_;
	//в реализации:
	//	в векторах внутри Module и System нумерация идет от 0,
	// 	в полях cur~~~~VersionNo_ - от 1
	//в интерфейсе:
	//	нумерация версий и модулей идет от 1
public:
	int getNModules() const {return modules.size();}
	int getNHardVersions(int moduleNo) const {
		return modules[moduleNo - 1].hardVersions.size();
	}
	int getNSoftVersions(int moduleNo) const {
		return modules[moduleNo - 1].softVersions.size();
	}
	const Hardware& getHardVersion(int moduleNo, int versionNo) const {
		return modules[moduleNo - 1].hardVersions[versionNo - 1];
	}
	const Software& getSoftVersion(int moduleNo, int versionNo) const {
		return modules[moduleNo - 1].softVersions[versionNo - 1];
	}
	void pushBackEmptyModule() {
		modules.push_back(Module());
		cout << "pushed back empty module, hwsize = " <<     //DEBUG
			modules[modules.size() - 1].hardVersions.size() << endl; //DEBUG
	}
	void pushBackHardVersion(int moduleNo, double rel, double cost) {
		modules[moduleNo - 1].hardVersions.push_back(Hardware(rel, cost));
	}	
	void pushBackSoftVersion(int moduleNo, double rel, double cost) {
		modules[moduleNo - 1].softVersions.push_back(Software(rel, cost));
	}	
	int& curHardVersionNo(int moduleNo) {
		return modules[moduleNo - 1].curHardVersionNo_;
	}
	int curHardVersionNo(int moduleNo) const {
		return modules[moduleNo - 1].curHardVersionNo_;
	}
	int& curSoftVersionNo(int moduleNo) {
		return modules[moduleNo - 1].curSoftVersionNo_;
	}
	int curSoftVersionNo(int moduleNo) const {
		return modules[moduleNo - 1].curSoftVersionNo_;
	}
	const Hardware& getCurHardVersion(int moduleNo) const {
		return getHardVersion(moduleNo, curHardVersionNo(moduleNo));
	}
	const Software& getCurSoftVersion(int moduleNo) const {
		return getSoftVersion(moduleNo, curSoftVersionNo(moduleNo));
	}
	double getRel();
	double getCost();
	const double& limitCost() const {return limitCost_;}
	double& limitCost() {return limitCost_;}
};

double System::getRel() {
	int i;
	double rel = 1.0;
	int nModules = getNModules();
	for(i = 0; i < nModules; i++) {
		rel *= getCurHardVersion(i).rel * getCurSoftVersion(i).rel;
	}
	return rel;
}

double System::getCost() {
	int i;
	double cost = 0.0;
	int nModules = getNModules();
	for(i = 0; i < nModules; i++) {
		cost += getCurHardVersion(i).cost + getCurSoftVersion(i).cost;
	}
	return cost;
}

void sysGenFromXml(System& system, const char* filename) {
//предполагается, что в xml-файле все модули и версии нумеруются
//непрерывно монотонно, начиная с 1
	pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename);
    cout << "Load result: " << result.description() << endl; //DEBUG
    if(!result)
		throw PugiXmlExc(result);
    pugi::xml_node sysNode = doc;
    sysNode = sysNode.child("system");
    system.limitCost() = stod( sysNode.attribute("limitcost").value() );
    cout << "sys limitCost = " << system.limitCost() << endl; //DEBUG
    int i = 1;
    for(pugi::xml_node iNode = sysNode.child("module"); 
			iNode;
			iNode = iNode.next_sibling("module"), i++ )
	{
		system.pushBackEmptyModule();
		const char* wareType[2] = {"hw", "sw"};
		cout << "in i-cycle\n"; //DEBUG
		for(int k = 0; k < 2; k++) {
			for(pugi::xml_node jNode = iNode.child(wareType[k]);
					jNode;
					jNode = jNode.next_sibling(wareType[k]) )
			{
				cout << "in j-cycle1\n"; //DEBUG
				string str1 = jNode.attribute("rel").value();
				double rel = stod(str1);
				string str2 = jNode.attribute("cost").value();
				double cost = stod(str2);
				cout << "in j-cycle2\n"; //DEBUG
				if(k == 0) {
					cout << "in j-cycle3h\n"; //DEBUG
					system.pushBackHardVersion(i, rel, cost);
					cout << "in j-cycle4h\n"; //DEBUG
				} else {
					cout << "in j-cycle3s\n"; //DEBUG
					system.pushBackSoftVersion(i, rel, cost);
					cout << "in j-cycle4s\n"; //DEBUG
				}
			}
		}
	}	
}

/*
void sysSaveToXml(System& system, const char* filename) {
//сохраняет данные о системе и всех версиях оборудования
	pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename);
    cout << "Load result: " << result.description() << endl; //DEBUG
    if(!result)
		throw PugiXmlExc(result);
    pugi::xml_node sysNode = doc;
    sysNode = sysNode.child("system");
    system.limitCost() = stod( sysNode.attribute("limitcost").value() );
    cout << "sys limitCost = " << system.limitCost() << endl; //DEBUG
    int i = 1;
    for(pugi::xml_node iNode = sysNode.child("module"); 
			iNode;
			iNode = iNode.next_sibling("module"), i++ )
	{
		system.pushBackEmptyModule();
		const char* wareType[2] = {"hw", "sw"};
		cout << "in i-cycle\n"; //DEBUG
		for(int k = 0; k < 2; k++) {
			for(pugi::xml_node jNode = iNode.child(wareType[k]);
					jNode;
					jNode = jNode.next_sibling(wareType[k]) )
			{
				cout << "in j-cycle1\n"; //DEBUG
				string str1 = jNode.attribute("rel").value();
				double rel = stod(str1);
				string str2 = jNode.attribute("cost").value();
				double cost = stod(str2);
				cout << "in j-cycle2\n"; //DEBUG
				if(k == 0) {
					cout << "in j-cycle3h\n"; //DEBUG
					system.pushBackHardVersion(i, rel, cost);
					cout << "in j-cycle4h\n"; //DEBUG
				} else {
					cout << "in j-cycle3s\n"; //DEBUG
					system.pushBackSoftVersion(i, rel, cost);
					cout << "in j-cycle4s\n"; //DEBUG
				}
			}
		}
	}	
}
*/

int main(int argc, const char** argv) {
	System system;
	sysGenFromXml(system, "example.xml");
	return 0;
}
