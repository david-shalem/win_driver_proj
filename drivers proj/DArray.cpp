#include "DArray.h"

ULONG tmp_tag = 'tag ';

void arrInitilize(darray& new_arr)
{
	// allocate memory for the array 
	new_arr.a = ExAllocatePool2(POOL_FLAG_RAISE_ON_FAILURE | POOL_FLAG_NON_PAGED, INITILIZE_SIZE * sizeof(int), tmp_tag++);

	// initilize the start size of the array and the start used space (0)
	new_arr.allocated = INITILIZE_SIZE;
	new_arr.used = 0;
}

void arrInsert(darray& dst, int val)
{
	if (dst.allocated - dst.used <= 1) { // if all of the space in the array is used
		// allocate new array with bugger sizes
		PVOID new_alloc = ExAllocatePool2(NonPagedPoolExecute, dst.allocated * 2 * sizeof(int), tmp_tag++);
		
		// copy the memory from the old and smaller array to the new one 
		RtlCopyMemory(new_alloc, dst.a, dst.used * sizeof(int));
		
		// free the old and small array
		ExFreePool(dst.a);

		// insert the new array to the array object
		dst.a = new_alloc;
	}

	// insert the new value 
	memcpy(((char*)dst.a + (dst.used * sizeof(int))), &val, sizeof(int)); 
	
	dst.used++;
}


void arrFree(darray dst)
{
	ExFreePool(dst.a);
}
