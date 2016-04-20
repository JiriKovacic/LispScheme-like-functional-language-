#include "translationTable.h"

int TranslationTable::addRecord(string name)
{	
	int idx = getIndex(name);
	if (idx >= 0) return idx;

	for (int i = 0; i < MAXIMAL_TRANSLATIONTABLE_SIZE; i++)
	{
		if (translationTable[i] == "")
		{
			translationTable[i] = name;
			return i; // povedlo se ulozit
		}
	}
	return -1; // nepovedlo se ulozit, pravdepodobna chyba => mala tabulka
}	
	
void TranslationTable::deleteRecord(string name)
{	
	for (int i = 0; i < MAXIMAL_TRANSLATIONTABLE_SIZE; i++)
	{
		if (translationTable[i] == name)
		{
			translationTable[i] == "";
			break;
		}
	}
}	
	
void TranslationTable::deleteRecord(int index)
{
	translationTable[index] = "";
}

int TranslationTable::getIndex(string name)
{
	for (int i = 0; i < MAXIMAL_TRANSLATIONTABLE_SIZE; i++)
	{
		if (translationTable[i] == name)
		{
			return i;
		}
	}
	return -1; // nenaslo se v tabulce
}

int TranslationTable::findIndex(int index) // je to strileni na slepo, nevime, o co se jedna, jen, jestli ten index je obsazeny
{
	if (translationTable[index] != "")
		return index;
	else
		return -1;
}

string TranslationTable::getName(int index)
{
	return translationTable[index];
}