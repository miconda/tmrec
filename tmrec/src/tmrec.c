/**
 * $Id: tmrec.c,v 1.1.1.1 2004-03-31 09:41:56 dcm Exp $
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

#include <string.h>

#include "trutil.h"
#include "tmrec.h"

#define _D(c) ((c) -'0')

tr_byxxx_p tr_byxxx_new()
{
	tr_byxxx_p _bxp = NULL;
	_bxp = (tr_byxxx_p)pkg_malloc(sizeof(tr_byxxx_t));
	if(!_bxp)
		return NULL;
	memset(_bxp, 0, sizeof(tr_byxxx_t));
	return _bxp;
}

int tr_byxxx_init(tr_byxxx_p _bxp, int _nr)
{
	if(!_bxp)
		return -1;
	_bxp->nr = _nr;
	_bxp->xxx = (int*)pkg_malloc(_nr*sizeof(int));
	if(!_bxp->xxx)
		return -1;
	_bxp->req = (int*)pkg_malloc(_nr*sizeof(int));
	if(!_bxp->req)
	{
		pkg_free(_bxp->xxx);
		return -1;
	}
	
	memset(_bxp->xxx, 0, _nr*sizeof(int));
	memset(_bxp->req, 0, _nr*sizeof(int));
	
	return 0;
}


int tr_byxxx_free(tr_byxxx_p _bxp)
{
	if(!_bxp)
		return -1;
	if(_bxp->xxx)
		pkg_free(_bxp->xxx);
	if(_bxp->req)
		pkg_free(_bxp->req);
	pkg_free(_bxp);
	return 0;
}

tmrec_p tmrec_new()
{
	tmrec_p _trp = NULL;
	struct tm *_tm;
	_trp = (tmrec_p)pkg_malloc(sizeof(tmrec_t));
	if(!_trp)
		return NULL;
	memset(_trp, 0, sizeof(tmrec_t));
	
	_tm = localtime(&_trp->dtstart);
	_trp->ts.tm_sec = _tm->tm_sec;       /* seconds */
	_trp->ts.tm_min = _tm->tm_min;       /* minutes */
	_trp->ts.tm_hour = _tm->tm_hour;     /* hours */
	_trp->ts.tm_mday = _tm->tm_mday;     /* day of the month */
	_trp->ts.tm_mon = _tm->tm_mon;       /* month */
	_trp->ts.tm_year = _tm->tm_year;     /* year */
	_trp->ts.tm_wday = _tm->tm_wday;     /* day of the week */
	_trp->ts.tm_yday = _tm->tm_yday;     /* day in the year */
	_trp->ts.tm_isdst = _tm->tm_isdst;   /* daylight saving time */
	
	return _trp;
}

int tmrec_free(tmrec_p _trp)
{
	if(!_trp)
		return -1;
	
	tr_byxxx_free(_trp->byday);
	tr_byxxx_free(_trp->bymday);
	tr_byxxx_free(_trp->byyday);
	tr_byxxx_free(_trp->bymonth);
	tr_byxxx_free(_trp->byweekno);

	pkg_free(_trp);
	return 0;
}

int tr_parse_dtstart(tmrec_p _trp, char *_in)
{
	struct tm *_tm;
	if(!_trp || !_in)
		return -1;
	_trp->dtstart = ic_parse_datetime(_in);
	
	_tm = localtime(&_trp->dtstart);
	_trp->ts.tm_sec = _tm->tm_sec;        /* seconds */
	_trp->ts.tm_min = _tm->tm_min;        /* minutes */
	_trp->ts.tm_hour = _tm->tm_hour;      /* hours */
	_trp->ts.tm_mday = _tm->tm_mday;      /* day of the month */
	_trp->ts.tm_mon = _tm->tm_mon;        /* month */
	_trp->ts.tm_year = _tm->tm_year;      /* year */
	_trp->ts.tm_wday = _tm->tm_wday;      /* day of the week */
	_trp->ts.tm_yday = _tm->tm_yday;      /* day in the year */
	_trp->ts.tm_isdst = _tm->tm_isdst;    /* daylight saving time */

	return 0;
}

int tr_parse_dtend(tmrec_p _trp, char *_in)
{
	if(!_trp || !_in)
		return -1;
	_trp->dtend = ic_parse_datetime(_in);
	return 0;
}

