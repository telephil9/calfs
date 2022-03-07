#include "a.h"

int debug = 0;
Event **events;
usize nevents;
usize eventsz;

void
fsread(Req *r)
{
	File *f;
	Event *e;

	f = r->fid->file;
	e = f->aux;
	if(strcmp(f->name, "uid") == 0)
		readstr(r, e->uid);
	else if(strcmp(f->name, "summary") == 0)
		readstr(r, e->summary);
	else if(strcmp(f->name, "description") == 0)
		readstr(r, e->description);
	else if(strcmp(f->name, "location") == 0)
		readstr(r, e->location);
	else if(strcmp(f->name, "start") == 0)
		readtm(r, &e->start);
	else if(strcmp(f->name, "end") == 0)
		readtm(r, &e->end);
	else if(strcmp(f->name, "last-modified") == 0)
		readtm(r, &e->lastmod);
	else if(strcmp(f->name, "created") == 0)
		readtm(r, &e->created);
	else{
		respond(r, "no such file or directory");
		return;
	}
	respond(r, nil);
}

Srv fs = {
	.read = fsread,
};

void
createevtfiles(File *p, Event *e)
{
	const char *filenames[] = {
		"uid",
		"summary",
		"description",
		"location",
		"start",
		"end",
		"last-modified",
		"created",
	};
	int i;

	for(i = 0; i < nelem(filenames); i++)
		createfile(p, filenames[i], nil, 0644, e);
}


enum { Cnone, Cyear, Cmonth, Cday };

int
change(Event *e, int y, int m, int d)
{
	if(e->start.year != y)
		return Cyear;
	else if(e->start.mon != m)
		return Cmonth;
	else if(e->start.mday != d)
		return Cday;
	return Cnone;
}

Tree*
buildtree(void)
{
	Tree *tree;
	int i, n, y, m, d;
	File *yd, *md, *dd, *nd;
	Event *e;
	char buf[16] = {0};

	tree = alloctree(nil, nil, DMDIR|0555, nil);
	y = m = d = -1;
	yd = md = dd = nil;
	n = 1;
	for(i = 0; i < nevents; i++){
		e = events[i];
		switch(change(e, y, m, d)){
		case Cyear:
			y = e->start.year;
			snprint(buf, sizeof buf, "%d", y+1900);
			yd = ecreatefile(tree->root, buf, nil, DMDIR|0555, nil);
		case Cmonth:
			m = e->start.mon;
			snprint(buf, sizeof buf, "%02d", m+1);
			md = ecreatefile(yd, buf, nil, DMDIR|0555, nil);
		case Cday:
			d = e->start.mday;
			n = 1;
			snprint(buf, sizeof buf, "%02d", d);
			dd = ecreatefile(md, buf, nil, DMDIR|0555, nil);
		}
		snprint(buf, sizeof buf, "%d", n);
		nd = createfile(dd, buf, nil, DMDIR|0555, nil);
		createevtfiles(nd, e);
		n += 1;
	}
	return tree;
}

void
usage(void)
{
	fprint(2, "usage: %s <filename>...\n", argv0);
	exits("usage");
}

void
main(int argc, char **argv)
{
	char *mtpt;
	int i;

	mtpt = "/mnt/cal";

	ARGBEGIN{
	case 'D':
		chatty9p++;
		break;
	case 'd':
		debug++;
		break;
	case 'm':
		mtpt = ARGF();
		break;
	}ARGEND;

	tmfmtinstall();
	if(argc < 1)
		usage();
	for(i = 0; i < argc; i++){
		if(readical(argv[i]) < 0)
			sysfatal("unable to read ical file '%s': %r", argv[i]);
	}
	qsort(events, nevents, sizeof(Event*), cmpevent);
	fs.tree = buildtree();
	postmountsrv(&fs, nil, mtpt, 0);
	exits(0);
}
