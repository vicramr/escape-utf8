/*
 * This is the equivalent of run.sh for Windows. integration_test.py
 * expects a single executable file or command as input, but that won't
 * work with Java. So this program will take the command-line args
 * and spawn a Java process with them.
 *
 * Compile with:
 * cl /permissive- /W4 /DNDEBUG winrun.c
 * Should compile without any warnings.
 *
 * This program includes a malloc without a corresponding free, but
 * it really shouldn't make a difference because by the time we could
 * free the memory, the program is about to terminate.
 */

#include <process.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	const char * *newargv = malloc(sizeof(char *) * (argc + 5));
	newargv[0] = "java";
	newargv[1] = "-ea";
	newargv[2] = "-classpath";
	newargv[3] = ".;commons-cli-1.4/commons-cli-1.4.jar";
	newargv[4] = "Escape";
	for (int i = 1; i < argc; ++i) {
		newargv[i + 4] = argv[i];
	}
	newargv[4 + argc] = NULL;
#ifndef NDEBUG
	// Debugging print statement
	for (int i = 0; newargv[i] != NULL; ++i) {
		printf("Arg %d: %s\n", i, newargv[i]);
	}
#endif
	intptr_t exitcode = _spawnvp(_P_WAIT, newargv[0], newargv);
	if (exitcode == -1) {
		printf("Error when calling _spawnvp\n");
		return 10;
	} else {
#ifndef NDEBUG
		printf("Spawned process exited with status %d\n", exitcode);
#endif
		return exitcode;
	}
}
