/*
   This file is part of SystemBurn.

   Copyright (C) 2012, UT-Battelle, LLC.

   This product includes software produced by UT-Battelle, LLC under Contract No.
   DE-AC05-00OR22725 with the Department of Energy.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the New BSD 3-clause software license (LICENSE).

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   LICENSE for more details.

   For more information please contact the SystemBurn developers at:
   systemburn-info@googlegroups.com

 */
#ifndef __PLAN_SOPENACCGEMM_H
#define __PLAN_SOPENACCGEMM_H

#include <openacc.h>

extern void *makeSOPENACCGEMMPlan(data *i);                        // <- Change YOUR_NAME to your module's name.
extern int initSOPENACCGEMMPlan(void *p);                          // <- Change YOUR_NAME to your module's name.
extern int execSOPENACCGEMMPlan(void *p);                          // <- Change YOUR_NAME to your module's name.
extern void *killSOPENACCGEMMPlan(void *p);                        // <- Change YOUR_NAME to your module's name.
extern int parseSOPENACCGEMMPlan(char *line, LoadPlan *output);    // <- Change YOUR_NAME to your module's name.
extern int perfSOPENACCGEMMPlan(void *p);
extern plan_info SOPENACCGEMM_info;
//If there are other functions to be used by your module, declare the prototypes here. Examples in plan_dstream.h and plan_istream.h

//Below is a sample data structure. This is optional, but your module will probably need some form of input to function.

typedef struct {
  float *A_buffer;
  float *B_buffer;
  float *C_buffer;
  size_t M;
  int device_id;
  int loop_count;
  size_t device_memory;
} SOPENACCGEMM_DATA;

#endif /* __PLAN_DOPENCLBLAS_H */
