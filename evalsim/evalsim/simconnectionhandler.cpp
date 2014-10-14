/***************************************************************************
                          simconnectionhandler.cpp  -  description
                             -------------------
    begin                : Tue Jan 28 2003
    copyright            : (C) 2003 by Alex Davelos
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

#include "simconnectionhandler.h"
#include "simtraffic.h"
#define ID_CRC_CORRECT 1
                                                                       
enum responseID { ID_CRC_OK=1, ID_CRC_ERROR=-1 };


SimConnectionHandler::SimConnectionHandler()
{
  //initialize buffer/counter member data
  counter=0;
  status=0;
  response=0;
}
SimConnectionHandler::~SimConnectionHandler()
{
  //ACE_Reactor::instance()->remove_handler(this,ACE_Event_Handler::READ_MASK);
  delete[] buffer;      
}


int SimConnectionHandler::open(void*)
{
 
  cout<<"Connection Established"<<endl;
  
  ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
  {
    //SimConnectionThread::tot_counter++;
    //SimConnectionThread::est_counter++;
  }
  //Register the nadler to the reactor in order 
  //to listen for input
  ACE_Reactor::instance()->register_handler(this,ACE_Event_Handler::READ_MASK);

  //send buffer to the peer      
  //sendBuffer();
  
  return 0;
}    
         
void SimConnectionHandler::setBuffer(unsigned char* buf)
{
  //allocate space for buffer data member
  buffer=new char[connectionMsgBlock::getDataSize()];
  
  //copy buf into buffer
  memcpy(buffer,buf,connectionMsgBlock::getDataSize());
}

int SimConnectionHandler::handle_input(ACE_HANDLE)
{

  //get the server's response
  response=process_input();
  //checkout and re-send till the counter becomes equal to 10
  if ( response >=  0 )
  {
    status=0;

    cout<<"ID:"<<ID_CRC_OK<<" EXITING: CORRECT"<<endl;
    ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
    {
 //     trafficInstance->incCounter(1);
    }

    return -1;    
  }
  else
  {
       cout<<"CRC WRONG - ABORT THREAD"<<endl;
       return -1;
  }
  return -1;
}

void SimConnectionHandler::sendBuffer()
{
 
  //cout<<"SIZE OF BUFFER:"<<connectionMsgBlock::getDataSize()<<endl;
  peer().send_n(buffer,connectionMsgBlock::getDataSize());
  cout<<"SEND BUFFER"<<endl;


}

int SimConnectionHandler::process_input()
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


/** No descriptions */
void SimConnectionHandler::showDebugInfo(){

    //COSTAS CHANGES
/*    char tgt[30], str_time[20],str_code[30],str_gid[10], *out;

    sprintf(str_time,"%s",TimeStamp(_time).toString(true).c_str());
    Crypt(str_time,strlen(str_time));

    sprintf(str_code,"%012lld",_code);
    Crypt(str_code,strlen(str_code));
    sprintf(str_gid,"%i",response);

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
 */        
}



