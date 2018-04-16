#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>

#include "system.h"
#include "funcs.h"
#include "exceptions.h"

using namespace std;

void testsOnExample()
{
	System system;
	sysReadFromXml(system, "example.xml");
	char buf[256];
	int iter;
	double limitCost = system.limitCost();
	double rel, cost;
	for(double lim = 140.0; lim <= 310.0; lim += 10.0) {
		system.limitCost() = lim;
		for(int variant = 1; variant <= 4; variant++) {
			iter = findOptGenerous(system, variant);
			rel = iter > 0 ? system.getRel() : 0.0;
			cost = iter > 0 ? system.getCost() : 0.0;
			printf("rel(limitCost = %5.5g, alg = generous%d) = %5.5lf, cost = %5.5g, iter = %d\n",
				lim, variant, rel, cost, iter);
			sprintf(buf, "out(%.3lf)(generousv%d).xml", lim, variant);
			sysConfigSaveToXml(system, iter, buf);
		}
		for(int variant = 1; variant <= 4; variant++) {
			iter = findOptGenerousBwd(system, variant);
			rel = iter > 0 ? system.getRel() : 0.0;
			cost = iter > 0 ? system.getCost() : 0.0;
			printf("rel(limitCost = %5.5g, alg = generousBwd%d) = %5.5lf, cost = %5.5g, iter = %d\n",
				lim, variant, rel, cost, iter);
			sprintf(buf, "out(%.3lf)(generousBwdv%d).xml", lim, variant);
			sysConfigSaveToXml(system, iter, buf);
		}
		iter = findOptBrute(system);
		rel = iter > 0 ? system.getRel() : 0.0;
		cost = iter > 0 ? system.getCost() : 0.0;
		printf("rel(limitCost = %5.5g, alg =     brute) = %5.5lf, cost = %5.5g, iter = %d\n",
				lim, rel, cost, iter);
		sprintf(buf, "out(%.3lf)(brute).xml", lim);
		sysConfigSaveToXml(system, 1, buf);
		cout << endl;
	}
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
			//sysConfigSaveToXml(system, iter, buf);
		}
		/*
		iter = findOptBrute(system);
		printf("rel(limitCost = %5.5g, alg =    brute) = %5.5lf, cost = %5.5g, iter = %d\n",
				lim, system.getRel(), system.getCost(), iter);
		*/
		//sprintf(buf, "out(%d)(brute).xml", lim);
		//sysConfigSaveToXml(system, 1, buf);
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

void saveOptGenerous(const System& system, int variant, const char* fileNamePrefix)
/* Пример задания аргумента fileNamePrefix:
 * fileNamePrefix = "example"
 * => file names:
 * "example (generous-v1).xml"
 * ...
 * "example (generous-v4).xml"
 */
{
	if(variant < 1 || variant > 4) throw Exc(Exc::BAD_ARGS);
	System system1 = system;
	char buf[1024];
	int iter = findOptGenerous(system1, variant);
	sprintf(buf, "%s (generous-v%d).xml", fileNamePrefix, variant);
	sysConfigSaveToXml(system1, iter, buf);
}

void saveOptBrute(const System& system, const char* fileNamePrefix)
/* Пример задания аргумента fileNamePrefix:
 * fileNamePrefix = "example"
 * => file name = "example (brute).xml"
 */
{
	System system1 = system;
	char buf[1024];
	int iter = findOptBrute(system1);
	sprintf(buf, "%s (brute).xml", fileNamePrefix);
	sysConfigSaveToXml(system1, iter, buf);
}
	
int main(int argc, const char** argv)
{
	try {
		/*
		System system;
		sysReadFromXml(system, "example.xml");
		//sysGen(system, 2, 2, 2);
		system.limitCost() = 150;
		//sysSaveToXml(system, "in.xml");
		saveOptGenerous(system, 1, "out.xml");
		saveOptGenerous(system, 2, "out.xml");
		saveOptGenerous(system, 3, "out.xml");
		saveOptGenerous(system, 4, "out.xml");
		saveOptBrute(system, "out.xml");
		*/
		testsOnExample();
		return 0;
	}
	catch(const Exc& exc) {
		exc.printErrMsg(cerr);
	}
	catch(const PugiXmlExc& exc) {
		exc.printErrMsg(cerr);
	}
	return -1;
}
