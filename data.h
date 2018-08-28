#define ERA_WIDTH 2
#define DADDR_WIDTH 2
#define SADDR_WIDTH 2
#define OPCODE_WIDTH 4
#define GROUP_WIDTH 2

#define ERA_OFFSET 0
#define DADDR_OFFSET 2
#define SADDR_OFFSET 4
#define OPCODE_OFFSET 6
#define GROUP_OFFSET 10

#define ERA_A	0x000
#define ERA_R	0x002
#define ERA_E	0x001



/* type for 12 bits data */
typedef struct dline {
    unsigned int data : 12;
} data_line;

/* type for an instruction line (using bit fields) */
typedef struct instrline {
    unsigned int era       : ERA_WIDTH;
    unsigned int dest_addr : DADDR_WIDTH;
    unsigned int src_addr  : SADDR_WIDTH;
    unsigned int opcode    : OPCODE_WIDTH;
    unsigned int group     : GROUP_WIDTH;
} instruction_line;

/* type for a whole line, with relevant fields helping the parsing */
typedef struct cline {
    char *line;
    instruction_line *instruction;
    unsigned char repeat;
    unsigned int line_number;
    char *src_opr;
    char *dest_opr;
    unsigned char src_reg;
    unsigned char dest_reg;
    char done;
} code_line;


/* this function returning data_line containing the char in the argument */
data_line char2data(char ch);
/* this function returning data_line containing the (boolean) instruction line in the argument */
data_line bline2data(instruction_line bl);
/* this function returning data_line containing the int in the argument with the sign bit for 12 bits*/
data_line num2data(int num);
/* this function clearing the values of an instruction line */
void init_instruction_line(instruction_line *il);
/* after allocating memory, the data need to be clear from garbage */
void init_data_line(data_line *dl);
