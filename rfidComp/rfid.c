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
//static void GetCurrentTimestamp(char *buf);

//static void GetCurrentTimestamp(char *buf)
/*{
    time_t     now;
    struct tm  ts;
    

    // Get current time
    time(&now);

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&now);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
    

}*/

COMPONENT_INIT
{
	//char timestamp[80] = {};
	char buf[28] = {0};
	// Atomic write example, File Descriptor case.
	
	time_t     now;
    struct tm  ts;
    

    // Get current time
    time(&now);

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&now);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
	
	//int fd = le_atomFile_TryCreate("./myfile.txt", LE_FLOCK_READ_AND_APPEND, LE_FLOCK_OPEN_IF_EXIST, O_RDWR);
	int fd = open("./myfile.txt", O_CREAT | O_APPEND | O_RDWR);
	//GetCurrentTimestamp(timestamp);
	
	if (fd < 0)
	{
		// Print error message and exit.
		LE_INFO("file could not be opened");
	}
	else{
	 
		// Write something in fd
		char myString[] = " rfid test\n";
		 
		// Now write this string to fd
		write(fd, buf, (sizeof(buf)-1));
		write(fd, myString, (sizeof(myString)-1));    // This string write doesn't go disk
		
		//printf("%s\n", buf);
		 
		le_result_t result = close(fd); // Transfers all changes to disk
		 
		if (result == LE_OK)
		{
			// Print success message
			LE_INFO("Data successfuly written");
		}
	}
	
	exit(0);
}
