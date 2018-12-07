#ifndef __DEFS_H__
#define __DEFS_H__

/********************************************* OS Version Check *******************************************************/

#ifdef _WIN32 // Winsock 구 버전 대신 Winsock 2 API 가 사용되는지 확인
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif
#endif

#ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
#include <Windows.h>
#define WINMARK
#ifdef _WIN64
   //define something for Windows (64-bit only)
#include <Windows.h>
#define WINMARK
#endif
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
	 // iOS Simulator
#elif TARGET_OS_IPHONE
	// iOS device
#elif TARGET_OS_MAC
	// Other kinds of Mac OS
#else
#   error "Unknown Apple platform"
#endif
#elif __linux__
	// linux
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#elif __unix__ // all unices not caught above
	// Unix
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#elif defined(_POSIX_VERSION)
	// POSIX
#else
#   error "Unknown compiler"
#endif



/***************************************************  Headers  ********************************************************/
/* Standard library included */
/* I/O basic library */
#include <cstdio>
#include <cstdlib>
#include <iostream>
/* Print real number with some precision */
#include <sstream>
#include <iomanip>
/* String library */
#include <cstring>
#include <string>
/* Console program library */
#include <conio.h>
#include <chrono>
/* Vector and iterator library */
#include <vector>
#include <iterator>
/* File library */
#include <io.h>
#include <fstream>
#include <direct.h>		//mkdir
#include <fcntl.h> // file I/O in low level
/* For the thread */
#include <thread>
#include <condition_variable>
#include <future>
#include <mutex>
/* For the sorting,random generator, simple algorithm */
#include <algorithm>
#include <limits>
#include <random>
/* Exceptional condition */
#include <errno.h>		//errno
#include <exception> // for exception
/* Declare the type of C++ */
#include <typeinfo>
/* Get the time info */
#include <ctime>

/* Deprecated Header*/
/* #include <ios>     // for <streamsize> */
// #include <cstdarg>


/* Not standard but required strongly, library */
#include "Thread.h"

/* Namespace declaration for convenience */
using namespace std;

/************************************************************************************************************************/

/************************************************ Define the keywords ***************************************************/

/* Type of Console UIs */
#define MESSAGE 1
#define CONSOLE 0
#define BUTTON -1
/* Setting of the console's basic information */
#define DEFAULT_CMD_X 1 /* from 0 to 1 */
#define DEFAULT_CMD_Y 1 /* from 0 to 1 */

#define DEFAULT_TEXT_SIZE 21 // text size in console

/* MOUSE possible state */
#define CLICKED 0
#define NOCHANGE 1

/* Maximum buffer size with I/O */
#define BUFSIZE 8192

/* Thread State */
#define RUNNING 1
#define PAUSE 0
#define WAITING -1

/* Type needed for Dir Manager Class */
#define NONE -1
#define DIR 0
#define _FILE 1

/************************************************************************************************************************/


/************************************************* Global Variables  ****************************************************/

/*** Enum Classes ***/
enum class Type : unsigned int {
	DEFAULT_TYPE,
	CHAR,
	UNSIGNED_CHAR,
	SHORT,
	UNSIGNED_SHORT,
	INT,
	UNSIGNED_INT,
	LONG,
	UNSIGNED_LONG,
	LL,
	UNSIGNED_LL,
	FLOAT,
	DOUBLE,
	LONG_DOUBLE,
	WCHAR,
	STRING,
	PDATA
};

enum class Color : unsigned int {
	BLACK,
	BLUE,
	GREEN,
	CYAN,
	RED,
	PURPLE,
	BROWN,
	LIGHTGRAY,
	GRAY,
	LIGHTBLUE,
	LIGHTGREEN,
	LIGHTCYAN,
	LIGHTRED,
	LIGHTPURPLE,
	YELLOW,
	WHITE
};

/*** Global variable for directory ***/
static const wchar_t * Dir_Exclusive[1] = {
	L" "
};

typedef struct _2D_pos {
	short x;
	short y;
	_2D_pos(short x = 0, short y = 0) :x(x), y(y) {};
}_2D_pos;

/* ui button or message data type */
class ui_type {
public:
	std::wstring name; // ui's name
	int width; // Message width
	int height; // Message height
	short pos_x; // start position x
	short pos_y; // start position y
	int idx; // ui's idx number
			 /*
			 0 : console's title
			 positive : button
			 negative : message
			 */
	Color col_back; // background color
	Color col_front; // color
	int type; // type of ui
	bool usage;
	ui_type(std::wstring name = L"UNKNOWN", int width = 7, int height = 1, short pos_x = 1, short pos_y = 1, int type = MESSAGE, Color col_back = Color::BLACK, Color col_front = Color::WHITE, bool usage = FALSE)
		:name(name), width(width), height(height), pos_x(pos_x), pos_y(pos_y), idx(-1), col_back(col_back), col_front(col_front), type(type), usage(usage) {};
	~ui_type() {
		this->name.clear();
	}
};

/************************************************************************************************************************/



/************************************************ External functions  ***************************************************/

/* Must be called first and the end */
extern void Setting();
extern void Ending();
/* Error report function */
extern void Print_Error(std::wstring class_name, std::wstring message, bool exit = TRUE);

/* For the convenience */
extern string To_str(const wstring & wstr);
extern wstring To_wstr(const string & str);
extern void clear_screen();

/************************************************************************************************************************/


#endif
