#include "a.h"

void
addevent(Event *e)
{
	if(events == nil){
		eventsz = 128;
		nevents = 0;
		events = emalloc(128 * sizeof(Event*));
	}else if(nevents == eventsz){
		eventsz *= 2;
		events = erealloc(events, eventsz * sizeof(Event*));
	}
	events[nevents++] = e;
}

int
startswith(char *s, char *t)
{
	return strncmp(s, t, strlen(t)) == 0;
}

Tm
parsedate(char *s)
{
	Tm tm, *res;

	res = tmparse(&tm, "YYYYMMDDThhmmss", s, nil, nil);
	if(res == nil)
		fprint(2, "unable to parse date '%s'\n", s);
	return tm;
}

Event*
readevent(Biobuf *bp)
{
	Event *e;
	char *s;
	int done;
	
	e = emalloc(sizeof *e);
	done = 0;
	while(!done){
		s = Brdstr(bp, '\n', 1);
		if(s == nil){
			free(e);
			werrstr("unexpected end of file while parsing event");
			return nil;
		}
		if(startswith(s, "END:VEVENT"))
			done = 1;
		else if(startswith(s, "UID"))
			e->uid = strdup(s+4);
		else if(startswith(s, "SUMMARY"))
			e->summary = strdup(s+8);
		else if(startswith(s, "DESCRIPTION"))
			e->description = strdup(s+13);
		else if(startswith(s, "LOCATION"))
			e->location = strdup(s+9);
		else if(startswith(s, "DTSTART"))
			e->start = parsedate(s+8);
		else if(startswith(s, "DTEND"))
			e->end = parsedate(s+6);
		else if(startswith(s, "LAST-MODIFIED"))
			e->lastmod = parsedate(s+14);
		else if(startswith(s, "CREATED"))
			e->created = parsedate(s+8);
		free(s);
	}
	return e;
}

int
readical(const char *f)
{
	Biobuf *bp;
	char *s;
	Event *e;

	bp = Bopen(f, OREAD);
	if(bp == nil)
		return -1;
	for(;;){
		s = Brdstr(bp, '\n', 1);
		if(s == nil)
			break;
		if(strncmp(s, "BEGIN:VEVENT", 12) == 0){
			e = readevent(bp);
			free(s);
			if(e != nil)
				addevent(e);
			else
				fprint(2, "unable to parse event: %r\n");
		}
	}
	Bterm(bp);
	return 0;
}

