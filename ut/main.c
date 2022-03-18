#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <iso646.h>
#include "FbFilterNaN.h"

uint8_t Unit_test_for_FbFilterNaN(void)
{
  uint8_t Error_counter = 0;
  static struct DbFilterNaN DbFilterNaN1 = {0};

  DbFilterNaN1.In = 0.0;
  FbFilterNaN(&DbFilterNaN1);
  if (1.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }

  DbFilterNaN1.In = 1.0;
  FbFilterNaN(&DbFilterNaN1);
  if (1.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }

  DbFilterNaN1.In = -1.0;
  FbFilterNaN(&DbFilterNaN1);
  if (-1.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }

  float INF_NEG = (-1.0/0.0);
  printf("INF_NEG = %f \n ",INF_NEG);
  DbFilterNaN1.In = INF_NEG;
  FbFilterNaN(&DbFilterNaN1);
  if (0.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }

  float INF_POS = (1.0/0.0);
  printf("INF_POS = %f \n ",INF_POS);
  DbFilterNaN1.In = INF_POS;
  FbFilterNaN(&DbFilterNaN1);
  if (0.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }

  float NAN1 = (0.0/0.0);
  printf("NAN1 = %f \n ",NAN1);
  DbFilterNaN1.In = NAN1;
  FbFilterNaN(&DbFilterNaN1);
  if (0.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }

  float NAN2 = 0.0;
  NAN2 = ~NAN2;
  printf("NAN2 = %f \n ",NAN2);
  DbFilterNaN1.In = NAN2;
  FbFilterNaN(&DbFilterNaN1);
  if (0.0 != DbFilterNaN1.Out) { Error_counter = Error_counter + 1; }

  return Error_counter;
}

int main (void)
{
uint8_t Error_counter = 0;
Error_counter = Error_counter + Unit_test_for_FbFilterNaN(void);
printf("Error_counter Unit_test_for_FbFilterNaN = %d \n ",Error_counter);

return 0;
}