#pragma once

#include <sys/errno.h>

#define RETERR(val, err)	{ errno = (err); return(val); }
