/**
 * log.h - Header of log class 
 * includes 2 methods to show warrnings and errors
 * @author Stas Elizarov
 * Copyright 2010 MDSP team
 */

#ifndef	LOG_H
#define LOG_H

#include <iostream>
#include <cassert>
#include <stdarg.h>
#include <cstdio>

using namespace std;

class log
{
public:
	/* Method to show warning message*/
	void warning(const char * message, ...) const;

	/* Method to show error and assert simulator*/
	void critical(const char * message, ...) const;
};


#endif /* LOG_H */
