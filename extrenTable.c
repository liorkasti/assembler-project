#include "Definitions.h"
#include "structures.h"
#include "assmbler.h"

obj ext_table[MAX_MEM_SIZE];
int externCounter=0;
/*
	Add a lable to the extern table. The lables are inserted on an ascending order.
*/
int addExtern (char *name, int address)
{
	if (externCounter >= MAX_MEM_SIZE) /* Table is full, shouldn't happen */
	{
		printf ("could not add %s to the external table", name);
		return FALSE;
	}
	strcpy(ext_table[externCounter].lable , name);
	ext_table[externCounter].address=address;
	ext_table[externCounter].isUsed=FALSE;
	externCounter++;
	return TRUE;
}

void resetExtren(){
    int ind;
	char rest[MAX_SYMBOL_LEN]= {'\0'};

	for (ind = 0; ind < externCounter; ind++)
	{
		ext_table[ind].address=0;
		strncpy(object_table[ind].lable,rest,MAX_SYMBOL_LEN);
		ext_table[ind].mechineCode=0;
		ext_table[ind].type=0;
	}
	externCounter=0;
}


int changeExternAddress (char *name, int newData)
{
	int loc=0;
	while (loc != externCounter)
	{
		if(strcmp(ext_table[loc].lable,name) && ext_table[loc].isUsed==FALSE)
			break;
		loc++;
	}
	if (loc == externCounter)
		return NOT_EXIST;
	ext_table[loc].address = newData;
	return SUCCESS;
}

void printExtern(){
    int len= externCounter, ind=0;
    while(ind != len){
        printf("\nExtern label is: %s address is %d\n",ext_table[ind].lable, ext_table[ind].address);
        ind++;
    }
}

int printExternToFile (char *externFileName)
{
	int i = 0;
	FILE *ext;

	if (!(ext = fopen(externFileName, "w")))
	{
		fprintf(stdin, "External File Error: Unable to open file for writing - cannot create file %s\n", externFileName);
		return ERROR;
	}
	while (i < externCounter)
	{
		fprintf (ext, "%s\t%o\n", ext_table[i].lable, ext_table[i].address);
		i++;
	}
	fclose (ext);
	return SUCCESS;
}

int getExtCounter(){
    return externCounter;
}
