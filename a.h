#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include <bio.h>

typedef struct Event Event;

struct Event
{
	char *uid;
	char *summary;
	char *description;
	char *location;
	Tm start;
	Tm end;
	Tm lastmod;
	Tm created;
};

/* ical */
int readical(const char*);

/* utils */
void* emalloc(ulong);
void* erealloc(void*, ulong);
File* ecreatefile(File*, char*, char*, ulong, void*);
void  readtm(Req*, Tm*);
int   cmpevent(void*, void*);

extern Event **events;
extern usize nevents;
extern usize eventsz;

