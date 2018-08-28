#include "assmbler.h"
#include "Definitions.h"
#include "structures.h"

int getLabel(char*line);
char copyPartStr(char *line ,char *symbol_name,int from,int to);
int countNumOfWords(char *line,int opCode);
int isLetter (char *ch);
int get_num_from_string (char *start, char *end);
int valid_immediate (char* ptr);
int valid_direct_register (char* ptr);
int valid_reg (char *line);
long valid_dynamic (char *ptr);
int valid_direct (char* ptr);
char* get_operand (char* line, int start, int end);
int get_addressing_method (char *line, int validMethods, int operand);
long convert_dec_to_binary_comp (long num);
int stringCheck(char *line, char *symbol_name);
int dynamicCheck(char *line);
int oneOperand(char *line, int opCode);
int twoOperand(char *line, int opCode);
int noOperand(char *line, int opCode);
int isLetter (char *ch);
char* get_operand (char* line, int start, int end);
int get_addressing_method (char *line, int validMethods, int operand);
long count_operands (char *line);

char string[]= "string", data[]= "data",ext[] ="extern", entry[]= "entry";
int IC=0, DC=0, L=0, label_size=0 , dataType=0, lineNumber=0;
long helpData=0;
int wordCount=0;
char *start, *end;
flags flags_struct;
Lable *root;
/* TODO (user#1#): add the test for the data and string about entry and extern*/

OpName opNames[14]=
{
	{"mov",2,&twoOperand},/*index of this array gives the code*/
	{"cmp",2,&twoOperand},
	{"add",2,&twoOperand},
	{"sub",2,&twoOperand},

	{"not",1,&oneOperand},
	{"clr",1,&oneOperand},
	{"lea",2,&twoOperand},
	{"inc",1,&oneOperand},
	{"dec",1,&oneOperand},
	{"jmp",1,&oneOperand},
	{"bne",1,&oneOperand},
	{"red",1,&oneOperand},
	{"prn",1,&oneOperand},
	{"jsr",1,&oneOperand},
};

OpName rtsNstop[2]={
	{"rts",0,&noOperand},
	{"stop",0,&noOperand}
};

