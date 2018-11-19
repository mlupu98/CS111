//Name: Matei Lupu

//Email: mateilupu20@g.ucla.edu

//ID: 504798407

#include <unistd.h>

#include <string.h>

#include <errno.h>

#include <getopt.h>

#include <stdbool.h>

#include <stddef.h>

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <signal.h>

#include <termios.h>

#include <fcntl.h>

#include <poll.h>

#include <sys/wait.h>

#define SHELL 's'
#define DEBUG 'd'

struct termios initialTerminal; // saved terminal attributes
struct termios changedTerminal; // modified terminal attributes

int pipe2Shell[2];      // Pipe going from terminal to shell (Child)
int pipe2Terminal[2];   // Pipe going from shell to Terminal (Parent)
pid_t pid;
int shellCalled = -1;

int setupTerminal()
{
    int retval = 0;

    tcgetattr(0, &initialTerminal);     // save initial attributes
    tcgetattr(0, &changedTerminal);     // store attributes in new termios

    // modify attributes
    changedTerminal.c_iflag = ISTRIP;
    changedTerminal.c_oflag = 0;
    changedTerminal.c_lflag = 0;

    // set new attributes to terminal
    retval = tcsetattr(0, TCSANOW, &changedTerminal);

    return retval; // returns -1 if new attributes were not set properly
};

void resetTerminal()
{
    if( tcsetattr(0, TCSANOW, &initialTerminal) < 0)
    {
        fprintf( stderr, "error %s", strerror(errno));
    }

    if( shellCalled == 1 )
    {
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            fprintf(stderr, "error: was not able to wait for child process to terminate");
        }

        int signal = (status & 0x007f);

        int shiftedSignal = (status >> 8);
        status = (shiftedSignal & 0x00ff);

        fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d", signal, status);

    }
    exit(0);
}

void setupChildProcess()
{
    if ( close(pipe2Shell[1]) == -1 || close(pipe2Terminal[0]) == -1 )
    {
        fprintf(stderr, "error: %s", strerror(errno));
        exit(1);
    }

    if ( dup2(pipe2Shell[0], 0) == -1 || dup2(pipe2Terminal[1], 1) == -1 )
    {
        fprintf(stderr, "error: %s", strerror(errno));
        exit(1);
    }

    if ( close(pipe2Shell[0]) == -1 || close(pipe2Terminal[1]) == -1 )
    {
        fprintf(stderr, "error: %s", strerror(errno));
        exit(1);
    }
}

void runChildProcess(char program[])
{
    char *execvpArgs[2] = { program, NULL };
    if( execvp(program, execvpArgs) == -1 )
    {
        fprintf( stderr, "error: %s", strerror(errno) );
        exit(1);
    }
}

