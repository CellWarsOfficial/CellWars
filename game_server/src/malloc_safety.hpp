#ifndef MALLOC_SAFETY_H
#define MALLOC_SAFETY_H

extern bool use_safety; // initialised by init.cpp in main using execution args.

void check_malloc(void*);

#endif
