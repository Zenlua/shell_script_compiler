#include <stdio.h>
#include <iostream>

#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <string>

// Define a macro for calling memfd_create
#define memfd_create(name, flags) syscall(__NR_memfd_create, (name), (flags))
#include "obfuscate.h"

#include "script.hpp"
#include "fexecve.h"

using namespace std;
int main(int argc, char *argv[])
{
    const char *rawname = OBFUSCATE("script");
	int script_fd = memfd_create(rawname, 0);
	if (script_fd < 0 || ftruncate(script_fd, strlen(shell_script)) == -1)
		return 1;
	
	// Map the file into memory with read and write permissions
	
 	// Copy the code into the mapped memory
 	
	
	// Map the file into memory with read and write permissions
	void *script_addr = mmap(NULL, strlen(shell_script), PROT_READ | PROT_WRITE,
                    MAP_SHARED, script_fd, 0);
 	// Copy the code into the mapped memory
 	memcpy(script_addr, shell_script, strlen(shell_script));
	// Change the memory protection to allow execution
	mprotect(script_addr, strlen(shell_script), PROT_READ | PROT_EXEC);

	// Prepare the arguments for execve
	// busybox sh PATH ARGVS
	char script_str[128];
	sprintf(script_str, OBFUSCATE("/proc/self/fd/%d"), script_fd);
	char *exec_argv[argc+4];
	exec_argv[0] = strdup("sh");
	exec_argv[1] = strdup("-o");
	exec_argv[2] = strdup("standalone");
	exec_argv[3] = strdup(script_str);
	for (int i = 1; i < argc; i++)
	    exec_argv[3+i] = strdup(argv[i]);
 	exec_argv[3+argc] = nullptr;
	setenv(OBFUSCATE("ASH_STANDALONE"), OBFUSCATE("1"), true);
	//setenv(OBFUSCATE("BUSYBOX_PATH"), OBFUSCATE("/proc/self/exe"), true);
	setenv(OBFUSCATE("SCRIPT_PATH"), script_str, true);
	setenv(OBFUSCATE("ORIGINAL_PATH"), argv[0], true);
	//xfexecve(exec_argv, environ);
	return -1;
}
