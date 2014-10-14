/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Tue Aug 13 11:44:07 EEST 2002
    copyright            : (C) 2002 Bullet S.A.
    author               : Konstantinos Margaritis
    email                : markos@bullet.gr
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

using namespace std;

#include <iostream>
#include <stdlib.h>

#include "contest.h"

int main(int argc, char *argv[])
{
	Contest *myContest = new Contest("localhost");
	
	myContest->start();

	return EXIT_SUCCESS;
}
