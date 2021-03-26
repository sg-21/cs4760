#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

int hflag=0;
int Lflag=0;
int tflag=0;
int pflag=0;
int iflag=0;
int uflag=0;
int gflag=0;
int sflag=0;
int dflag=0;

int opt;
char command[100];

static char error[100];
static char usage[100];

 /*queue struct*/
struct Queue                       
{
    int front, rear, size;
    unsigned capacity;
    char** array;
};

struct Queue* createQueue(unsigned capacity)        
{
    struct Queue* queue = malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = malloc(queue->capacity * sizeof(char*));
    return queue;
}

int isFull(struct Queue* queue)
{
    return (queue->size == queue->capacity);
}

int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}

/*adds item to back of queue*/
void enqueue(struct Queue* queue, char* item)   
{
    if (isFull(queue))
    {
        return;
    }
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

/*removes item from front of queue*/
char* dequeue(struct Queue* queue)          
{
    if (isEmpty(queue))
    {
        return "";
    }
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return "";
}

/*returns char array stored on front of array*/
char* front(struct Queue* queue)            
{
    if (isEmpty(queue))
    {
        return "";
    }
    return queue->array[queue->front];
}

/*return output of unix system command*/
char* getCmdOutput(const char *cmd)          
{
    FILE *fp = popen(cmd, "r");
    static char output[40];
    if (fp != NULL)
    {
        if (fgets(output, sizeof(output), fp))
        {
            /*strips newline off string*/
            output[strcspn(output, "\n\r")] = 0;        
        }
        pclose(fp);
    }
    return output;
}

/*capture all files and dir paths - enter into the queue*/
void breadthFirst(struct Queue *queue, char *path)     
{
    static char cmd[40];
    strcpy(cmd,"ls -1");
    strcat(cmd," ");
    strcat(cmd,path);

    FILE *fp = popen(cmd, "r");             
    static char temp[100];

    if (fp != NULL)
    {
        /*traverses output line by line - stores in tvariable "temp"*/
        while (fgets(temp, sizeof(temp), fp))       
        {
            char *fileName = malloc(100);
            temp[strcspn(temp, "\n\r")] = 0;

            /*if path not empty, copy path into "fileName"*/
            if(strcmp(path, "") != 0)          
            {
                strcpy(fileName,path);
                strcat(fileName, "/");
            }

            /*append path to file/directory onto the cpd*/
            strcat(fileName, temp);             
            enqueue(queue, fileName);         
        }
        pclose(fp);
    }
}

int isDirectory(char *path)        
{
    static char temp5[100];
    strcpy(temp5, "stat --format=%A");
    strcat(temp5, " ");
    strcat(temp5, path);
    strcat(temp5, " | awk '{print substr($0,1,1)}'");

    /*checks if dir*/
    if(strcmp("d", getCmdOutput(temp5))==0)         
    {
        return 1;
    } else
    {
        return 0;
    }
}

/*prints info for each path argument given*/
void printInfo(char *path)      
{
    /*char arrays for storing each option*/
    char type[20];                  
    char perm[20];
    char link[20];
    char uid[20];
    char gid[20];
    char size[20];
    char time[20];

    if (hflag == 1)       
    {
        printf("%s\n", usage);
        printf("Prints all file and directories in breadth order with selected options.\n\n");
        printf("-h Print a help message and exit.\n");
        printf("-L Follow symbolic links\n");
        printf("-t Print information on ﬁle type\n");
        printf("-p Print permission bits(r-read w-write x-execute)\n");
        printf("-i Print the number of links to ﬁle in inode table\n");
        printf("-u Print the UID associated with the ﬁle\n");
        printf("-g Print the GID associated with the ﬁle\n");
        printf("-s Print the size of ﬁle in bytes\n");
        printf("-d Show the time of last modiﬁcation\n");
        printf("-l Print information on the ﬁle as if all of the options are speciﬁed\n");

        exit(0);
    }


    /*if (Lflag == 1)
    {
      fprintf(stderr, "Lflag!\n");
      system("readlink -f bt.c");

      NOT FUNCTIONAL
    }*/
    

    /*File type*/
    if (tflag == 1)                 
    {
        strcpy(command, "stat --format=%A");
        strcat(command, " ");
        strcat(command, path);
        strcat(command, " | awk '{print substr($0,1,1)}'");
        strcpy(type, getCmdOutput(command));

        printf("%-1s", type);

         /*adds space behind file type output if permissions DNE*/
        if (pflag == 0)            
        {
            printf(" ");
        }
    }

    /*permissions*/
    if (pflag == 1)                 
    {
        strcpy(command, "stat --format=%A");
        strcat(command, " ");
        strcat(command, path);
        strcat(command, " | awk '{print substr($0,2,10)}'");
        strcpy(perm, getCmdOutput(command));

        printf("%-9s ", perm);
    }

    /*inode links to file*/
    if (iflag == 1)                 
    {
        strcpy(command, "stat --format=%h");
        strcat(command, " ");
        strcat(command, path);
        strcpy(link, getCmdOutput(command));

        printf("%-3s ", link);
    }

    /*user ID name*/
    if (uflag == 1)                 
    {
        strcpy(command, "stat --format=%U");
        strcat(command, " ");
        strcat(command, path);
        strcpy(uid, getCmdOutput(command));

        printf("%-9s ", uid);
    }

    /*group ID name*/
    if (gflag == 1)                 
    {
        strcpy(command, "stat --format=%G");
        strcat(command, " ");
        strcat(command, path);
        strcpy(gid, getCmdOutput(command));

        printf("%-9s ", gid);
    }

    /*file size*/
    if (sflag == 1)                 
    {
        strcpy(command, "ls -ldh");
        strcat(command, " ");
        strcat(command, path);
        strcat(command, " | awk '{ print $5}'");
        strcpy(size, getCmdOutput(command));

        printf("%-5s ", size);
    }

    /*last modification time*/
    if (dflag == 1)                 
    {
        strcpy(command, "date +\"%b %d, %Y\" -r");
        strcat(command, " ");
        strcat(command, path);
        strcpy(time, getCmdOutput(command));

        printf("%-12s ", time);
    }

    /*prints path and name for current file/directory*/
    printf("%-s\n", path);         
}


int main (int argc, char *argv[])
{
    int index;
    strcpy(error,argv[0]);
    strcat(error,": Error");

    strcpy(usage,"Usage: ");
    strcat(usage,argv[0]);
    strcat(usage," [-h] [-L -d -g -i -p -s -t -u | -l] [dirname]");

    while ((opt = getopt(argc, argv, "hLtpiugsdl")) != -1)		
    {
        switch (opt)
        {
            case 'h':
                hflag = 1;
                break;
            case 'L':
                Lflag = 1;
                break;
            case 't':
                tflag = 1;
                break;
            case 'p':
                pflag = 1;
                break;
            case 'i':
                iflag = 1;
                break;
            case 'u':
                uflag = 1;
                break;
            case 'g':
                gflag = 1;
                break;
            case 's':
                sflag = 1;
                break;
            case 'd':
                dflag = 1;
                break;
            case 'l':
                Lflag = dflag = gflag = iflag = pflag = sflag = tflag = uflag = 1;
                break;
            default:
                fprintf(stderr, "%s\n", usage);
                exit(EXIT_FAILURE);
        }
    }

    /*checks for only 1 argument, if any, after options*/
    if(argc-optind > 1)             
    {
        printf("%s: Too many arguments\n",error);
        fprintf(stderr, "%s\n", usage);
        exit(EXIT_FAILURE);
    }

    /*creates queue*/
    struct Queue* queue = createQueue(1000);        
    char path[200];
    strcpy(path, "");
    char *temp2 = malloc(strlen(path));

    for (index = optind; index < argc; index++)
    {
        int result;

        /*checks if valid file or directory*/
        result = access(argv[index], F_OK);         

        if ( result == 0 )
        {
            strcpy(path,argv[index]);
        }
        else
        {
            /*perror message if file/directory not valid*/
            perror(error);                          
            exit(EXIT_FAILURE);
        }
    }

    strcpy(temp2,path);
    enqueue(queue, temp2);

    while(queue->size > 0)
    {
        /*reading from queue and stores into "path"*/
        strcpy(path,front(queue));                 

        if(strcmp(path, "") != 0)                       
        {
            if(isDirectory(path) == 1)                  
            {
                /*call breadthfirst function*/
                breadthFirst(queue, path);              
            }
        }else
        {
            /*call breadthfirst function if path is empty*/
            breadthFirst(queue, path);                
        }

        if(strcmp(path, "") != 0)                       
        {
            printInfo(path);
        }

        /*call dequeue*/
        dequeue(queue);                                 
    }

    free(queue->array);                         
    free(queue);
    free(temp2);

    return 0;
}

