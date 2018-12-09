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



//--------------------------------------------------------------------------------------------------
/*
 * rfidTmer runs every 15 seconds when called as rfidTmerRef handler
 */
//--------------------------------------------------------------------------------------------------

static void rfidTimer(le_timer_Ref_t rfidTimerRef)
{
    //char timestamp[80] = {};
	char timestamp[28] = {0};
	// Atomic write example, File Descriptor case.
	
	time_t     now;
    struct tm  ts;
    
    // Get current time
    time(&now);

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
	
	FILE* fd = fopen ("sdcard/log.txt", "a");
	//GetCurrentTimestamp(timestamp);
	
	if (fd == NULL)
	{
		// Print error message and exit.
		LE_INFO("file could not be opened");
	}
	else{
		// Write something in fd
		fprintf(fd, "%s %s", timestamp, " rfid test data\n");
		

		 
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
