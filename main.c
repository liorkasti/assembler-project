#include "assmbler.h"
#include "Definitions.h"
#include "structures.h"
#define FILE_EXTENSION ".as"

int fileToline(FILE* file, char *name);
int fileNameCheck(char* s,int ind);
void printEntNext(char *name);
void printObFile(char *name);
void loadAndRun(char *fileName,  char *name);
void reset();
int passNumber=1;
flags flags_struct;

void main(int args, char *argv[]){

	int count=0;
	int error=0;
	FILE* file;
	if(args== 1){
		printf("\nfile Name(s) is needed\n");
		exit(1);
	}
	while(++count != args ){/*read args number of files entered*/
		reSetMemory();
		resetAs();
		if((error=fileNameCheck(argv[count], count))==1 ){
			file= fopen(argv[count],"r");/*read only*/
			if(file==NULL)
				printf("\an erorr apper when trying read file %s\n", argv[count]);
			else

				fileToline(file, argv[count]);/*start read line after line*/
			printf("\n**Assembling file number %d has finished successfully\n",count);
		}
		if(error== 0){
			printf("\nnot a file name\n");
			/*exit(1);*/
		}
	}

	printf("\nprogram fin\n");

}

void loadAndRun(char *fileName,  char *name){
    FILE* file;
    file = fopen(fileName, "r");
    if(file != NULL){
        fileToline(file, name);
        if(flags_struct.error==FALSE)
            printf("\nfile %s finished successfully.", fileName);
        else
            printf("\nfile %s finished with error can't create '.ob' file.", fileName);
        return;
    }
    printf("\nError: Can't open or find file named %s \n", fileName);
    return;
}

void reset(){
   reSetMemory();
   resetExtren();
   resetObject();
   IC=0;
   DC=0;
   L=0;
   lineNumber=0;
   flags_struct.entry=0;
   flags_struct.error=0;
   flags_struct.symbol=0;

}



int fileNameCheck(char* s,int ind){
	if(ind== 1){
		s+=3;
		if(isalpha(*s) || isdigit(*s))
			return 1;
	}
	if(s[0]==',')
		return 2;
	else if(isalpha(*s)|| isdigit(*s))
		return 1;
	else
		return 0;
}


int fileToline(FILE* file,char *name){
	char line[MAX_LINE];
	char* res = " ";
	DC=0, IC=0 , L=0;
	lineNumber=0;
	createSymbolTable();
	passNumber=1;
	while(passNumber != 3){/*read the file twice for the two pass*/
		res = fgets(line, MAX_LINE, file);
		while (res != NULL){
			assemble(line);
			lineNumber++;
			res =fgets(line, MAX_LINE, file);
		}
		rewind(file);
		lineNumber=0;
        if(passNumber==FIRST_PASS){
            setMemcounter(IC);
            updateObjAddress();
        }
		passNumber++;/*go to the second pass*/
		DC=IC;
        IC=0;
	}
	fclose(file);
	printEntNext(name);
	printObFile(name);
	freeTable();
	return 1;
}

void printEntNext(char *name){
    int len =strlen(name);
    char ext[len+4];
    char ent[len+4];

    if(flags_struct.error== TRUE)
        return;/*if thare is a problam(s) with asamller dont print*/

    copyPartStr(name , ext,0,len);
    strcat(ext, ".ext");

    copyPartStr(name , ent,0,len);
    strcat(ent, ".ent");

    if(flags_struct.entry==TRUE){
        printEntryToFile(ent);
    }
    if(getExtCounter()!= 0){
        printExternToFile(ext);
    }
}

void printObFile(char *name){
    int len =strlen(name);
    char object[len+3];
    copyPartStr(name ,object,0,len);
    strcat(object, ".ob");
    printMemory(object);
}



