#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>

#include "system.h"
#include "funcs.h"
#include "exceptions.h"

using namespace std;

void testsOnExample(ostream& stream = cout)
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
			sprintf(buf, "rel(limitCost = %5.5g, alg =    generous%d) = %5.5lf, cost = %5.5g, iter = %d\n",
				lim, variant, rel, cost, iter);
			stream << buf;
			sprintf(buf, "./tests_on_example/out(%.3lf)(generousv%d).xml", lim, variant);
			sysConfigSaveToXml(system, iter, buf);
		}
		for(int variant = 1; variant <= 4; variant++) {
			iter = findOptGenerousBwd(system, variant);
			rel = iter > 0 ? system.getRel() : 0.0;
			cost = iter > 0 ? system.getCost() : 0.0;
			sprintf(buf, "rel(limitCost = %5.5g, alg = generousBwd%d) = %5.5lf, cost = %5.5g, iter = %d\n",
				lim, variant, rel, cost, iter);
			stream << buf;
			sprintf(buf, "./tests_on_example/out(%.3lf)(generousBwdv%d).xml", lim, variant);
			sysConfigSaveToXml(system, iter, buf);
		}
		iter = findOptBrute(system);
		rel = iter > 0 ? system.getRel() : 0.0;
		cost = iter > 0 ? system.getCost() : 0.0;
		sprintf(buf, "rel(limitCost = %5.5g, alg =        brute) = %5.5lf, cost = %5.5g, iter = %d\n",
				lim, rel, cost, iter);
		stream << buf;		
		sprintf(buf, "./tests_on_example/out(%.3lf)(brute).xml", lim);
		sysConfigSaveToXml(system, 1, buf);
		cout << endl;
	}
}

void testsOnGenerated(System& system, ostream& stream = cout, int num = 1)
{
	char buf[256];
	sprintf(buf, "./tests%d/gen input%d.xml", num, num); 
	sysSaveToXml(system, buf);
	int iter;
	double limitCost = system.limitCost();
	double rel, cost;
	for(double lim = limitCost * 0.8; lim > limitCost / 8.0; 
		lim -= limitCost / 15.0)
	{
		system.limitCost() = lim;
		for(int variant = 1; variant <= 4; variant++) {
			iter = findOptGenerous(system, variant);
			rel = iter > 0 ? system.getRel() : 0.0;
			cost = iter > 0 ? system.getCost() : 0.0;
			sprintf(buf, "rel(limitCost = %5.5g, alg =    generous%d) = %5.5lf, cost = %5.5g, iter = %d\n",
				lim, variant, rel, cost, iter);
			stream << buf;
			sprintf(buf, "./tests%d/out%d(%.3lf)(generousv%d).xml", num, num, lim, variant);
			sysConfigSaveToXml(system, iter, buf);
		}
		for(int variant = 1; variant <= 4; variant++) {
			iter = findOptGenerousBwd(system, variant);
			rel = iter > 0 ? system.getRel() : 0.0;
			cost = iter > 0 ? system.getCost() : 0.0;
			sprintf(buf, "rel(limitCost = %5.5g, alg = generousBwd%d) = %5.5lf, cost = %5.5g, iter = %d\n",
				lim, variant, rel, cost, iter);
			stream << buf;
			sprintf(buf, "./tests%d/out%d(%.3lf)(generousv%d).xml", num, num, lim, variant);
			sysConfigSaveToXml(system, iter, buf);
		}
		stream << endl;
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

void saveOptGenerousBwd(const System& system, int variant, const char* fileNamePrefix)
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
	int iter = findOptGenerousBwd(system1, variant);
	sprintf(buf, "%s (generousBwd-v%d).xml", fileNamePrefix, variant);
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
	/* перед выполнением программы надо создать папки:
	 * ./tests_on_example, ./tests1, ./tests2, ..., ./tests6
	 */
	try {
		ofstream ofile;
		ofile.open("./tests_on_example/results_brief.txt");
		testsOnExample(ofile);
		ofile.close();
		ofile.clear();
		
		System system[6];
		char buf[256];
		sysGen(system[0], 10, 10, 10, -1.0, 0.9, 15.0, 0.1);
		sysGen(system[1], 10, 10, 10, -1.0, 0.9, 15.0, 0.2);
		sysGen(system[2], 10, 10, 10, -1.0, 0.9, 15.0, 0.4);
		sysGen(system[3], 10, 10, 10, -1.0, 0.9, 15.0, 0.9);
		sysGen(system[4], 40, 60, 50, -1.0, 0.9, 15.0, 0.2);
		sysGen(system[5], 40, 60, 50, -1.0, 0.9, 15.0, 0.6);
		for(int i = 1; i <= 6; i++) {
			sprintf(buf, "./tests%d/results_brief.txt", i);
			ofile.open(buf);
			testsOnGenerated(system[i - 1], ofile, i);
			ofile.close();
			ofile.clear();
		}
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
