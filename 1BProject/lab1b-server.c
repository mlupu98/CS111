//server
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

#include <poll.h>

#include <sys/wait.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>

#include <mcrypt.h>

#define PORT    'p'
#define ENCRYPT 'e'

int socketfd;
int acceptedSocketFD;
int encryption = -1;
int encryptionKeyLength = 0;
MCRYPT encryptFD;
MCRYPT decryptFD;


int pipe2Shell[2];      // Pipe going from terminal to shell (Child)
int pipe2Terminal[2];   // Pipe going from shell to Terminal (Parent)
pid_t pid;

void createPipes()
{
    if( pipe(pipe2Shell) == -1 )
    {
        fprintf( stderr, "could not create pipe from terminal to shell" );
        exit(1);
    }

    if( pipe(pipe2Terminal) == -1 )
    {
        fprintf( stderr, "could not create pipe from terminal to shell" );
        exit(1);
    }
}

void setupChildProcess()
{
    if ( close(pipe2Shell[1]) == -1 || close(pipe2Terminal[0]) == -1 )
    {
        fprintf(stderr, "error: %s", strerror(errno));
        exit(1);
    }

    if ( dup2(pipe2Shell[0], 0) == -1 || dup2(pipe2Terminal[1], 1) == -1 || dup2(pipe2Terminal[1], STDERR_FILENO) == -1 )
    {
        fprintf(stderr, "error: %s", strerror(errno));
        exit(1);
    }

    //was the other way around
    if ( close(pipe2Shell[0]) == -1 || close(pipe2Terminal[1]) == -1 )
    {
        fprintf(stderr, "error: %s", strerror(errno));
        exit(1);
    }
}

void setupParentProcess()
{
    if( close(pipe2Shell[0]) == -1 ||  close(pipe2Terminal[1]) == -1 )
    {
        fprintf(stderr, "error: %s", strerror(errno));
        exit(1);
    }
}

void createEncryption(char* encryptionFile)
{
    struct stat encrFile;

    int keyfd = open(encryptionFile, O_RDONLY);
    if ( keyfd < 0 )
    {
        fprintf(stderr, "error: %s", strerror(errno));
    }

    if( fstat(keyfd, &encrFile) < 0)
    {
        fprintf(stderr, "error: could not retrieve the encryption file's size");
        exit(1);
    }

    //char* key = malloc(encrFile.st_size);

    char* key = (char*) malloc (encrFile.st_size * sizeof(char));

    if(read(keyfd, key, encrFile.st_size) < 0)
    {
        fprintf(stderr, "error: could not retrieve key");
        exit(1);
    }

    encryptFD = mcrypt_module_open("twofish", NULL, "cfb", NULL);
    if(encryptFD == MCRYPT_FAILED)
    {
        fprintf(stderr, "error: encryption failed");
        exit(1);
    }

    int IVsize = mcrypt_enc_get_iv_size( encryptFD );

    char* IV = malloc(IVsize);

    memset(IV, 0, IVsize);



    int encrInit = mcrypt_generic_init(encryptFD, key, encrFile.st_size, IV );
    if( encrInit < 0 )
    {
        fprintf(stderr, "error: encryption generic_init failed");
        exit(1);
    }


    decryptFD = mcrypt_module_open("twofish", NULL, "cfb", NULL);
    if(encryptFD == MCRYPT_FAILED)
    {
        fprintf(stderr, "error: decryption failed");
        exit(1);
    }


    encrInit = mcrypt_generic_init(decryptFD, key, encrFile.st_size, IV );
    if( encrInit < 0 )
    {
        fprintf(stderr, "error: decryption generic_init failed");
        exit(1);
    }

}


void runChildProcess()
{
    char shellName[] = "/bin/bash";
    char *execvpArgs[2] = { shellName, NULL };
    if( execvp( shellName, execvpArgs) == -1 )
    {
        fprintf( stderr, "error: %s", strerror(errno) );
        exit(1);
    }
}