void resetAs(){
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

void assemble(char* line){
	int opCode=0, ind=0, helpInd=0, extOrEnt=0, commandSize=0;
	char symbol_name[MAX_SYMBOL_LEN]="\0";
	flags_struct.symbol=FALSE;

	if(line == NULL || line[0]== '\n'|| line[ind]== '\r')
		return;/*if this is emtay line*/

	while(isspace(line[ind]) != 0){
		++ind;/*ignore space*/
	}
	if(line[ind] == ';' || line[ind]== '\n'|| line[ind]== '\r')
		return;/* or a comment ignore*/

	/*label check*/
	label_size=labelCheck(line+ind);/*strat to check from the place thare is no space*/

	if(label_size > 0){
		flags_struct.symbol=TRUE;
		*symbol_name = copyPartStr(line, symbol_name, ind, ind+label_size-1);
		ind=ind+label_size+1;
	}
	if(label_size == ERROR && passNumber==FIRST_PASS){
		printf("\nproblem occured when trying to read label in (%d)line number\n" ,lineNumber);
	}
	/*label check ends*/

	while(isspace(line[ind]) != 0){
		++ind;/*ignore space*/
	}

	if(line[ind] == '\n' && flags_struct.symbol==TRUE){
		printf("\nA label declaration founded without any comand or data (%d)liner number\n", lineNumber);
		return;
	}

	/*data string extren entry check*/
	if(line[ind] == '.'){
		ind++;
		dataType=varIdentifinder(line+ind);/*search for the data type (1-4)*/

		if(dataType == DATA){
			if(passNumber == SECOND_PASS)
				return;/*the data already saved and we know the address*/
			else{/*if it's the first pass*/
                if(flags_struct.symbol==FALSE)
                    printf("\nWarning: founded a data without a label can't reach data in (%d) line number\n", lineNumber);
                if(preDataCheck(line+ind+4,symbol_name) == TRUE){ /*skip 4 note and save the data if no error*/
                    if(addSymbol(symbol_name)== SYMBOL_EXISTS);/*mark the sybol as a data*/
                        changeAttribute(symbol_name,FALSE,getAddressObject(symbol_name));
                     DC++;
                    return;
				}
				else
					flags_struct.error=TRUE;
					return;
			}
		}
		else if(dataType == STRING){
			if(passNumber==SECOND_PASS)
				return; /*the data already saved*/
			else{/*if it's the first pass*/
                if(flags_struct.symbol==FALSE)
                    printf("\nWarning: founded a string without a label can't reach data in (%d) line number\n", lineNumber);
				if(stringCheck(line+ind+6,symbol_name)== TRUE){/*if the string data is valid*/
					if(addSymbol(symbol_name)== SYMBOL_EXISTS);/*mark the sybol as a data*/
                        changeAttribute(symbol_name,FALSE,getAddressObject(symbol_name));
					DC++;
					return;
				}
				else
					flags_struct.error=TRUE;/*error in deaclrtion already given*/
                    return;
                }
        }

		else if(dataType == ENTRY){
			ind=ind+5;
			while(line[ind]==' ' ||line[ind]=='\t')
                ind++;
			label_size=getLabel(line+ind);
			if(label_size == ERROR ){
				flags_struct.error=TRUE;
				return;
			}
            *symbol_name = copyPartStr(line, symbol_name, ind, ind+label_size-1);
			if(passNumber== FIRST_PASS){
                flags_struct.entry=TRUE;
                addSymbol(symbol_name);
				changeAttribute(symbol_name, TRUE, ENTRY);/*mark the sybol as a entry*/
				DC++;
				return;
			}
			else{/*if it's the second pass*/
				return;
			}
		}

		else if(dataType == EXTERN){
			ind=ind+6;
			while(line[ind]==' ' ||line[ind]=='\t')
                ind++;
			label_size=getLabel(line+ind);
                if(label_size == ERROR ){
                    flags_struct.error=TRUE;
                    return;
                }
			*symbol_name = copyPartStr(line, symbol_name, ind, ind+label_size-1);
            if(passNumber == FIRST_PASS){
				addSymbol(symbol_name);
				changeAttribute(symbol_name, TRUE, EXTERN);
				DC++;
				return;
            }
			else{/*if it's second pass*/
				if(searchObject(symbol_name)== TRUE) {/*if an extren variable decler inner the file*/
					flags_struct.error=TRUE;/*return an error*/
					printf("\nConflict declaration founded in the sybol %s in (%d) line number\n", symbol_name, lineNumber);
					return;
				}
			}
		}
	}
	if(varIdentifinder(line+ind) != FALSE){
		printf("\nMissing an '.' note before declaration of new data in (%d)line number\n",lineNumber);
		return;
	}
	/*data string extren entry check ends*/

	/*opcode check*/
	opCode=findOp(line+ind);/*find if thare is a */
	if(opCode == ERROR && passNumber== FIRST_PASS){
		printf("\nproblem occured when trying to read operation command in (%d)line number\n" ,lineNumber);
		flags_struct.error=TRUE;
		return;
	}
	if(passNumber== SECOND_PASS && opCode != ERROR ){
        if(flags_struct.symbol == TRUE){
            extOrEnt= getSymbolType(symbol_name);
            if(extOrEnt == EXTERN){
                flags_struct.error=TRUE;
                printf("\nthe label (%s) has been declared as a extern can't be at the begining of order in(%d) line number \n", symbol_name, lineNumber);
                return;
            }
            else if(extOrEnt == ENTRY){
                changeAttribute(symbol_name,FALSE,IC+BASE_ADDRESS);
            }
        }

		if(opCode < 14)
			opNames[opCode].operands(line+ind, opCode);/*run to the spesific function*/
		else{
            if(opCode ==14)
                rtsNstop[0].operands(line+ind, opCode);
            else
                rtsNstop[1].operands(line+ind, opCode);
        }
	}

	if(passNumber == FIRST_PASS){
		if((helpInd=validTypeCompDbl(line+ind, opCode)) != ERROR){
            setOpCode(IC+BASE_ADDRESS, opCode);
            setDataInfo(IC+BASE_ADDRESS,'a');
            if(flags_struct.symbol==TRUE){
                addSymbol(symbol_name);
                changeAttribute(symbol_name, FALSE, IC+BASE_ADDRESS);
                set_command(symbol_name);
            }
			IC++;/*one cell for the instrtion himself*/
			ind+=helpInd;/*decease by the number of note(s) to get right to the operands*/
			countNumOfWords(line+ind, opCode);
			commandSize = count_operands(line+ind) + 1;
			if (flags_struct.symbol==TRUE)
                set_command_size(symbol_name,commandSize);
			return;
		}
		else{
			flags_struct.error=TRUE;
			return;/*the error print already in the function*/
		}
	}

}

int countNumOfWords(char *line,int opCode){
	int ind = 0;
	if(opCode==14 || opCode== 15)
		return SUCCESS;/*if thare no operand neened return 1*/

	while(line[ind] == ' ' || line[ind] == '\t')
        ind++;/*ignore space*/


	if(line[ind] == '\n' || line[ind] == '\r')/*if needed operands but can't find any return error*/
		return ERROR;

	if(valid_reg(line+ind)== FALSE && dynamicCheck(line+ind) != TRUE )
		IC++;
	else if(dynamicCheck(line+ind)==TRUE)
		IC+=2;
	if((opCode >= 0 && opCode < 4 ) || opCode == 6){ /*if it's op with more than one operand*/
		while(line[ind] != ',' && (line[ind] != '\n' && line[ind] != '\r'))
			ind++;
		if(line[ind] =='\n' || line[ind] == '\r')
			return ERROR;
		ind++;/*if it's not the end of the line jump over the comma*/
		while(line[ind] == ' ' || line[ind] == '\t')
            ind++;
		if(valid_reg(line+ind)== FALSE && dynamicCheck(line+ind) != TRUE )
			IC++;
		else if(dynamicCheck(line+ind)==TRUE)
			IC+=2;
		return SUCCESS;
	}
	return SUCCESS;
}

int dynamicCheck(char *line){
    int ind=0;

    while(line[ind] != '{' && (line[ind] != ',' && line[ind] != '\n' && line[ind] != '\r'))
      ind++;
    if(line[ind] == '{')
        return TRUE;
    return FALSE;
}

char copyPartStr(char *line ,char *symbol_name,int from,int to){
	int ind=0;
	int length= (to-from)+1;
	if(length> MAX_SYMBOL_LEN)
		return '\0';
	while (from<= to )
	{
		symbol_name[ind]=line[from];
		ind++;
		from++;
	}

	return *symbol_name;
}

int labelCheck(char *line){
	int lim= colonCheck(line);
	int size=lim;
	if(lim == NOT_EXIST)
		return FALSE;
	lim--;
	while(lim != 0){
		if(validChars(line[lim]) != 1)
			return NOT_EXIST;
		lim--;
	}
	if(isalpha(line[0]) == 0)
		return NOT_EXIST;

	return size;
}

int getLabel(char*line){
	int ind=0, afterlbl=0;/*to check if the rest of the string is empty*/
	while (line[ind] == ' '  || line[ind] == '\t')
		ind++;
	if(! isalpha(line[ind]) ){
		if(passNumber == FIRST_PASS)
			printf("\nCan't create label with this sort of note in (%d)line number\n",lineNumber);
		return ERROR;
	}
	while (isalpha(line[ind]) || isdigit(line[ind]))/*valid label can be alphanurmic*/
		ind++;
	afterlbl=ind;
	while (line[afterlbl] == ' '  || line[afterlbl] == '\t')
		afterlbl++;
	if(line[ind] != '\n' && line[ind] != '\r')
		return ERROR;

	if(ind >= MAX_ARG_SIZE) {
		if(passNumber == FIRST_PASS)
			printf("\nLaybel found in (%d) line number too long\n", lineNumber);
		return ERROR;
	}
	return ind;

}

int validChars(char note){
	if(isdigit(note))
		return TRUE;
	else if(isalpha(note))
		return TRUE;
	else
		return FALSE;
}

int colonCheck(char *line){
	int ind=0;
	while(ind < strlen(line)){
		if(line[ind]== ':')
			return ind;
		ind++;
	}
	return NOT_EXIST;
}

/*cheack if the note is a number or part of number*/
int isNum(char note){
	if(isdigit(note))
		return TRUE;/*return 1 if it is*/
	else if(note == '-')
		return TRUE;
	else if(note == '.')
		return TRUE;
	else
		return FALSE;/*or 0 if not*/
}

/*cheack if given line has name of valid command
*/
int findOp(char *line){
	int ind=0;
	char *word=line;

	if(line== NULL ||strlen(line)==0)
		return NOT_EXIST;

	if(strlen(line) < 3)/*if string is too short exit*/
		return NOT_EXIST;

	while ( ind != NUMBER_OF_COMMANDS){
		if (isEqualStr(opNames[ind].name,line, 3)){
			word+=3;
			if(*word != '/')/*if thare is a number or note carry on*/
				break;
			else
				return ind;
		}
		else
			ind++;
	}

	if(isEqualStr(rtsNstop[0].name,line, 4)){
		if(isalpha(*word) || isNum(*word))/*if thare is a number or note carry on*/
			return NOT_EXIST;
		else
			return 14;
	}
	else if(isEqualStr(rtsNstop[1].name,line, 4))
		return 15;
	else
		return NOT_EXIST;
}

/*check if both string are equal in num numbers of char's
* from the beginning of the strings
*/
int isEqualStr(char *s1,char *s2, int num) {
	if(num==0)
		return TRUE;
	if(*s1 != *s2)
		return FALSE;
	return isEqualStr(s1+1,s2+1,num-1);
}


int preDataCheck(char *line, char *symbol_name){
	char token[8]={'\n'};
	int res=0;
	res =dataCheck(line,token,0,0,0,symbol_name, 0);
	return res;
}


/*check if "line" has valid variable declrtion
*/
int dataCheck(char *line,char *token ,int ind,int tokenInd, int closed, char *symbol_name,int count){
	if(line[ind]== '\n' || line[ind]== '\0' || line[ind]== '\r'){/*if this is the last char*/
		if(token[0] != '\n'){
			addObject(symbol_name, DATA,atol(token), BASE_ADDRESS+DC );
			count++;
			if(count > 1)
                addObject(symbol_name, DATA,0, BASE_ADDRESS+DC );
		}
		return TRUE;
	}
	else if(line[ind] == ','){/*if this is comma load the number to memory and carry on*/
		addObject(symbol_name, DATA,atol(token), BASE_ADDRESS+DC );
		L++;
		tokenInd=0;
		clrstr(token);
		dataCheck(line, token,ind+1,tokenInd,0,symbol_name,count+1);
	}

	else if(isdigit(line[ind]) || line[ind]== '+' || line[ind]== '-'){
		if(tokenInd >=9){
			printf("\ntoo long number in (%d)line number\n", lineNumber);
			return FALSE;
		}
		if(closed==1){
			printf("\nError ouccded when trying to read data in (%d)line number\n",lineNumber);
			return FALSE;
		}
		token[tokenInd]=line[ind];
		dataCheck(line, token,ind+1,tokenInd+1,closed,symbol_name,count);
	}

	else if(line[ind]==' ' || line[ind]=='\t'){
		closed =(isdigit(token[0]) !=0 || token[0] =='-' || token[0]=='+') ? 1:0;
		dataCheck(line, token,ind+1,tokenInd,closed,symbol_name,count);
	}
	else{
		printf("\nError ouccded when trying to read data in (%d)line number\n",lineNumber);
		return FALSE;
	}
	return SUCCESS;
}

void clrstr(char *token){
	int ind=0;

	while(ind < 8){
		token[ind++]='\n';
	}

}

int stringCheck(char *line, char *symbol_name){
	int ind=0, from=0, to=0, zero=0;

	while(line[ind] != '\"' && line[ind] != '\n' && line[ind] != '\r'){
		if (line[ind]!= ' ' && line[ind]!='\t'){
			printf("\nA note found before the string in (%d)line number\n", lineNumber);
			return FALSE;
		}
		ind++;
	}
	if(line[ind] !='\"'){
		printf("\nMissing '\"' note for a string in (%d)line number\n", lineNumber);
		return FALSE;
	}
	ind++;
	from=ind;/*sava the string indexes*/
	while(line[ind] != '\"' && (line[ind] != '\n' && line[ind] != '\r')){
		ind++;
	}
	to=ind;/*'from'= strat of the string, 'to'= the end of the string*/
	if(line[ind] !='\"'){
		printf("\nCan't read string to memory in (%d)line number 2\n",lineNumber);
		return FALSE;
	}
	ind++;
	/*if(line[ind] =='\n' || line[ind] != '\r'){
		return TRUE;
	}*/

	while((line[ind]==' ' || line[ind]=='\t') && (line[ind] != '\n' && line[ind] != '\r')){
		ind++;/*skip spaces*/
	}

	if(line[ind] == '\n' || line[ind] == '\0' || line[ind] == '\r') {
        zero= (to-from==1) ? 0: 1;
		while(from != to){
			addObject(symbol_name, STRING,(long)line[from], BASE_ADDRESS+DC );
			L++;
			from++;
		}
		if(zero==TRUE){
           addObject(symbol_name, STRING,0, BASE_ADDRESS+DC );
		}
		return TRUE;
	}
	else
		printf("\nProbalm has occurred when trying to finish read (%d)line number\n",lineNumber);
	return FALSE;
}

/*cover array of chars to long var number*/
long atol( const char *c ) {
	long value = 0;
	long sign = 1;
	if( *c == '+' || *c == '-' ) {
		if( *c == '-' ) sign = -1;
		c++;
	}
	while ( isdigit( *c ) ) {
		value *= 10;
		value += (int) (*c-'0');
		c++;
	}
	return value * sign;
}

int signs(char note){
	if(note== '+')
		return 1 ;
	else if(note == '-')
		return -1;
	else
		return 0;
}

int varIdentifinder (char *line){
	if(isEqualStr(line, data, 4))
		return DATA;
	else if (isEqualStr(line,string,5))
		return STRING;
	else if(isEqualStr(line,entry,5))
		return ENTRY;
	else if(isEqualStr(line,ext,6))
		return EXTERN;
	return FALSE;
}

/*This function serach for valid bits declaration
*the return will be -1 if problam ouccer or non zero int if fine
*the number reatun the index in the line after the bits ware cheaced
*/
int validTypeCompDbl(char* line, int opCode){
	int ind=0, type=0, dbl=0, comp=0, comp0=0, comp1=0;

	while(line[ind] != '/' && (line[ind] != '\n' && line[ind] != '\r' )){
		ind++;
	}
	ind+=1;/*these two loop made to make sure that only one \ was enterd*/
	while ((line[ind] == ' ' || line[ind] == '\t') && (line[ind] != '\n' && line[ind] != '\r' ))
		ind++;

	if(line[ind] != '0' && (!isdigit(line[ind]))){/*need to be number at this point*/
		printf("\nProblam ouccerd when tryng read operation command in (%d) line number\n",lineNumber);
		return ERROR;
	}


	type=(line[ind++] - '0');/*save the type and jump to the next char after */
	if(type > 2){
		printf("\nCan't read type in (%d)line number\n", lineNumber);
		return ERROR;
	}
	setType(IC+BASE_ADDRESS, type);
	while ((line[ind] == ' ' || line[ind] == '\t') && (line[ind] != '\n' && line[ind] != '\r' ))
		ind++;

	if(line[ind]== '/'){/*means that comp bits need to be edit*/
		ind++;
		while ((line[ind] == ' ' || line[ind] == '\t') && (line[ind] != '\n' && line[ind] != '\r' ))
			ind++;

		if((line[ind] != '\n' && line[ind] != '\r' && line[ind] != '0') && (!isdigit(line[ind]))){
			printf("\nCan't read comp bits in (%d)line number\n", lineNumber);
			return ERROR;
		}

		comp0=(line[ind]-'0');/*save the first bit*/

		ind++;
		while ((line[ind] == ' ' || line[ind] == '\t') && (line[ind] != '\n' && line[ind] != '\r' ))
			ind++;


		if(line[ind]== '/'){/*start the serach for the next bit*/
			ind++;
			while ((line[ind] == ' ' || line[ind] == '\t') && (line[ind] != '\n' && line[ind] != '\r' ))
				ind++;

			if(line[ind] != '0' && (!isdigit(line[ind]))){
				printf("\nCan't read comp bits in (%d)line number\n", lineNumber);
				return ERROR;
			}

			comp1=(line[ind]-'0');
			if(comp <	4){/*cheack if is valid comp bits*/
				setComb(IC+BASE_ADDRESS, comp1);
				setCombOne(IC+BASE_ADDRESS, comp0);
            }
			else{
				printf("\nNot valid comp number(s) in (%d) line number\n", lineNumber);
				return ERROR;
			}

			ind++;
			while ((line[ind] == ' ' || line[ind] == '\t') && (line[ind] != '\n' && line[ind] != '\r' ))
				ind++;

			if(line[ind] == ','){/*after the comp cheack need to be dbl bit*/
				ind++;
				while ((line[ind] == ' ' || line[ind] == '\t') && (line[ind] != '\n' && line[ind] != '\r' ))
					ind++;
				if(line[ind] != '0' && (!isdigit(line[ind]))){
					printf("\nCan't read dbl bit in(%d) line number\n", lineNumber);
					return ERROR;
				}
				dbl=(line[ind]-'0');
				if(dbl >1){
					printf("\nUnsupported dbl type in (%d) line number\n", lineNumber);
					return ERROR;
				}
				setDBI(IC+BASE_ADDRESS,dbl);
				ind++;
			}
			return ind;/*return the ind for the next cheack*/
		}
		else
		{
			printf("\nCan't read comp bits in (%d) line number\n", lineNumber);
			return ERROR;
		}

	}
	else if(line[ind]== ','){/*notice that' if type bit is on this cheack cannot be possible*/
		if(type != 1){
			ind++;
			while((line[ind] == ' ' || line[ind] == '\t') && (line[ind] != '\n' && line[ind] != '\r' ))
				ind++;
			if(line[ind] != '0' && (!isdigit(line[ind]))){
				printf("\nCan't read dbl bit in(%d) line number\n", lineNumber);
				return ERROR;
			}
			dbl=(line[ind]-'0');
			if(dbl >2){
				printf("\nUnsupported bdl type in(%d) line number\n", lineNumber);
				return ERROR;
			}
			setDBI(IC+BASE_ADDRESS,dbl);
			ind++;
			return ind;/*return the index from here is fine horeyy!*/
		}
		else{
			printf("\nType bit declared '1' cannot found comp bits in(%d) line number\n", lineNumber);
			return ERROR;
		}
	}
	else{
		printf("\nCan't reach dbl bit in(%d) line number\n", lineNumber);
		return ERROR;
	}
}

/*those three functions provide a way to distinguish what the type(s)
* of addresing method(s) is/are allowed in specific opcode.
* this function calls the function "get_addressing_method"
* when the mode is 1; means that we need to check only for the direct addressing method
* when the mode is 3; means that we need to check for immediate , direct or dynamic addressing method
* when the mode is 0; means that we need to check for any method available.
* when the mode is -1; means that we need to check thare is no operands at all.
*
* return: 1 if the addressing method is fine or -1 if an error occurred
*/
int oneOperand(char *line, int opCode){
	int destOrSurs = DEST, res=0, skipInd=0;
	wordCount = 1;

	while((line[skipInd] != '\t' && line[skipInd] != ' ')  && (line[skipInd] != '\n' ||  line[skipInd] != '\r' )){
        skipInd++;/*skip to the operands's part in the line*/
	}
	skipInd = (line[skipInd] == '\n' || line[skipInd] == '\r') ? 0:skipInd;

	if(opCode == 12)/*if the op is prm*/
		res=get_addressing_method(line+skipInd,EVERY_METHOD,destOrSurs);/*check for any thing include zero*/
	else if(opCode == 13)/*if the op is jsr*/
		res=get_addressing_method(line+skipInd,ONLY_DIRECT,destOrSurs);/*check only for 1 addresing methoed*/
	else
		res=get_addressing_method(line+skipInd,DIRECT_INDEX_REG,destOrSurs);/*check 1,2,3*/

	flags_struct.error = (res==ERROR) ? 1:0; /*error flag goes 1 if error ouccded*/

	if (res == DYNAMIC_ADDRESSING || res == DYNAMIC_ADDRESSING_WITH_ERROR)
		wordCount+=2;
	else if (res != DIRECT_REG_ADDRESSING)
		wordCount++;
    IC+=wordCount;
	return res;
}

int twoOperand(char *line, int opCode){
	int destOrSurs =SOURCE, res=0, skipInd=0;
    wordCount = 1;
    while((line[skipInd] != '\t' && line[skipInd] != ' ')  && (line[skipInd] != '\n' ||  line[skipInd] != '\r' )){
        skipInd++;/*skip to the operands part in the line*/
	}
	skipInd = (line[skipInd] == '\n' || line[skipInd] == '\r') ? 0:skipInd;

	if(opCode == 6)/*if it's lea op*/
		res=get_addressing_method(line+skipInd,DIRECT_INDEX_REG,destOrSurs);/*check 1,2,3*/
	else/*if it is cmp op*/
		res=get_addressing_method(line+skipInd,EVERY_METHOD,destOrSurs);/*check for any thing include zero*/
	if(res==ERROR){
		flags_struct.error=1;
		return res;
	}
	if (res == DYNAMIC_ADDRESSING || res == DYNAMIC_ADDRESSING_WITH_ERROR)
	{
		wordCount+=2;
    }
	else if (res != DIRECT_REG_ADDRESSING)
	{
		wordCount++;
    }

	destOrSurs=DEST;/*mark we on the second operand*/

	while(line[skipInd] != ',' && line[skipInd] != '\n') {
		skipInd++;/*go to the next operand*/
	}

	if(line[skipInd] == '\n' || line[skipInd] == '\r' ){/*if no operand in line*/
		printf("missing sours operand in:(%d) line number", lineNumber);
		return ERROR;
	}
	skipInd++;/*jump over the ',' note*/

	if(opCode == 1)/*if it's  cmp op*/
		res=get_addressing_method(line+skipInd,EVERY_METHOD,destOrSurs);/*check for any thing include zero*/
	else/*if it's not*/
		res=get_addressing_method(line+skipInd,DIRECT_INDEX_REG,destOrSurs);/*check 1,2,3*/
    IC++;
    if (res == DYNAMIC_ADDRESSING)
        wordCount++;
    else if (res == DIRECT_REG_ADDRESSING)
        wordCount--;
    IC+= wordCount;

	flags_struct.error = (res==ERROR) ? 1:0; /*error flag goes 1 if error ouccded*/

	return res;
}

int noOperand(char *line, int opCode){
	int skipInd=0;
	while((line[skipInd] != '\t' && line[skipInd] != ' ')  && (line[skipInd] != '\n' ||  line[skipInd] != '\r' )){
        skipInd++;/*skip to the operands part in the line*/
	}
	skipInd = (line[skipInd] == '\n' || line[skipInd] == '\r') ? 0:skipInd;
	return get_addressing_method(line+skipInd,-1,-1);/*check no operands at all*/
}

int isLetter (char *ch)
{
	if (*ch >= 'a' || *ch <= 'z' || *ch >= 'A' || *ch <= 'Z')
		return TRUE;
	else
		return FALSE;
}

int get_num_from_string (char *start, char *end)
{
	int num=0;
	char stringNum[MAX_ARG_SIZE];
	char* digit = &stringNum[0];

	while (start != end)
	{
		*digit = *start;
		digit++;
		start++;
	}
	*(++digit) = '\0'; /* End of the string */

	num = atoi(stringNum);
	return num;
}

/* helpData is the number */
int valid_immediate (char* ptr)
{
	char *start, *end;
	int ind=0;

	if (ptr[ind] != '#')
		return FALSE;
	ind++;
	if (ptr[ind] == '-' || ptr[ind] == '+')
	{
		start = ptr+ind;
		ind++;
	}
	else
		start = ptr+ind;
	while (ptr[ind] != '\0')
	{
		if (! isdigit(ptr[ind]))
		{
			if(passNumber == SECOND_PASS)
				printf ("Line %d: Illegal number as operand.", lineNumber);
			return ERROR;
		}
		ind++;
	}
	end = ptr+ind;
	helpData = get_num_from_string(start,end);
	return TRUE;
}

/* helpData is the register number in case it is a valid register */
int valid_direct_register (char* ptr)
{
	if (valid_reg(ptr))
	{
		helpData = atol(ptr+1);
		return TRUE;
	}
	else
		return FALSE;
}

int valid_reg (char *line)
{
	if (line[0] == 'r' && line[1] >= '0' && line[1] <= '7')
		return TRUE;
	else
		return FALSE;
}

/* If the addressing is valid, the returned value is the command size of the index lable */
/* helpData is the address of the symbol (first one) */
long valid_dynamic (char *ptr)
{
	char firstLabel[MAX_SYMBOL_LEN]="\0", secondLabel[MAX_SYMBOL_LEN]="\0";
	int i = 0;
	long command=0;
	int ind = 0;
	while (ptr[ind] != '\0' && ptr[ind] != '{')
	{
		firstLabel[i] = ptr[ind];
		ind++;
		i++;
	}
	i = 0;
	if (ptr[ind] == '\0')
		return FALSE;
	if (ptr[ind] == '{' && ptr[ind+1] == '!')
	{
		ind+=2;
		while (ptr[ind] != '}')
		{
			secondLabel[i] = ptr[ind];
			ind++;
			i++;
		}
	}
	else
	{
		if(passNumber == SECOND_PASS)
			printf ("Line %d: Missing the symbol '!'.", lineNumber);
		return DYNAMIC_ADDRESSING_WITH_ERROR;
	}
	if (! lable_exists(firstLabel))
	{
		if(passNumber == SECOND_PASS)
			printf ("Line %d: The label '%s' does not exist", lineNumber, firstLabel);
		return ERROR;
	}
	else
		helpData = getAddressOfSymbol(firstLabel);
	if (! lable_exists(secondLabel)) /* need to add a check if the label is a command label */
	{
		if(passNumber == SECOND_PASS)
			printf ("Line %d: The label '%s' does not exist", lineNumber, secondLabel);
		return ERROR;
	}
	else
		if (! is_command(secondLabel))
		{
			printf ("Line %d: The index lable does not contain a command", lineNumber);
			return DYNAMIC_ADDRESSING_WITH_ERROR;
		}
    	command = get_command_size(secondLabel);
	return command;
}

/* helpData is the address of the symbol */
int valid_direct (char* ptr)
{
    int symbolType=0;
    if (! lable_exists(ptr))
    {
        if(passNumber == FIRST_PASS)
            {
                printf ("\nLine %d: Could not find the label (%s) in the symbol table.\n", lineNumber, ptr);
                return ERROR;
            }
            return FALSE;
    }
	else
	{
		helpData = getAddressOfSymbol(ptr);
		symbolType = getSymbolType(ptr);
		if (! symbolType)
            return TRUE;
		return symbolType;
	}
}

/* This function returns a string array ending with a null terminator */
char* get_operand (char* line, int start, int end)
{
	char operand[MAX_SYMBOL_LEN]="\0";
	char *op = operand;
	int i=0;

	for (i = start; i < end; i++)
		operand[i-start] = line[i];
	operand[++i] = '\0';
	return op;
}

int get_addressing_method (char *line, int validMethods, int operand)
{
	int start=0, end=0, ind=0, res=-1, lableType=0, commandSize=0, memRes=0;
	char op[MAX_SYMBOL_LEN]="\0";

	if(operand == -1 )
        return SUCCESS;

	if (operand == SOURCE)
	{
        while (isspace(line[ind]))
            ind++;
        start = ind;
        while ((!isspace(line[ind]) && line[ind] != ','))
            ind++;
        end = ind;
	}
	else if (operand == DEST)
	{
        while (isspace(line[ind]))
            ind++;
        start = ind;
        while ((!isspace(line[ind]) && line[ind] != '\n'))
            ind++;
        end = ind;
	}

	strcpy(op,get_operand(line, start, end));
	ind = 0;

	/* Check the operand according to the available methods */
	/* If a valid addressing method is found, the function will write the apropriate data to the memory */
	if (validMethods == EVERY_METHOD)
	{
        if (valid_immediate(op))
            res = IMMEDIATE_ADDRESSING;
        else if ((lableType = valid_direct(op)) != 0)
            res = DIRECT_ADDRESSING;
        else if (commandSize=valid_dynamic(op))
            res = DYNAMIC_ADDRESSING;
        else if (valid_direct_register(op))
            res = DIRECT_REG_ADDRESSING;
	}
	else if (validMethods == DIRECT_INDEX_REG)
	{
        if ((lableType = valid_direct(op)) != 0)
            res = DIRECT_ADDRESSING;
        else if (commandSize=valid_dynamic(op))
            res = DYNAMIC_ADDRESSING;
        else if (valid_direct_register(op))
            res = DIRECT_REG_ADDRESSING;
	}
	else if (validMethods == ONLY_DIRECT)
	{
        if ((lableType = valid_direct(op)) != 0)
            res = DIRECT_ADDRESSING;
	}
	if ((count_operands (line)) != 0 && operand == -1)
	{
		printf ("\nLine %d: Found an operand while the command should not have any\n", lineNumber);
		return ERROR;
	}
	if (res < 0)
	{
		printf ("\nLine %d: Could not detemine the addressing method.\n", lineNumber);
		return ERROR;
	}
	if (res == DYNAMIC_ADDRESSING) /* saving only the first lable */
	{
		while (op[ind] != '{')
			ind++;
		while (op[ind] != '}')
		{
			op[ind] = '\0';
			ind++;
        }
        op[ind] = '\0';
        lableType = getSymbolType(op);
	}
	write_addressing_to_mem (res, op, operand, helpData, lableType, commandSize);

	return res;
}

void write_addressing_to_mem (int addressingMethod, char *operand, int operandType, long data, int lableType, long commandSize)
{
	int res = 0;
	long number = data;


	if (addressingMethod == IMMEDIATE_ADDRESSING)
	{
        if (number < 0)
            number = convert_dec_to_binary_comp(data);
		res = setNumber(IC + BASE_ADDRESS + 1,number);
		setDataInfo(IC + BASE_ADDRESS + wordCount, 'a');
	}
	else if (addressingMethod == DIRECT_ADDRESSING)
	{
		if (lableType == EXTERN)
		{
			addExtern(operand, IC + BASE_ADDRESS+ wordCount);
			setDataInfo(IC + BASE_ADDRESS+ wordCount, 'e');
        }
        else
            setDataInfo(IC + BASE_ADDRESS+ wordCount, 'r');
		res = setNumber(IC + BASE_ADDRESS + wordCount,number);
	}
	else if (addressingMethod == DYNAMIC_ADDRESSING)
	{
		if (lableType == EXTERN)
		{
			addExtern(operand, IC + BASE_ADDRESS+ wordCount);
			setDataInfo(IC + BASE_ADDRESS+ wordCount, 'e');
        }
        else
            setDataInfo(IC + BASE_ADDRESS+ wordCount, 'r');
		res = setNumber(IC + BASE_ADDRESS + wordCount, data);
		if (res == ERROR)
			return ERROR;
		res = setNumber(IC + BASE_ADDRESS+ wordCount+1, commandSize);
		setDataInfo(IC + BASE_ADDRESS + wordCount + 1, 'a');
	}
	else if (addressingMethod == DIRECT_REG_ADDRESSING)
	{
		setReg (IC + BASE_ADDRESS, data, operandType);
	}
	setAddressing(IC + BASE_ADDRESS, addressingMethod, operandType);
}

long count_operands (char *line)
{
	int ind=0;
	long count = 0, dynamic=0;

	while (line[ind] != '\n')
	{
		if (line[ind] == ',')
		{
			if (!count)
				count+=2;
			else
				count++;
		}
		else if (line[ind] == '{')
			dynamic++;
		else if (line[ind] == 'r' && (line[ind+1] >= 0 || line[ind+1] <= 9))
			ind++;
		ind++;
	}
	return (count+dynamic);
}


long convert_dec_to_binary_comp (long num)
{
	int remain=0, i, base, sign, firstDigit=0;
	long res = 0;
	char converted[MAX_WORD];
	i = MAX_WORD - 1;
	base = 2;

	if (num >= 0)
		sign = 0;
	else
		sign = 1;

	while ((remain > 0 || num > 0) && i >= 0) /*convert to 2's complement*/
	{
		remain = num % base;
		num = num / base;
		if (sign)
		{
			if (remain == 1)
				sign = 0;
			converted[i] = '1';
		}
		else
		{
			if (remain == 1)
				converted[i] = '0';
			else
				converted[i] = '1';
		}
		i--;
	}
	firstDigit = i+1;
	if (i < 0)
		return ERROR;
	else
	{
		while (i >= 0)
		{
			converted[i] = converted[firstDigit];
			i--;
		}
	}
	res = atol (converted+i+1);
	return res;
}

