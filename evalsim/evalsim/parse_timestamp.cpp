#include "timestamp.h"
#include "timestamp.cpp"
#include <stdio.h>
#include <fstream.h>

int main(int argc,char** argv)
{
	
	fstream TimestampStream;
	TimestampStream.open("_timestamps",fstream::in);
	int i=0;
	int ts;
	
	while(!TimestampStream.eof())
 	{
	  TimeStamp t(ts);
          TimestampStream>>ts;
	  cout<<ts<<" "<<t.toString(0)<<endl;
	}
	return 0;
}
