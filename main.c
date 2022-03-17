#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <iso646.h>

int main (void)
{

bool var = false;
printf("var = %d \n ",var);

var = not(var);
printf("var = %d \n ",var);

return 0;
}