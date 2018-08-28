#include "Definitions.h"
#include "structures.h"
#include "assmbler.h"

obj object_table[MAX_MEM_SIZE];
int objectCounter=0;

int getObjCounter(){
    return objectCounter;
}

int addObject(char*name, int type, long mechineCode, int address){
	if(objectCounter == MAX_MEM_SIZE)
		return ERROR;
	if (mechineCode < 0)
		 mechineCode = convert_dec_to_binary_comp (mechineCode);
	strncpy(object_table[objectCounter].lable, name, MAX_SYMBOL_LEN);/*copy the name to object table*/
	object_table[objectCounter].mechineCode = mechineCode;
	object_table[objectCounter].type=type;
	object_table[objectCounter].address=address;
	objectCounter++;
	return SUCCESS;
}

int getAddressObject(char*sybolName){
	int ind=0;
	int type=0;

	while(ind != objectCounter){
		if(strcmp(sybolName, object_table[ind].lable)==0){
            type=getSymbolType(object_table[ind].lable);
                if(type != EXTERN && type != ENTRY)
                    return object_table[ind].address;
                else
                    return NOT_EXIST;
        }
		ind++;
	}

	return NOT_EXIST;
}

int getAddressObjectPlus(char*sybolName){
    int ind=0;
	while(ind != objectCounter){
		if(strcmp(sybolName, object_table[ind].lable)==0){
                    return object_table[ind].address;
        }
		ind++;
	}
	return NOT_EXIST;
}

int searchObject(char *sybolname){
	int ind=0;

	while(ind != DC){
		if(strcmp(sybolname, object_table[ind].lable)==0)
			return TRUE;
		ind++;
	}

	return FALSE;
}

unsigned long getMechineCode(char*name){
	int ind=0;

	while(ind !=DC){
		if( strcmp(name, object_table[ind].lable) == 0 && (object_table[ind].isUsed=FALSE) ) {
			return object_table[ind].mechineCode;
		}
		ind++;
	}

	return ERROR;
}

void resetObject(){
	int ind;
	char rest[MAX_SYMBOL_LEN]= {'\0'};

	for (ind = 0; ind < objectCounter; ind++)
	{
		object_table[ind].address=0;
		strncpy(object_table[ind].lable,rest,MAX_SYMBOL_LEN);
		object_table[ind].mechineCode=0;
		object_table[ind].type=0;
	}

	objectCounter=0;
}

void printObject(){
 int ind=0 , len =objectCounter;

 while(ind != len){
    printf( "\nobj name: %s adrs: %d  type: %d \n", object_table[ind].lable , object_table[ind].address, object_table[ind].type);
    ind++;
 }
}

void updateObjAddress(){
    int ind=0, adrs=IC+BASE_ADDRESS;
    while(ind != objectCounter){
        object_table[ind].address=adrs;
        ind++;
        adrs++;
    }
    updateSymbolTable();/*sync with the onject table*/
}

void printObjectFile(FILE *obFile){
    int i=0;

    while(i != objectCounter){
        fprintf (obFile, "\n%o\t%.7o\t", object_table[i].address, object_table[i].mechineCode);
		i++;
    }

}

