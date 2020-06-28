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

#include <stdlib.h>
#include <string.h>

#include "screen.h"

Screen_t *ncs_new()
{
	Screen_t *s = malloc(sizeof(Screen_t));

	return s;
}

int ncs_init(Screen_t *s)
{
	s->wd = initscr();
	if (s->wd == NULL)
	{
		return -1;
	}

	ncs_max_xy(s, &s->w, &s->h);

	s->colors = 0;
	if (has_colors())
	{
		start_color();

		init_pair(1, COLOR_WHITE, COLOR_BLACK);
		init_pair(2, COLOR_BLACK, COLOR_WHITE);

		s->colors = 1;
	}

	keypad(s->wd, TRUE);
	scrollok(s->wd, TRUE);
	idlok(s->wd, TRUE);
	nonl();
	cbreak();
	echo();

	wrefresh(s->wd);

	return 0;
}

void ncs_set_scrolling(Screen_t *s, int state)
{
	if (state)
		scrollok(s->wd, TRUE);
	else
		scrollok(s->wd, FALSE);
}

void ncs_start_color(Screen_t *s, int cp)
{
	attron(COLOR_PAIR(cp));
}

void ncs_stop_color(Screen_t *s, int cp)
{
	attroff(COLOR_PAIR(cp));
}

void ncs_max_xy(Screen_t *s, int *x, int *y)
{
	getmaxyx(s->wd, *y, *x);
}

void ncs_xy(Screen_t *s, int *x, int *y)
{
	getyx(s->wd, *y, *x);
}

void ncs_set_cursor(Screen_t *s, int x, int y)
{
	int xx, yy;
	ncs_xy(s, &xx, &yy);

	wmove(s->wd, y, x);
	wrefresh(s->wd);

	ncs_xy(s, &xx, &yy);

	s->r_x = x;
	s->r_y = y;
}

void ncs_cursor_revert(Screen_t *s)
{
	wmove(s->wd, s->r_y, s->r_x);
	wrefresh(s->wd);
}

void ncs_cursor_lf(Screen_t *s, int n)
{
	int xx, yy;

	if ((s->r_x - n) < 0)
		n = s->r_x;

	s->r_x -= n;

	wmove(s->wd, s->r_y, s->r_x);
	wrefresh(s->wd);

	ncs_xy(s, &xx, &yy);
}

void ncs_cursor_rt(Screen_t *s, int n)
{
	int xx, yy;

	if (s->r_x + n >= s->w)
		n = s->w - s->r_x - 1;

	s->r_x += n;

	wmove(s->wd, s->r_y, s->r_x);
	wrefresh(s->wd);

	ncs_xy(s, &xx, &yy);
}

int ncs_cursor_up(Screen_t *s, int n)
{
	int scrl; 

	scrl = 0;

	s->r_y -= n;
	if (s->r_y < 0)
	{
		scrl = s->r_y;
		s->r_y = 0;
	}

	wmove(s->wd, s->r_y, s->r_x);
	wrefresh(s->wd);

	return scrl;
}

int ncs_cursor_dw(Screen_t *s, int n)
{
	int scrl; 

	scrl = 0;

	s->r_y += n;
	if (s->r_y >= s->h - 3)
	{
		scrl = s->r_y - (s->h - 3);
		s->r_y = s->h - 3;
	}

	wmove(s->wd, s->r_y, s->r_x);
	wrefresh(s->wd);


	return scrl;
}

void ncs_addch_xy(Screen_t *s, char c, int x, int y)
{
	mvwaddch(s->wd, y, x, c);
	wrefresh(s->wd);
}

void ncs_addch(Screen_t *s, char c)
{
	ncs_xy(s, &s->r_x, &s->r_y);

	mvwaddch(s->wd, s->r_y, s->r_x, c);

	ncs_xy(s, &s->r_x, &s->r_y);

	wrefresh(s->wd);
}

void ncs_addstrf(Screen_t *s, int x, int y, const char *fmt, ...)
{
	va_list ap;
	char	buffer[256];

	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	mvwprintw(s->wd, y, x, "%s", buffer);

	ncs_cursor_revert(s);

	wrefresh(s->wd);
}

int ncs_render_data(Screen_t *s, char *p)
{
	int x, y;

	mvwaddnstr(s->wd, 0, 0, p, strlen(p));
	wrefresh(s->wd);

	ncs_xy(s, &x, &y);

	return 0;
}

int ncs_scroll(Screen_t *s, int n)
{
	wscrl(s->wd, n);
	wrefresh(s->wd);

	return 0;
}

char ncs_get_ch(Screen_t *s, int x, int y)
{
	chtype ch;

	ch = mvwinch(s->wd, y, x);

	return (char)(ch & A_CHARTEXT);
}

int ncs_rm_current_line(Screen_t *s)
{
	wclrtoeol(s->wd);

	return 0;
}

int ncs_clr_line(Screen_t *s, int y)
{
	wmove(s->wd, y, 0);
	wrefresh(s->wd);

	wclrtoeol(s->wd);

	wmove(s->wd, s->r_y, s->r_x);
	wrefresh(s->wd);

	return 0;
}

int ncs_clear(Screen_t *s)
{
	werase(s->wd);

	return 0;
}

void ncs_close(Screen_t *s)
{
	wrefresh(s->wd);
	delwin(s->wd);

	free(s);
}

void ncs_quit()
{
	endwin();
}

