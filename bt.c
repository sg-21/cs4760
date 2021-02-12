#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>


void format(int *returnsize, char *dir, int level, int indentation, bool symlink);
char * datechange(char *str, size_t size, time_t val);

void breadthfirst(char *dir, int level, int width, int indentation, bool symlink, char *options);
void enqueue (struct queue q*, char *dir);

//------------------------------------------

int main(int argc, char *argv[])
{
	//Optional Variables
	int indent = 4;
	bool follow_symlink = false;
	char option_string[10];

	//Options
	int opt;

	while((opt = getopt(argc, argv, "h:Ldgipstul")) != -1)
	{
		switch(opt)
		{
			case 'h':
				printf("NAME:\n");
				printf("	%s - traverse directory in breadthfirst order.\n", argv[0]);
				printf("\nUSAGE:\n");
				printf("	%s bt [-h] [-L -d -g -i -p -s -t -u | -l] [dirname].\n", argv[0]);
				printf("\nDESCRIPTION:\n");
				printf("	-h	: Print a help message and exit.\n");
				printf("	-L	: Follow symbolic links, if any. Default will be to not follow symbolic links.\n");
				printf("	-d	: Show the time of last modification.\n");
				printf("	-g	: Print the GID associated with the file.\n");
				printf("	-i	: Print the number of links to file in inode table.\n");
				printf("	-p	: Print permission bits as rwxrwxrwx.\n");
				printf("	-s	: Print the size of file in bytes...\n");
				printf("	-t	: Print information on file type.\n");
				printf("	-u	: Print the UID associated with the file.\n");
				printf("	-l	: Print information on the file as if the options \"tpiugs\" are all specified.\n\n");
				return EXIT_SUCCESS;

			case 'L':
				follow_symlink = true;
				break;

			case 'd':
				strcat(option_string, "d");
				break;

			case 'g':
				strcat(option_string, "g");
				break;

			case 'i':
				strcat(option_string, "i");
				break;

			case 'p':
				strcat(option_string, "p");
				break;

			case 's':
				strcat(option_string, "s");
				break;

			case 't':
				strcat(option_string, "t");
				break;

			case 'u':
				strcat(option_string, "u");
				break;

			case 'l':
				strcat(option_string, "tpiugs");
				break;

			default:
				fprintf(stderr, "%s: Please use \"-h\" option for more info.\n", argv[0]);
				return EXIT_FAILURE;
		}
	}

	//default to current working directory.
	char *topdir, *targetdir, current[2]=".";
    	if(argv[optind] == NULL)
	{
		char origin[4096];
        	getcwd(origin, sizeof(origin));
        	topdir = origin;
		targetdir = current;
    	}
	else
	{
        	topdir=argv[optind];
		targetdir = topdir;
	}

	//longest name in directory
	int longest_name;
	format(&longest_name, topdir, 0, indent, follow_symlink);

	//Breadthfirst search traverse directories
	breadthfirst(topdir, 0, longest_name, indent, follow_symlink, option_string);

	return EXIT_SUCCESS;
}

