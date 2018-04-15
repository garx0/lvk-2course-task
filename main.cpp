#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>

#include "system.h"
#include "funcs.h"
#include "exceptions.h"

using namespace std;

void testProg1(const char* inFile, const char* outFileGenerous, 
	const char* outFileGreedy)
{
	System system;
	sysReadFromXml(system, inFile);
	int iter = findOptGenerous(system);
	sysCombSaveToXml(system, iter, outFileGenerous);
	findOptGreedy(system);
	sysCombSaveToXml(system, 1, outFileGreedy);
}

void testProg2()
{
	System system;
	sysReadFromXml(system, "example.xml");
	int saveLim = system.limitCost();
	int iter;
	char buf[64];
	cout.precision(3);
	for(int lim = 150; lim <= 310; lim += 10) {
		system.limitCost() = lim;
		
		iter = findOptGenerous(system);
		cout << "rel(" << lim << ", generous) = " << 
			system.getRel() << endl;
		sprintf(buf, "out(%d)(generous).xml", lim);
		sysCombSaveToXml(system, iter, buf);
		
		findOptGreedy(system);
		cout << "rel(" << lim << ",   greedy) = " << 
			system.getRel() << endl << endl;
		sprintf(buf, "out(%d)(greedy).xml", lim);
		sysCombSaveToXml(system, 1, buf);
	}
	system.limitCost() = saveLim;
}

void testProg3()
{
	System system;
	sysGen(system, 5, 5, 7, 300, 0.75, 15.0, 0.0);
	system.printTest();
	sysSaveToXml(system, "out1.xml");
	System system1 = system;
	sortVersions(system1);
	sysSaveToXml(system1, "out2.xml");
	system1.printTest();
	char buf[64];
	for(int lim = 230; lim > 0; lim -= 10) {
		system.limitCost() = lim;
		
		int iter = findOptGenerous(system, 1);
		cout << "rel(" << lim << ", generousv1) = " << 
			system.getRel() << endl;
		sprintf(buf, "out(%d)(generousv1).xml", lim);
		//sysCombSaveToXml(system, iter, buf);
		
		iter = findOptGenerous(system, 2);
		cout << "rel(" << lim << ", generousv2) = " << 
			system.getRel() << endl;
		sprintf(buf, "out(%d)(generousv2).xml", lim);
		//sysCombSaveToXml(system, iter, buf);
		
		/*
		findOptGreedy(system);
		cout << "rel(" << lim << ",   greedy) = " << 
			system.getRel() << endl << endl;
		sprintf(buf, "out(%d)(greedy).xml", lim);
		*/
		//sysCombSaveToXml(system, 1, buf);
	}
}

void testProg4()
{
	cout.precision(3);
	for(double rel = 0.8; rel <= 1.0; rel += 0.01) {
		cout << rel << " => ";
		double slp = 0.97;
		//cout << genCostCurve(rel, slp) << ", ";
		cout << genCost(rel, slp, 15.0, 1.0) << endl;
	}
}

int main(int argc, const char** argv)
{
	srand(10);
	/*
	System system;
	sysReadFromXml(system, "example.xml");
	System system1 = system;
	sortVersions(system1);
	system.printTest();
	system1.printTest();
	sysSaveToXml(system, "out.xml");
	sysSaveToXml(system, "out2.xml");
	*/
	//testProg1("example.xml", "out(generous).xml", "out(greedy).xml");
	testProg3();
	return 0;
}