void setupPolling(int fd1)
{

    char shellBuffer[256];
    char shellCrlfBuffer[2] = {'\r', '\n'};

    struct pollfd polls[2];

    polls[0].fd = fd1;
    polls[1].fd = pipe2Terminal[0];
    polls[0].events = (POLLIN | POLLHUP | POLLERR);
    polls[1].events = (POLLIN | POLLHUP | POLLERR);

    while(1)
    {
        int pollVal = poll(polls, 2, 0);
        if (pollVal < 0)
        {
            fprintf(stderr, "error: Could not poll correctly");
            exit(1);
        }

        if (polls[0].revents & POLLIN)
        {
            int size0 = read(fd1, shellBuffer, 256 * sizeof(char));
            if (size0 >= 0) {
                int retWrite = -1;
                int i;
                for (i = 0; i < size0; i++)
                {
                    char character = shellBuffer[i];
                    // Remember to remove this, this is just here to change the end of line characters
                    switch (character)
                    {
                        case '\3':
                            kill(pid, SIGINT);
                            break;
                        case '\4':
                            retWrite = close(pipe2Shell[1]);
                            if (retWrite < 0) { fprintf(stderr, "error: could not close pipe to child"); }
                            resetTerminal();
                            break;
                        case '\r':
                        case '\n':
                            retWrite = write(STDOUT_FILENO, &shellCrlfBuffer[0], 2 * sizeof(char));
                            if (retWrite < 0) { fprintf(stderr, "error: could not write to terminal in shell mode"); }

                            retWrite = write(pipe2Shell[1], &shellCrlfBuffer[1], sizeof(char));
                            if (retWrite < 0) { fprintf(stderr, "error: could not write to shell in shell mode1"); }
                            break;
                        default:
                            retWrite = write(STDOUT_FILENO, &character, sizeof(char));
                            if (retWrite < 0) { fprintf(stderr, "error: could not write to terminal in shell mode"); }

                            retWrite = write(pipe2Shell[1], &character, sizeof(char));
                            if (retWrite < 0) { fprintf(stderr, "error: could not write to shell in shell mode"); }
                    }
                }

                memset(shellBuffer, 0, 256 * sizeof(char)); // clear buffer of previous data
            }
            else
            {
                fprintf(stderr, "error: could not read from keyboard");
                exit(1);
            }
        }

        if (polls[1].revents & POLLIN)
        {
            int size1 = read(pipe2Terminal[0], shellBuffer, 256 * sizeof(char));

            if (size1 >= 0)
            {
                int retWrite = 0;
                int i;

                for (i = 0; i < size1; i++)
                {
                    char character = shellBuffer[i];
                    switch (character)
                    {
                        case '\n':
                            retWrite = write(STDOUT_FILENO, &shellCrlfBuffer, 2 * sizeof(char));
                            if (retWrite < 0) { fprintf(stderr, "error: could not write to terminal in shell mode"); }
                            break;
                        default:
                            retWrite = write(STDOUT_FILENO, &character, sizeof(char));
                            if (retWrite < 0) { fprintf(stderr, "error: could not write to terminal in shell mode"); }
                    }
                }

                memset(shellBuffer, 0, 256 * sizeof(char)); // clear buffer of previous data
            }
            else
            {
                fprintf(stderr, "error: %s", strerror(errno));
                exit(1);
            }
        }

        if (polls[0].fd & (POLLERR | POLLHUP))
        {
            if (kill(pid, SIGINT) < 0)
            {
                fprintf(stderr, "error: could not kill process");
            }

            if (close(pipe2Terminal[0]) < 0)
            {
                fprintf(stderr, "could not close pipe to terminal input");
                exit(1);
            }
            break;
        }

        if (polls[1].fd & (POLLERR | POLLHUP))
        {
            if (close(pipe2Terminal[0]) < 0)
            {
                fprintf(stderr, "error: %s", strerror(errno));
                exit(1);
            }

            break;
        }

    }
}

int main(int argc, char* argv[]) {

    if ( setupTerminal() <  0 )
    {
        fprintf(stderr, "error: could not set termninal in canonical non echo mode");
        exit(1);
    }

    atexit(resetTerminal);

    int getoptReturnChar = -1; //returns -1 if not found
    int bufSize = 0;

    char buffer[256];                   // buffer for input
    char crlfBuffer[2] = {'\r', '\n'};  // end of line replacement


    static struct option long_options[] = {
            {"shell", required_argument, NULL, SHELL},
            {"debug", no_argument      , NULL, DEBUG},
            { 0     , 0                , 0   ,     0}
    };


    if( pipe(pipe2Shell) == -1 )
    {
        fprintf( stderr, "could not create pipe from terminal to shell" );
    }

    if( pipe(pipe2Terminal) == -1 )
    {
        fprintf( stderr, "could not create pipe from terminal to shell" );
    }


    char *shellProgram;

    while ((getoptReturnChar = getopt_long(argc, argv, "", long_options, NULL)) != -1)
    {
        if (getoptReturnChar == SHELL)
        {
            pid = fork();
            shellCalled = 1;

            shellProgram = optarg;

            if( pid == -1)
            {
                fprintf( stderr, "error: %s", strerror(errno) );
                exit(1);
            }
        }
        else
        {
            fprintf(stderr, "Not one of the specified options");
            exit(1);
        }

    }

    if (shellCalled == 1)
    {
        if( pid == 0 )
        {
            setupChildProcess();
            runChildProcess(shellProgram);
        }
        else
        {
            if( close(pipe2Shell[0]) == -1 ||  close(pipe2Terminal[1]) == -1 )
            {
                fprintf(stderr, "error: %s", strerror(errno));
                exit(1);
            }

            setupPolling(0);
        }
    }

    bufSize = read(STDIN_FILENO, buffer, 256*sizeof(char));
    while(bufSize)
    {
        int i;
        for (i = 0; i < bufSize; i++)
        {
            char character = buffer[i];
            if ( character == '\4' )
            {
                exit(0);
            }
            else if ( character == '\r' || character == '\n')
            {
                write(1, &crlfBuffer, 2*sizeof(char));
                continue;
            }
            else if (write( 1 , &buffer[i], sizeof(char)) < 0)
            {
                fprintf( stderr, "error: %s Could not write.", strerror(errno));
            }

        }

        memset(buffer, 0, 256*sizeof(char)); // clear buffer of previous data

        bufSize = read(STDIN_FILENO, buffer, 256*sizeof(char));
    }

    exit(0);
}






