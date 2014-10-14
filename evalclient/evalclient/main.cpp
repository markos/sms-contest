/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mon Sep  2 14:10:55 EEST 2002
    copyright            : (C) 2002 by Alex Davelos
    email                : dav@bullet.gr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <ace/Svc_Handler.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Reactor.h>
#include <ace/Connector.h>
#include <ace/Synch.h>
#include <ace/Thread.h>
#include <ace/SOCK_Connector.h>
#include <ace/Reactor.h>

#include "partdetails.h"
#include "connectionmsgblock.h"
#include "clientobject.h"
#include "connectionhandler.h"

#define PORT_NO 10101

typedef ACE_Connector<connectionHandler,ACE_SOCK_CONNECTOR> evalConnector;

int main(int argc, char *argv[])
{   
    //Parse command line arguments using the ACE_Get_Opt object
    ACE_Get_Opt get_opt(argc,argv,"h:c:m:t:");
    
    //Declare variables to store the arguments
    char* hostname;
    long long code;
    char* msdn;
    int time;

    //check number of aguments ( argc must be 3 )
    if (argc<5 )
    {
      cout<<"Usage: evalclient -c <code> -m <msdn> -t <timestamp>"<<endl;
      ACE_OS::exit(1);
    }

    //Parse ACE_Get_Opt 
    char c;
    while ((c=get_opt()) != EOF)
    {
      switch(c)
      {
        case 'h':
          hostname=get_opt.optarg;
          break;
        case 'c':
        	char **endptr;
          code=strtoll(get_opt.optarg, endptr, 10);
/*          if (strlen(get_opt.optarg) != 12 || **endptr != '\0') {
       	    cout<<"PRESENT: -1" <<endl;
          	ACE_OS::exit(1);
          }*/
          break;
        case 'm':
          msdn=get_opt.optarg;
          break;
        case 't':
          time=strtoll(get_opt.optarg, NULL, 10);
          break;
        default:
          cout<<"Unknown option"<<endl;
          ACE_OS::exit(1);
      }
    }

  
#ifdef CLDEBUG    
    cout<<"Code:"<<code<<" MSDN:"<<msdn<<" TIME:"<<time<<endl;
#endif

    //Declare varibales to store the buffer
    partDetails messageDetails(msdn,time,0);
    connectionMsgBlock dataBlock(code,messageDetails);
#ifdef CLDEBUG
    cout<<"Checksum:"<<hex<<dataBlock.retrieveCRC()<<endl;
#endif    
    
    
    //Address to call connect()
    ACE_INET_Addr addr(PORT_NO);

    //Create the connection handler object
    connectionHandler* handler=new connectionHandler();
    
    //Create the connector object 
    evalConnector conn;    
    
    //Set the buffer member data in the handler object
    //in order to pass the buffer to the underlying stream
    //using peer() function
    handler->setBuffer(dataBlock.getData());
    handler->setPart(code,msdn,time);

//    ACE_OS::sleep(1);

    //call the connect() function:
    //the open() function of the handler is going 
    //to be called
    if (conn.connect(handler,addr) == -1) 
    {
#ifdef CLDEBUG
        ACE_DEBUG((LM_DEBUG,"Connection Failed\n"));
#endif
        ACE_OS::exit(1);
    }

    while(handler->readyToShutdown == false)
      ACE_Reactor::instance()->handle_events();

    return EXIT_SUCCESS;
}
