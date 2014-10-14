/***************************************************************************
                          connectionhandler.cpp  -  description
                             -------------------
    begin                : Mon Sep 2 2002
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

#include "connectionhandler.h"
#include "connectionmsgblock.h"
#define ID_CRC_CORRECT 1

enum responseID { ID_CRC_OK=1, ID_CRC_ERROR=-1 };

void Crypt(char *inp, u_int inplen, char* key = "", u_int keylen = 0)
{
    //we will consider size of sbox 256 bytes
    //(extra byte are only to prevent any mishep just in case)
    char Sbox[257], Sbox2[257];
    unsigned long i, j, t, x;

    //this unsecured key is to be used only when there is no input key from user
    static const char  OurUnSecuredKey[] = "-vodafone.competition-" ;
    static const u_int OurKeyLen = strlen(OurUnSecuredKey);
    char temp , k;
    i = j = k = t =  x = 0;
    temp = 0;

    //always initialize the arrays with zero

    for (i=0; i<=256; i++ )
    {
	    Sbox[i]=0;
	    Sbox2[i]=0;
    }
    
//    ZeroMemory(Sbox, sizeof(Sbox));
//    ZeroMemory(Sbox2, sizeof(Sbox2));

    //initialize sbox i
    for(i = 0; i < 256U; i++)
    {
        Sbox[i] = (char)i;
    }

    j = 0;
    //whether user has sent any inpur key
    if(keylen)
    {
        //initialize the sbox2 with user key
        for(i = 0; i < 256U ; i++)
        {
            if(j == keylen)
            {
                j = 0;
            }
            Sbox2[i] = key[j++];
        }
    }
    else
    {
        //initialize the sbox2 with our key
        for(i = 0; i < 256U ; i++)
        {
            if(j == OurKeyLen)
            {
                j = 0;
            }
            Sbox2[i] = OurUnSecuredKey[j++];
        }
    }

    j = 0 ; //Initialize j
    //scramble sbox1 with sbox2
    for(i = 0; i < 256; i++)
    {
        j = (j + (unsigned long) Sbox[i] + (unsigned long) Sbox2[i]) % 256U ;
        temp =  Sbox[i];
        Sbox[i] = Sbox[j];
        Sbox[j] =  temp;
    }

    i = j = 0;
    for(x = 0; x < inplen; x++)
    {
        //increment i
        i = (i + 1U) % 256U;
        //increment j
        j = (j + (unsigned long) Sbox[i]) % 256U;

        //Scramble SBox #1 further so encryption routine will
        //will repeat itself at great interval
        temp = Sbox[i];
        Sbox[i] = Sbox[j] ;
        Sbox[j] = temp;

        //Get ready to create pseudo random  byte for encryption key
        t = ((unsigned long) Sbox[i] + (unsigned long) Sbox[j]) %  256U ;

        //get the random byte
        k = Sbox[t];

        //xor with the data and done
        inp[x] = (inp[x] ^ k);
    }
}


connectionHandler::connectionHandler()
{
  //initialize buffer/counter member data
  counter=0;
  status=0;

  readyToShutdown = false;
  
  int timeout = 10;
  ACE_Reactor::instance()->schedule_timer(this, (const void *)true, timeout);
}

connectionHandler::~connectionHandler()
{
  delete[] buffer;  
}


int connectionHandler::open(void*)
{
#ifdef CLDEBUG
  cout<<"Connection Established"<<endl;
#endif
  //Register the nadler to the reacto in order 
  //to listen for input
  ACE_Reactor::instance()->register_handler(this,ACE_Event_Handler::READ_MASK);

  //send buffer to the peer      
  sendBuffer();
  
  return 0;
}    
         
void connectionHandler::setBuffer(unsigned char* buf)
{
  //allocate space for buffer data member
  buffer=new char[connectionMsgBlock::getDataSize()];
  
  //copy buf into buffer
  memcpy(buffer,buf,connectionMsgBlock::getDataSize());
}

int connectionHandler::handle_timeout(const ACE_Time_Value &tv,
																			const void *argument) {

  bool arg = (bool)(argument);
  if (arg == true) {
    cout << "Timeout! Exiting..." << endl;
    readyToShutdown = true;
  }
  return 0;
}

int connectionHandler::handle_input(ACE_HANDLE)
{
  //get the server's response
  response=process_input();

  //checkout and re-send till the counter becomes equal to 10
  if ( response >=  0 )
  {
    status=0;
#ifdef CLDEBUG
    cout<<"ID:"<<ID_CRC_OK<<" EXITING: CORRECT"<<endl;
#endif

    char tgt[30], str_time[20],str_code[30],str_gid[10], *out;

    sprintf(str_time,"%s",TimeStamp(_time).toString(true).c_str());
//    Crypt(str_time,strlen(str_time));

    sprintf(str_code,"%012lld",_code);
//    Crypt(str_code,strlen(str_code));
    sprintf(str_gid,"%i",response);

    if (response != 1) {
      out = base64_encode(_msdn, _msdn +strlen(_msdn), tgt);
      *out = '\0';
      cout<<"MS-ISDN: "<< tgt <<endl;

      out = base64_encode(str_code, str_code +strlen(str_code), tgt);
      *out = '\0';
      cout<<"CODE: "<< tgt <<endl;

      out = base64_encode(str_time, str_time +strlen(str_time), tgt);
      *out = '\0';
      cout<<"TIMESTAMP: "<< tgt <<endl;
      cout<<"PRESENT: "<<str_gid<<endl;
    } else {      
      cout<<"MS-ISDN: "<< _msdn <<endl;
      cout<<"CODE: "<< str_code <<endl;
      cout<<"TIMESTAMP: "<< str_time <<endl;
      cout<<"PRESENT: "<<str_gid<<endl;
    }
    exit(0);    
  }
  
  if ( counter >=10 )
  {
    status = -1;
#ifdef CLDEBUG
    cout<<"EXITING: OUT OF COUNTS"<<endl;
#endif
    exit(1);
  }
  counter++;
        
#ifdef CLDEBUG
  cout<<"CRC WRONG!!! - RESENDING"<<endl;
#endif
  sendBuffer();

    
  
  return 0;
}

void connectionHandler::sendBuffer()
{
#ifdef CLDEBUG
  cout<<"SIZE OF BUFFER:"<<connectionMsgBlock::getDataSize()<<endl;
#endif
  peer().send_n(buffer,connectionMsgBlock::getDataSize());
}

int connectionHandler::process_input()
{
  ssize_t bytes_read;
  switch ( (bytes_read = this->peer().recv_n(&response, sizeof(int))) )
  {
    case -1:
      ACE_ERROR_RETURN ((LM_ERROR,
                         "",
                         ""),
                        -1);
    case 0:
      ACE_ERROR_RETURN ((LM_ERROR,
                         ""),
                        -1);
    default:
      ACE_DEBUG ((LM_DEBUG,
                  "",
                  response));
  }
  return response;
}



