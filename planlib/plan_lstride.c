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
#include <systemheaders.h>
#include <systemburn.h>
#include <planheaders.h>

#ifdef HAVE_PAPI
#define NUM_PAPI_EVENTS 1 
#define PAPI_COUNTERS { PAPI_FP_OPS } 
#define PAPI_UNITS { "FLOPS" } 
#endif //HAVE_PAPI

/**
 * \brief Sets the values of a and b to 0
 * \param [out] a Array to be initialized.
 * \param [out] b Array to be initialized.
 * \param [in] m Size of a and b. 
 */
void LSet(long int* a, long int* b, int m) {
	int i;
	for(i=0;i<m;i++) {
		a[i] = b[i] = 0;
	}
}

/**
 * \brief Fills the indices of a with random values
 * \param [out] a Array to be initialized.
 */
void LFill(long int* a) {
	int i;
	for(i=0;i<CACHE;i++) {
		a[i] = rand();
	}
}

/**
 * \brief Allocates and returns the data struct for the plan
 * \param [in] m Holds the input data for the plan.
 * \return void* Data struct 
 * \sa parseLStridePlan
 * \sa initLStridePlan
 * \sa execLStridePlan
 * \sa perfLStridePlan
 * \sa killLStridePlan
*/
void * makeLStridePlan(data *m) {
	Plan *p;
	LStridedata *d;
	p=(Plan*)malloc(sizeof(Plan));
	assert(p);
	if(p) {
		p->fptr_initplan = &initLStridePlan;
		p->fptr_execplan = &execLStridePlan;
		p->fptr_killplan = &killLStridePlan;
		p->fptr_perfplan = &perfLStridePlan;
		p->name = LSTRIDE;
		d = (LStridedata *)malloc(sizeof(LStridedata));
		assert(d);
		if(d) {
			if(m->isize==1) d->M = m->i[0]/(2*sizeof(long int));
			else d->M = m->d[0]/(2*sizeof(long int));
		}
		(p->vptr)=(void*)d;
	}
	return p;
}

/**
 * \brief Creates and initializes the working data for the plan
 * \param [in] plan Holds the data and memory for the plan.
 * \return int Error flag value
 * \sa parseLStridePlan 
 * \sa makeLStridePlan
 * \sa execLStridePlan
 * \sa perfLStridePlan
 * \sa killLStridePlan
*/
int    initLStridePlan(void *plan) {
	size_t M;
	int ret = make_error(ALLOC,generic_err);
	Plan *p;
	LStridedata *d = NULL;
	p = (Plan *)plan;

    #ifdef HAVE_PAPI
        int temp_event, i;
        int PAPI_Events [NUM_PAPI_EVENTS] = PAPI_COUNTERS;
        char* PAPI_units [NUM_PAPI_EVENTS] = PAPI_UNITS;
    #endif //HAVE_PAPI

	if (p) {
		d = (LStridedata*)p->vptr;
		p->exec_count = 0;
                if(DO_PERF){
                        perftimer_init(&p->timers, NUM_TIMERS);

#ifdef HAVE_PAPI
                        /* Initialize plan's PAPI data */
                        p->PAPI_EventSet = PAPI_NULL;
                        p->PAPI_Num_Events = 0;

                        TEST_PAPI(PAPI_create_eventset(&p->PAPI_EventSet), PAPI_OK, MyRank, 9999, PRINT_SOME);
                        
                        //Add the desired events to the Event Set; ensure the dsired counters
                        //  are on the system then add, ignore otherwise
                        for(i=0; i<TOTAL_PAPI_EVENTS && i<NUM_PAPI_EVENTS; i++){
                            temp_event = PAPI_Events[i];
                            if(PAPI_query_event(temp_event) == PAPI_OK){
                                p->PAPI_Num_Events++;
                                TEST_PAPI(PAPI_add_event(p->PAPI_EventSet, temp_event), PAPI_OK, MyRank, 9999, PRINT_SOME);
                            }
                        }

                        PAPIRes_init(p->PAPI_Results, p->PAPI_Times);
                        PAPI_set_units(p->name, PAPI_units, NUM_PAPI_EVENTS);
                
                        TEST_PAPI(PAPI_start(p->PAPI_EventSet), PAPI_OK, MyRank, 9999, PRINT_SOME);
#endif //HAVE_PAPI
                } //DO_PERF
	}
	if (d) {
		M = d->M;

		//EmitLog(MyRank,111,"Allocating",sizeof(long int)*(M*2+CACHE),0);

		d->one   = (long int*) malloc(sizeof(long int)*M);
		assert(d->one);
		d->two   = (long int*) malloc(sizeof(long int)*M);
		assert(d->two);
		d->three = (long int*) malloc(sizeof(long int)*CACHE);
		assert(d->three);
		if(d->one && d->two && d->three) ret = ERR_CLEAN;
		srand(0);
	}
	return ret;
}

