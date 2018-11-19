//client
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
#define LOG     'l'
#define ENCRYPT 'e'

struct termios initialTerminal; // saved terminal attributes
struct termios changedTerminal; // modified terminal attributes

int pipe2Shell[2];    // Pipe going from terminal to shell (Child)
int pipe2Server[2];   // Pipe going from shell to Server   (Parent)
char buffer[256];     // buffer for input

pid_t pid;
int socketfd;
int logfd;

int serverCalled  = -1;
int encryptCalled = -1;
int logCalled     = -1;

int encryptionKeyLength = 0;
MCRYPT encryptFD;
MCRYPT decryptFD;

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

void setupPolling()
{
    char shellBuffer[256];
    char shellCrlfBuffer[2] = {'\r', '\n'};

    struct pollfd polls[2];
    int bufferSize;
    char currChar;

    polls[0].fd = STDIN_FILENO;
    polls[1].fd = socketfd;
    polls[0].events = (POLLIN | POLLHUP | POLLERR);
    polls[1].events = (POLLIN | POLLHUP | POLLERR);

    while(1)
    {
        int pollVal = poll( polls, 2, 0 );
        if ( pollVal < 0 )
        {
            fprintf(stderr, "error: %s", strerror(errno));
            exit(1);
        }
        else
        {
            if( polls[0].revents & POLLIN )
            {
                // DOUBLE CHECK
                bufferSize = read(STDIN_FILENO, shellBuffer, 256 * sizeof(char));

                if( logCalled == 1)
                {
                    dprintf( logfd, "SENT %d bytes: ", bufferSize );
                }

                if( bufferSize < 0 )
                {
                    fprintf(stderr, "error: %s", strerror(errno));
                    exit(1);
                }
                else
                {
                    int i = 0;
                    for( ; i < bufferSize; i++ )
                    {
                        currChar = shellBuffer[i];
                        int retWrite = -1;

                        if( currChar == '\r' || currChar == '\n' )
                        {

                            retWrite = write(STDOUT_FILENO, &shellCrlfBuffer, 2 * sizeof(char));
                            if (retWrite < 0) { fprintf(stderr, "error: could not write to terminal in shell mode"); }

                            char encryptedChar = shellCrlfBuffer[1];

                            if( encryptCalled == 1)
                            {
                                retWrite = mcrypt_generic(encryptFD, &encryptedChar, sizeof(char));
                                if( retWrite < 0)
                                {
                                    fprintf(stderr, "error: could not encrypt lf");
                                    exit(1);
                                }
                            }

                            retWrite = write(socketfd, &encryptedChar, sizeof(char));
                            if (retWrite < 0) { fprintf(stderr, "error: could not write to socked in client mode"); }

                            if( logCalled == 1 )
                            {
                                retWrite = write(logfd, &encryptedChar, sizeof(char));
                                if (retWrite < 0) { fprintf(stderr, "error: could not write to log in client mode"); }
                            }

                        }
                        else
                        {
                            retWrite = write(STDOUT_FILENO, &currChar, sizeof(char));
                            if(retWrite < 0 ) if (retWrite < 0) { fprintf(stderr, "error: could not write to terminal in client mode"); }

                            char encryptedChar = currChar;

                            if( encryptCalled == 1)
                            {
                                retWrite = mcrypt_generic(encryptFD, &encryptedChar, sizeof(char));
                                if( retWrite < 0)
                                {
                                    fprintf(stderr, "error: could not encrypt lf");
                                    exit(1);
                                }
                            }

                            retWrite = write(socketfd, &encryptedChar, sizeof(char));
                            if(retWrite < 0 ) if (retWrite < 0) { fprintf(stderr, "error: could not write to socket in client mode"); }

                            if( logCalled == 1 )
                            {
                                retWrite = write(logfd, &encryptedChar, sizeof(char));
                                if (retWrite < 0) { fprintf(stderr, "error: could not write to log in client mode"); }
                            }

                        }

                    }

                    if( logCalled == 1)
                    {
                        dprintf(logfd, "\n");
                    }
                }
                //read and write to server
            }

            if( polls[1].revents & POLLIN )
            {
                bufferSize = read(socketfd, shellBuffer, 256 * sizeof(char));

                if( logCalled == 1)
                {
                    dprintf( logfd, "RECEIVED %d bytes: ", bufferSize );
                }

                if( bufferSize < 0 )
                {
                    fprintf(stderr, "error: %s", strerror(errno));
                    exit(1);
                }
                else if( bufferSize == 0)
                {
                    close(socketfd);
                    exit(0);
                }
                else
                {
                    int i = 0;
                    for( ; i < bufferSize; i++ )
                    {
                        currChar = shellBuffer[i];

                        if( logCalled == 1 )
                        {
                            if( write(logfd, &currChar, sizeof(char)) < 0 )
                            {
                                fprintf(stderr, "error: %s", strerror(errno));
                            }
                        }

                        if( encryptCalled == 1)
                        {
                            if( mdecrypt_generic(decryptFD, &currChar, sizeof(char)) < 0 )
                            {
                                fprintf(stderr, "error: could not decrypt data from shell");
                                exit(1);
                            }
                        }

                        if( write(STDOUT_FILENO, &currChar, sizeof(char)) < 0)
                        {
                            fprintf(stderr, "error: could not write data to terminal");
                            exit(1);
                        }

                    }
                    if( logCalled == 1)
                    {
                        dprintf(logfd, "\n");
                    }

                }

            }

            if (polls[1].revents & (POLLERR | POLLHUP))
            {
                close(socketfd);
                fprintf( stderr, "error occured: shutdown");
                exit(0);
            }

            //also need to do pollhup and pollerr
        }



    }

}


