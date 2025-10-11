//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformNetwork.cpp
//    Purpose:    Class for handling network communication
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2006-2025 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
//
//
//     This file is part of the PEBL project.
//
//    PEBL is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    PEBL is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with PEBL; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////
#include "PlatformNetwork.h"
#include "../../devices/PNetwork.h"
#include "../../utility/PError.h"

#ifdef PEBL_NETWORK

#ifdef PEBL_OSX
#include "SDL.h"
#include "SDL_net.h"
#else
#include "SDL.h"
#include "SDL_net.h"
#endif


#include <cstring> //for strdup
#include <string>
#include <stdlib.h>

using std::string;
using std::cout;
using std::endl;


PlatformNetwork::PlatformNetwork()
{
    //mAddress is SDL-specific, so it is
    //not exposed to users.

    mAddress = new IPaddress;
    mAddress->host=0;
    mAddress->port=0;
    mSocket = NULL;
    mMyAddress = NULL;
    mListener = NULL;
}

PlatformNetwork::~PlatformNetwork()
{
    if(mAddress)
        delete mAddress;
    SDLNet_Quit();
}


void PlatformNetwork::Init()
{
    std::cout << "Initializing network\n";
    int val = SDLNet_Init();

    if(val==0)
        {
            std::cerr << "Network System Initialized\n";

        }
    else
        {

            PError::SignalFatalError("Network System Failed to Initialize.");
        }

}

void PlatformNetwork::SetHostIP(unsigned int ip)
{

    PNetwork::SetHostIP(ip);    //mHost = ip;
    mAddress->host=ip;
}

void PlatformNetwork::SetPort(unsigned int port)
{

    //SDL swaps the byte order in the SDL_Net library for some reason.
    //So we need to pre-swap to ensure we get the right port.

    //This should swap the byte order
    //PNetwork::SetPort(SDL_SwapBE16(port));
    PNetwork::SetPort((port));
    mAddress->port = SDL_SwapBE16(mPort);

    //std::cout << "Port: " << port <<"|"<<mPort << "|"<<mAddress->port<< std::endl;
    //std::cout << "tmp: " <<tmp << std::endl;

}


Variant PlatformNetwork::GetMyIPAddress()
{
    if(mMyAddress)
        return ConvertAddress(mMyAddress);
    else
        {
            mMyAddress = new IPaddress;
            SDLNet_ResolveHost(mMyAddress,NULL,mAddress->port);
            return ConvertAddress(mMyAddress);
        }
}

Variant PlatformNetwork::GetIPAddress()
{

    //This will get the IP address that was connected to you,
    //assuming you are a host.
    if(mAddress==NULL)
        return "<UNKNOWN ADDRESS>";//SetHostIP(SDLNet_TCP_GetPeerAddress(mSocket));
    return ConvertAddress(mAddress);
}

//This resolves the IP currently stored in mAddress
void PlatformNetwork::SetHostName(std::string hostname)
{

    PNetwork::SetHostName(hostname);

    //This will use hostname to obtain mAddress, an IPaddress.
    SDLNet_ResolveHost(mAddress,
                       mHostName.c_str(),
                       mAddress->port);

  //std::cout<< "Host address from name ["  << mAddress->host << "]"<< std::endl;
}


//This gets the IP address of the other side of the connection.


Variant PlatformNetwork::ConvertAddress(IPaddress* address)
{
    Uint32 host = address->host;
    Variant v = PNetwork::ConvertAddress(host);
    return v;
}

// This opens a connection to some host that is listening for it.
//
void PlatformNetwork::Open()
{

    if(mAddress->host == 0)
        PError::SignalFatalError("Trying to open connection to unspecified host");

    mSocket = SDLNet_TCP_Open(mAddress);


    if(!mSocket)
        {
            // PError::SignalWarning("Unable to establish connection to " + mHostName);
            PNetwork::SetOpen(false);
        }
    else
        {
            PNetwork::SetOpen(true);
        }
}

// This listens for a connection.
//
bool PlatformNetwork::CheckForConnection()
{

  //First, create the socket to listen on:

    //  IPaddress * tmpAddress = new IPaddress;
    //  tmpAddress->host = INADDR_ANY;
    //  tmpAddress->port = SDL_SwapBE16(mPort);


  //std::cout << "unable"  << tmpAddress->port << endl;
  //TCPsocket listener = SDLNet_TCP_Open(tmpAddress);

  //an mListener socket already needs to have been opened.
  if(!mListener)
      PError::SignalFatalError(Variant("Unable to listen for connection in PlatformNetwork::CheckForConnection on port:") + Variant((int)mPort) );

  //If we haven't opened the socket yet,  do so now:

  if(mSocket == NULL)
      mSocket = SDLNet_TCP_Accept(mListener);

  if(mSocket)
      PNetwork::SetOpen(true);


  //  if(!mIsOpen)
  //Close the listener, because we won't need it again the way we operate.
  //SDLNet_TCP_Close(mListener);


  //  delete tmpAddress;
  return mIsOpen;
}