/**
 * \brief Frees the memory used in the plan
 * \param [in] plan Points to the memory to be free'd.
 * \sa parseLStridePlan
 * \sa makeLStridePlan
 * \sa initLStridePlan
 * \sa execLStridePlan
 * \sa perfLStridePlan
 */
void * killLStridePlan(void *plan) {
	Plan *p;
	LStridedata *d;
	p = (Plan *)plan;
	d = (LStridedata*)p->vptr;

	//EmitLog(MyRank,111,"Freeing   ",sizeof(long int)*(d->M*2+CACHE),0);
    #ifdef HAVE_PAPI
        TEST_PAPI(PAPI_stop(p->PAPI_EventSet, NULL), PAPI_OK, MyRank, 9999, PRINT_SOME);
    #endif //HAVE_PAPI

	if(d->one)   free(d->one);
	if(d->two)   free(d->two);
	if(d->three) free(d->three);
	free(d);
	free(p);
	return (void*)NULL;
}

/**
 * \brief An integer load which accesses memory with changing stride, using "size" bytes of memory. 
 * \param [in] plan Holds the data and memory for the plan.
 * \return int Error flag value
 * \sa parseLStridePlan
 * \sa makeLStridePlan
 * \sa initLStridePlan
 * \sa perfLStridePlan
 * \sa killLStridePlan
 */
int execLStridePlan(void *plan) {
    #ifdef HAVE_PAPI
        int j;
        long long start, end;
    #endif //HAVE_PAPI

	Plan *p;
	LStridedata* d;
	p = (Plan *)plan;
	d = (LStridedata*)p->vptr;
	size_t i,k,r, Inc[] = {1,2,8,64,512,1028,2056,4112,16448,32896};
	long int sum=0;
	int ret = ERR_CLEAN;
	ORB_t t1, t2;
	/* update execution count */
	p->exec_count++;
	
	LSet(d->one,d->two,d->M);
	LFill(d->three);

        if(DO_PERF){
#ifdef HAVE_PAPI
                /* Start PAPI counters and time */
                TEST_PAPI(PAPI_reset(p->PAPI_EventSet), PAPI_OK, MyRank, 9999, PRINT_SOME);
                start = PAPI_get_real_usec();
#endif //HAVE_PAPI
                ORB_read(t1);
        }

	for(k=0;k<REPEAT;k++) {
		for(r=0;r<10;r++) {
			for(i=0;i<d->M;i+=Inc[r]) {
				sum+=d->one[i]*d->two[i];
			}
			sum+=d->three[0];
		}
		sum++;
	}

        if(DO_PERF){
                ORB_read(t2);

#ifdef HAVE_PAPI
                end = PAPI_get_real_usec(); //PAPI time

                /* Collect PAPI counters and store time elapsed */
                TEST_PAPI(PAPI_accum(p->PAPI_EventSet, p->PAPI_Results), PAPI_OK, MyRank, 9999, PRINT_SOME);
                for(j=0; j<p->PAPI_Num_Events && j<TOTAL_PAPI_EVENTS; j++){
                    p->PAPI_Times[j] += (end - start);
                }
    #endif //HAVE_PAPI
                perftimer_accumulate(&p->timers, TIMER0, ORB_cycles_a(t2, t1));
                perftimer_accumulate(&p->timers, TIMER1, ORB_cycles_a(t2, t1));
        } //DO_PERF

	if (CHECK_CALC) {
		ret = (sum == (REPEAT * (1 + 10 * d->three[0]))) ? ERR_CLEAN : make_error(CALC, generic_err);
	}
	return ret;
}

