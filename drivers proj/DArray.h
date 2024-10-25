#pragma once
#include <wdm.h>


#define INITILIZE_SIZE 120

typedef struct {
	unsigned short used;
	unsigned short allocated;
	void* a; 
} darray;



/// <summary>
/// allocate memory for the array, and init the value of the struct
/// </summary>
/// <param name="new_arr"> the struct to init </param>
void arrInitilize(darray& new_arr);

/// <summary>
/// insert value into the array (allocate more memory if needed)
/// </summary>
/// <param name="dst"> the struct of the array </param>
/// <param name="val"> the value to add </param>
void arrInsert(darray& dst, int val); 


/// <summary>
/// free the allocated space of the array
/// </summary>
/// <param name="dst"> the struct of the array </param>
void arrFree(darray dst);