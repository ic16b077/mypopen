/**
 * @file mypopen.c
 * Betriebssysteme Mypopen File.
 * Beispiel 2
 *
 * @author Aleksandar Pavlovic, Johanna Hoffmann, Michael Käfer
 * @date 2017/04/30
 *
 * @version 1
 *
 * @todo Review
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */
#include "mypopen.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

/*
 * --------------------------------------------------------------- defines --
 */
#define READ "r"
#define WRITE "w"

/*
 * --------------------------------------------------------------- globals --
 */
/* pid is initialized with -1 at the beginning because this value indicates the error
 * retval in reference to fork() invokation */
static pid_t pid = -1;        
static FILE *file_ptr = NULL;

/**
 *
 * \brief Implementaion of C standard library popen function
 *
 * \param command pointer to a null-terminated string containing a shell command line
 * \param type pointer to a null-terminated string which must contain either the letter 'r' for reading or the letter 'w' for writing
 *
 * \return on success a pointer to an open stream, on error NULL
 * \retval pointer to an open stream that can be used to read or write to the pipe
 * \retval NULL if arguments are invalid, if the fork or pipe calls fail, or if it cannot allocate memory
 *
 */
FILE *mypopen(const char *command, const char *type) {
	int fd[2] = {0};

	/* Verify that no other filedescriptor is currently opened
	 * also consider to address errno appropriately */
	if (file_ptr != NULL)  
	{
		errno = EAGAIN;
		return NULL;
	}	
	if (command == NULL || command[0] == '\0')
	{
		errno = EINVAL;
		return NULL;
	}
	if (pid >= 0) {
		/* Verify that the function was not invoked before and thus
		 * another child process could be running in the background */
		errno = EAGAIN;
		return NULL;
	}
	if (*type != 'r' && *type != 'w' || *(type+1) != '\0')
	{
		errno = EINVAL;
		return NULL;
	}
	
	/* Errno is set appropriately within the system call procedure */
	if (pipe(fd) == -1)
		return NULL;
	
	switch (pid = fork())
	{
		/* Error */
		case -1:
			(void) close(fd[0]);
			(void) close(fd[1]);
			return NULL;
			break;
		/* Child */
		case 0:
			if (strcmp(type, READ) == 0)
			{
				(void) close(fd[0]);
				/* Verify if the corresponding file descriptor already includes the value from stdin/stdout */
				if (fd[1] != STDOUT_FILENO) 
				{
					if (dup2(fd[1], STDOUT_FILENO) == -1)
					{
						(void) close(fd[1]);
						/* Terminate the child process immediately
						 * and release ressources in order to not interfere with the parent process*/
						_exit(EXIT_FAILURE); 
					}
					(void) close(fd[1]);
				}
			}
			else
			{
				(void) close(fd[1]);
				if (fd[0] != STDIN_FILENO)
				{
					if (dup2(fd[0], STDIN_FILENO) == -1)
					{
						(void) close(fd[0]);
						_exit(EXIT_FAILURE);
					}
					(void) close(fd[0]);
				}
			}
			(void) execl("/bin/sh", "sh", "-c", command, (char *)NULL);
			/*this fraction will be only executed if execl() fails and an error occurs*/
			_exit(EXIT_FAILURE);
			break;
		/* Parent */
		default:
			if (strcmp(type,READ) == 0) 
			{
				(void) close(fd[1]);
				if ((file_ptr = fdopen(fd[0], READ)) == NULL)
				{
					(void) close(fd[0]);
					return NULL;
				}
			}
			else 
			{
				(void) close(fd[0]);
				if ((file_ptr = fdopen(fd[1], WRITE)) == NULL) 
				{
					(void) close(fd[1]);
					return NULL;
				}
			}
			break;
	}
	return file_ptr;
}

/**
 *
 * \brief Implementaion of C standard library pclose function
 *
 * \param stream a standard I/O stream
 *
 * \return on success the termination status of the command language interpreter. Otherwise, on error -1
 * \retval pointer to an open stream that can be used to read or write to the pipe
 * \retval -1 on error
 *
 */
int mypclose(FILE *stream) {
	pid_t wait_pid;
	int status = 0;
	
	/* The static PID should contain a number >0 in order to indicate successfull process creation */
	if (pid == -1)
	{
		errno = ECHILD;
		return -1;
	}
	if (file_ptr == NULL)
	{
		errno = EINVAL;
		return -1;
	}
	if (stream == NULL || stream != file_ptr) 
	{
		/* Verify that only a valid file pointer (referring to one of the pipe ends) was committed to mypclose() */
		errno = EINVAL;
		return -1;
	}
	if (fclose(stream) == EOF)
	{  
		/* Globals are reset in order to avoid another fclose() call => it would lead to undefined behavior */
		file_ptr = NULL;
		pid = -1;
		return -1;
	}
	
	while ((wait_pid = waitpid(pid, &status, 0)) != pid)
	{
		if (wait_pid == -1)
		{
			if (errno == EINTR)
			{
				/* Waitpid() interrupted => continue */
				continue;
			}
			/* An error occurs and errno has to be initialized appropriately
			 * also the globals have to be reset in order to avoid side effects, respectively an infinite loop */
			errno = ECHILD;
			pid = -1;
			file_ptr = NULL;
			return -1;
		}
	}
	if (WIFEXITED(status))
	{
		pid = -1;
		file_ptr = NULL;
		return WEXITSTATUS(status);
	}
	else
	{
		errno = ECHILD;
		pid = -1;
		return -1;
	}
}