void signalHandler( int signum )
{
    if( signum == SIGINT )
    {
        if( kill(pid, SIGINT) < 0 )
        {
            fprintf(stderr, "error: %s", strerror(errno));
            exit(1);
        }
    }
    else if ( signum == SIGTERM )
    {
        if( kill(pid, SIGTERM) < 0 )
        {
            fprintf(stderr, "error: %s", strerror(errno));
            exit(1);
        }
    }
    else if ( signum == SIGPIPE )
    {
        exit(0);
    }

}

void returnStatus()
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

    //
    //exit(0);
}

void setupPolling()
{
    char shellBuffer[256];
    char shellCrlfBuffer[2] = {'\r', '\n'};
    struct pollfd polls[2];
    //
    createPipes();

    signal(SIGPIPE, signalHandler);
    signal( SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    pid = fork();
    if ( pid < 0 )
    {
        fprintf( stderr, "error: %s", strerror(errno));
        exit(1);
    }
    else
    {
        if (pid == 0)
        {
            setupChildProcess();
            runChildProcess();
        }
        else
        {
            setupParentProcess();

            polls[0].fd = acceptedSocketFD;
            polls[0].events = (POLLIN | POLLHUP | POLLERR);

            polls[1].fd = pipe2Terminal[0];
            polls[1].events = (POLLIN | POLLHUP | POLLERR);


            while (1)
            {
                int pollVal = poll(polls, 2, 0);
                if (pollVal < 0) {
                    fprintf(stderr, "error: Could not poll correctly");
                    exit(1);
                }

                if(polls[0].revents & POLLIN)
                {
                    int size0 = read(acceptedSocketFD, shellBuffer, 256 * sizeof(char));
                    //if read is 0 might mean EOF

                    if (size0 > 0) {
                        int retWrite = -1;
                        int i;
                        for (i = 0; i < size0; i++) {
                            char character = shellBuffer[i];

                            if( encryption == 1)
                            {
                                if( mdecrypt_generic(decryptFD, &character, sizeof(char)) < 0 )
                                {
                                    fprintf(stderr, "error: could not decrypt data from shell");
                                    exit(1);
                                }
                            }
                            // Remember to remove this, this is just here to change the end of line characters
                            switch (character) {
                                case '\3':
                                    retWrite = kill(pid, SIGINT);
                                    if (retWrite < 0) { fprintf(stderr, "error: could not kill process"); }
                                    //DO I NEED TO EXIT??
                                    break;
                                case '\4':
                                    retWrite = close(pipe2Shell[1]);
                                    if (retWrite < 0) { fprintf(stderr, "error: could not close pipe to child"); }
                                    exit(0);
                                    break;
                                case '\r':
                                case '\n':
                                    retWrite = write(pipe2Shell[1], &shellCrlfBuffer[1], sizeof(char));
                                    if (retWrite < 0) {
                                        fprintf(stderr, "error: could not write to shell in shell mode");
                                    }
                                default:
                                    retWrite = write(pipe2Shell[1], &character, sizeof(char));
                                    if (retWrite < 0) {
                                        fprintf(stderr, "error: could not write to shell in shell mode");
                                    }
                            }
                        }
                        memset(shellBuffer, 0, 256 * sizeof(char)); // clear buffer of previous data
                        //not sure if this should be here
                        //retWrite = close(pipe2Shell[1]);
                        //if (retWrite < 0) { fprintf(stderr, "error: could not close pipe to child"); }
                    }
                    else if (size0 < 0 )
                    {
                        exit(0);
                    }
                    else if (size0 == 0 )
                    {
                        kill( pid, SIGTERM );
                        exit(1);
                    }
                }

                if ( polls[0].fd & (POLLERR | POLLHUP) )
                {
                    if (kill(pid, SIGINT) < 0)
                    {
                        fprintf(stderr, "error: could not kill process");
                    }

                    returnStatus();
                    exit(1);

                }

                if ( polls[1].revents & POLLIN )
                {
                    int size1 = read(pipe2Terminal[0], shellBuffer, 256 * sizeof(char));

                    if (size1 > 0)
                    {
                        int retWrite = 0;
                        int i;

                        for (i = 0; i < size1; i++)
                        {
                            char character = shellBuffer[i];

                            if( encryption == 1)
                            {
                                retWrite = mcrypt_generic(encryptFD, &character, sizeof(char));
                                if(retWrite < 0 )
                                {
                                    fprintf(stderr, "error: could not decrypt data from shell");
                                    exit(1);
                                }
                            }
                           /* if ( character == '\r' )
                            {
                                retWrite = write(acceptedSocketFD, &shellCrlfBuffer[1], sizeof(char));
                                if (retWrite < 0) { fprintf(stderr, "error: could not write to terminal in shell mode"); }
                                i++;
                            }
                            else*/
                            //{
                                write(1, &character, sizeof(char));
                                retWrite = write(acceptedSocketFD, &character, sizeof(char));
                                if (retWrite < 0) { fprintf(stderr, "error: could not write to terminal in shell mode"); }
                            //}

                        }

                        memset(shellBuffer, 0, 256 * sizeof(char)); // clear buffer of previous data
                    }
                    else if( size1 == 0)
                    {
                        close(pipe2Shell[1]);
                        exit(0);
                    }
                    else
                    {
                        fprintf(stderr, "error: %s", strerror(errno));
                        exit(1);
                    }
                }

                //maybe else
                if ( polls[1].revents & (POLLHUP | POLLERR) )
                {
                    returnStatus();

                    if (close(pipe2Terminal[0]) < 0)
                    {
                        fprintf(stderr, "could not close pipe to terminal input");
                        exit(1);
                    }

                    exit(0);
                }

//                if ( polls[1].revents & (POLLHUP | POLLERR) )
//                {
//
//                    if (kill(pid, SIGINT) < 0)
//                    {
//                        fprintf(stderr, "error: could not kill process");
//                    }
//
//                    if (close(pipe2Terminal[0]) < 0)
//                    {
//                        fprintf(stderr, "could not close pipe to terminal input");
//                        exit(1);
//                    }
//                    break;
//                }

            }
        }

    }
}

int main(int argc, char *argv[])
{
    int getOptReturnChar = -1;
    int portnum = -777;

    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    struct option long_options[] = {
            {"port"   , required_argument, NULL, PORT},
            {"encrypt", required_argument, NULL, ENCRYPT}
    };

    while( (getOptReturnChar = getopt_long(argc, argv, "p:e:", long_options, NULL)) != -1 )
    {
        if( getOptReturnChar == PORT )
        {
            portnum = strtol( optarg, NULL, 10);
            if( portnum == -777)
            {
                fprintf( stderr, "could not retrieve port number");
                exit(1);
            }
        }
        else if (getOptReturnChar == ENCRYPT )
        {
            encryption= 1;
            createEncryption(optarg);
        }
        else
        {
            fprintf(stderr, "Not one of the specified options");
            exit(1);
        }
    }

    socketfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( socketfd < 0 )
    {
        fprintf(stderr, "error: %s", strerror(errno));
        exit(1);
    }



    bzero( (char*) &serv_addr, sizeof(serv_addr) );

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portnum);

    if( bind(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 )
    {
        fprintf( stderr, " error: %s", strerror(errno));
        exit(errno);
    }


    if( listen(socketfd, 5) < 0 )
    {
        fprintf( stderr, "error: %s", strerror(errno));
        exit(1);
    }

    socklen_t client_length = sizeof(cli_addr);

    acceptedSocketFD = accept(socketfd, (struct sockaddr *) &cli_addr, &client_length);
    if( acceptedSocketFD < 0 )
    {
        fprintf(stderr, "errno: %s", strerror(errno));
        exit(1);
    }

    setupPolling();

    exit(0);
}





