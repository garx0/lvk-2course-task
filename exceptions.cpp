#include <iostream>
#include <string>
#include "./src/pugixml.hpp"

#include "exceptions.h"

using namespace std;

Exc::Type Exc::getType() const
{	
	return type;
}

void Exc::printErrMsg(ostream& stream) const
{
	switch(type) {
		case UNKNOWN_ERR :
			stream << "unknown error" << endl;
			break;
		case BAD_ARGS :
			stream << "error: invalid arguments were passed in some function" << endl;
			break;
	}
}

pugi::xml_parse_result PugiXmlExc::getResult() const
{
	return result;
}

void PugiXmlExc::printErrMsg(ostream& stream) const
{
	stream << "error while reading xml:" << result.description() << endl;
}
