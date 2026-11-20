/*
 *  ExtraVersion - prints the extra version string ($EXTVER:) of a file.
 *
 *  A $EXTVER: string holds a program's original, non-Amiga version (for
 *  example "0.3-Alpha_3") next to its normal Amiga $VER: string:
 *
 *      "\0$EXTVER: Program_Name ExtraVersionString (dd.mm.yyyy) Comment\0"
 *
 *  Written by Marko K. Seppänen for OpenAmiga.org (2014-2018).
 *  1.3 (2026): builds with GCC 15, brings back VER=VERSTAG and VERIFY
 *  of 1.2.0b (whose source was never committed) and reads tags the way
 *  the Version command reads $VER: strings. See docs/ExtraVersion.doc.
 *
 *  The same file builds for Linux (make host): the tests in tests/ run
 *  against both builds.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __amigaos4__
#include <dos/dos.h>
#include <proto/dos.h>
#include <proto/exec.h>
#else
#include <errno.h>
#define RETURN_OK	0
#define RETURN_WARN	5
#define RETURN_ERROR	10
#define RETURN_FAIL	20
#endif

#define APP_NAME	"ExtraVersion"
#define APP_VERSION	"1.3"
#define APP_DATE	"20.11.2026"
#define APP_YEARS	"2014-2026"
#define APP_AUTHOR	"Marko K. Seppänen / OpenAmiga.org"

/* Both strings stay in the binary (used), so Version and ExtraVersion
   find them. */
static const char ver_tag[] __attribute__((used))
	= "\0$VER: " APP_NAME " " APP_VERSION " (" APP_DATE ")";
static const char extver_tag[] __attribute__((used))
	= "\0$EXTVER: " APP_NAME " " APP_VERSION " (" APP_DATE ")";

static const char argTemplate[] = "NAME,FULL/S,HELP/S,VSTRING/S,VER=VERSTAG/S,VERIFY/S";

enum { ARG_NAME, ARG_FULL, ARG_HELP, ARG_VSTRING, ARG_VERSTAG, ARG_VERIFY, ARG_COUNT };

/* The longest tag read; anything longer is not a version string. */
#define TAG_MAX		512
/* "(dd.mm.yyyy)" with room for other date styles */
#define DATE_MAX	32

struct extver {
	char found[TAG_MAX + 16];	/* key + tag as found, trailing blanks removed */
	char name[TAG_MAX];
	char version[TAG_MAX];
	char date[TAG_MAX];
	char comment[TAG_MAX];
};

static struct extver ev;

static int is_blank(int c)
{
	return c == ' ' || c == '\t';
}

static void copy_part(char *dst, const char *start, const char *end)
{
	size_t n = (size_t)(end - start);
	memcpy(dst, start, n);
	dst[n] = '\0';
}

/*  Splits a tag body (the text after "$EXTVER:" or "$VER:") into name,
	version, date and comment:

		Program_Name ExtraVersionString (dd.mm.yyyy) Comment

	The name is the first word; the version is everything from there to
	the '(' of the date; the date is required; the comment is optional.
	Returns 1 for a valid tag, 0 if the body is not a version string
	(an empty "$EXTVER:" search key in a binary, for example).
 */
static int parse_tag(const char *body, struct extver *e)
{
	const char *p = body, *q, *end;

	e->name[0] = e->version[0] = e->date[0] = e->comment[0] = '\0';

	while (is_blank(*p)) p++;
	for (q = p; *q && !is_blank(*q) && *q != '('; q++) ;
	if (q == p) return 0;			/* no name */
	copy_part(e->name, p, q);

	for (p = q; is_blank(*p); p++) ;
	q = strchr(p, '(');
	if (q == NULL) return 0;		/* no date */
	for (end = q; end > p && is_blank(end[-1]); end--) ;
	if (end == p) return 0;			/* no version */
	copy_part(e->version, p, end);

	p = q + 1;
	q = strchr(p, ')');
	if (q == NULL || q - p > DATE_MAX) return 0;
	copy_part(e->date, p, q);

	for (p = q + 1; is_blank(*p); p++) ;
	for (end = p + strlen(p); end > p && is_blank(end[-1]); end--) ;
	copy_part(e->comment, p, end);
	return 1;
}

/* The file is read in blocks of READ_SIZE bytes (static: AmigaOS command
   stacks are small); a byte at a time through DOS is ten times slower. */
#define READ_SIZE	65536
#define EV_EOF		(-1)

#ifdef __amigaos4__
typedef BPTR ev_file;
#else
typedef FILE *ev_file;
#endif

static unsigned char rbuf[READ_SIZE];

