#include "bem.h"
#include <em_path.h>

#ifndef NORSCID
static char rcs_id[] = "$Header$" ;
#endif

/* generate temporary files etc */

FILE	*emfile;
FILE	*tmpfile;
FILE	*datfile;

initialize()
{
	sprintf(tmpfname,"%s/abc%d",TMP_DIR,getpid());
	strcpy(datfname,program);
	strcat(datfname,".d");
	yyin= fopen(inpfile,"r");
	emfile= fopen(outfile,"w");
	tmpfile= fopen(tmpfname,"w");
	if( yyin==NULL || emfile== NULL || tmpfile== NULL )
		fatal("Improper file permissions");
	fillkex();	/* initialize symbol table */
	fprintf(emfile,"#\n");
	fprintf(emfile," mes 2,EM_WSIZE,EM_PSIZE\n");
	initdeftype();	/* set default symbol declarers */
}
