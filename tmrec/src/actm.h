/**
 * $Id: actm.h,v 1.2 2004-03-31 09:58:31 dcm Exp $
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

#ifndef _ACTM_H_
#define _ACTM_H_

#include <time.h>


// USE_YWEEK_U	-- Sunday system - see strftime %U
// USE_YWEEK_V	-- ISO 8601 - see strftime %V
// USE_YWEEK_W	-- Monday system - see strftime %W

#ifndef USE_YWEEK_U
# ifndef USE_YWEEK_V
#  ifndef USE_YWEEK_W
#   define USE_YWEEK_W
#  endif
# endif
#endif

#define is_leap_year(yyyy) ((((yyyy)%400))?(((yyyy)%100)?(((yyyy)%4)?0:1):0):1)

typedef struct _ac_maxval
{
	int yweek;
	int yday;
	int ywday;
	int mweek;
	int mday;
	int mwday;
} ac_maxval_t, *ac_maxval_p;

typedef struct _ac_tm
{
	time_t time;
	struct tm t;
	int mweek;
	int yweek;
	int ywday;
	int mwday;
	ac_maxval_p mv;
} ac_tm_t, *ac_tm_p;

ac_tm_p ac_tm_new();

int ac_tm_set(ac_tm_p, struct tm*);
int ac_tm_set_time(ac_tm_p, time_t);

int ac_tm_reset(ac_tm_p);
int ac_tm_free(ac_tm_p);

int ac_get_mweek(struct tm*);
int ac_get_yweek(struct tm*);
ac_maxval_p ac_get_maxval(ac_tm_p);
int ac_get_wkst();

int ac_print(ac_tm_p);

#endif

