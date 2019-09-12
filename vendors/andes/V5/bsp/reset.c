/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */

extern void c_startup(void);
extern void system_init(void);
extern void __libc_init_array(void);

__attribute__((weak)) void reset_handler(void)
{
	extern int main(void);

	/*
	 * Initialize LMA/VMA sections.
	 * Relocation for any sections that need to be copied from LMA to VMA.
	 */
	c_startup();

	/* Call platform specific hardware initialization */
	system_init();

	/* Do global constructors */
	__libc_init_array();

	/* Entry function */
	main();
}

/*
 * When compiling C++ code with static objects, the compiler inserts
 * a call to __cxa_atexit() with __dso_handle as one of the arguments.
 * The dummy versions of these symbols should be provided.
 */
void __cxa_atexit(void (*arg1)(void*), void* arg2, void* arg3)
{
}

void*   __dso_handle = (void*) &__dso_handle;