struct reader {
	ev_file f;
	size_t pos, len;
	int stop;		/* 1 end of file, 2 Ctrl-C, 3 read error */
};

static int rd_fill(struct reader *r)
{
#ifdef __amigaos4__
	int32 n;
	if (IExec->SetSignal(0, SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C) {
		r->stop = 2;
		return EV_EOF;
	}
	n = IDOS->Read(r->f, rbuf, READ_SIZE);
	if (n < 0) {
		r->stop = 3;
		return EV_EOF;
	}
#else
	size_t n = fread(rbuf, 1, READ_SIZE, r->f);
	if (n == 0 && ferror(r->f)) {
		r->stop = 3;
		return EV_EOF;
	}
#endif
	if (n == 0) {
		r->stop = 1;
		return EV_EOF;
	}
	r->len = (size_t)n;
	r->pos = 0;
	return 0;
}

static int rd_getc(struct reader *r)
{
	if (r->pos == r->len && (r->stop || rd_fill(r) == EV_EOF))
		return EV_EOF;
	return rbuf[r->pos++];
}

/* only right after an rd_getc() that returned a byte */
static void rd_ungetc(struct reader *r)
{
	r->pos--;
}

/*  Searches the file for the first valid tag after key ("$EXTVER:" or
	"$VER:", case sensitive like Version). A tag ends at a NUL, a line
	end or the end of the file. Tags that don't parse (such as the bare
	search key inside another program) are skipped and the search goes
	on. Returns 1 found, 0 not found, -1 stopped by Ctrl-C, -2 read
	error.
 */
static int scan(ev_file f, const char *key, struct extver *e)
{
	struct reader rd = { f, 0, 0, 0 };
	char body[TAG_MAX];
	size_t klen = strlen(key), m = 0, n, k;
	int c;

	while ((c = rd_getc(&rd)) != EV_EOF) {
		if (c != key[m]) {
			m = (c == key[0]);
			continue;
		}
		if (++m < klen) continue;
		m = 0;

		/* the key is there: read the rest of the tag */
		n = 0;
		while ((c = rd_getc(&rd)) != EV_EOF && c != '\0' && c != '\n' && c != '\r') {
			if (n == sizeof(body) - 1) {
				/* too long: not a tag; look for the key again from here */
				rd_ungetc(&rd);
				break;
			}
			body[n++] = (char)c;
		}
		body[n] = '\0';

		if (parse_tag(body, e)) {
			for (k = n; k > 0 && is_blank(body[k - 1]); k--) ;
			body[k] = '\0';
			snprintf(e->found, sizeof(e->found), "%s%s", key, body);
			return 1;
		}
		if (c == EV_EOF) break;
	}
	return rd.stop == 2 ? -1 : rd.stop == 3 ? -2 : 0;
}

/* Returns 1 found, 0 not found, -1 error (printed), -2 Ctrl-C. */
static int scan_file(const char *file, const char *key, struct extver *e)
{
	int r = 0;
#ifdef __amigaos4__
	char msg[100];
	int32 err = 0;
	BPTR f = IDOS->Open(file, MODE_OLDFILE);
	if (f == 0) {
		err = IDOS->IoErr();
		if (err == 0) err = ERROR_OBJECT_NOT_FOUND;
	} else {
		r = scan(f, key, e);
		if (r == -2) err = IDOS->IoErr();
		IDOS->Close(f);
	}
	if (err != 0) {
		IDOS->Fault(err, NULL, msg, sizeof(msg));
		printf("%s: %s: %s\n", APP_NAME, msg, file);
		return -1;
	}
#else
	FILE *f = fopen(file, "rb");
	if (f == NULL) {
		printf("%s: %s: %s\n", APP_NAME, strerror(errno), file);
		return -1;
	}
	r = scan(f, key, e);
	if (r == -2) {
		int err = errno;
		fclose(f);
		printf("%s: %s: %s\n", APP_NAME, strerror(err), file);
		return -1;
	}
	fclose(f);
#endif
	return r == -1 ? -2 : r;
}

static void print_help(void)
{
	/* "$" comes in as an argument: the binary must not hold a second
	   "$EXTVER: ..." string that could be taken for its own */
	const char *d = "$";

	printf("ExtraVersion %s (%s) (C) %s %s\n\n", APP_VERSION, APP_DATE, APP_YEARS, APP_AUTHOR);
	printf("   NAME\n");
	printf("    %s - Finds the extra version string (non-Amiga version).\n\n", APP_NAME);
	printf("   FORMAT\n");
	printf("    %s [<file>] [FULL] [VSTRING] [VER=VERSTAG] [VERIFY] [HELP]\n\n", APP_NAME);
	printf("   TEMPLATE\n");
	printf("    %s\n\n", argTemplate);
	printf("   FUNCTION\n");
	printf("    %s finds the %sEXTVER: string of a file: the version of the\n", APP_NAME, d);
	printf("    original (non-Amiga) software, stored next to the %sVER: string.\n", d);
	printf("    By default it prints the name and the extra version. Without a\n");
	printf("    file it prints its own.\n\n");
	printf("    FULL     also prints the date and the comment.\n");
	printf("    VSTRING  prints the whole string, starting with %sEXTVER:; an\n", d);
	printf("             empty line if the file has none (for scripts).\n");
	printf("    VER      reads the %sVER: string instead.\n", d);
	printf("    VERIFY   prints the string as found and each part of it.\n\n");
	printf("   STRING\n");
	printf("    \"\\0%sEXTVER: Program_Name ExtraVersionString (dd.mm.yyyy) Comment\\0\"\n", d);
}

/*  Does the work for both builds. name NULL = ExtraVersion's own
	strings. Returns the shell return code.
 */
static int run(const char *name, int full, int vstring, int verstag, int verify)
{
	char key[10];
	int r;

	/* built here, so the binary holds no bare "$EXTVER:" string */
	key[0] = '$';
	strcpy(key + 1, verstag ? "VER:" : "EXTVER:");

	if (name != NULL) {
		r = scan_file(name, key, &ev);
		if (r == -1) return RETURN_FAIL;
		if (r == -2) {
			printf("***Break\n");
			return RETURN_WARN;
		}
	} else {
		const char *own = verstag ? ver_tag + 1 : extver_tag + 1;
		r = parse_tag(own + strlen(key), &ev);
		snprintf(ev.found, sizeof(ev.found), "%s", own);
	}

	if (r == 0) {
		if (vstring && !full && !verify) {
			printf("\n");
			return RETURN_OK;
		}
		printf("%s: Could not find %sversion information for \"%s\".\n",
		       APP_NAME, verstag ? "" : "extra ", name);
		return RETURN_FAIL;
	}

	if (!full && !vstring && !verify)
		printf("%s %s\n", ev.name, ev.version);
	if (full) {
		printf("%s %s (%s)\n", ev.name, ev.version, ev.date);
		if (ev.comment[0]) printf("%s\n", ev.comment);
	}
	if (vstring) {
		printf("%s %s %s (%s)%s%s\n", key, ev.name, ev.version, ev.date,
		       ev.comment[0] ? " " : "", ev.comment);
	}
	if (verify) {
		printf("%s found = \"%s\"\n", key, ev.found);
		printf("Name = \"%s\"\n", ev.name);
		printf("Version = \"%s\"\n", ev.version);
		printf("Date = \"%s\"\n", ev.date);
		printf("Comment = \"%s\"\n", ev.comment);
	}
	return RETURN_OK;
}

#ifdef __amigaos4__

int main(void)
{
	int32 args[ARG_COUNT] = { 0 };
	struct RDArgs *rda;
	int rc;

	rda = IDOS->ReadArgs(argTemplate, args, NULL);
	if (rda == NULL) {
		IDOS->PrintFault(IDOS->IoErr(), APP_NAME);
		return RETURN_ERROR;
	}
	if (args[ARG_HELP]) {
		print_help();
		rc = RETURN_OK;
	} else {
		rc = run((const char *)args[ARG_NAME], args[ARG_FULL] != 0, args[ARG_VSTRING] != 0,
			 args[ARG_VERSTAG] != 0, args[ARG_VERIFY] != 0);
	}
	IDOS->FreeArgs(rda);
	return rc;
}

#else

#include <strings.h>

/* Linux build for the tests: the same keywords, no ReadArgs. */
int main(int argc, char **argv)
{
	const char *name = NULL;
	int full = 0, help = 0, vstring = 0, verstag = 0, verify = 0, i;

	for (i = 1; i < argc; i++) {
		const char *a = argv[i];
		if (!strcasecmp(a, "FULL")) full = 1;
		else if (!strcasecmp(a, "HELP")) help = 1;
		else if (!strcasecmp(a, "VSTRING")) vstring = 1;
		else if (!strcasecmp(a, "VER") || !strcasecmp(a, "VERSTAG")) verstag = 1;
		else if (!strcasecmp(a, "VERIFY")) verify = 1;
		else if (name == NULL) name = a;
		else {
			printf("%s: wrong number of arguments\n", APP_NAME);
			return RETURN_ERROR;
		}
	}
	if (help) {
		print_help();
		return RETURN_OK;
	}
	return run(name, full, vstring, verstag, verify);
}

#endif
