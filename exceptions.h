#pragma once

#include "./src/pugixml.hpp"

class Exc {
public:
	enum Type {
		UNKNOWN_ERR,
		BAD_ARGS
	};
private:
	Type type;
public:
	Exc(Type a_type = UNKNOWN_ERR): type(a_type) {}
};

class PugiXmlExc {
	pugi::xml_parse_result result;
public:
	PugiXmlExc(pugi::xml_parse_result a_result): result(a_result) {}
};

