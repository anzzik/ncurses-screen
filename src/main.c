/*
   This file is part of ncurses-screen wrapper library. 
   Copyright (C) 2020 Anssi Kulju 

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>

#include "screen.h"

int main(void)
{
	Screen_t *s = ncs_new();
	ncs_init(s);

	/* code goes here */

	ncs_close(s);
	ncs_quit();
}

