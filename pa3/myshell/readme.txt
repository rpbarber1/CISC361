Ryan Barber		Humphrey Asare
ID: 702382750		ID: 702452289


Programming Assignment 3: Simple Shell

MEMORY LEAKS:
	Using valgrind to compile shows there is around 99 bytes lost
	in the "get_path()" call given in our skeleton code. Specifically,
	the memory that is lost is alloc'd in the get_path.c file. We have
	freed all the other memory that is alloc'd in the sh.c file. Since
	the get_path.c code was given, we did not change it or add any
	free() calls.


