#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
//segfault subroutine
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

void segFault()
{
    char* p = NULL;
    *p = 'a';
}

void sighandler()
{
    fprintf(stderr, "caught "); //stderr and whatever else spec requires
    exit(4);
}

enum optionsVal
{
    INPUT  = 'i',
    OUTPUT = 'o',
    SFAULT = 's',
    CATCH  = 'c'
};

int main(int argc, char* argv[])
{
    int arg = 0; //why 0 could also be undef
    int input    = -1;
    int output   = -1;
    int segfault = -1;
    int catch    = -1;
    int ifd      = -1;
    int ofd      = -1;
//must define options first

    static struct option long_options[] = {
        {"input"   , required_argument, NULL, INPUT },
        {"output"  , required_argument, NULL, OUTPUT},
        {"segfault", no_argument      , NULL, SFAULT},
        {"catch"   , no_argument      , NULL, CATCH },
        {0         , 0                , 0   ,   0   }
    };

//long_options is the name of the struct

    while ((arg = getopt_long(argc, argv, "", long_options, NULL)) != -1 )
    {
        switch(arg){

            case INPUT:
	        input = 0;
                ifd = open(optarg, O_RDONLY);
                //handle errors
                break;

            case OUTPUT:
	        output = 0;
                ofd = creat(optarg, 0666);
                //handle errors
                break;

            case SFAULT:
                segfault = 0;
                break;

            case CATCH:
                catch = 0;
                break;

            default:
                fprintf(stderr, "Not one of the specified options");
		exit(1);
                break;
                //print error message because unknown options was called
        }
    }

////////////////////  actions that need to be taken based off of args  //////////////////////////

////////////////////  segfault handler  /////////////////////////////////////////////////////////
    if( catch == 0 )
    {
        signal(SIGSEGV, sighandler);
    }

//////////////////////segfault//////////////////////////////////////////////////////////////////

    if( segfault == 0 )
    {
        segFault();
        //remember error messages
    }

////////////////////  input  ///////////////////////////////////////////////////////////////////
    if( input == 0 ){
        if(ifd >= 0){
          //should close fd0 automatically if open
          close(0);
	  dup(ifd);
	  close(ifd);
        }
	else{
	  fprintf(stderr, "Error: %s, Could not open the requested input file.\n", strerror(errno));
	  exit(2);
	}
    }



////////////////////  output  /////////////////////////////////////////////////////////////////

    if( output == 0 ){
        if(ofd >= 0){
	  close(1);
	  dup(ofd);
	  close(ofd);
        }
	else{
	  fprintf(stderr, "Error: %s, Could not open the requested output file.\n", strerror(errno));
	  exit(3);
	}
    }

////////////  stdin to stdout  //////////////////////////////////////////////////////////////

    char character;

    while(read( 0, &character, sizeof(char))>0) //returns 0 if nothing was read, after every succesful read the file position is advanced
    {
        if(write( 1, &character, sizeof(char)) < 0)//if fewer than 1 byte is returned it is an error
        {
            //write out error
        }
    }

    close(0);
    //close(ofd);
    exit(0);

//argc[0] is the command name
//use read(2) to read until EOF
//write using write(2) to file desciptor 1
//if no errors other than EOF are encountered then exit(0)


//Optional commands:



//--input --> use specified file as input --> make it the new fd0, if cannot open specified input the report failure to stderr(file descriptor 2)

//by using printf with return code 2 then exit(2)



//--output --> create the specified file and use it as std output --> make it new fd1 --> this is pipe std for output

//if unable to make file the report failure through stderr by using printf with return code 3 then exit(2)



//--segfault --> LOL force segfault, set char* to NULL and store pointer --> do this first do not copy from stdin to stdout



//--catch -->use signal(2) to register SIGSEV handler to catch segfault, log error message on stderr, file descriptor 2 and exit(2) with return code 4

}
