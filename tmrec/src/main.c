/**
 * $Id: main.c,v 1.2 2004-03-31 09:58:31 dcm Exp $
 *
 * Copyright (C) 2003-2005 Fhg Fokus
 *
 * This file is part of tmrec.
 *
 * tmrec is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * For a license to use the tmrec software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact the author of the project.
 *
 * tmrec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * History:
 * --------
 *  - first version, by Daniel
 */

/*** ***/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>

#include "tmrec.h"

/* --- static variables --- */
static char id[]="@(#) $Id: main.c,v 1.2 2004-03-31 09:58:31 dcm Exp $";
static char version[]=  NAME " " VERSION " (" ARCH "/" OS ")" ;
static char compiled[]= __TIME__ " " __DATE__ ;

static char help_msg[]= "\
Usage: " NAME " [options]\n\
Options:\n\
    --rfile file\n\
     -f file          File with recurence definitions\n\
    --stime time\n\
     -s time          Time to check, given as timestamp\n\
    --time time\n\
     -t time          Time to check, given as RFC 2445 DATE-TIME\n\
    --dtstart <val>   Start of interval (RFC 2445 DATE-TIME)\n\
    --dtend <val>     End of interval (RFC 2445 DATE-TIME)\n\
    --duration <val>  Length of interval (RFC 2445 DURATION)\n\
    --until <val>     Bound of recurrence (RFC 2445 DATE-TIME)\n\
    --freq <val>      Frequency of recurrence (\"daily\",\"weekly\", \n\
                      \"monthly\", or \"yearly\")\n\
    --interval <val>  How often the recurrence repeats\n\
    --byday <val>     List of days of the week\n\
    --bymday <val>    List of days of the month\n\
    --byyday <val>    List of days of the year\n\
    --bymonth <val>   List of months of the year\n\
    --byweekno <val>  List of weeks of the year\n\
    --version\n\
     -v               Version number\n\
    --help\n\
     -h               This help message\n\n"
;

static struct option long_options[] = {
	{"duration",  1, 0, 'D'},
	{"byday",     1, 0, 'd'},
	{"dtend",     1, 0, 'E'},
	{"freq",      1, 0, 'F'},
	{"rfile",     1, 0, 'f'},
	{"help",      0, 0, 'h'},
	{"interval",  1, 0, 'I'},
	{"bymday",    1, 0, 'M'},
	{"bymonth",   1, 0, 'm'},
	{"dtstart",   1, 0, 'S'},
	{"stime",     1, 0, 's'},
	{"time",      1, 0, 't'},
	{"until",     1, 0, 'U'},
	{"version",   0, 0, 'v'},
	{"byyday",    1, 0, 'Y'},
	{"byweekno",  1, 0, 'W'},
	{"wkst",      1, 0, 'w'},
	{0, 0, 0, 0}
};


int check_file(char *fname, ac_tm_p _atp);

int main(int argc, char **argv)
{
	int c;
	int option_index = 0;
	char fname[128];
	ac_tm_t _at;
	tmrec_t _tr;
	tr_res_t _rt;

	memset(&_at, 0, sizeof(ac_tm_t));
	memset(&_tr, 0, sizeof(tmrec_t));
	fname[0] = '\0';

	while (1)
	{

		c = getopt_long (argc, argv, "D:d:E:F:f:hI:M:m:S:s:t:U:vY:W:w:",
			long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
			case 0:
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
					printf ("\n");
			break;

			case 'D':
				printf(NAME": duration = '%s'\n", optarg);
				tr_parse_duration(&_tr, optarg);
			break;

			case 'd':
				printf(NAME": byday = '%s'\n", optarg);
				tr_parse_byday(&_tr, optarg);
			break;

			case 'E':
				printf(NAME": end date = '%s'\n", optarg);
				tr_parse_dtend(&_tr, optarg);
			break;

			case 'F':
				printf(NAME": frequnece = '%s'\n", optarg);
				tr_parse_freq(&_tr, optarg);
			break;

			case 'f':
				printf(NAME": use recurrence file '%s'\n", optarg);
				strcpy(fname, optarg);
			break;

			case 'h':
				printf("version: %s\n", version);
				printf("%s", help_msg);
				exit(0);
			break;

			case 'I':
				printf(NAME": interval = '%s'\n", optarg);
				tr_parse_interval(&_tr, optarg);
			break;

			case 'M':
				printf(NAME": bymday = '%s'\n", optarg);
				tr_parse_bymday(&_tr, optarg);
			break;

			case 'm':
				printf(NAME": bymonth = '%s'\n", optarg);
				tr_parse_bymonth(&_tr, optarg);
			break;

			case 'S':
				printf(NAME": start date = '%s'\n", optarg);
				tr_parse_dtstart(&_tr, optarg);
			break;

			case 's':
				printf(NAME": time to check = '%s'\n", optarg);
				ac_tm_set_time(&_at, atoi(optarg));

			break;

			case 't':
				printf(NAME": time to check = '%s'\n", optarg);
				if(!strncasecmp(optarg, "now", 3))
					ac_tm_set_time(&_at, time(NULL));
				else
					ac_tm_set_time(&_at, ic_parse_datetime(optarg));
			break;

			case 'U':
				printf(NAME": until = '%s'\n", optarg);
				tr_parse_until(&_tr, optarg);
			break;

			case 'v':
				printf("version: %s\n", version);
				printf("%s\n",id);
				printf("%s compiled on %s with %s\n", __FILE__,
					compiled, COMPILER );
				exit(0);
			break;

			case 'Y':
				printf(NAME": byyday = '%s'\n", optarg);
				tr_parse_byyday(&_tr, optarg);
			break;

			case 'W':
				printf(NAME": byweekno = '%s'\n", optarg);
				tr_parse_byweekno(&_tr, optarg);
			break;

			case 'w':
				printf(NAME": wkst = '%s'\n", optarg);
			break;

			default:
				printf (NAME": getopt returned unknow option\n"
						"        character code [%o] %c ??\n", c, (char)c);
		}
	}

	if (optind < argc) {
		printf ("Non-option ARGV-elements: ");
		while (optind < argc)
			printf ("%s ", argv[optind++]);
		printf ("\n");
	}

	ac_print(&_at);
	if(fname[0] != 0 )
		return check_file(fname, &_at);

	printf("\n\n ----- recurence\n");
	tr_print(&_tr);
	if(!check_tmrec(&_tr, &_at, &_rt))
	{
		printf("\nRECURENCE MATCHED -- rtime=%lu [%ld:%ld:%ld]\n", 
			_rt.rest, _rt.rest/3600, (_rt.rest%3600)/60,
			(_rt.rest%3600)%60);
		return 0;
	}
	else
		printf("\nRECURENCE NOT MATCHED\n");

	return 1;
}

