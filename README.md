# calfs
A 9p calendar filesystem. 
calfs reads ical files and exposes calendar events with the following structure:
```
<mtpt>
|- <year>
  |- <month>
    |- <day>
	  |- <n>
	    |- uid
		|- summary
		|- description
		|- location
		|- start
		|- end
		|- created
		|- last-modifed
```

*NB:* this is work in progress, the ical parser is just a quick hack and will surely break.

## Usage
```sh
% mk install
% calfs <file.ics>...
```

## License
MIT

## Bugs
Guaranteed.

