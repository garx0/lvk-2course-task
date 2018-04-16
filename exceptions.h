#pragma once

#include "./src/pugixml.hpp"

using namespace std;

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
	Type getType() const;
	void printErrMsg(ostream& stream) const;
	// Печатает сообщение об ошибке в заданный поток
};

class PugiXmlExc {
	pugi::xml_parse_result result;
public:
	PugiXmlExc(pugi::xml_parse_result a_result): result(a_result) {}
	pugi::xml_parse_result getResult() const;
	void printErrMsg(ostream& stream) const;
	// Печатает сообщение об ошибке в заданный поток
};