int tr_parse_duration(tmrec_p _trp, char *_in)
{
	if(!_trp || !_in)
		return -1;
	_trp->duration = ic_parse_duration(_in);
	return 0;
}

int tr_parse_until(tmrec_p _trp, char *_in)
{
	if(!_trp || !_in)
		return -1;
	_trp->until = ic_parse_datetime(_in);
	return 0;
}

int tr_parse_freq(tmrec_p _trp, char *_in)
{
	if(!_trp || !_in)
		return -1;
	if(!strcasecmp(_in, "daily"))
	{
		_trp->freq = FREQ_DAILY;
		return 0;
	}
	if(!strcasecmp(_in, "weekly"))
	{
		_trp->freq = FREQ_WEEKLY;
		return 0;
	}
	if(!strcasecmp(_in, "monthly"))
	{
		_trp->freq = FREQ_MONTHLY;
		return 0;
	}
	if(!strcasecmp(_in, "yearly"))
	{
		_trp->freq = FREQ_YEARLY;
		return 0;
	}

	_trp->freq = FREQ_NOFREQ;
	return 0;
}

int tr_parse_interval(tmrec_p _trp, char *_in)
{
	if(!_trp || !_in)
		return -1;
	_trp->interval = strz2int(_in);
	return 0;
}

int tr_parse_byday(tmrec_p _trp, char *_in)
{
	if(!_trp || !_in)
		return -1;
	_trp->byday = ic_parse_byday(_in); 
	return 0;
}

int tr_parse_bymday(tmrec_p _trp, char *_in)
{
	if(!_trp || !_in)
		return -1;
	_trp->bymday = ic_parse_byxxx(_in); 
	return 0;
}

int tr_parse_byyday(tmrec_p _trp, char *_in)
{
	if(!_trp || !_in)
		return -1;
	_trp->byyday = ic_parse_byxxx(_in); 
	return 0;
}

int tr_parse_bymonth(tmrec_p _trp, char *_in)
{
	if(!_trp || !_in)
		return -1;
	_trp->bymonth = ic_parse_byxxx(_in); 
	return 0;
}

int tr_parse_byweekno(tmrec_p _trp, char *_in)
{
	if(!_trp || !_in)
		return -1;
	_trp->byweekno = ic_parse_byxxx(_in); 
	return 0;
}

int tr_parse_wkst(tmrec_p _trp, char *_in)
{
	if(!_trp || !_in)
		return -1;
	_trp->wkst = ic_parse_wkst(_in);
	return 0;
}

int tr_print(tmrec_p _trp)
{
	static char *_wdays[] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"}; 
	int i;
	
	if(!_trp)
	{
		printf("\n(null)\n");
		return -1;
	}
	printf("\nRecurence definition\n-- start time ---\n");
	printf("Sys time: %d\n", (int)_trp->dtstart);
	printf("Time: %02d:%02d:%02d\n", _trp->ts.tm_hour, 
				_trp->ts.tm_min, _trp->ts.tm_sec);
	printf("Date: %s, %04d-%02d-%02d\n", _wdays[_trp->ts.tm_wday],
				_trp->ts.tm_year+1900, _trp->ts.tm_mon+1, _trp->ts.tm_mday);
	printf("---\n");
	printf("End time: %d\n", (int)_trp->dtend);
	printf("Duration: %d\n", (int)_trp->duration);
	printf("Until: %d\n", (int)_trp->until);
	printf("Freq: %d\n", (int)_trp->freq);
	printf("Interval: %d\n", (int)_trp->interval);
	if(_trp->byday)
	{
		printf("Byday: ");
		for(i=0; i<_trp->byday->nr; i++)
			printf(" %d%s", _trp->byday->req[i], _wdays[_trp->byday->xxx[i]]);
		printf("\n");
	}
	if(_trp->bymday)
	{
		printf("Bymday: %d:", _trp->bymday->nr);
		for(i=0; i<_trp->bymday->nr; i++)
			printf(" %d", _trp->bymday->xxx[i]*_trp->bymday->req[i]);
		printf("\n");
	}
	if(_trp->byyday)
	{
		printf("Byyday:");
		for(i=0; i<_trp->byyday->nr; i++)
			printf(" %d", _trp->byyday->xxx[i]*_trp->byyday->req[i]);
		printf("\n");
	}
	if(_trp->bymonth)
	{
		printf("Bymonth: %d:", _trp->bymonth->nr);
		for(i=0; i< _trp->bymonth->nr; i++)
			printf(" %d", _trp->bymonth->xxx[i]*_trp->bymonth->req[i]);
		printf("\n");
	}
	if(_trp->byweekno)
	{
		printf("Byweekno: ");
		for(i=0; i<_trp->byweekno->nr; i++)
			printf(" %d", _trp->byweekno->xxx[i]*_trp->byweekno->req[i]);
		printf("\n");
	}
	printf("Weekstart: %d\n", _trp->wkst);
	return 0;
}

