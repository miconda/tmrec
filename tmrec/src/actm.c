/**
 * $Id: actm.c,v 1.1.1.1 2004-03-31 09:41:56 dcm Exp $
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

#include <stdio.h>
#include <string.h>

#include "trutil.h"
#include "actm.h"

//#define USE_YWEEK_U		// Sunday system
//#define USE_YWEEK_V		// ISO 8601
#ifndef USE_YWEEK_U
#ifndef USE_YWEEK_V
#ifndef USE_YWEEK_W
#define USE_YWEEK_W		// Monday system
#endif
#endif
#endif

#ifdef USE_YWEEK_U
#define SUN_WEEK(t)	(int)(((t)->tm_yday + 7 - \
				((t)->tm_wday)) / 7)
#else
#define MON_WEEK(t)	(int)(((t)->tm_yday + 7 - \
				((t)->tm_wday ? (t)->tm_wday - 1 : 6)) / 7)
#endif

#define ac_get_wday_yr(t) (int)((t)->tm_yday/7)
#define ac_get_wday_mr(t) (int)(((t)->tm_mday-1)/7)

ac_tm_p ac_tm_new()
{
	ac_tm_p _atp = NULL;
	_atp = (ac_tm_p)pkg_malloc(sizeof(ac_tm_t));
	if(!_atp)
		return NULL;
	memset(_atp, 0, sizeof(ac_tm_t));
	
	return _atp;
}

int ac_tm_fill(ac_tm_p _atp, struct tm* _tm)
{
	if(!_atp || !_tm)
		return -1;
	_atp->t.tm_sec = _tm->tm_sec;       /* seconds */
	_atp->t.tm_min = _tm->tm_min;       /* minutes */
	_atp->t.tm_hour = _tm->tm_hour;     /* hours */
	_atp->t.tm_mday = _tm->tm_mday;     /* day of the month */
	_atp->t.tm_mon = _tm->tm_mon;       /* month */
	_atp->t.tm_year = _tm->tm_year;     /* year */
	_atp->t.tm_wday = _tm->tm_wday;     /* day of the week */
	_atp->t.tm_yday = _tm->tm_yday;     /* day in the year */
	_atp->t.tm_isdst = _tm->tm_isdst;   /* daylight saving time */
	
	_atp->mweek = ac_get_mweek(_tm);
	_atp->yweek = ac_get_yweek(_tm);
	_atp->ywday = ac_get_wday_yr(_tm);
	_atp->mwday = ac_get_wday_mr(_tm);
	
	return 0;
}

int ac_tm_set(ac_tm_p _atp, struct tm* _tm)
{
	if(!_atp || !_tm)
		return -1;
	_atp->time = mktime(_tm);
	return ac_tm_fill(_atp, _tm);
}

int ac_tm_set_time(ac_tm_p _atp, time_t _t)
{
	if(!_atp)
		return -1;
	_atp->time = _t;
	return ac_tm_fill(_atp, localtime(&_t));
}

int ac_get_mweek(struct tm* _tm)
{
	if(!_tm)
		return -1;
#ifdef USE_YWEEK_U
	return ((_tm->tm_mday-1)/7 + (7-_tm->tm_wday+(_tm->tm_mday-1)%7)/7);
#else
	return ((_tm->tm_mday-1)/7 + (7-(6+_tm->tm_wday)%7+(_tm->tm_mday-1)%7)/7);
#endif
}

int ac_get_yweek(struct tm* _tm)
{
	int week = -1;
#ifdef USE_YWEEK_V
	int days;
#endif
	
	if(!_tm)
		return -1;
	
#ifdef USE_YWEEK_U
	week = SUN_WEEK(_tm);
#else
	week = MON_WEEK(_tm);
#endif

#ifdef USE_YWEEK_V
	days = ((_tm->tm_yday + 7 - (_tm->tm_wday ? _tm->tm_wday-1 : 6)) % 7);

	if(days >= 4) 
		week++;
	else 
		if(week == 0) 
			week = 53;
#endif
	return week;
}

