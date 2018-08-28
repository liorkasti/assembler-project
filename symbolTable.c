#include "Definitions.h"
#include "structures.h"
#include "assmbler.h"

Lable *root;
int symbolCounter=0;

void createSymbolTable ()
{
	symbolCounter = 0;	/* Initializing the symbol counter */
	root = (struct Lable *) malloc (sizeof(struct Lable));
	root -> next = NULL;
}

void updateSymbolTable(){
    struct Lable *curr = root;
    int andrs=0;
	while (curr){
        andrs=getAddressObjectPlus(curr->name);
        if(andrs != NOT_EXIST)
            curr->address=andrs;
        curr=curr->next;
	}
}


int addSymbol (char *name)
{
	struct Lable *curr = root;
	Lable *temp = (Lable *) malloc (sizeof(Lable));
	int count=1;
	if (!temp)
	{
		printf ("Allocation problem");
		return ERROR;
	}
	if (symbolCounter == 0) {
		strcpy((temp->name), name);
		root = temp;
		symbolCounter++;
		return SUCCESS;
	}
	else {
 		while (count < symbolCounter/*curr -> next*/) {
			if (strcmp (curr -> name, name)==0)
					return SYMBOL_EXISTS;
			curr = curr -> next;
			count++;
		}
	}
	strcpy((temp->name), name);
	curr-> next= temp;
	temp -> next = NULL;
	symbolCounter++;
	return SUCCESS;
}

void printSymbolTable (struct Lable *node)
{
	while (node) {
		printf ("name: %s, address: %d, type: %d\n", node -> name, node -> address, node -> type);
		node = node -> next;
	}
}

/*
	Changes the address / type of a given lable.
	Use the defined TRUE / FALSE as the parameter for isType.
*/
int changeAttribute (char *name, int isType, int newData)
{
	struct Lable *curr = root;
	while (curr != 0) {
		if (! strcmp(name, curr -> name))
		{
			if (isType)
				curr -> type = newData;
			else
				curr -> address = newData;
			return SUCCESS;
		}
		curr = curr -> next;
	}
	return NO_SUCH_SYMBOL;
}

int getAddressOfSymbol (char *name)
{
	struct Lable *curr = root;
	while (curr)
	{
		if (! strcmp(name, curr -> name))
			return (curr -> address);
		curr = curr -> next;
	}
	return NO_SUCH_SYMBOL;
}

void freeTable ()
{
	struct Lable *temp;

	while (root)
	{
		temp = root;
		root = root -> next;
		free (temp);
	}

	symbolCounter=0;
}

int isNotExtern(char *name){
 struct Lable *curr = root;
    while(curr){
        if((strcmp(name,curr->name)==0)  && curr->type != EXTERN)
            return TRUE;
        curr=curr->next;
    }
    return FALSE;
}

int lable_exists (char* name)
{
	struct Lable *curr = root;
	while (curr)
	{
		if (strcmp(name, curr -> name) == 0)
			return TRUE;

        curr=curr->next;
	}
	return FALSE;
}

int printEntryToFile (char *entryFileName)
{
	FILE *ent;
	struct Lable *curr = root;
	if (!(ent = fopen (entryFileName, "w")))
	{
		fprintf (stdin, "EntryFile Error: Unable to open for writing, cannot create file %s\n", entryFileName);
		return ERROR;
	}
	while (curr)
	{
        if(curr->type==ENTRY)
            fprintf(ent, "%s\t%o\n", curr->name, curr->address);
	    curr=curr->next;
	}
	fclose (ent);
	return SUCCESS;
}

int getSymbolType(char* name){
    struct Lable *curr = root;
	while (curr)
	{
		if (strcmp(name, curr -> name) == 0)
			return curr ->type;

        curr=curr->next;
	}
	return NOT_EXIST;

}

int is_command(char *name)
{
	struct Lable *curr = root;
	int i;
	while (curr)
	{
		if (strcmp(name, curr -> name) == 0)
			return (curr -> command);
        curr = curr -> next;
	}
	return FALSE;
}

int set_command (char *name)
{
	struct Lable *curr = root;
	while (curr)
	{
		if (! strcmp(name, curr -> name))
		{
			curr -> command = TRUE;
			return SUCCESS;
		}
		curr = curr -> next;
	}
	return NO_SUCH_SYMBOL;
}



int set_command_size (char *name, int commandSize)
{
	struct Lable *curr = root;
	while (curr)
	{
		if (! strcmp(name, curr -> name))
		{
			curr -> commandSize = commandSize;
			return SUCCESS;
		}
		curr = curr -> next;
	}

	return NO_SUCH_SYMBOL;
}

int get_command_size (char *name)
{
	struct Lable *curr = root;
	while (curr)
	{
		if (! strcmp(name, curr -> name))
			return (curr -> commandSize);
		curr = curr -> next;
	}
	return NO_SUCH_SYMBOL;
}
