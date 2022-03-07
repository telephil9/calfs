#include "a.h"

void*
emalloc(ulong size)
{
	void *p;

	p = malloc(size);
	if(p == nil)
		sysfatal("malloc: %r");
	return p;
}

void*
erealloc(void *p, ulong size)
{
	void *q;

	q = realloc(p, size);
	if(q == nil)
		sysfatal("realloc: %r");
	return q;
}

File*
ecreatefile(File *dir, char *name, char *uid, ulong mode, void *aux)
{
	File *f;

	f = createfile(dir, name, uid, mode, aux);
	if(f == nil)
		sysfatal("createfile: %r");
	return f;
}

void
readtm(Req *r, Tm *tm)
{
	char buf[64] = {0};

	snprint(buf, sizeof buf, "%τ", tmfmt(tm, nil));
	readstr(r, buf);
}

int
cmpevent(void *a, void *b)
{
	Event **e0, **e1;
	vlong t0, t1;

	e0 = a;
	e1 = b;
	t0 = tmnorm(&(*e0)->start);
	t1 = tmnorm(&(*e1)->start);
	return t0 - t1;
}

