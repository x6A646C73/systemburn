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
#include <systemheaders.h> // <- Good to include since it has the basic headers in it.
#include <systemburn.h> // <- Necessary to include to get the Plan struct and other neat things.
#include <planheaders.h> // <- Add your header file (plan_YOUR_NAME.h) to planheaders.h to be included. For uniformity, do not include here, and be sure to leave planheaders.h included.

//If there are other functions to be used by your module, include them somewhere outside of the four required functions. I put them here. Examples in plan_dstream.h and plan_stream.h 

// Note: the comment blocks that begin /** and contain tags such as "\brief" are Doxygen comments, used for creating an HTML manual of SystemBurn.
// Feel free to edit these descriptions (these are greatly generalized), but do not delete them, so Doxygen can document the software.

/**
 * \brief Allocates and returns the data struct for the plan
 * \param i The struct that holds the input data.
 * \return void* Data struct
 */
void * makeYOUR_NAMEPlan(data *i) { // <- Replace YOUR_NAME with the name of your module.
	Plan *p; // <- Plan pointer, necessary to integrate with systemburn. Do not change.
	YOUR_TYPE *ip; // <- Change YOUR_TYPE with the name of your data type defined in the header file.
	p=(Plan*)malloc(sizeof(Plan)); // <- Allocating the necessary Plan pointer. Do not change.
	assert(p);
	if(p) { // <- Checking the Plan pointer was allocated. Do not change.
		p->fptr_initplan = &initYOUR_NAMEPlan; // <- Replace YOUR_NAME with the name of your module. 
		p->fptr_execplan = &execYOUR_NAMEPlan; // <- Replace YOUR_NAME with the name of your module.
		p->fptr_killplan = &killYOUR_NAMEPlan; // <- Replace YOUR_NAME with the name of your module.
		p->fptr_perfplan = &perfYOUR_NAMEPlan;
		p->name          = YOUR_NAME;          // <- Replace YOUR_NAME with the name of your module.
		ip=(YOUR_TYPE*)malloc(sizeof(YOUR_TYPE)); // <- Change YOUR_TYPE to your defined data type.
		assert(ip);
		if(ip) ip->YOUR_MEMBER = *i; // <- Unless is it just an int, change this so that whatever field of your type uses an int get defined here.
		(p->vptr)=(void*)ip; // <- Setting the void pointer member of the Plan struct to your data structure. Only change if you change the name of ip earlier in this function.
	}
	return p; // <- Returning the Plan pointer. Do not change.
}

/************************
 * This is the place where the memory gets allocated, and data types get initialized to their starting values.
 ***********************/
/**
 * \brief Creates and initializes the working data for the plan
 * \param plan The Plan struct that holds the plan's data values.
 * \return Error flag value
 */
int  initYOUR_NAMEPlan(void *plan) { // <- Replace YOUR_NAME with the name of your module.
	if(!plan) return make_error(ALLOC, generic_err); // <- This is the error code for one of the malloc fails.
	Plan *p;
	YOUR_NAMEdata *d;
	p = (Plan *)plan;
	if (p) {
		d = (YOUR_NAMEdata *)p->vptr;
		p->exec_count = 0;   // Initialize the plan execution count to zero.
		perftimer_init(&p->timers, NUM_TIMERS); // Initialize all performance timers to zero.
	}
	if (d) {
		
		// Initialize plan specific data here.
		
	}
	return ERR_CLEAN; // <- This indicates a clean run with no errors. Does not need to be changed.
}

/************************
 * This is where the plan gets executed. Place all operations here.
 ***********************/
/**
 * \brief <DESCRIPTION of your plan goes here..> 
 * \param plan The Plan struct that holds the plan's data values.
 * \return int Error flag value
 */
int execYOUR_NAMEPlan(void *plan) { // <- Replace YOUR_NAME with the name of your module.
	ORB_t t1, t2;     // Storage for timestamps, used to accurately find the runtime of the plan execution.
	Plan *p;
	p = (Plan *)plan;
	p->exec_count++;   // Update the execution counter stored in the plan.
	
	ORB_read(t1);     // Store the timestamp for the beginning of the execution.
	
	// --------------------------------------------
	// Plan is executed here...
	// --------------------------------------------
	
	ORB_read(t2);     // Store timestamp for the end of execution.
	perftimer_accumulate(&p->timers, TIMER0, ORB_cycles_a(t2, t1));  // Store the difference between the timestamps in the plan's timers.
	
	if (CHECK_CALC) {     // Evaluates to true if the '-t' option is passed on the commandline.
		ORB_read(t1);
		
		// ----------------------------------------------------------------
		// Optional: Check calculations performed in execution above.
		// ----------------------------------------------------------------
		
		ORB_read(t2);
		perftimer_accumulate(&p->timers, TIMER1, ORB_cycles_a(t2, t1));
	}
	
	return ERR_CLEAN; // <- This inicates a clean run with no errors. Does not need to be changed.
}

