/**
 * $Id: trutil.h,v 1.1.1.1 2004-03-31 09:41:56 dcm Exp $
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

#ifndef _TRUTIL_H_
#define _TRUTIL_H_

#ifndef pkg_malloc
#include <malloc.h>
#define pkg_malloc malloc
#define pkg_free   free
#endif

#ifndef DBG
#include <stdio.h>
#define DBG printf
#endif

static inline int strz2int(char *_bp)
{
	int _v;
	char *_p;
	if(!_bp)
		return 0;
	_v = 0;
	_p = _bp;
	while(*_p && *_p>='0' && *_p<='9')
	{
		_v += *_p - '0';
		_p++;
	}
	return _v;
}

char *trim(char*);

#endif