//column formatting.
void format(int *returnsize, char *dir, int level, int indentation, bool symlink)
{
	DIR *dp;		//A type representing a directory stream.
	struct dirent *entry;	.
	struct stat filestat;	

	char *buf;
	size_t size;
	int max_length = 0;

	//indentation space
	int spaces = level * indentation;

	//opendir able to open a directory stream 
	if((dp = opendir(dir)) == NULL)
	{
		return;
	}

	//change the current working directory.
	chdir(dir);

	//path of the current directory
	char cwd[4096];
	getcwd(cwd, sizeof(cwd));

	while((entry = readdir(dp)) != NULL)
	{
		//attributes of the file named by filename 
		lstat(entry->d_name, &filestat);

		//checks if directory
		if(S_ISDIR(filestat.st_mode))
		{
			//ignore . and ..
			if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
			{
				continue;
			}

			//find longest name in directory
			size = snprintf(NULL, 0, "%*s%s/", spaces, "", entry->d_name);
			buf = (char *)malloc(size + 1);
			snprintf(buf, size + 1, "%*s%s/", spaces, "", entry->d_name);
			max_length = strlen(buf);
			free(buf);

			//determine longest name
			if(*returnsize < max_length)
			{
				*returnsize = max_length;
			}

			format(returnsize, entry->d_name, level + 1, indentation, symlink);
		}
		else if(S_ISLNK(filestat.st_mode))
                {
			//execute if follow symbolic link is true
			if(symlink)
			{
				//found directory, ignore . and ..
				if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
				{
					continue;
				}

				stat(entry->d_name, &filestat);

				//find longest name in directory
				size = snprintf(NULL, 0, "%*s%s/", spaces, "", entry->d_name);
				buf = (char *)malloc(size + 1);
				snprintf(buf, size + 1, "%*s%s/", spaces, "", entry->d_name);
				max_length = strlen(buf);
				free(buf);

				//determine longest name
				if(*returnsize < max_length)
				{
					*returnsize = max_length;
				}

				format(returnsize, entry->d_name, level + 1, indentation, symlink);

				//previous path
				chdir(cwd);
			}
                }
		else
		{
			//find longest name in directory
			size = snprintf(NULL, 0, "%*s%s", spaces, "", entry->d_name);
                        buf = (char *)malloc(size + 1);
                        snprintf(buf, size + 1, "%*s%s", spaces, "", entry->d_name);
			max_length = strlen(buf);
			free(buf);


			//determine longest name
			if(*returnsize < max_length)
			{
				*returnsize = max_length;
			}
		}
	}

	//current working directory
	chdir("..");

	//close directory 
	closedir(dp);
}

//formats the time represented in the structure 
char * formatdate(char *str, size_t size, time_t val)
{
	strftime(str , size, "%b %d, %Y", localtime(&val));
	return str;
}

int isEmpty (struct queue *q){
	return !(q->index);
}

//initialization of queue
struct queue *initQueue(){
	struct queue *q = (struct queue *)malloc(sizeof(struct queue));
	q->index=0;
	q->front = NULL;
	q->rear = NULL; 
	return q;
}

//add directory to queue
void enqueue(struct queue *q, char *dir){
	struct node *newNode = (struct node *)malloc(sizeof(struct node));
	newNode->nextNode = NULL;

	if(!isEmpty(q)){
		q->rear->nextNode = newNode;
		q->rear = newNode;
	}
	else{
		q->front = q->rear =newNode;
	}
}

//remove directory from queue
struct node *dequeue (struct queue *q){
	struct node *temp;
	temp = q->front;
	q->front = q->nextNode;
	return temp;
}

