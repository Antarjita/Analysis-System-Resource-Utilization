/*		TEST_DAEMON.C			*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <string.h>
#include <fcntl.h>

char DIRNAME[100]= "/home/antarjita/mini/proc";
static int proj_id = 234;
static FILE *fp=NULL;
static int counter = 0;
char dest[100];


void create_dir(char *dirname)
{
	int check=mkdir(dirname,0777);
	if(check)
	{
		fprintf(fp,"STATUS:%d ERROR IN CREATING A FILE!!\n",check);
		exit(1);
		
	}
	
}

void signal_handler (int sigNum)
{
	/* Write a syslog message */
	/* set the alarm again */
	/* reset the signal handler */
	
	fprintf(fp,"logged %d\n",++counter);

	signal (SIGALRM, signal_handler);
	alarm (5);
}

void copying( char * src_path,char * dst_path) 
{

    int src_fd, dst_fd, n, err;
    unsigned char buffer[10000];


    // Assume that the program takes two arguments the source path followed
    // by the destination path.
    
    src_fd = open(src_path, O_RDONLY);
    dst_fd = open(dst_path, O_CREAT | O_WRONLY);

    while (1) {
        err = read(src_fd, buffer,10000);
        if (err == -1) 
        {
            fprintf(fp,"COPYING: Error reading file!\n");
            //exit(1);
        }
        n = err;

        if (n == 0) break;

        err = write(dst_fd, buffer, n);
        if (err == -1) {
            fprintf(fp,"COPYING: Error writing to file.\n");
            //exit(1);
        }
    }
    struct stat tmp;
    stat(src_path,&tmp);
    chmod(dst_path,tmp.st_mode);
    close(src_fd);
    close(dst_fd);
}


int main(int argc, char* argv[])
{
	pid_t process_id = 0;
	pid_t sid = 0;

	// Create child process
	process_id = fork();
	// Indication of fork() failure
	if (process_id < 0)
	{
		perror ("fork failed!\n");
		// Return failure in exit status
		exit(1);
	}

	// PARENT PROCESS. Need to kill it.
	if (process_id > 0)
	{
		//perror ("process_id of child process %d \n", process_id);
		// return success in exit status
		exit(0);
	}
	signal (SIGALRM, signal_handler);

	//openlog ("MyDaemon", LOG_CONS | LOG_PID | LOG_NOWAIT, LOG_USER);
	fp=fopen("log.txt","w+");
	char *dirname=DIRNAME;
	create_dir(dirname);
	//unmask the file mode
	umask(0);
	//set new session
	sid = setsid();

	if(sid < 0)
	{
		// Return failure
		exit(1);
	}

	// Change the current working directory to root.
	chdir("/");
	// Close stdin. stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	alarm (5);

	while (1)
	{
		//snprintf(buf,100,"cp /proc/meminfo %s",dirname);
		//system(buf);
		
		system("touch meminfo.txt");
		snprintf(dest,100,"%s/%s",dirname,"meminfo.txt");
		copying("/proc/meminfo",dest);
		dest[0]='\0';
		snprintf(dest,100,"%s/%s",dirname,"stat.txt");
		copying("/proc/stat",dest);
		dest[0]='\0';
		snprintf(dest,100,"%s/%s",dirname,"vmstat.txt");
		copying("/proc/vmstat",dest);
		fflush(fp);
		fprintf(fp,"%d:",sid);
		pause ();
		system("rm -Rf /home/antarjita/mini/proc/*");
	}
	fclose(fp);
	system("rm /home/antarjita/mini/log.txt");
	return (0);
}