int ac_get_wkst()
{
#ifdef USE_YWEEK_U
	return 0;
#else
	return 1;
#endif
}

int ac_tm_reset(ac_tm_p _atp)
{
	if(!_atp)
		return -1;
	memset(_atp, 0, sizeof(ac_tm_t));
	return 0;
}

int ac_tm_free(ac_tm_p _atp)
{
	if(!_atp)
		return -1;
	if(_atp->mv)
		pkg_free(_atp->mv);
	pkg_free(_atp);
	return 0;
}

ac_maxval_p ac_get_maxval(ac_tm_p _atp)
{
	struct tm _tm;
	int _v;
	ac_maxval_p _amp = NULL;

	if(!_atp)
		return NULL;
	_amp = (ac_maxval_p)pkg_malloc(sizeof(ac_maxval_t));
	if(!_amp)
		return NULL;
	
	// the number of the days in theyear
	_amp->yday = 365 + is_leap_year(_atp->t.tm_year+1900);

	// the number of the days in the month
	switch(_atp->t.tm_mon)
	{
		case 1:
			if(_amp->yday == 366)
				_amp->mday = 29;
			else
				_amp->mday = 28;
		break;
		case 3: case 5: case 8: case 10:
			_amp->mday = 30;
		break;
		default:
			_amp->mday = 31;
	}
	
	// maximum ocurrences of a week day in the year
	memset(&_tm, 0, sizeof(struct tm));
	_tm.tm_year = _atp->t.tm_year;
	_tm.tm_mon = 11;
	_tm.tm_mday = 31;
	mktime(&_tm);
	_v = 0;
	if(_atp->t.tm_wday > _tm.tm_wday)
		_v = _atp->t.tm_wday - _tm.tm_wday + 1;
	else
		_v = _tm.tm_wday - _atp->t.tm_wday;
	_amp->ywday = (int)((_tm.tm_yday-_v)/7) + 1;
	
	// maximum number of weeks in the year
	_amp->yweek = ac_get_yweek(&_tm) + 1;
	
	// maximum number of the week day in the month
	_amp->mwday=(int)((_amp->mday-1-(_amp->mday-_atp->t.tm_mday)%7)/7)+1;
	
	// maximum number of weeks in the month
	_v = (_atp->t.tm_wday + (_amp->mday - _atp->t.tm_mday)%7)%7;
#ifdef USE_YWEEK_U
	_amp->mweek = (int)((_amp->mday-1)/7+(7-_v+(_amp->mday-1)%7)/7)+1;
#else
	_amp->mweek = (int)((_amp->mday-1)/7+(7-(6+_v)%7+(_amp->mday-1)%7)/7)+1;
#endif

	_atp->mv = _amp;
	return _amp;
}

int ac_print(ac_tm_p _atp)
{
	static char *_wdays[] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"}; 
	if(!_atp)
	{
		printf("\n(null)\n");
		return -1;
	}
	
	printf("\nSys time: %d\nTime: %02d:%02d:%02d\n", (int)_atp->time,
				_atp->t.tm_hour, _atp->t.tm_min, _atp->t.tm_sec);
	printf("Date: %s, %04d-%02d-%02d\n", _wdays[_atp->t.tm_wday],
				_atp->t.tm_year+1900, _atp->t.tm_mon+1, _atp->t.tm_mday);
	printf("Year day: %d\nYear week-day: %d\nYear week: %d\n", _atp->t.tm_yday,
			_atp->ywday, _atp->yweek);
	printf("Month week: %d\nMonth week-day: %d\n", _atp->mweek, _atp->mwday);
	if(_atp->mv)
	{
		printf("Max ydays: %d\nMax yweeks: %d\nMax yweekday: %d\n",
				_atp->mv->yday, _atp->mv->yweek, _atp->mv->ywday);;
		printf("Max mdays: %d\nMax mweeks: %d\nMax mweekday: %d\n",
				_atp->mv->mday, _atp->mv->mweek, _atp->mv->mwday);;
	}
	return 0;
}

