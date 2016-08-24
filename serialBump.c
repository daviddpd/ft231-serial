#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


static struct termios oldterminfo;
char *serialdev = "/dev/cuaU0";
int rts,rtsRequest = 0;
int dtr,dtrRequest = 0;
int le,leRequest = 0;
int statusRequest = 0;



void closeserial(int fd)
{
    tcsetattr(fd, TCSANOW, &oldterminfo);
    if (close(fd) < 0)
        perror("closeserial()");
}


int openserial(char *devicename)
{
    int fd;
    struct termios attr;

    if ((fd = open(devicename, O_RDWR)) == -1) {
        perror("openserial(): open()");
        return 0;
    }
    if (tcgetattr(fd, &oldterminfo) == -1) {
        perror("openserial(): tcgetattr()");
        return 0;
    }
    attr = oldterminfo;
    attr.c_iflag = 0;
    attr.c_oflag = 0;
	attr.c_cflag |= CLOCAL;
    attr.c_cflag = 0;
    attr.c_lflag = 0;
    attr.c_ispeed = B115200;
    attr.c_ospeed = B115200;
    if (tcflush(fd, TCIOFLUSH) == -1) {
        perror("openserial(): tcflush()");
        return 0;
    }
    if (tcsetattr(fd, TCSANOW, &attr) == -1) {
        perror("initserial(): tcsetattr()");
        return 0;
    }
    return fd;
}

/*
	#define TIOCMGET    _IOR('t', 106, int) // get all modem bits 
	#define     TIOCM_LE    0001        // line enable 
	#define     TIOCM_DTR   0002        // data terminal ready 
	#define     TIOCM_RTS   0004        // request to send 
	#define     TIOCM_ST    0010        // secondary transmit 
	#define     TIOCM_SR    0020        // secondary receive 
	#define     TIOCM_CTS   0040        // clear to send 
	#define     TIOCM_DCD   0100        // data carrier detect 
	#define     TIOCM_RI    0200        // ring indicate 
	#define     TIOCM_DSR   0400        // data set ready 
*/


int _status(int fd) 
{
    int status,x,n,i;
    
    char bits[16];
    
	x = ioctl(fd, TIOCMGET, &status);
	
	
    if (x == -1) {
        perror("_status(): TIOCMGET");
    } else {
		printf ( "_status(): TIOCMGET: " );
		n = status;
		for (i=0; i<16; i++) {
			if (n & 1) {
				bits[15-i] = (char)'1';
			} else {
				bits[15-i] = (char)'0';
			}
			n >>= 1;
		}
		
		for (i=0; i<16; i++) {
			if ( ( i% 4 ) == 0 ) { printf (" " ); }
			printf( "%c", bits[i] );
		}
		printf("\n");
    }

	return status;
}


int setRTS(int fd, int level)
{
    int status;

	status = _status(fd);

    if (level)
        status |= TIOCM_RTS;
    else
        status &= ~TIOCM_RTS;

	if (ioctl(fd, TIOCMSET, &status) == -1) {
		perror("ERROR: setRTS(): TIOCMSET");
		return 0;
	}

//	status = _status(fd);

    return 1;
}


int setDTR(int fd, int level)
{
    int status;
	status = _status(fd);

    if (level)
        status |= TIOCM_DTR;
    else
        status &= ~TIOCM_DTR;

	if (ioctl(fd, TIOCMSET, &status) == -1) {
		perror("ERROR: setRTS(): TIOCMSET");
		return 0;
	}

//	status = _status(fd);
 
    return 1;
}

int setLE(int fd, int level)
{
    int status;
	status = _status(fd);

    if (level)
        status |= TIOCM_LE;
    else
        status &= ~TIOCM_LE;

	if (ioctl(fd, TIOCMSET, &status) == -1) {
		perror("ERROR: setLE(): TIOCMSET");
		return 0;
	}

//	status = _status(fd);
 
    return 1;
}



void _usage(char *p) {
	printf ("   -p --port                 serial port\n");
	printf ("   -s --status                 RTS/DTR\n");
	printf ("   -r --rts 0|1                  RTS/DTR\n");
	printf ("   -d --dtr 0|1                  RTS/DTR\n");
	printf ("   -l --le 0|1                  RTS/DTR\n");

	return;
}

void _args(int argc, char**argv) {
	char ch;
	struct option longopts[] = {
		{	"ports",	required_argument,		NULL,	'p'	},
		{	"status",	no_argument,		NULL,	's'	},
		{	"rts",	required_argument,		NULL,	'r'	},
		{	"dtr",	required_argument,		NULL,	'd'	},
		{	"le",	required_argument,		NULL,	'l'	},

		/*  remember a zero line, else 
			getopt_long segfaults with unknown options */
	    {NULL, 			0, 					0, 		0	}
			
	};
	
	while ((ch = getopt_long(argc, argv, "p:sr:d:l:", longopts, NULL)) != -1) {
		switch (ch) {

			case 'p':
				//printf ( "Arg --debug : Value : %s \n", optarg );
				serialdev = optarg;
			break;
			
			case 's':
				statusRequest = 1;
			break;
			case 'r':
				rts = atoi(optarg);
				rtsRequest =1;
			break;
			case 'd':
				dtr = atoi(optarg);
				dtrRequest = 1;
			break;
			case 'l':
				le = atoi(optarg);
				leRequest = 1;
			break;
			
			default:
				_usage(argv[0]);
			break;
		}
	}
	argc -= optind;
	argv += optind;


}


int main(int argc, char**argv)
{
    int fd;
	_args(argc, argv);
	
    fd = openserial(serialdev);
    if (!fd) {
        fprintf(stderr, "Error while initializing %s.\n", serialdev);
        return 1;
    }

	if (statusRequest) { 
		_status(fd);
	};
	
	if (dtrRequest) {
	    setDTR(fd, dtr);
	}
	if (rtsRequest) {
	    setRTS(fd, rts);
	}

	if (leRequest) {
	    setLE(fd, rts);
	}


    closeserial(fd);
    return 0;
}

