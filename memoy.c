#include "assmbler.h"
#include "Definitions.h"
#include "structures.h"

#define MASK_BITS 0xFFFFF
#define MASK_COMB 0x3
#define MASK_DEST_REG 0x1C
#define MASK_DEST_ADRS 0x60
#define MASK_SURS_REG 0x380
#define MASK_SURS_ADRS 0xC00
#define MASK_OPCODE 0xF000
#define MASK_TYPE 0x10000
#define MASK_DBI 0x20000

mem memory[MAX_MEM_SIZE];
int memCounter=0;
void reSetMemory(){
	int i=0;
	for( ; i<MAX_MEM_SIZE ; i++){
		memory[i].data = 0;
		memory[i].dataInfo=' ';
		memory[i].address=0;
	}
	memCounter=0;
}


int getType(int ind){
	int type= MASK_TYPE & memory[ind].data;
	type= (type >> 16);
	return type;
}

void setMemcounter(int num){
    memCounter=num;
}

int getComp(int ind){
	int comp = MASK_COMB & memory[ind].data;
	return comp;
}

int setNumber(int ind, long number){
	if(number < MAX_NUMBER_NEGATIVE || number > MAX_NUMBER_POSITIVE)
        return ERROR;
	else {
        memory[ind].data=number;
		return SUCCESS;
    }
}

void setComb(int ind, int number){
	memory[ind].data = memory[ind].data | number;
}

void setCombOne(int ind, int number){
	int shift= (number << 1);
	memory[ind].data=memory[ind].data | shift;
}


void setDestReg(int ind, int number){
	int shift= (number << 2);
	memory[ind].data=memory[ind].data | shift;
}

void setDestAdres(int ind, int number){
	int shift= (number << 5);
	memory[ind].data=memory[ind].data | shift;
}

void setSursReg(int ind, int number){
	int shift= (number << 7);
	memory[ind].data=memory[ind].data | shift;
}

void setSursAdrs(int ind, int number){
	int shift= (number << 10);
	memory[ind].data=memory[ind].data | shift;
}

void setOpCode(int ind, int number){
	int shift= (number << 12);
	memory[ind].data=memory[ind].data | shift;
}

void setType(int ind, int number){
	int shift= (number << 16);
	memory[ind].data=memory[ind].data | shift;
}

void setDBI(int ind, int number){
	long shift= (number << 17);
	memory[ind].data=memory[ind].data | shift;
}

void setDataInfo(int ind, char note){
	memory[ind].dataInfo=note;
}

void setReg (int ind, int number, int operand)
{
	if (operand == SOURCE)
		setSursReg(ind,number);
	else
		setDestReg(ind,number);
}

void setAddressing (int ind, int number, int operand)
{
	if (operand == SOURCE)
		setSursAdrs(ind,number);
	else
		setDestAdres(ind,number);
}

void memoryDump(){
    int ind =BASE_ADDRESS;

    while(ind != memCounter+BASE_ADDRESS){
        /*printf("\nthe code in memory is: %lu the indes is: %d the sign is: %c\n",memory[ind].data ,ind,memory[ind].dataInfo );*/
        printf("\n%d\n", memory[ind].data);
        /*printf("\n%d\n", ind);*/
        ind++;
    }
}
int printMemory(char *object){
    int i=BASE_ADDRESS;
    int len=memCounter+BASE_ADDRESS;
    FILE *obFile;
    if(flags_struct.error== TRUE)
        return ERROR;/*if thare is a problam(s) with asamller dont print*/

     	if (!(obFile = fopen(object, "w")))
	{
		fprintf(stdin, "External File Error: Unable to open file for writing - cannot create file %s\n", object);
		return ERROR;
	}
	fprintf (obFile, "\n\t%o\t%o",memCounter, getObjCounter());
	while (i < len)
	{
		fprintf (obFile, "\n%o\t%.7o\t%c", i, memory[i].data, memory[i].dataInfo);
		i++;
	}
	printObjectFile(obFile);/*print the data to the file.*/
	fclose (obFile);
	return SUCCESS;
}