//function of breadth first search traversal
void breadthfirst(char *dir, int level, int width, int indentation, bool symlink, char *options)
{
	DIR *dp;		//A type representing a directory stream.
	struct dirent *entry;	
	struct stat filestat;	
	struct group *grp;	
	struct passwd *pwp;	

	//indentation space 
	int spaces = level * indentation;

	//initialize queue
	q = initQueue();
	enqueue=(q, dir);
	
	//check if able open a directory 
	if((dp = opendir(dir)) == NULL)
	{
		fprintf(stderr, "%*sERROR: %s\n", spaces, "", strerror(errno));
		return;
	}

	//change current working directory
	chdir(dir);

	//current directory
	char cwd[4096];
	getcwd(cwd, sizeof(cwd));

	while((entry = readdir(dp)) != NULL)
	{
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		continue;
	}

		//file, directory, or link list.
		if(S_ISDIR(filestat.st_mode))
		{
			strcat(permission_bit, "d");
			enqueue(q, name);
			printf(name);
			display();
		}
		
		else if(S_ISLNK(filestat.st_mode))
		{
			strcat(permission_bit, "l");
			printf(name);
			display();
		}
		else if(S_ISREG(filestat.st_mode))
		{
			strcat(permission_bit, "-");
			printf(name);
			display();
		}

		//read, write, execute
		(filestat.st_mode & S_IRUSR) ? strcat(permission_bit, "r") : strcat(permission_bit, "-");
		(filestat.st_mode & S_IWUSR) ? strcat(permission_bit, "w") : strcat(permission_bit, "-");
		(filestat.st_mode & S_IXUSR) ? strcat(permission_bit, "x") : strcat(permission_bit, "-");

		(filestat.st_mode & S_IRGRP) ? strcat(permission_bit, "r") : strcat(permission_bit, "-");
		(filestat.st_mode & S_IWGRP) ? strcat(permission_bit, "w") : strcat(permission_bit, "-");
		(filestat.st_mode & S_IXGRP) ? strcat(permission_bit, "x") : strcat(permission_bit, "-");

		(filestat.st_mode & S_IROTH) ? strcat(permission_bit, "r") : strcat(permission_bit, "-");
		(filestat.st_mode & S_IWOTH) ? strcat(permission_bit, "w") : strcat(permission_bit, "-");
		(filestat.st_mode & S_IXOTH) ? strcat(permission_bit, "x") : strcat(permission_bit, "-");

		//size of a directory/file 
		long long int byte = (long long)filestat.st_size;
		char *sizesuffix = " ";
		if(byte >= 1000000000)
		{
			byte = (long long)(byte/1000000000);
			sizesuffix = "G";
		}
		else if(byte >= 1000000)
		{
			byte = (long long)(byte/1000000);
			sizesuffix = "M";
		}
		else if(byte >= 1000)
		{
			byte = (long long)(byte/1000);
			sizesuffix = "K";
		}

		//determine file type.
		char *filecategory = "";
		if(S_ISDIR(filestat.st_mode))
		{
			filecategory = "directory";
		}
		else if(S_ISLNK(filestat.st_mode))
		{
			filecategory = "symlink";
		}
		else
		{
			filecategory = "file";
		}

		//GID and UID
		grp = getgrgid(filestat.st_gid);
		pwp = getpwuid(filestat.st_uid);

		//directory, ignore . and ..
		if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
		{
			continue;
		}

		//print directory, symlink, or file
		if(S_ISDIR(filestat.st_mode))
		{
			printf("%*s%s/%*s", spaces, "", entry->d_name, 20 - spaces - (int)strlen(entry->d_name) + width, "");
		}
		else if(S_ISLNK(filestat.st_mode))
		{
			printf("%*s%s/%*s", spaces, "", entry->d_name, 20 - spaces - (int)strlen(entry->d_name) + width, "");
		}
		else
		{
			printf("%*s%s%*s", spaces, "", entry->d_name, 21 - spaces - (int)strlen(entry->d_name) + width, "");
		}

		//print attribute of the directory, symlink, or file
		int i;
		char date[20];

		for(i = 0; i < strlen(options); i++)
		{
			switch(options[i])
			{
				case 'd':
					printf("[%s]  ", formatdate(date, sizeof(date), filestat.st_mtime));
					break;

				case 'g':
					if(grp != NULL)
					{
						printf("[%*.8s]  ", 8, grp->gr_name);
					}
					else
					{
						printf("[%*d]  ", 8, filestat.st_gid);
					}
					break;

				case 'i':
					printf("[%*d]  ", 3, (unsigned int)filestat.st_nlink);
					break;

				case 'p':
					printf("[%s]  ", permission_bit);
					break;

				case 's':
					printf("[%*lld%s]  ", 4, byte, sizesuffix);
					break;

				case 't':
					printf("[%*s]  ", 9, filecategory);
					break;

				case 'u':
					if(pwp != NULL)
					{
						printf("[%*.8s]  ", 8, pwp->pw_name);
					}
					else
					{
						printf("[%*d]  ", 8, filestat.st_uid);
					}
					break;

				default:
					return;
                                }
                        }
                        printf("\n");


		if(S_ISDIR(filestat.st_mode))
		{

			breadthfirst(entry->d_name, level + 1, width, indentation, symlink, options);
		}
		else if(S_ISLNK(filestat.st_mode))
		{
			//follow if symbolic link true
			if(symlink)
			{
				//attributes of file 
				stat(entry->d_name, &filestat);

				breadthfirst(entry->d_name, level + 1, width, indentation, symlink, options);

				chdir(cwd);
			}
		}
	}

	//change current working directory 
	chdir("..");

	//close directory
	closedir(dp);
}
