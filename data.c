#include <string.h>
#include "data.h"

/* auxilary array of maskes requested by width */
unsigned char mask_by_width[] = {0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF};

/* after allocating memory, the data need to be clear from garbage */
void init_data_line(data_line *dl)
{
    dl->data = 0;
}

/* this function returning data_line containing the char in the argument */
data_line char2data(char ch)
{
    data_line tmp;
    init_data_line(&tmp);
    tmp.data = ch;
    return tmp;
}

/* this function returning data_line containing the (boolean) instruction line in the argument */
data_line bline2data(instruction_line bl)
{
    data_line tmp;
    init_data_line(&tmp);

    bl.era = 0;

    tmp.data |= (mask_by_width[ERA_WIDTH]   << ERA_OFFSET)   & (bl.era << ERA_OFFSET);
    tmp.data |= (mask_by_width[DADDR_WIDTH]   << DADDR_OFFSET)   & (bl.dest_addr << DADDR_OFFSET);
    tmp.data |= (mask_by_width[SADDR_WIDTH]  << SADDR_OFFSET)  & (bl.src_addr << SADDR_OFFSET);
    tmp.data |= (mask_by_width[OPCODE_WIDTH] << OPCODE_OFFSET) & (bl.opcode << OPCODE_OFFSET);
    tmp.data |= (mask_by_width[GROUP_WIDTH]   << GROUP_OFFSET)   & (bl.group << GROUP_OFFSET);

    /* memcpy((char *)(&tmp), (char*)(&bl), sizeof(data_line)); */

    return tmp;
}

/* this function returning data_line containing the int in the argument with the sign bit a number is in 12 bits*/
data_line num2data(int num)
{
	unsigned int bit_mask = 0x7FF;
    data_line tmp;
    init_data_line(&tmp);

    if (num >= 0)
    {
        tmp.data = num & bit_mask;
    }
    else
    {
		tmp.data = (num * -1) & bit_mask;
		tmp.data = ~(tmp.data);
		(tmp.data)++;
    }

    return tmp;


}


/* this function clearing the values of an instruction line */
void init_instruction_line(instruction_line *il)
{

	il->era = 0;
    il->dest_addr = 0;
    il->src_addr = 0;
    il->opcode = 0;
    il->group = 0;

   /* memset((char *)(&il), 0, sizeof(data_line)); */

}

