/*	esize:		prints info from e.out header
*/

#include <stdlib.h>
#include <stdio.h>

#ifndef	MAGIC
#define	MAGIC		07255
#endif	/* MAGIC */

FILE *load_fp;
int eof;

/*	Much of the code has been borrowed from the EM interpreter
*/

typedef	/* unsigned */ long ptr;	/* pointer to EM adress */

long magic;
long flags;
long uref;
long version;
long wsize;
long psize;
long int7;
long int8;
long ntext;
long ndata;
long nproc;
long entrypoint;
long nline;
long szdata;
long ptr7;
long ptr8;

void esize(char *fname);
void rd_close();

int main(int argc, char *argv[])
{
	printf("TPFCRE uref vers  w/p   text  nproc  szdata\n");
	
	if (argc == 1)	{
		esize("e.out");
	}
	else	{
		while (argc > 1)	{
			esize(argv[1]);
			argc--, argv++;
		}
	}
	exit(0);
	return 0;
}

void esize(char *fname)
{
	eof = 0;
	if (!rd_open(fname))	{
		printf("%s: cannot open\n", fname);
	}
	else	{
		if (!rd_header())	{
			printf("%s: not EM object format\n", fname);
		}
		else	{
			printf("%c", flags&0001 ? 'T' : '-');
			printf("%c", flags&0002 ? 'P' : '-');
			printf("%c", flags&0004 ? 'F' : '-');
			printf("%c", flags&0010 ? 'C' : '-');
			printf("%c", flags&0020 ? 'R' : '-');
			printf("%c", flags&0040 ? 'E' : '-');
			printf("%c", flags&0100 ? '?' : ' ');
			printf("%c", flags&0200 ? '?' : ' ');
			
			printf("%3ld  ", uref);
			printf("%3ld  ", version);
			printf("%1ld/%1ld", wsize, psize);
			printf("%c", int7 ? '?' : ' ');
			printf("%c", int8 ? '?' : ' ');
				
			printf("%5ld  ", ntext);
			printf("%5ld  ", nproc);
			printf("%6ld", szdata);
			printf("%c", ptr7 ? '?' : ' ');
			printf("%c", ptr8 ? '?' : ' ');
			printf("%s\n", fname);
		}
		rd_close();
	}
}

#define	btol(a)		((long)(((long) (a)) & 0xFF))

int rd_open(char *load_file)
{
	return (load_fp = fopen(load_file, "r")) != NULL;
}

int rd_byte()
{
	int i;

	if ((i = fgetc(load_fp)) == EOF)
		eof = 1;
	return (i);
}

long rd_int(long n)
{
	long l;
	int i;

	l = btol(rd_byte());
	for (i = 1; i < n; i++)
		l = l | (btol(rd_byte()) << (long) (i*8));
	return (l);
}

#define	rd_ptr()	((ptr) rd_int(psize))

int rd_header()
{
	magic = rd_int(2L);
	if (magic != MAGIC || eof)
		return 0;

	flags = rd_int(2L);
	uref = rd_int(2L);
	version = rd_int(2L);
	wsize = rd_int(2L);
	psize = rd_int(2L);
	int7 = rd_int(2L);	/* Entry 7 is unused */
	int8 = rd_int(2L);	/* Entry 8 is unused */

	ntext = rd_ptr();
	ndata = rd_ptr();
	nproc = rd_ptr();
	entrypoint = rd_ptr();
	nline = rd_ptr();
	szdata = rd_ptr();
	ptr7 = rd_ptr();	/* entry 7 is unused */
	ptr8 = rd_ptr();	/* entry 8 is unused */
	
	return !eof;
}

void rd_close()
{
	fclose(load_fp);
}

