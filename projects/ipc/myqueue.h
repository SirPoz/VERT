#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY 420257999 /* unique key e.g., matriculation number */
#define PERM 0660 /* starts with 0 to declare it as octal */

#define MAX_DATA 255

typedef struct
{
	long mType;
	char mText[MAX_DATA];
} message_t;
