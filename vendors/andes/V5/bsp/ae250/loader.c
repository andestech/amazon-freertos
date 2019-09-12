/*
 * Copyright (c) 2012-2019 Andes Technology Corporation
 * All rights reserved.
 *
 */

#ifdef CFG_BURN

extern unsigned long __flash_start, __text_start, __text_lmastart, __text_lmaend, __data_start, __data_lmastart, __data_lmaend;
extern unsigned long __loader_start, __loader_lmaend, __loader_lmastart;
extern void _start(void);


/* The entry function when boot, executing on ROM/FLASH. */
void __attribute__((naked, no_execit, no_profile_instrument_function, section(".bootloader"))) boot_loader(void) {

	register unsigned long *src_ptr, *dst_ptr;
	register unsigned long i, size;
	register void (*ldr_entry)(void) = (void *)&__loader_start;

	/* Initial $GP incase used by compiler */
	asm (
		".option push\n"
		".option norelax\n"
		"la gp, __global_pointer$\n"
		".option pop\n"
	);

	/* Copy loader() code to VMA area (On memory) */
	size = ((unsigned long)&__loader_lmaend - (unsigned long)&__loader_lmastart + (sizeof(long) - 1)) / sizeof(long);

	src_ptr = (unsigned long *)((unsigned long)&__flash_start + (unsigned long)&__loader_lmastart);
	dst_ptr = (unsigned long *)&__loader_start;

	for (i = 0; i < size; i++)
		*dst_ptr++ = *src_ptr++;

	/*
	 * Call the real program loader which executes on memory instead
	 * of flash for batter coping performance.
	 */
	ldr_entry();

}

void __attribute__((naked, no_execit, no_profile_instrument_function, section(".loader"))) loader(void) {

	register unsigned long *src_ptr, *dst_ptr;
	register unsigned long i, size;

	/* Copy TEXT section to VMA area (On memory) */
	size = ((unsigned long)&__text_lmaend - (unsigned long)&__text_lmastart + (sizeof(long) - 1)) / sizeof(long);

	src_ptr = (unsigned long *)((unsigned long)&__flash_start + (unsigned long)&__text_lmastart);
	dst_ptr = (unsigned long *)&__text_start;

	for (i = 0; i < size; i++)
		*dst_ptr++ = *src_ptr++;

	/* Copy DATA section to VMA area (On memory) */
	size = ((unsigned long)&__data_lmaend - (unsigned long)&__data_lmastart + (sizeof(long) - 1)) / sizeof(long);

	src_ptr = (unsigned long *)((unsigned long)&__flash_start + (unsigned long)&__data_lmastart);
	dst_ptr = (unsigned long *)&__data_start;

	for (i = 0; i < size; i++)
		*dst_ptr++ = *src_ptr++;

	/* Go to entry function */
	_start();

}


#endif	// CFG_BURN