time_t ic_parse_datetime(char *_in)
{
	struct tm _tm;
	
	if(!_in)
		return 0;
	
	memset(&_tm, 0, sizeof(struct tm));
	_tm.tm_year  = _D(_in[0])*1000 + _D(_in[1])*100 
			+ _D(_in[2])*10 + _D(_in[3]) - 1900;
	_tm.tm_mon   = _D(_in[4])*10 + _D(_in[5]) - 1;
	_tm.tm_mday  = _D(_in[6])*10 + _D(_in[7]);
	_tm.tm_hour  = _D(_in[9])*10 + _D(_in[10]);
	_tm.tm_min   = _D(_in[11])*10 + _D(_in[12]);
	_tm.tm_sec   = _D(_in[13])*10 + _D(_in[14]);
	_tm.tm_isdst = -1;
	return mktime(&_tm);
}

time_t ic_parse_duration(char *_in)
{
	time_t _t, _ft;
	char *_p;
	int _fl;
	
	if(!_in || (*_in!='+' && *_in!='-' && *_in!='P' && *_in!='p'))
		return 0;
	
	if(*_in == 'P' || *_in=='p')
		_p = _in+1;
	else
	{
		if(strlen(_in)<2 || (_in[1]!='P' && _in[1]!='p'))
			return 0;
		_p = _in+2;
	}
	
	_t = _ft = 0;
	_fl = 1;
	
	while(*_p)
	{
		switch(*_p)
		{
			case '0': case '1': case '2':
			case '3': case '4': case '5':
			case '6': case '7': case '8':
			case '9':
				_t = _t*10 + *_p - '0';
			break;
			
			case 'w':
			case 'W':
				if(!_fl)
					return 0;
				_ft += _t*7*24*3600;
				_t = 0;
			break;
			case 'd':
			case 'D':
				if(!_fl)
					return 0;
				_ft += _t*24*3600;
				_t = 0;
			break;
			case 'h':
			case 'H':
				if(_fl)
					return 0;
				_ft += _t*3600;
				_t = 0;
			break;
			case 'm':
			case 'M':
				if(_fl)
					return 0;
				_ft += _t*60;
				_t = 0;
			break;
			case 's':
			case 'S':
				if(_fl)
					return 0;
				_ft += _t;
				_t = 0;
			break;
			case 't':
			case 'T':
				if(!_fl)
					return 0;
				_fl = 0;
			break;
			default:
				return 0;
		}
		_p++;
	}

	return _ft;
}

