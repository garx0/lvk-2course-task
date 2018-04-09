#include "./src/pugixml.hpp"

#include <iostream>

int main()
{
// tag::code[]
    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file("tree.xml");

    std::cout << "Load result: " << result.description() << ", mesh name: " << doc.child("mesh").attribute("name").value() << std::endl;
    std::cout << doc.child("mesh").child("node").attribute("attr1").value() << std::endl;
// end::code[]
}

// vim:et

