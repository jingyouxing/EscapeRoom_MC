#include "head.h"

int get_json_id(unsigned char *id)
{
	struct timeval timeSeed;
	gettimeofday(&timeSeed, NULL);
	srand(timeSeed.tv_sec * 1000000 + timeSeed.tv_usec);  // milli time
	//srand(time(NULL));
	sprintf(id, "0x%08x", rand());
	//printf(" id: %s\n", id); 
	return 0;
}