/**
 * \brief Calculates (and optionally displays) performance data for the plan.
 * \param [in] plan The Plan structure that contains all the plan data.
 * \returns An integer error code.
 * \sa parseLStridePlan
 * \sa makeLStridePlan
 * \sa initLStridePlan
 * \sa execLStridePlan
 * \sa killLStridePlan
 */
int perfLStridePlan (void *plan) {
	int ret = ~ERR_CLEAN;
	int i, Inc[] = {1,2,8,64,512,1028,2056,4112,16448,32896};
	uint64_t opcounts[NUM_TIMERS];
	Plan *p;
	LStridedata *d;
	p = (Plan *)plan;
	d = (LStridedata *)p->vptr;
	if (p->exec_count > 0) {
		uint64_t count = 0;
		for (i = 0; i < 10; i++) {
			count += d->M / Inc[i];
		}
		
		opcounts[TIMER0] = (2 * count + 10 + 1) * REPEAT * p->exec_count;     // Count # of integer operations
		opcounts[TIMER1] = ((2 * count * REPEAT) + (1 * 10 * REPEAT)) * p->exec_count; // Count memory accesses.
		opcounts[TIMER2] = 0;
		
		perf_table_update(&p->timers, opcounts, p->name);
            #ifdef HAVE_PAPI
		PAPI_table_update(p->name, p->PAPI_Results, p->PAPI_Times, p->PAPI_Num_Events);
            #endif //HAVE_PAPI
		
		double ips  = ((double)opcounts[TIMER0]/perftimer_gettime(&p->timers, TIMER0))/1e6;
		double mbps = ((double)opcounts[TIMER1]/perftimer_gettime(&p->timers, TIMER1))/1e6;
		EmitLogfs(MyRank, 9999, "LSTRIDE plan performance:", ips, "MI64OPS", PRINT_SOME);
		EmitLogfs(MyRank, 9999, "LSTRIDE plan performance:", mbps, "MB/s", PRINT_SOME);
		EmitLog  (MyRank, 9999, "LSTRIDE execution count :", p->exec_count, PRINT_SOME);
		ret = ERR_CLEAN;
	}
	return ret;
}

/**
 * \brief Reads the input file, and pulls out the necessary data for use in the plan
 * \param [in] line The input line for the plan.
 * \param [out] output Holds the data for the load.
 * \return int True if the data was read, false if it wasn't
 * \sa makeLStridePlan 
 * \sa initLStridePlan
 * \sa execLStridePlan
 * \sa perfLStridePlan
 * \sa killLStridePlan
*/
int parseLStridePlan(char *line, LoadPlan *output) {
	output->input_data = get_sizes(line);	
	output->name = LSTRIDE;
	return (output->input_data->isize + output->input_data->csize + output->input_data->dsize > 0);
}

/**
 * \brief The data structure for the plan. Holds the input and all used info. 
 */
plan_info LSTRIDE_info = {
	"LSTRIDE",
	NULL,
	0,
	makeLStridePlan,
	parseLStridePlan,
	execLStridePlan,
	initLStridePlan,
	killLStridePlan,
	perfLStridePlan,
	{ "I64OPS", "B/s", NULL }
};
