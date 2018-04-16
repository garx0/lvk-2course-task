#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "exceptions.h"

using namespace std;

struct Ware {
	/* Структура для представления версии ПО/О. Все поля открыты.
	 * Поля rel, cost содержат надежность и стоимость версии соответственно.
	 * Поле num - вспомогательное, используется для алгоритма findOptGenerous
	 * 	   поиска оптимальной конфигурации системы. Обычно содержит 
	 *     порядковый номер версии в системе
	 * Перечислимый тип Type позволяет вызывать функции, которые определены позже,
	 *     для конкретного типа ПО/О - либо ПО, либо оборудование
	 * Ware::SW ~ ПО ~ 0
	 * Ware::HW ~ оборуд. ~ 1
	 * Можно приводить целое число из {0, 1} к соответствующему типу из {SW, HW}
	 *     при помощи функции intToType
	 * Можно вызывать конструктор с 0, 2 или 3 параметрами, соответствующие
	 *     полям rel, cost, num.
	 * Ware имеет две производных структуры Software и Hardware, описывающие версии
	 *     типа ПО и О. Содержат статическое константное поле с соответствующим 
	 *     значением из {SW, HW}
	 */
	enum Type { SW, HW };
	//static char* const wareTypeStr[] = {"sw", "hw"};
	static Type intToType(int num) {
		switch(num) {
			case 0: return SW;
			case 1: return HW;
			default: throw Exc(Exc::BAD_ARGS);
		}
	}
	double rel;
	double cost;
	int num;
	Ware(): rel(1.0), cost(0.0), num(1) {}
	Ware(double a_rel, double a_cost, int a_num = 1): 
		rel(a_rel), cost(a_cost), num(a_num) {}
};

struct Software : Ware {
	static const Type type = SW;
	Software(): Ware() {}
	Software(double rel, double cost, int num = 1):
		Ware(rel, cost, num) {}
};

struct Hardware : Ware {
	static const Type type = HW;
	Hardware(): Ware() {}
	Hardware(double rel, double cost, int num = 1): 
		Ware(rel, cost, num) {}
};

class System {
	/* описывает все версии ПО/О для каждого модуля системы,
	 *     а также выбранную конфигурацию этих версий и ограничение
	 *     на стоимость конфигурации.
	 */
	class Module {
		friend System;
		vector<Software> softVersions;
		vector<Hardware> hardVersions;
		int curSoftVersionNo;
		int curHardVersionNo;
		/* Номера выбранных в конфигурации системы версий ПО и О
		 *     (с учетом, что в интерфейсе класса версии нумеруются
		 *     от 1).
		 * При создании системы посредством интерфейса класса 
		 *     поля num всех объектов класса Ware совпадают с 
		 *     порядковыми (в плане интерфейса) номерами соответствующего 
		 *     ПО/О в данном модуле.
		 * Если переставить элементы в векторе из версий ПО/О,
		 *     то настоящие порядковые номера будут содержаться только в
		 *     полях num этих версий.
		 */
		friend void sortVersions(System& system, int variant = 1);
		/* Используется для реализации алгоритма findOptGenerous
		 *     поиска оптимальной конфигурации системы.
		 */
	public:
		Module(): softVersions(), hardVersions(), 
			curSoftVersionNo(1), curHardVersionNo(1) {}
		Module(const Module& module);
		~Module() { 
			softVersions.clear(); hardVersions.clear(); 
		}
	};

	vector<Module> modules;
	double limitCost_;
	/* В реализации:
	 *     В векторах внутри Module и System нумерация идет от 0,
	 *     В полях cur~~~~VersionNo_ - от 1.
	 * В интерфейсе:
	 *     Нумерация версий и модулей идет от 1.
	 */
	friend void sortVersions(System& system, int variant);
public:
	System(): limitCost_(0) {}
	System(const System& system);
	~System() { 
		modules.clear();
	}
	System& operator=(const System& system);
	int getNModules() const;
	// Выдает количество модулей в системе по значению.
	int getNWare(int moduleNo, Ware::Type wareType) const;
	// Выдает количество версий ПО/О типа wareType в указанном модуле.
	const Ware& getWare(int moduleNo, Ware::Type wareType, 
		int versionNo) const;
	/* Выдает объект типа Ware, представляющий указанную версию ПО/О
	 *     типа wareType в указанном модуле (по конст. ссылке).
	 */
	void pushBackEmptyModule();
	// Добавляет новый пустой модуль в систему
	void pushBackWare(int moduleNo, Ware::Type wareType, 
		double rel, double cost, int num = 0);
	/* Добавляет новую версию ПО/О с заданными типом и характеристиками
	 *     в указанный модуль.
	 * Если не указать последний параметр, поле num заполнится
	 *     автоматически в соответствии с нумерацией в системе
	 *     (если она правильная).
	 */
	int& curWareNo(int moduleNo, Ware::Type wareType);
	int curWareNo(int moduleNo, Ware::Type wareType) const;
	/* Выдает ссылку на номер выбранного в конфигурации системы ПО/О
	 *     типа wareType в указанном модуле
	 *     (используется как setter/getter, может использоваться
	 *     в качестве getter'а в методе, где нельзя изменять объект 
	 *     класса System).
	 */
	const Ware& getCurWare(int moduleNo, 
		Ware::Type wareType) const;
	/* Выдает объект типа Ware, представляющий выбранную в конфигурации
	 *     системы версию ПО/О типа wareType в указанном модуле 
	 *     (по конст. ссылке).
	 */
	double getRel() const; 
	// Вычисляет надежность данной конфигурации системы.
	double getCost() const;
	// Вычисляет стоимость данной конфигурации системы.
	double limitCost() const;
	double& limitCost();
	/* Выдает ссылку на значение ограничения по стоимости
	 *     (используется как setter/getter, может использоваться
	 *     в качестве getter'а в методе, где нельзя изменять объект 
	 *     класса System).
	 */
	void clear();
	// Очищает всю информацию в данном объекте типа System.
	void printTest() const; //DEBUG
};


bool cmpLess1(const Ware&, const Ware&);
// Сравнение версий ПО/О по надежности.

bool cmpLess2(const Ware&, const Ware&);
// Сравнение версий по надежности, либо по стоимости, если надежности равны.

bool cmpLess3(const Ware&, const Ware&);
// Сравнение версий по стоимости.

bool cmpLess4_(const Ware&, const Ware&, 
	double relDiffThres, double costRelationThres);
/* Сравнение версий либо по надежности, либо по стоимости по правилу:
 *     Если надежности "сильно" различаются, сравниваем по надежности.
       Если надежности не "сильно" различаются, а стоимости - "сильно",
           сравниваем по цене.
       Если надежности не "сильно" различаются, и стоимости - не "сильно",
           сравниваем по надежности.
 * Понятие "сильного" различия определяется через значения порогов для 
 *     надежности и стоимости, которые задаются в последних двух параметрах.
 * Два значения надежности различается "сильно", если разность большего
 *     и меньшего значений больше relDiffThres
 * Два значения стоимости различаются "сильно", если частное меньшего
 *     и большего значений меньше costRelationThres
 * Если задать пороги как 0.0, 0.0, то получится cmpLess1
 * 
 * Такая операция сравнения не транзитивна!
 */
 
bool cmpLess4(const Ware&, const Ware&);
/* Версия предыдущей функции с двумя параметрами, где значения порогов
 *     зафиксированы.
 */
 
bool cmpLess(const Ware&, const Ware&, int variant = 1);
/* Позволяет вызвать одну из четырех функций CmpLess{variant}
 *     через указание параметра variant, variant in [1, 4]
 */