bool PlatformNetwork::CreateListener()
{

  //First, create the socket to listen on:

  IPaddress * tmpAddress = new IPaddress;
  tmpAddress->host = INADDR_ANY;
  tmpAddress->port = SDL_SwapBE16(mPort);


  //  std::cout << "unable"  << tmpAddress->port << endl;
  mListener = SDLNet_TCP_Open(tmpAddress);
  if(!mListener)
      PError::SignalFatalError("Unable to create listener Socket." );

  return true;
}

// This requires a listener socket to have been opened.
//
bool PlatformNetwork::Accept(int mstimeout)
{

    if(mListener==NULL)
        {
            PError::SignalFatalError("Trying to accept a connection on a network port that has not been opened.");
        }
    int now = SDL_GetTicks();
    int end = now + mstimeout;
    //This doesn't work too well.  It should have a timeout.
    while(mSocket == NULL & SDL_GetTicks()<end )
        {
            mSocket = SDLNet_TCP_Accept(mListener);
            SDL_Delay(1);//Wait a little bit so we don't peg the CPU
        }

    //cout << "opened socket\n";

  //  if(mAddress)
  //      delete mAddress;
  //mAddress = SDLNet_TCP_GetPeerAddress(mSocket);

  //Close the 'listening' socket.
  SDLNet_TCP_Close(mListener);

  if(mSocket==NULL)
      {
          PNetwork::SetOpen(false);
      }
  else
      {
          PNetwork::SetOpen(true);
      }
  return  mSocket!=NULL;
}


// This is an all-on-one open-socket-listen-connect.
// One might prefer a two-step process giving a bit more
// control.

bool PlatformNetwork::Accept()
{

  //First, create the socket to listen on:

  IPaddress * tmpAddress = new IPaddress;
  tmpAddress->host = INADDR_ANY;
  tmpAddress->port = SDL_SwapBE16(mPort);


  //  std::cout << "unable"  << tmpAddress->port << endl;
  TCPsocket listener = SDLNet_TCP_Open(tmpAddress);
  if(!listener)
      PError::SignalFatalError("Unable to listen for connection 2." );



  //This doesn't work too well; maybe it should have an optional timeout?
  while(mSocket == NULL)
      {
          mSocket = SDLNet_TCP_Accept(listener);
          SDL_Delay(1);//Wait a little bit so we don't peg the CPU
      }

  //cout << "opened socket\n";

  //  if(mAddress)
  //      delete mAddress;
  //mAddress = SDLNet_TCP_GetPeerAddress(mSocket);

  //Close the 'listening' socket.
  SDLNet_TCP_Close(listener);
  delete tmpAddress;
  if(mSocket==NULL)
      {
          PNetwork::SetOpen(false);
      }
  else
      {
          PNetwork::SetOpen(true);
      }
  return  mSocket!=NULL;
}


void PlatformNetwork::Close()
{
    if(mSocket)
        SDLNet_TCP_Close(mSocket);
    mSocket = NULL;
    PNetwork::SetOpen(false);
}


bool PlatformNetwork::SendString(std::string data)
{

  char* buffer = NULL;
  if(mIsOpen)
	{

	  buffer = strdup(data.c_str());
	  unsigned long int sent = SDLNet_TCP_Send(mSocket, buffer,
                                               (int)(data.length()));


	  if(sent != (int)data.length())
		{
            std::cerr << "Tried to send: " << data.length() << std::endl;
            std::cerr << "Succeeded at:  " << sent << std::endl;
            PError::SignalWarning("Error sending data to " + mHostName + ". Not all data sent. Retry send");
            free(buffer);
            return false;
		}
  	}else{
	  PError::SignalFatalError("Trying to send data without an open connection.");
      return false;
	}

  free(buffer);
  return true;
}



bool PlatformNetwork::SendByte(int byte)
{

  if(mIsOpen)
	{

	  int sent = SDLNet_TCP_Send(mSocket, &byte,1);
	  if(sent != 1)
		{
		  PError::SignalWarning("Error sending data to " + mHostName + ". Entire Message Not Sent.");
          return 0;
		}
	}else{
	  PError::SignalFatalError("Trying to send data without an open connection.");
	}
  return true;
}

std::string PlatformNetwork::Receive(int length)
{
  char * message = NULL;
  if(mIsOpen)
	{
        message = new char[length+1];
        int pos = 0;
        while(pos<length)
            {

                int result = SDLNet_TCP_Recv(mSocket,message+pos,length);
                //cout << "length: " << length <<"|" << pos <<  " <<<< " << result << endl;

                if(result == -1)
                    {
                        PError::SignalFatalError("Error receiving  data from " + mHostName + ".");
                    }
                else if(result == 0)
                    {
                        PError::SignalFatalError("Error receiving data from " + mHostName + ". Connection Closed by peer.");
                    }else{

                    pos = pos + result;
                }
            }
        //Put a null char at the end.
        message[length] = '\0';
	}else{
	  PError::SignalFatalError("Trying to receive data without an open connection.");
	}

  //Make a copy and return the message.
  std::string ret = std::string(message);
  // free(message); tHanks to Martin Strunz
  delete [] message;
  return ret;
}


std::ostream& PlatformNetwork::SendToStream(std::ostream& out)const
{
  out << "<Network Connection: " << mHostName << ">";
	return out;
}

#endif