tr_byxxx_p ic_parse_byday(char *_in)
{
	tr_byxxx_p _bxp = NULL;
	int _nr, _s, _v;
	char *_p;

	if(!_in)
		return NULL;
	_bxp = tr_byxxx_new();
	if(!_bxp)
		return NULL;
	_p = _in;
	_nr = 1;
	while(*_p)
	{
		if(*_p == ',')
			_nr++;
		_p++;
	}
	if(tr_byxxx_init(_bxp, _nr) < 0)
	{
		tr_byxxx_free(_bxp);
		return NULL;
	}
	_p = _in;
	_nr = _v = 0;
	_s = 1;
	while(*_p && _nr < _bxp->nr)
	{
		switch(*_p)
		{
			case '0': case '1': case '2':
			case '3': case '4': case '5':
			case '6': case '7': case '8':
			case '9':
				_v = _v*10 + *_p - '0';
			break;
			
			case 's':
			case 'S':
				_p++;
				switch(*_p)
				{
					case 'a':
					case 'A':
						_bxp->xxx[_nr] = WDAY_SA;
						_bxp->req[_nr] = _s*_v;
					break;
					case 'u':
					case 'U':
						_bxp->xxx[_nr] = WDAY_SU;
						_bxp->req[_nr] = _s*_v;
					break;
					default:
						goto error;
				}
				_s = 1;
				_v = 0;
			break;
			case 'm':
			case 'M':
				_p++;
				if(*_p!='o' && *_p!='O')
					goto error;
				_bxp->xxx[_nr] = WDAY_MO;
				_bxp->req[_nr] = _s*_v;
				_s = 1;
				_v = 0;
			break;
			case 't':
			case 'T':
				_p++;
				switch(*_p)
				{
					case 'h':
					case 'H':
						_bxp->xxx[_nr] = WDAY_TH;
						_bxp->req[_nr] = _s*_v;
					break;
					case 'u':
					case 'U':
						_bxp->xxx[_nr] = WDAY_TU;
						_bxp->req[_nr] = _s*_v;
					break;
					default:
						goto error;
				}
				_s = 1;
				_v = 0;
			break;
			case 'w':
			case 'W':
				_p++;
				if(*_p!='e' && *_p!='E')
					goto error;
				_bxp->xxx[_nr] = WDAY_WE;
				_bxp->req[_nr] = _s*_v;
				_s = 1;
				_v = 0;
			break;
			case 'f':
			case 'F':
				_p++;
				if(*_p!='r' && *_p!='R')
					goto error;
				_bxp->xxx[_nr] = WDAY_FR;
				_bxp->req[_nr] = _s*_v;
				_s = 1;
				_v = 0;
			break;
			case '-':
				_s = -1;
			break;
			case '+':
			case ' ':
			case '\t':
			break;
			case ',':
				_nr++;
			break;
			default:
				goto error;
		}
		_p++;
	}

	return _bxp;

error:
	tr_byxxx_free(_bxp);
	return NULL;
}

tr_byxxx_p ic_parse_byxxx(char *_in)
{
	tr_byxxx_p _bxp = NULL;
	int _nr, _s, _v;
	char *_p;

	if(!_in)
		return NULL;
	_bxp = tr_byxxx_new();
	if(!_bxp)
		return NULL;
	_p = _in;
	_nr = 1;
	while(*_p)
	{
		if(*_p == ',')
			_nr++;
		_p++;
	}
	if(tr_byxxx_init(_bxp, _nr) < 0)
	{
		tr_byxxx_free(_bxp);
		return NULL;
	}
	_p = _in;
	_nr = _v = 0;
	_s = 1;
	while(*_p && _nr < _bxp->nr)
	{
		switch(*_p)
		{
			case '0': case '1': case '2':
			case '3': case '4': case '5':
			case '6': case '7': case '8':
			case '9':
				_v = _v*10 + *_p - '0';
			break;
			
			case '-':
				_s = -1;
			break;
			case '+':
			case ' ':
			case '\t':
			break;
			case ',':
				_bxp->xxx[_nr] = _v;
				_bxp->req[_nr] = _s;
				_s = 1;
				_v = 0;
				_nr++;
			break;
			default:
				goto error;
		}
		_p++;
	}
	if(_nr < _bxp->nr)
	{
		_bxp->xxx[_nr] = _v;
		_bxp->req[_nr] = _s;
	}
	return _bxp;

error:
	tr_byxxx_free(_bxp);
	return NULL;
}

int ic_parse_wkst(char *_in)
{
	if(!_in || strlen(_in)!=2)
		goto error;
	
	switch(_in[0])
	{
		case 's':
		case 'S':
			switch(_in[1])
			{
				case 'a':
				case 'A':
					return WDAY_SA;
				case 'u':
				case 'U':
					return WDAY_SU;
				default:
					goto error;
			}
		case 'm':
		case 'M':
			if(_in[1]!='o' && _in[1]!='O')
				goto error;
			return WDAY_MO;
		case 't':
		case 'T':
			switch(_in[1])
			{
				case 'h':
				case 'H':
					return WDAY_TH;
				case 'u':
				case 'U':
					return WDAY_TU;
				default:
					goto error;
			}
		case 'w':
		case 'W':
			if(_in[1]!='e' && _in[1]!='E')
				goto error;
			return WDAY_WE;
		case 'f':
		case 'F':
			if(_in[1]!='r' && _in[1]!='R')
				goto error;
			return WDAY_FR;
		break;
		default:
			goto error;
	}
	
error:
#ifdef USE_YWEEK_U
	return WDAY_SU;
#else
	return WDAY_MO;
#endif
}