/************************
 * This is where everything gets cleaned up. Be sure to free() your data types (free the members first) in addition to the ones included below.
 ***********************/
/**
 * \brief Frees the memory used in the plan
 * \param plan Holds the information and memory location for the plan.
 */
void * killYOUR_NAMEPlan(void *plan) { // <- Replace YOUR_NAME with the name of your module.
	Plan *p; // <- Plan type. Do not change.
	p = (Plan *)plan; // <- Setting the Plan pointer. Do not change.
	free((void*)(p->vptr)); // <- Freeing the used void pointer member of Plan. Do not change.
	free((void*)(plan)); // <- Freeing the used Plan pointer. Do not change.
	return (void*)NULL; // <- Return statement to ensure nice exit from module.
}

/************************
 * This is the parsing function that the program uses to read in your load information from the load file. This lets you have a custom number of int arguments for your module while still maintaining modularity.
 ***********************/
/**
 * \brief Reads the input file, and pulls out the necessary data for use in the plan
 * \param line The line of input text.
 * \param output The struct that holds the values for the load.
 * \return True if the data was read, false if it wasn't
 */
int parseYOUR_NAMEPlan(char *line, LoadPlan *output) { // <- Replace YOUR_NAME with the name of your module.
	output->input_data = get_sizes(line);
        output->name = YOUR_NAME; // <- Replace YOUR_NAME with the name of your module.
        output->input_data; // <- Change NUMBER_OF_YOUR_MEMBERS to equal however many int fields your data type holds.
	return output->input_data->isize + output->input_data->csize + output->input_data->dsize; // <- Return flag to know whether the parsing succeeded or not. Do not change.
}

/**
 * \brief Calculate and store performance data for the plan.
 * \param plan The Plan structure that holds the plan's data.
 * \returns Success or failure in calculating the performance.
 */
int perfYOUR_NAMEPlan(void *plan) {
	int ret = ~ERR_CLEAN;
	uint64_t opcounts[NUM_TIMERS];
	Plan *p;
	YOUR_NAMEdata *d;
	p = (Plan *)plan;
	d = (YOUR_NAMEdata *)p->vptr;
	if (p->exec_count > 0) {  // Ensures the plan has been executed at least once...
		// Assign appropriate plan-specific operation counts to the opcount[] array, such that the 
		// indices correspond with the timers used in the exec function.
		opcounts[TIMER0] = p->exec_count * YOUR_OPERATIONS_PER_EXECUTION; // Where operations can be a function of the input size.
		
		perf_table_update(&p->timers, opcounts, p->name);  // Updates the global table with the performance data.
		
		double flops  = ((double)opcounts[TIMER0]/perftimer_gettime(&p->timers, TIMER0))/1e6;  // Example for computing MFLOPS
		EmitLogfs(MyRank, 9999, "YOUR_PLAN plan performance:", flops, "MFLOPS", PRINT_SOME);           // Displays calculated performance when the '-v2' command line option is passed.
		EmitLog  (MyRank, 9999, "YOUR_PLAN execution count :", p->exec_count, PRINT_SOME);
		ret = ERR_CLEAN;
	}
	return ret;
}

/**
 * \brief Holds the custom error messages for the plan
 */
// This is not necessary; if you do not have any errors other than allocation or computation, remove this and leave its spot NULL in the plan_info stuct below
char* YOUR_ERRORS[] = {
	"ERROR message goes here"
};

plan_info YOUR_NAME = {
	"", //YOUR_NAME
	NULL, //YOUR_ERRORS (if applicable. If not, leave NULL.)
	0, // Size of ^
	makeYOUR_NAMEPlan,
	parseYOUR_NAMEPlan,
	execYOUR_NAMEPlan,
	initYOUR_NAMEPlan,
	killYOUR_NAMEPlan,
	perfYOUR_NAMEPlan,
	{ NULL, NULL, NULL } //YOUR_UNITS strings naming the units of each timer value (leave NULL if unneeded)
};
