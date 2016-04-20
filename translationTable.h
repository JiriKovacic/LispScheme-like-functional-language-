#ifndef TRANSLATIONTABLE_H_INCLUDED
#define TRANSLATIONTABLE_H_INCLUDED

#include <iostream>

using namespace std;

const unsigned int MAXIMAL_TRANSLATIONTABLE_SIZE (1024);

class TranslationTable {
private:
	// Globalni prekladova tabulka, nazev <==> index
	string *translationTable = new string[MAXIMAL_TRANSLATIONTABLE_SIZE];
	int index;
public:
	int addRecord(string name); // pridat zaznam, jmeno promenne nebo funkce
	void deleteRecord(string name);
	void deleteRecord(int index);
	int getIndex(string name);
	int findIndex(int index);
	string getName(int index);
};

#endif // TRANSLATIONTABLE_H_INCLUDED