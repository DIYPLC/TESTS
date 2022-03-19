#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <iso646.h>
#include "add.h"

int main (void)
{

uint32_t var = 0;
var = add(3,2);
printf("var = %d\n",var);

return 0;
}
