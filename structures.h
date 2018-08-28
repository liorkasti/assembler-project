#include "Definitions.h"


/*

void init(){
    label_size=0;
    dataType=0;
    lineNumber=0;
    start="\0";
    end="\0";
}

*/



typedef struct mem{
	unsigned long data:20;
	char dataInfo;/*a, r , e or nothing*/
	int address;
}mem;

/*to indecate wiech opretion apper*/
typedef struct OpName{
	char name[5];
	int numOfArg;
	int(*operands)(char*,int);
}OpName;

typedef enum {
	r0=0 , r1 , r2, r3, r4, r5,r6,r7
} registers;

typedef struct flags
{
	unsigned int symbol:1;
	unsigned int entry:1;
	unsigned int extren:1;
	unsigned int error:1;
}flags;

typedef struct Lable {
	char name[MAX_SYMBOL_LEN];
	int address;
	int type;
	int command;
	long commandSize;
	struct Lable *next;
}Lable;


typedef struct obj{
	unsigned long mechineCode;
	char lable[MAX_SYMBOL_LEN];
	int type;
	int isUsed;
	int address;
}obj;

extern registers reg;
extern obj ent_table[MAX_MEM_SIZE];
extern mem memory[MAX_MEM_SIZE];
extern obj ext_table[MAX_MEM_SIZE];
extern obj object_table[MAX_MEM_SIZE];
extern int IC, DC, L, passNumber;
extern flags flags_struct;
extern Lable *root;




