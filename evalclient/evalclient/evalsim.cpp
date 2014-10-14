#define _RND (int) ( rand() )
//#define _RND_LONG ( (long long)( rand() ) )

using namespace std;

#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <map>
#include <ctime>

long long makeRandomLong()
{
        long long randNum;

        randNum = ( (long long)( rand() & 0xFF ) << 32LL ) | (long long)( rand() ) ;

        return randNum;
}

int main( int argc, char *argv )
{
	long long longNumber;
	int intNumber, length;
	
	char intChar[30];
	char longChar[65];
	char timeChar[15];

	string systemCall;

	srand(time(0));

	map< int, long long > valueMap;

	map< int, long long >::iterator it = valueMap.begin();

	for ( int i=0; i<=100000; i++ )
	{
		longNumber = makeRandomLong();
		valueMap.insert( it, pair< int, long long >( _RND, longNumber ) );
		it++;
	}

	map< int, long long >::iterator it2;
	
	for ( it2 = valueMap.begin(); it2 != valueMap.end(); it2++ )
	{
		longNumber = (*it2).second;
		intNumber = (*it2).first;
		
		sprintf( intChar, "%d", intNumber );
		sprintf( longChar, "%lld", longNumber );
		sprintf( timeChar, "%d", time(0) );

		length = strlen(longChar);

		systemCall = "./evalclient -h localhost -m ";// + intChar + " -c " + longChar;
		systemCall += intChar;
		systemCall += " -c ";
		systemCall += longChar;
		systemCall += " -t ";
		systemCall += timeChar;
		systemCall += " &";

		cout << systemCall.c_str() << endl; 				
		usleep(min(50000, max(rand(),5000000)));
		system( systemCall.c_str() );

	}

	return 0;
}