int check_file(char *fname, ac_tm_p _atp)
{
	tmrec_p _trp = NULL;
	FILE *f;
	int _new;
	tr_res_t _rt;
	char buf1[512], buf2[512];
	
	if(!_atp)
		return -1;
	if(!fname)
		return -1;
	f = fopen(fname, "rt");
	if(!f)
		return -1;
	
	printf("Parsing file : %s\n", fname);

	_new = 1;
	while(!feof(f))
	{
		if(fscanf(f, "%s%s", buf1, buf2) != 2)
			return -1;
		if(!strncasecmp(buf1, "dtstart", 7))
		{
			if(_trp)
			{
				printf("\n\n ----- recurence\n");
				tr_print(_trp);
				if(!check_tmrec(_trp, _atp, &_rt))
					printf("\nRECURENCE MATCHED -- rtime=%lu [%ld:%ld:%ld]\n", 
						_rt.rest, _rt.rest/3600, (_rt.rest%3600)/60,
						(_rt.rest%3600)%60);
				else
					printf("\nRECURENCE NOT MATCHED\n");
			
				tmrec_free(_trp);
			}
			_new = 0;
			_trp = tmrec_new();
			if(!_trp)
				return 1;
			tr_parse_dtstart(_trp, buf2);
		}
		if(!strncasecmp(buf1, "dtend", 7))
			tr_parse_dtend(_trp, buf2);
		if(!strncasecmp(buf1, "duration", 7))
			tr_parse_duration(_trp, buf2);
		if(!strncasecmp(buf1, "until", 7))
			tr_parse_until(_trp, buf2);
		if(!strncasecmp(buf1, "freq", 7))
			tr_parse_freq(_trp, buf2);
		if(!strncasecmp(buf1, "interval", 7))
			tr_parse_interval(_trp, buf2);
		
		if(!strncasecmp(buf1, "byday", 7))
			tr_parse_byday(_trp, buf2);
		if(!strncasecmp(buf1, "bymday", 7))
			tr_parse_bymday(_trp, buf2);
		if(!strncasecmp(buf1, "byyday", 7))
			tr_parse_byyday(_trp, buf2);
		if(!strncasecmp(buf1, "bymonth", 7))
			tr_parse_bymonth(_trp, buf2);
		if(!strncasecmp(buf1, "byweekno", 7))
			tr_parse_byweekno(_trp, buf2);
	}
	
	fclose(f);
	if(_trp)
	{
		printf("\n\n ----- recurence\n");
		tr_print(_trp);
		if(!check_tmrec(_trp, _atp, &_rt))
			printf("\nRECURENCE MATCHED -- rtime=%lu [%ld:%ld:%ld]\n", 
				_rt.rest, _rt.rest/3600, (_rt.rest%3600)/60,
				(_rt.rest%3600)%60);
		else
			printf("\nRECURENCE NOT MATCHED\n");
		tmrec_free(_trp);
	}
	return 0;
}
