/*
 * rfidTemp 
 * logs a timestamp with dummy data onto log.txt every 15 seconds
 *
 * Challes Product Development
 * 18/12/09
 */
 
#include "legato.h"
#include "interfaces.h"
#include "le_tty.h"
#include "string.h"
#include "unistd.h"
#include "stdio.h"
#include "time.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RFID_SAMPLE_INTERVAL_IN_MILLISECONDS (15000)

int static open_port(void);
void getTimestamp(char *timestamp);
void rfidWrite(int port, char *command, char size);
void rfidRead(int port, char *buffer, int size);
static void rfidTimer(le_timer_Ref_t rfidTimerRef);

/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */

int static open_port(void)
{
	int port; /* File descriptor for the port */

	port = le_tty_Open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
	if (port == -1)
	{
		/*
		* Could not open the port.
		*/
		LE_INFO("open_port: Unable to open /dev/ttyUSB0 - ");
	}
	else
	{
   //Set Serial Communication parameters

     le_tty_SetBaudRate(port, LE_TTY_SPEED_9600);
     le_tty_SetRaw(port, 2, 2000);
     char *parity = "N";
     le_tty_SetFraming(port, *parity, 8, 1);
	}
  return (port);
}

//--------------------------------------------------------------------------------------------------
/*
 *  getTimestamp returns string of timestamp
*/
//--------------------------------------------------------------------------------------------------

void getTimestamp(char *timestamp)
{
	time_t     now;
    struct tm  ts;
    
    // Get current time
    time(&now);
    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&now);
    strftime(timestamp, 28, "%a %Y-%m-%d %H:%M:%S %Z", &ts);
} 

//--------------------------------------------------------------------------------------------------
/*
 *  rfidWrite writes command to device (/dev/ttyUSB0) and prints status to LE_INFO
*/
//--------------------------------------------------------------------------------------------------

void rfidWrite(int port, char *command, char size)
{
	char output[64];
	snprintf(output,sizeof(output),"%s\r\n", command);
	
	int n = write(port, output, (size+2));
	
	if (n < 0)
	{
		LE_INFO("write() of 5 bytes failed!");
	}
	LE_INFO("data bytes written: %d", n);
}




//--------------------------------------------------------------------------------------------------
/*
 *  rfidRead: reads (size) bytes from device (/dev/ttyUSB0), stores to (buffer),
 *            and prints status to LE_INFO
*/
//--------------------------------------------------------------------------------------------------

void rfidRead(int port, char *buffer, int size)
{
	le_tty_SetRaw(port, size, 200);
	memset(&buffer[0], 0, 64);
	int n = read(port, buffer, size);
	if (n < 0)
	{
		LE_INFO("read of tty/USB0 failed");
	}
	else
	{
		LE_INFO("read of tty/USB0 successful");
	}
	buffer[size-1] = 0;
	
}



//--------------------------------------------------------------------------------------------------
/*
 *  rfidTmer runs every 15 seconds when called as rfidTmerRef handler
*/
//--------------------------------------------------------------------------------------------------

static void rfidTimer(le_timer_Ref_t rfidTimerRef)
{
	char temperature[2] = {0};
	//rfidRead data buffer array
    char buffer[64] = {0};
    
	//get Timestamp
	char timestamp[28] = {0};
	getTimestamp(timestamp);
	
	//open port to rfid
	int port = open_port();
	
	//write command to rfid (turn rfid ON)
	rfidWrite(port, "sra", 3);
	
	
	
	//check for OK
	rfidRead(port, buffer, 3);
	
	if((strcmp(buffer, "OK"))==0)
	{
		LE_INFO("rfid: OK");
	}
	else
	{
		LE_INFO("Strcmp: %d", strcmp(buffer, "OK"));
		LE_INFO("buffer: %s", buffer);
	}
	
	
	//check for Tag Match
	rfidRead(port, buffer, 17);
	if((strcmp(buffer, "985_141001083940"))==0)
	{
		LE_INFO("rfid: 985_141001083940");
	}
	else
	{
		LE_INFO("Strcmp: %d", strcmp(buffer, "985_141001083940"));
		LE_INFO("buffer: %s", buffer);
	}
	
	//write command to rfid (Read Animal Tag)
	rfidWrite(port, "rat", 3);
	//expecting 985_141001083940_1_0_81C3_000XXX
	rfidRead(port, buffer, 33);
	LE_INFO("buffer: %s", buffer);
	if (buffer[0] == '9' && buffer[1] == '8' && buffer[2] == '5')
	{
		temperature[0] = buffer [30];
		temperature[1] = buffer [31];
	}
	
	
	rfidWrite(port, "srd", 3);
	
	//check for OK
	rfidRead(port, buffer, 3);
	
	if((strcmp(buffer, "OK"))==0)
	{
		LE_INFO("rfid: OK");
	}
	else
	{
		LE_INFO("Strcmp: %d", strcmp(buffer, "OK"));
		LE_INFO("buffer: %s", buffer);
	}
	
	le_tty_Close(port);
	
	//GetCurrentTimestamp(timestamp);	
	FILE* fd = fopen ("sdcard/log.txt", "a");

	if (fd == NULL)
	{
		// Print error message and exit.
		LE_INFO("file could not be opened");
	}
	else{
		// Write something in fd
		fprintf(fd, "%s %s%c%c\n", timestamp, "tempdata = ", temperature[0], temperature[1]);
		

		 
		// Now write this string to fd
		if (fclose(fd) == 0)
		{
			// Print success message
			LE_INFO("Data successfuly written");
		}
		else
		{
			LE_INFO("Error closing file");
		}
	}
}

//--------------------------------------------------------------------------------------------------
/*
 * Main program starts here
 */
//--------------------------------------------------------------------------------------------------

COMPONENT_INIT
{
	LE_INFO("rfidTemp application has started");
	
	le_timer_Ref_t rfidTimerRef = le_timer_Create("rfid Timer");
    le_timer_SetMsInterval(rfidTimerRef, RFID_SAMPLE_INTERVAL_IN_MILLISECONDS);
    le_timer_SetRepeat(rfidTimerRef, 0);

    le_timer_SetHandler(rfidTimerRef,rfidTimer);
    le_timer_Start(rfidTimerRef);
	
}