void resetTerminal()
{
    if( tcsetattr(0, TCSANOW, &initialTerminal) < 0)
    {
        fprintf( stderr, "error %s", strerror(errno));
    }

    /*if( shellCalled == 1 )
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

    }*/

    //exit(0);
}

int main(int argc, char* argv[])
{

    int getoptReturnChar = -1; //returns -1 if not found
    int portnum = -777;
    struct sockaddr_in server_address;

    static struct option long_options[] = {
            {"port"   , required_argument, NULL, PORT   },
            {"log"    , required_argument, NULL, LOG    },
            {"encrypt", required_argument, NULL, ENCRYPT},
            {0,       0,                 0,    0}
    };

    while ((getoptReturnChar = getopt_long(argc, argv, "p:e:l:", long_options, NULL)) != -1)
    {
        if ( getoptReturnChar == PORT )
        {
            serverCalled = 1;
            portnum = strtol( optarg, NULL, 10 );
            if( portnum == -777)
            {
                fprintf(stderr, "error: could not find port");
                exit(1);
            }
        }
        else if ( getoptReturnChar == ENCRYPT )
        {
            encryptCalled = 1;
            createEncryption(optarg);
        }
        else if ( getoptReturnChar == LOG )
        {
            logfd = creat( optarg, S_IRWXU );

            if ( logfd < 0 )
            {
                fprintf( stderr, "error: %s ", strerror(errno));
                exit(1);
            }

            logCalled = 1;
        }

    }

    if (setupTerminal() < 0)
    {
        fprintf(stderr, "error: could not set termninal in canonical non echo mode");
        exit(1);
    }
    atexit(resetTerminal);


    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd < 0)
    {
        fprintf(stderr, "error: %s", strerror(errno));
        exit(1);
    }

    struct hostent* server = gethostbyname("localhost");
    if(server == NULL)
    {
        fprintf(stderr, "error: %s", strerror(errno));
        exit(1);
    }

    memset((char*) &server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port  = htons(portnum); //maybe 5150
    bcopy((char*) server->h_addr, (char*) &server_address.sin_addr.s_addr, server->h_length);

    if( connect(socketfd,(struct sockaddr*) &server_address, sizeof(server_address)) < 0 )
    {
        fprintf(stderr, "error: %s", strerror(errno));
        exit(1);
    }

    setupPolling();

    exit(0);
}



