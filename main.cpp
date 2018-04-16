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
	const char* outFileBrute)
{
	System system;
	sysReadFromXml(system, inFile);
	int iter = findOptGenerous(system);
	sysCombSaveToXml(system, iter, outFileGenerous);
	findOptBrute(system);
	sysCombSaveToXml(system, 1, outFileBrute);
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
		
		findOptBrute(system);
		cout << "rel(" << lim << ",   brute) = " << 
			system.getRel() << endl << endl;
		sprintf(buf, "out(%d)(brute).xml", lim);
		sysCombSaveToXml(system, 1, buf);
	}
	system.limitCost() = saveLim;
}

void testProg3()
{
	System system;
	sysGen(system, 100, 50, 50, -1.0, 0.9, 15.0, 0.3);
	cout << system.limitCost() << endl;
	/*
	for(int i = 1; i <= 10; i++) {
		srand(i);
		cout << "avg = " << sysAvgCost(system, 5000) << endl;
	}
	*/
	//return;
	//system.printTest();
	sysSaveToXml(system, "out1.xml");
	//System system1 = system;
	//sortVersions(system1);
	//sysSaveToXml(system1, "out2.xml");
	//system1.printTest();
	char buf[64];
	int iter;
	double limitCost = system.limitCost();
	for(double lim = limitCost; lim > limitCost / 8; lim -= 500) {
		system.limitCost() = lim;
		for(int variant = 2; variant <= 4; variant++) {
			iter = findOptGenerous(system, variant);
			printf("rel(limitCost = %5.5g, alg = generous%d) = %5.5lf, cost = %5.5g, iter = %d\n",
				lim, variant, system.getRel(), system.getCost(), iter);
			//sprintf(buf, "out(%.3lf)(generousv%d).xml", lim, variant);
			//sysCombSaveToXml(system, iter, buf);
		}
		/*
		iter = findOptBrute(system);
		printf("rel(limitCost = %5.5g, alg =    brute) = %5.5lf, cost = %5.5g, iter = %d\n",
				lim, system.getRel(), system.getCost(), iter);
		*/
		//sprintf(buf, "out(%d)(brute).xml", lim);
		//sysCombSaveToXml(system, 1, buf);
		cout << endl;
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

void saveOptGenerous(const System& system, int variant, const char* filename)
{
	if(variant < 1 || variant > 4) throw Exc(Exc::BAD_ARGS);
	System system1 = system;
	char buf[1024];
	int iter = findOptGenerous(system1, variant);
	sprintf(buf, "%s (generous-v%d).xml", filename, variant);
	sysCombSaveToXml(system1, iter, buf);
}

void saveOptBrute(const System& system, const char* filename)
{
	System system1 = system;
	char buf[1024];
	for(int variant = 2; variant <= 4; variant++) {
		int iter = findOptBrute(system1);
		sprintf(buf, "%s (brute).xml", filename);
		sysCombSaveToXml(system1, iter, buf);
	}
}
	
int main(int argc, const char** argv)
{
	System system;
	sysReadFromXml(system, "example.xml");
	//sysGen(system, 2, 2, 2);
	system.limitCost() = 150;
	//sysSaveToXml(system, "in.xml");
	cout << "read\n";
	saveOptGenerous(system, 1, "out.xml");
	saveOptGenerous(system, 2, "out.xml");
	saveOptGenerous(system, 3, "out.xml");
	saveOptGenerous(system, 4, "out.xml");
	cout << "generous\n";
	saveOptBrute(system, "out.xml");
	cout << "brute\n";
	return 0;
}
