#include <log.h>

/* Show warning message*/
void log::warning(const char * message, ...) const
	{
		cout <<"Warning: ";
		va_list ptr; /* extract argument ptr */

		 /*Initialization of ptr, it becomes a pointer to the first argument followed the format string*/
		va_start(ptr, message);

		/* show message */
		vprintf(message, ptr);

        cout <<"\n";
		va_end(ptr);
	}

/* Show error message and assert simulator*/
void log::critical(const char * message, ...) const
	{
		cout <<"Error: ";
		va_list ptr; /* extract argument ptr */

		 /*Initialization of ptr, it becomes a pointer to the first argument followed the format string*/
		va_start(ptr, message);

		/* show message */
		vprintf(message, ptr);
		cout <<"\n";
		va_end(ptr);
		assert(0);
	}