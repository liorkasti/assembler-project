asebl: main.o assembler.o extrenTable.o memoy.o objectTable.o symbolTable.o
	gcc -Wall -ansi  -g  -o assembler main.o assembler.o extrenTable.o memoy.o objectTable.o symbolTable.o

main.o: main.c assmbler.h Definitions.h structures.h
	gcc -Wall -ansi  -g  -c main.c 

assembler.o: assembler.c assmbler.h Definitions.h structures.h
	gcc -Wall -ansi  -g -c assembler.c 

extrenTable.o: extrenTable.c assmbler.h Definitions.h structures.h
	gcc -Wall -ansi  -g -c extrenTable.c 

memoy.o: memoy.c assmbler.h Definitions.h structures.h
	gcc -Wall -ansi  -g -c memoy.c 

objectTable.o: objectTable.c assmbler.h Definitions.h structures.h
	gcc -Wall -ansi  -g -c objectTable.c 

symbolTable.o: objectTable.c assmbler.h Definitions.h structures.h
	gcc -Wall -ansi  -g -c symbolTable.c 


