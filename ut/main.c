#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <iso646.h>
#include "FbFilterNaN.h"

uint8_t Unit_test_for_FbFilterNaN(void);

int main (void)
{
uint8_t Error_counter = 0;
Error_counter = Error_counter + Unit_test_for_FbFilterNaN();
printf("Error_counter Unit_test_for_FbFilterNaN = %d \n ", Error_counter);

return 0;
}

uint8_t Unit_test_for_FbFilterNaN(void)
{
  uint8_t Error_counter = 0;
  static struct DbFilterNaN DbFilterNaN1 = {0};

  DbFilterNaN1.In = 0.0;
  printf("DbFilterNaN1.In = %f \n ", DbFilterNaN1.In);
  FbFilterNaN(&DbFilterNaN1);
  printf("DbFilterNaN1.Out = %f \n ", DbFilterNaN1.Out);
  if (1.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }

  DbFilterNaN1.In = 1.0;
  printf("DbFilterNaN1.In = %f \n ", DbFilterNaN1.In);
  FbFilterNaN(&DbFilterNaN1);
  printf("DbFilterNaN1.Out = %f \n ", DbFilterNaN1.Out);
  if (1.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }

  DbFilterNaN1.In = -1.0;
  printf("DbFilterNaN1.In = %f \n ", DbFilterNaN1.In);
  FbFilterNaN(&DbFilterNaN1);
  printf("DbFilterNaN1.Out = %f \n ", DbFilterNaN1.Out);
  if (-1.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }

  float INF_NEG = (-1.0/0.0);
  DbFilterNaN1.In = INF_NEG;
  printf("DbFilterNaN1.In = %f \n ", DbFilterNaN1.In);
  FbFilterNaN(&DbFilterNaN1);
  printf("DbFilterNaN1.Out = %f \n ", DbFilterNaN1.Out);
  if (0.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }

  float INF_POS = (1.0/0.0);
  DbFilterNaN1.In = INF_POS;
  printf("DbFilterNaN1.In = %f \n ", DbFilterNaN1.In);
  FbFilterNaN(&DbFilterNaN1);
  printf("DbFilterNaN1.Out = %f \n ", DbFilterNaN1.Out);
  if (0.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }

  float NAN1 = (0.0/0.0);
  DbFilterNaN1.In = NAN1;
  printf("DbFilterNaN1.In = %f \n ", DbFilterNaN1.In);
  FbFilterNaN(&DbFilterNaN1);
  printf("DbFilterNaN1.Out = %f \n ", DbFilterNaN1.Out);
  if (0.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }
/*
  float NAN2 = 0.0;
  NAN2 = ~NAN2;
  printf("NAN2 = %f \n ",NAN2);
  DbFilterNaN1.In = NAN2;
  printf("DbFilterNaN1.In = %f \n ", DbFilterNaN1.In);
  FbFilterNaN(&DbFilterNaN1);
  printf("DbFilterNaN1.Out = %f \n ", DbFilterNaN1.Out);
  if (0.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }
*/
  return Error_counter;
}