#include "./src/pugixml.hpp"

#include <iostream>
#include <string>

using namespace std;

int main()
{
// tag::code[]
    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file("example.xml");

    cout << "Load result: " << result.description() << endl;
    pugi::xml_node node = doc;
    sysNode = node.child("system");
    double limitCost = stod( sysNode.attribute("limitcost").value() );
    
    cout << "sys limitcost = " << limitcost << endl;
    //cout << doc.child("mesh").child("node").attribute("attr1").value() << endl;
// end::code[]
}


