#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

extern int lineNumber;

/*memory.c*/
void reSetMemory();
void setComb(int ind, int number);
void setDestReg(int ind, int number);
void setDestAdres(int ind, int number);
void setSursReg(int ind, int number);
void setSursAdrs(int ind, int number);
void setOpCode(int ind, int number);
void setType(int ind, int number);
void setDBI(int ind, int number);
void setDataInfo(int address,char note);
void memoryDump();
void setCombOne(int ind, int number);
void setMemcounter(int num);
void setReg (int ind, int number, int operand);
void setAddressing (int ind, int number, int operand);
int printMemory(char *object);

/*assmbler.c*/
void assemble(char* line);
int setNumber(int ind, long number);
void resetAs();
int isNum(char note);
int findOp(char *line);
char copyPartStr(char *line ,char *symbol_name,int from,int to);
int labelCheck(char *line);
int validChars(char note);
int varIdentifinder (char *line);
int signs(char note);
int colonCheck(char *line);
int isEqualStr(char *s1,char *s2, int num);
int addToTable(int pos,int type,char *label ,long value);
long atol( const char *c );
int preDataCheck(char *line, char *symbol_name);
int dataCheck(char *line,char *token ,int ind,int tokenInd, int closed, char *symbol_name,int count);
void clrstr(char *token);
int validTypeCompDbl(char* line, int opCode);
long convert_dec_to_binary_comp (long num);

void init();


/*sybol table.c*/
int addSymbol (char *name);
void createSymbolTable ();
void printSymbolTable (struct Lable *node);
int changeAttribute (char *name, int isType, int newData);
void freeTable ();
int lable_exists (char* name);
int getAddressOfSymbol (char *name);
int isNotExtern(char *name);
int getSymbolType(char* name);
int set_command_size (char *name, int commandSize);
int get_command_size (char *name);
void updateSymbolTable();
int printEntryToFile (char *entryFileName);

/*ext table*/
int addExtern (char *name, int address);
int changeExternAddress (char *name, int newData);
void printExtern();
int printExternToFile (char *externFileName);
int getExtCounter();
void resetExtren();

/*object table*/
int searchObject(char *sybolname);
int getAddressObject(char*sybolName);
int addObject(char*name, int type, long mechineCode, int address);
unsigned long getMechineCode(char*name);
void resetObject();
void printObject();
void updateObjAddress();
int getObjCounter();
void printObjectFile(FILE *obFile);




