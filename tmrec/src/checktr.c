/**
 * $Id: checktr.c,v 1.2 2004-03-31 09:58:31 dcm Exp $
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

#define REC_ERR    -1
#define REC_MATCH   0
#define REC_NOMATCH 1

#define _IS_SET(x) (((x)>0)?1:0)

/*** local headers ***/
int get_min_interval(tmrec_p);
int check_min_unit(tmrec_p, ac_tm_p, tr_res_p);
int check_freq_interval(tmrec_p _trp, ac_tm_p _atp);
int check_byxxx(tmrec_p, ac_tm_p);

/**
 *
 * return 0/REC_MATCH - the time falls in
 *       -1/REC_ERR - error
 *        1/REC_NOMATCH - the time falls out
 */
int check_tmrec(tmrec_p _trp, ac_tm_p _atp, tr_res_p _tsw)
{
	if(!_trp || !_atp || (!_IS_SET(_trp->duration) && !_IS_SET(_trp->dtend)))
		return REC_ERR;

	// it is before start date
	if(_atp->time < _trp->dtstart)
		return REC_NOMATCH;
	// compute the duration of the reccurence interval
	if(!_IS_SET(_trp->duration))
		_trp->duration = _trp->dtend - _trp->dtstart;
	
	
	if(_atp->time <= _trp->dtstart+_trp->duration)
	{
		if(_tsw)
		{
			if(_tsw->flag & TSW_RSET)
			{
				if(_tsw->rest>_trp->dtstart+_trp->duration-_atp->time)
					_tsw->rest = _trp->dtstart+_trp->duration - _atp->time;
			}
			else
			{
				_tsw->flag |= TSW_RSET;
				_tsw->rest = _trp->dtstart+_trp->duration - _atp->time;
			}
		}
		return REC_MATCH;
	}
	
	// after the bound of recurrence
	if(_IS_SET(_trp->until))
	{
		if(_atp->time >= _trp->until + _trp->duration)
			return REC_NOMATCH;
	}
	else
	{
		if(!_IS_SET(_trp->freq) &&
				_atp->time >= _trp->dtstart + _trp->duration)
			return REC_NOMATCH;
	}
	
	// check if the instance of recurence matches the 'interval'
	if(check_freq_interval(_trp, _atp)!=REC_MATCH)
		return REC_NOMATCH;

	if(check_min_unit(_trp, _atp, _tsw)!=REC_MATCH)
		return REC_NOMATCH;

	if(check_byxxx(_trp, _atp)!=REC_MATCH)
		return REC_NOMATCH;

	return REC_MATCH;
}


int check_freq_interval(tmrec_p _trp, ac_tm_p _atp)
{
	int _t0, _t1;
	struct tm _tm;
	if(!_trp || !_atp)
		return REC_ERR;
	
	if(!_IS_SET(_trp->freq))
		return REC_NOMATCH;
	
	if(!_IS_SET(_trp->interval) || _trp->interval==1)
		return REC_MATCH;
	
	switch(_trp->freq)
	{
		case FREQ_DAILY:
		case FREQ_WEEKLY:
			memset(&_tm, 0, sizeof(struct tm));
			_tm.tm_year = _trp->ts.tm_year;
			_tm.tm_mon = _trp->ts.tm_mon;
			_tm.tm_mday = _trp->ts.tm_mday;
			_t0 = (int)mktime(&_tm);
			memset(&_tm, 0, sizeof(struct tm));
			_tm.tm_year = _atp->t.tm_year;
			_tm.tm_mon = _atp->t.tm_mon;
			_tm.tm_mday = _atp->t.tm_mday;
			_t1 = (int)mktime(&_tm);
			if(_trp->freq == FREQ_DAILY)
				return (((_t1-_t0)/(24*3600))%_trp->interval==0)?
					REC_MATCH:REC_NOMATCH;
#ifdef USE_YWEEK_U
			_t0 -= _trp->ts.tm_wday*24*3600;
			_t1 -= _atp->t.tm_wday*24*3600;
#else
			_t0 -= ((_trp->ts.tm_wday+6)%7)*24*3600;
			_t1 -= ((_atp->t.tm_wday+6)%7)*24*3600;
#endif
			return (((_t1-_t0)/(7*24*3600))%_trp->interval==0)?
					REC_MATCH:REC_NOMATCH;
		case FREQ_MONTHLY:
			_t0 = (_atp->t.tm_year-_trp->ts.tm_year)*12
					+ _atp->t.tm_mon-_trp->ts.tm_mon;
			return (_t0%_trp->interval==0)?REC_MATCH:REC_NOMATCH;
		case FREQ_YEARLY:
			return ((_atp->t.tm_year-_trp->ts.tm_year)%_trp->interval==0)?
					REC_MATCH:REC_NOMATCH;
	}
	
	return REC_NOMATCH;
}

int get_min_interval(tmrec_p _trp)
{
	if(!_trp)
		return FREQ_NOFREQ;
	
	if(_trp->freq == FREQ_DAILY || _trp->byday || _trp->bymday || _trp->byyday)
		return FREQ_DAILY;
	if(_trp->freq == FREQ_WEEKLY || _trp->byweekno) 
		return FREQ_WEEKLY;
	if(_trp->freq == FREQ_MONTHLY || _trp->bymonth)
		return FREQ_MONTHLY;
	if(_trp->freq == FREQ_YEARLY)
		return FREQ_YEARLY;
	
	return FREQ_NOFREQ;
}

int check_min_unit(tmrec_p _trp, ac_tm_p _atp, tr_res_p _tsw)
{
	int _v0, _v1;
	if(!_trp || !_atp)
		return REC_ERR;
	switch(get_min_interval(_trp))
	{
		case FREQ_DAILY:
		break;
		case FREQ_WEEKLY:
			if(_trp->ts.tm_wday != _atp->t.tm_wday)
				return REC_NOMATCH;
		break;
		case FREQ_MONTHLY:
			if(_trp->ts.tm_mday != _atp->t.tm_mday)
				return REC_NOMATCH;
		break;
		case FREQ_YEARLY:
			if(_trp->ts.tm_mon != _atp->t.tm_mon 
					|| _trp->ts.tm_mday != _atp->t.tm_mday)
				return REC_NOMATCH;
		break;
		default:
			return REC_NOMATCH;
	}
	_v0 = _trp->ts.tm_hour*3600 + _trp->ts.tm_min*60 + _trp->ts.tm_sec;
	_v1 = _atp->t.tm_hour*3600 + _atp->t.tm_min*60 + _atp->t.tm_sec;
	if(_v1 >= _v0 && _v1 < _v0 + _trp->duration)
	{
		if(_tsw)
		{
			if(_tsw->flag & TSW_RSET)
			{
				if(_tsw->rest>_v0+_trp->duration-_v1)
					_tsw->rest = _v0 + _trp->duration - _v1;
			}
			else
			{
				_tsw->flag |= TSW_RSET;
				_tsw->rest = _v0 + _trp->duration - _v1;
			}
		}
		return REC_MATCH;
	}
	
	return REC_NOMATCH;
}

int check_byxxx(tmrec_p _trp, ac_tm_p _atp)
{
	int i;
	ac_maxval_p _amp = NULL;
	if(!_trp || !_atp)
		return REC_ERR;
	if(!_trp->byday && !_trp->bymday && !_trp->byyday && !_trp->bymonth 
			&& !_trp->byweekno)
		return REC_MATCH;
	
	_amp = ac_get_maxval(_atp);
	if(!_amp)
		return REC_NOMATCH;
	
	if(_trp->bymonth)
	{
		for(i=0; i<_trp->bymonth->nr; i++)
		{
			if(_atp->t.tm_mon == 
					(_trp->bymonth->xxx[i]*_trp->bymonth->req[i]+12)%12)
				break;
		}
		if(i>=_trp->bymonth->nr)
			return REC_NOMATCH;
	}
	if(_trp->freq==FREQ_YEARLY && _trp->byweekno)
	{
		for(i=0; i<_trp->byweekno->nr; i++)
		{
			if(_atp->yweek == (_trp->byweekno->xxx[i]*_trp->byweekno->req[i]+
							_amp->yweek)%_amp->yweek)
				break;
		}
		if(i>=_trp->byweekno->nr)
			return REC_NOMATCH;
	}
	if(_trp->byyday)
	{
		for(i=0; i<_trp->byyday->nr; i++)
		{
			if(_atp->t.tm_yday == (_trp->byyday->xxx[i]*_trp->byyday->req[i]+
						_amp->yday)%_amp->yday)
				break;
		}
		if(i>=_trp->byyday->nr)
			return REC_NOMATCH;
	}
	if(_trp->bymday)
	{
		for(i=0; i<_trp->bymday->nr; i++)
		{
#ifdef EXTRA_DEBUG
			DBG("Req:bymday: %d == %d\n", _atp->t.tm_mday,
				(_trp->bymday->xxx[i]*_trp->bymday->req[i]+
				_amp->mday)%_amp->mday + ((_trp->bymday->req[i]<0)?1:0));
#endif
			if(_atp->t.tm_mday == (_trp->bymday->xxx[i]*_trp->bymday->req[i]+
						_amp->mday)%_amp->mday + (_trp->bymday->req[i]<0)?1:0)
				break;
		}
		if(i>=_trp->bymday->nr)
			return REC_NOMATCH;
	}
	if(_trp->byday)
	{
		for(i=0; i<_trp->byday->nr; i++)
		{
			if(_trp->freq==FREQ_YEARLY)
			{
#ifdef EXTRA_DEBUG
				printf("Req:byday:y: %d==%d && %d==%d\n", _atp->t.tm_wday,
					_trp->byday->xxx[i], _atp->ywday+1, 
					(_trp->byday->req[i]+_amp->ywday)%_amp->ywday);
#endif
				if(_atp->t.tm_wday == _trp->byday->xxx[i] &&
						_atp->ywday+1 == (_trp->byday->req[i]+_amp->ywday)%
						_amp->ywday)
					break;
			}
			else
			{
				if(_trp->freq==FREQ_MONTHLY)
				{
#ifdef EXTRA_DEBUG
					printf("Req:byday:m: %d==%d && %d==%d\n", _atp->t.tm_wday,
						_trp->byday->xxx[i], _atp->mwday+1, 
						(_trp->byday->req[i]+_amp->mwday)%_amp->mwday);
#endif
					if(_atp->t.tm_wday == _trp->byday->xxx[i] &&
							_atp->mwday+1==(_trp->byday->req[i]+
							_amp->mwday)%_amp->mwday)
						break;
				}
				else
				{
					if(_atp->t.tm_wday == _trp->byday->xxx[i])
						break;
				}
			}
		}
		if(i>=_trp->byday->nr)
			return REC_NOMATCH;
	}

	return REC_MATCH;
}

