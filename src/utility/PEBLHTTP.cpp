#include "PEBLHTTP.h"


#ifdef PEBL_WIN32
#include <winsock2.h> //avoid collision
#include <windows.h>
#endif


//Set this to PEBL_HAPPY or PEBL_CURL (or PEBL_FETCH on emscripten) to use
// alternate http libraries for get/post, etc.

//set the HTTPLIB library we want to use for get/post
//in the Makefile

//Originally, we used happyhttp, but this seems to have some problems with
//post commands when using simple file upload servers.  The curl requires an
//external dependency, but seems more robust.  The happy branch is retained here
// but going forward the curl version will be supported.

#ifdef PEBL_HTTP

#include "PError.h"
#include <stdio.h>
#include "../base/PList.h"
#include "../base/PComplexData.h"
#include "../utility/rc_ptrs.h"

//#define HTTP_LIB PEBL_HAPPY
#ifndef HTTP_LIB
#define HTTP_LIB PEBL_CURL
#endif

#if HTTP_LIB == PEBL_HAPPY

#include "happyhttp.h"

int count=0;
// This prints out to stdout; useful for POST where we don't care about the returned page.
//
void OnBegin( const happyhttp::Response* r, void* userdata )
{
  printf( "BEGIN (%d %s)\n", r->getstatus(), r->getreason() );
  count = 0;
}

void OnData( const happyhttp::Response* r, void* userdata, const unsigned char* data, int n )
{
  fwrite( data,1,n, stdout );
  count += n;
}

void OnComplete( const happyhttp::Response* r, void* userdata )
{
  printf( "COMPLETE (%d bytes)\n", count );
}




//These are used for the commands where we save to a file.  userdata should contain
//an PEBLHTTP object.get a file:
void http_begin_cb( const happyhttp::Response* r,
		    void* userdata )
{
  //I don't think any of this is needed:
  PEBLHTTP* http = (PEBLHTTP*)(userdata);
  //  FILE * filestream = http->GetFileObject();
  printf( "BEGIN (%d %s)\n", r->getstatus(), r->getreason() );
  http->SetByteCount(0);
  http->SetStatus(r->getstatus());
  http->SetReason(std::string(r->getreason()));

}

void http_getdata_cb( const happyhttp::Response* r, void* userdata,
		      const unsigned char* data, int n )
{
  std::cout << "http_getdata_cb 1\n";

  PEBLHTTP* http = (PEBLHTTP*) (userdata);
  FILE * filestream = http->GetFileObject();


  fwrite( data,1,n, filestream);

  http->SetByteCount(http->GetByteCount()+n);
}

void http_complete_cb( const happyhttp::Response* r, void* userdata )
{
  //. PEBLHTTP* http = (PEBLHTTP*) userdata;
  //  FILE * filestream = http->GetFileObject();
  //  fclose(filestream);

  //printf( "COMPLETE (%d bytes)\n", http->GetByteCount() );
  //printf( "COMPLETE (%d bytes)\n", 33);

}

//
//These save the http retrieved to a s strstream for later use,
// rather than a file.
//


//These are used for Get/ POST and PUT where we are returning text, not a file.
//they should slurp the text into a text object that gets set to http->mText
void http_begin_cb2( const happyhttp::Response* r,
		     void* userdata )
{
  //I don't think any of this is needed:
  PEBLHTTP* http = (PEBLHTTP*)(userdata);
  //  FILE * filestream = http->GetFileObject();

  //printf( "BEGIN (%d %s)\n", r->getstatus(), r->getreason() );
  http->SetStatus(r->getstatus());
  http->SetReason(std::string(r->getreason()));
  http->SetByteCount(0);

}

void http_getdata_cb2( const happyhttp::Response* r, void* userdata,
		       const unsigned char* data, int n )
{


  //FILE * filestream = http->GetFileObject();
  //FILE * filestream = (FILE*)userdata;

  PEBLHTTP* http = (PEBLHTTP*) (userdata);
  FILE * filestream = http->GetFileObject();

  //This appends text
  std::string * text = (http->GetTextObject());
  text->append((const char*)data, n);

  std::cout << "tmp text:" << *text << std::endl;

  http->SetByteCount(http->GetByteCount()+n);
}

void http_complete_cb2( const happyhttp::Response* r, void* userdata )
{
  PEBLHTTP* http = (PEBLHTTP*) userdata;
  std::string * text = http->GetTextObject();
  FILE * filestream = http->GetFileObject();


  printf( "COMPLETE (%d bytes)\n", http->GetByteCount() );
  //printf( "COMPLETE (%d bytes)\n", 33);

}



PEBLHTTP::PEBLHTTP(Variant host, int port)
{
  mHost = host;
  mPort = port;
#ifdef WIN32
  WSAData wsaData;
  int code = WSAStartup(MAKEWORD(1, 1), &wsaData);
  if( code != 0 )
    {
      PError::SignalFatalError(Variant("Unable to start http library.") + Variant(code));
    }
#endif //WIN32



}


PEBLHTTP::~PEBLHTTP()
{
}


//This gets an http url and saves it to the specified file.
//this assumes you are getting a binary-formatted file.
int PEBLHTTP::GetHTTPFile(Variant  filename,
			  Variant savename)
{

  // simple simple GET
  happyhttp::Connection conn(mHost.GetString().c_str(), mPort );
  //we should set binary/text here too?
  //such as png, gif, jpg, .wav, .ogg, .mp3, etc?


  //open file for binary/write access:
  mFile = fopen(savename.GetString().c_str(),"wb");
  if(mFile == NULL)
    {
      std::cerr << "Error opening file \n";
    }
  //Somehow, we need to 'prime' the file, or else it comes out
  //of the void* cast as NULL.
  fwrite( "",1,0,mFile);

  conn.setcallbacks( http_begin_cb,
		     http_getdata_cb,
		     http_complete_cb, this);


  try
    {

      std::cout << "trying to get file: " << filename.GetString() << std::endl;
      conn.request( "GET", filename.GetString().c_str(), 0, 0,0 );
      while( conn.outstanding() )
	{
	  //std::cout << "+" << std::flush;
	  conn.pump();
	}

      fclose(mFile);
      mFile = NULL;
      return mStatus;

    }
 catch( happyhttp::Wobbly& e )
   {
     char s[300];
     sprintf(s,"Exception:\n%s\n", e.what() );
     PError::SignalWarning(s);
     fclose(mFile);
     mFile = NULL;
     return 0;  //mstatus may not exist here
   }

}



//This gets an http url and saves it to the specified file.
//this assumes you are getting a binary-formatted file.
std::string PEBLHTTP::GetHTTPText(Variant  filename)
{

  // simple simple GET
  happyhttp::Connection conn(mHost.GetString().c_str(), mPort );
  //we should set binary/text here too?
  //such as png, gif, jpg, .wav, .ogg, .mp3, etc?


  conn.setcallbacks( http_begin_cb2,
		     http_getdata_cb2,
		     http_complete_cb2, this);

  try
    {

      conn.request( "GET", filename.GetString().c_str(), 0, 0,0 );

      while( conn.outstanding() )
	{
	  //std::cout << "+" << std::flush;
	  conn.pump();
	}

      return mText;

    }
  catch( happyhttp::Wobbly& e )
    {
      std::cerr << "Wobbly caught\n";
      char s[300];
      sprintf(s,"Exception:\n%s\n", e.what() );
      PError::SignalWarning(s);
      return mText;  //mstatus may not exist here
    }
}



Variant PEBLHTTP::PostHTTP(Variant name,
		       Variant args,
		       Variant bodyx)
{


  //mFile = fopen("tmp.html","wb");
  //  fwrite( "",1,0,mFile);






  // simple POST command
  happyhttp::Connection conn(mHost.GetString().c_str(), mPort );



  //use the callback that puts the retrieved data in mText:
  //conn.setcallbacks( http_begin_cb2,
  //http_getdata_cb2,
  //http_complete_cb2, this);

  conn.setcallbacks( OnBegin, OnData, OnComplete, 0 );

  //headersx is a variant-list, we need to create a const char* list
  //to contain it (with a 0 at the end).
  PError::AssertType(args, PEAT_LIST, "PostHTTP headers must be a list");

  try
    {

      conn.putrequest("POST", name.GetString().c_str()); //name of page


      // std::cout << headersx.GetComplexData() << std::endl;
      //   std::cout << (headersx.GetComplexData())->GetObject().get() << std::endl;
      PList * dataList = (PList*)(args.GetComplexData()->GetPEBLObject().get());
      //PComplexData * pcd = (headersx.GetComplexData());
      //counted_ptr<PEBLObjectBase> pl = pcd->GetObject();
      //PList * dataList = pl.get();
      //(PList*)(pcd->GetObject().get());


      std::vector<Variant>::iterator p1 = dataList->Begin();
      std::vector<Variant>::iterator p1end = dataList->End();


      while(p1 != p1end)
	{
	  std::string head = *p1;
	  p1++;
	  std::string value = *p1;

	  p1++;
	  conn.putheader(head.c_str(),value.c_str()); //the content-length might need to be a number

	}

      conn.endheaders();



      std::string body1 = bodyx.GetString();
      const unsigned char* body = (unsigned char*)(body1.c_str());


      unsigned int l = strlen(bodyx.GetString().c_str());

      conn.send(body,l);


      //The request is sent; now retrieve the response text:
      while( conn.outstanding() )
	{

	  std::cout << "outer-outstanding"<<conn.outstanding() << "+" <<std::endl;
	  conn.pump();

	}

      std::cout << "No more ouststanding data to process\n";

      return mStatus;
    }

  catch( happyhttp::Wobbly& e )
    {
      char s[300];
      sprintf(s,"Exception:\n%s\n", e.what() );
      PError::SignalWarning(s);


      //      fclose(mFile);
      //      mFile = NULL;
      return mStatus;
    }
}



Variant PEBLHTTP::PostMulti(Variant pagename,
			    Variant args,
			    Variant uploadname,
			    Variant form)
{


  //mFile = fopen("tmp.html","wb");
  //  fwrite( "",1,0,mFile);

  return Variant("");

}




#elif  HTTP_LIB == PEBL_CURL

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//  CURL library for http support.  This is an alternate that uses an external
//  library, rather than the lightweight happyhttp stuff which can be compiled
//  in.  But it might be more robust.
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <curl/curl.h>



size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size,
				  size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}




PEBLHTTP::PEBLHTTP(Variant host, int port)
{
  mHost = host;
  mPort = port;


  curl_global_init(CURL_GLOBAL_ALL);
  mCurl = NULL;

}


PEBLHTTP::~PEBLHTTP()
{
}

//
//This gets an http url and saves it to the specified file.
//this assumes you are getting a binary-formatted file.


int PEBLHTTP::GetHTTPFile(Variant  filename,
			  Variant savename)
{

  //establish the curl library

  mCurl = curl_easy_init();
  CURLcode res;
  if(mCurl)
    {
      //set the basename url:
      Variant fname = mHost + filename;
      std::cout << "--------------*************Getting filename:" << fname << std::endl;
      curl_easy_setopt(mCurl, CURLOPT_URL,fname.GetString().c_str());

      /* specify port*/
      curl_easy_setopt(mCurl, CURLOPT_PORT,mPort);

      mFile = fopen(savename.GetString().c_str(),"wb");

      //Somehow, we need to 'prime' the file, or else it comes out
      //of the void* cast as NULL.
      fwrite( "",1,0,mFile);


      CURLcode ret;
      //this will set the write function to save to mFile.
      curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, mFile);
      //ret = curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, write_callback);


      /* First set the URL that is about to receive our POST. This URL can
	 just as well be a https: URL if that is what should receive the
	 data. */


      /* use a GET to fetch this */
      curl_easy_setopt(mCurl, CURLOPT_HTTPGET, 1L);

      /* Perform the request */
      ret  = curl_easy_perform(mCurl);

      if( CURLE_OK == ret)
	{
	  char *ct;
	  res = curl_easy_setopt(mCurl, CURLOPT_HTTPGET, 1L);

	  if((CURLE_OK == res) && ct)
	    printf("We received Content-Type: %s\n", ct);
	}
      long http_code = 0;
      curl_easy_getinfo(mCurl,CURLINFO_RESPONSE_CODE,&http_code);
      mStatus = http_code;

      /* always cleanup */
      curl_easy_cleanup(mCurl);

      fclose(mFile);
    }
  return mStatus;

}



 //This gets an http url and saves it to the specified file.
 //this assumes you are getting a binary-formatted file.
std::string PEBLHTTP::GetHTTPText(Variant  filename)
{


  /* init the curl session */
  curl_global_init(CURL_GLOBAL_ALL);
  mCurl = curl_easy_init();
  CURLcode res;

  if(mCurl)
    {
      //set the basename url
      Variant fname = mHost + filename;

      struct MemoryStruct chunk;

      chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
      chunk.size = 0;    /* no data at this point */





      /* specify URL to get */
      curl_easy_setopt(mCurl, CURLOPT_URL, fname.GetString().c_str());

      /* specify port*/
      curl_easy_setopt(mCurl, CURLOPT_PORT,mPort);



      /* send all data to this function  */
      curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

      /* we pass our 'chunk' struct to the callback function */
      curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, (void *)&chunk);

      /* some servers don't like requests that are made without a user-agent
	 field, so we provide one */
      curl_easy_setopt(mCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

      /* get it! */
      res = curl_easy_perform(mCurl);

      /* check for errors */
      if(res != CURLE_OK) {
	fprintf(stderr, "curl_easy_perform() failed: %s\n",
		curl_easy_strerror(res));
      }
      else {
	/*
	 * Now, our chunk.memory points to a memory block that is chunk.size
	 * bytes big and contains the remote file.
	 *
	 * Do something nice with it!
	 */

	printf("%lu bytes retrieved\n", (long)chunk.size);
	mText = std::string((char*)(chunk.memory));
      }

      long http_code = 0;
      curl_easy_getinfo(mCurl,CURLINFO_RESPONSE_CODE,&http_code);
      mStatus = http_code;


      /* cleanup curl stuff */
      curl_easy_cleanup(mCurl);

      free(chunk.memory);

      /* we're done with libcurl, so clean it up */
      curl_global_cleanup();

      return mText;


    }
  return mText;
}


Variant PEBLHTTP::PostMulti(Variant pagename,
			    Variant args,
			    Variant uploadname,
			    Variant form)
{


  //mFile = fopen("tmp.html","wb");
  //  fwrite( "",1,0,mFile);




  Variant fname = mHost + pagename;
  const std::string form2 = form;
  const std::string fname2 = fname;
  const std::string upload2 = uploadname;


  CURLcode res;

  /* In windows, this will init the winsock stuff */
  curl_global_init(CURL_GLOBAL_ALL);

  /* get a curl handle */
  if(!mCurl) mCurl = curl_easy_init();
  if(mCurl)
    {
      /* First set the URL that is about to receive our POST. This URL can
	 just as well be a https:// URL if that is what should receive the
	 data. */
      struct MemoryStruct chunk;
      chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
      chunk.size = 0;    /* no data at this point */



      struct curl_httppost *formpost=NULL;
      struct curl_httppost *lastptr=NULL;
      struct curl_slist *headerlist=NULL;
      static const char buf[] = "Expect:";



      /* Now specify the POST data */


      //First, add the page arguments &a=b&c=d etc..
      //to contain it (with a 0 at the end).
      PError::AssertType(args, PEAT_LIST, "PostHTTP arguments must be a list");
      PList * dataList = (PList*)(args.GetComplexData()->GetPEBLObject().get());

      std::vector<Variant>::iterator p1 = dataList->Begin();
      std::vector<Variant>::iterator p1end = dataList->End();


      while(p1 != p1end)
	{
	  std::string head = *p1;
	  p1++;
	  std::string value = *p1;
	  p1++;
	  curl_formadd(&formpost,&lastptr,
		       CURLFORM_COPYNAME,head.c_str(),
		       CURLFORM_COPYCONTENTS,value.c_str(),
		       CURLFORM_END);
	}

      curl_formadd(&formpost,
		   &lastptr,
		   CURLFORM_COPYNAME, form2.c_str(),
		   CURLFORM_FILE,     upload2.c_str(),
		   CURLFORM_END);

      /* Fill in the 'filename' field. This might differ based on server. */
      curl_formadd(&formpost,
		   &lastptr,
		   CURLFORM_COPYNAME,"filename",
		   CURLFORM_COPYCONTENTS,  upload2.c_str(),
		   CURLFORM_END);

      /* Fill in the submit field too, even if this is rarely needed */
      curl_formadd(&formpost,
		   &lastptr,
		   CURLFORM_COPYNAME, "submit",
		   CURLFORM_COPYCONTENTS, "send",
		   CURLFORM_END);


      curl_easy_setopt(mCurl, CURLOPT_URL, fname2.c_str()); //set name and port:
      curl_easy_setopt(mCurl, CURLOPT_PORT,mPort);

      curl_easy_setopt(mCurl, CURLOPT_VERBOSE, 1L);

      curl_easy_setopt(mCurl, CURLOPT_HTTPHEADER, headerlist);
      curl_easy_setopt(mCurl, CURLOPT_HTTPPOST, formpost);


      /* send all data to this function  */
      curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

      /* we pass our 'chunk' struct to the callback function */
      curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, (void *)(&chunk));

      /* some servers don't like requests that are made without a user-agent
	 field, so we provide one */
      curl_easy_setopt(mCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0");


      /* Perform the request, res will get the return code */
      res = curl_easy_perform(mCurl);


      /* Check for errors */
      if(res != CURLE_OK)
	{
	fprintf(stderr, "PEBL HTTP Multi File POST failed: %s\n",
		curl_easy_strerror(res));
	}
      else
	{
	  /*
	   * Now, our chunk.memory points to a memory block that is chunk.size
	   * bytes big and contains the remote file.
	   *
	   * Do something nice with it!
	   */

	  printf("%lu bytes retrieved\n", (long)chunk.size);
	  mText = std::string((char*)(chunk.memory));
	}




      /* always cleanup */
      curl_easy_cleanup(mCurl);
    }
  curl_global_cleanup();

  return mText;

}



Variant PEBLHTTP::PostHTTP(Variant pagename,
			   Variant args,
			   Variant uploadname)

{


  //mFile = fopen("tmp.html","wb");
  //  fwrite( "",1,0,mFile);




  Variant fname = mHost + pagename;


  const std::string fname2 = fname;
  const std::string upload2 = uploadname;


  CURLcode res;

  /* In windows, this will init the winsock stuff */
  curl_global_init(CURL_GLOBAL_ALL);

  /* get a curl handle */
  if(!mCurl) mCurl = curl_easy_init();
  if(mCurl)
    {
      /* First set the URL that is about to receive our POST. This URL can
	 just as well be a https:// URL if that is what should receive the
	 data. */
      struct MemoryStruct chunk;
      chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
      chunk.size = 0;    /* no data at this point */



      struct curl_httppost *formpost=NULL;
      struct curl_httppost *lastptr=NULL;
      struct curl_slist *headerlist=NULL;
      static const char buf[] = "Expect:";



      /* Now specify the POST data */


      //First, add the page arguments &a=b&c=d etc..
      //to contain it (with a 0 at the end).
      PError::AssertType(args, PEAT_LIST, "PostHTTP arguments must be a list");

      PList * dataList = (PList*)(args.GetComplexData()->GetPEBLObject().get());

      std::vector<Variant>::iterator p1 = dataList->Begin();
      std::vector<Variant>::iterator p1end = dataList->End();


      while(p1 != p1end)
	{
	  std::string head = *p1;
	  p1++;
	  std::string value = *p1;
	  p1++;
	  curl_formadd(&formpost,&lastptr,
		       CURLFORM_COPYNAME,head.c_str(),
		       CURLFORM_COPYCONTENTS,value.c_str(),
		       CURLFORM_END);
	}



      curl_formadd(&formpost,
		   &lastptr,
		   CURLFORM_COPYNAME, "sendfile",
		   CURLFORM_FILE, upload2.c_str(),
		   CURLFORM_END);

      /* Fill in the filename field */
      curl_formadd(&formpost,
		   &lastptr,
		   CURLFORM_COPYNAME, "filename",
		   CURLFORM_COPYCONTENTS, upload2.c_str(),
		   CURLFORM_END);

      /* Fill in the submit field too, even if this is rarely needed */
      curl_formadd(&formpost,
		   &lastptr,
		   CURLFORM_COPYNAME, "submit",
		   CURLFORM_COPYCONTENTS, "send",
		   CURLFORM_END);


      curl_easy_setopt(mCurl, CURLOPT_URL, fname2.c_str()); //set name and port:
      curl_easy_setopt(mCurl, CURLOPT_PORT,mPort);

      curl_easy_setopt(mCurl, CURLOPT_VERBOSE, 1L);

      //headerlist is NULL
      //curl_easy_setopt(mCurl, CURLOPT_HTTPHEADER, headerlist);
      curl_easy_setopt(mCurl, CURLOPT_HTTPPOST, formpost);


      /* send all data to this function  */
      curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

      /* we pass our 'chunk' struct to the callback function */
      curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, (void *)(&chunk));

      /* some servers don't like requests that are made without a user-agent
	 field, so we provide one */
      curl_easy_setopt(mCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0");


      /* Perform the request, res will get the return code */
      res = curl_easy_perform(mCurl);


      /* Check for errors */
      if(res != CURLE_OK)
	{
	fprintf(stderr, "PEBL HTTP Multi File POST failed: %s\n",
		curl_easy_strerror(res));
	}
      else
	{
	  /*
	   * Now, our chunk.memory points to a memory block that is chunk.size
	   * bytes big and contains the remote file.
	   *
	   * Do something nice with it!
	   */

	  printf("%lu bytes retrieved\n", (long)chunk.size);
	  mText = std::string((char*)(chunk.memory));
	}




      /* always cleanup */
      curl_easy_cleanup(mCurl);
    }
  curl_global_cleanup();

  return mText;

}
#elif  HTTP_LIB == PEBL_FETCH

#include <emscripten.h>
#include <emscripten/fetch.h>

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
//  emscripten/fetch for http support.
//
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>

void uploadSucceeded(emscripten_fetch_t *fetch) {
  printf("Finished uploading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
  // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
  emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void uploadFailed(emscripten_fetch_t *fetch) {
  printf("Uploading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  emscripten_fetch_close(fetch); // Also free data on failure.
}



PEBLHTTP::PEBLHTTP(Variant host, int port)
{
  mHost = host;
  mPort = port;
  std::cout <<"Creating PEBLHTTP object\n";

}


PEBLHTTP::~PEBLHTTP()
{
}

//
//This gets an http url and saves it to the specified file.
//this assumes you are getting a binary-formatted file.


int PEBLHTTP::GetHTTPFile(Variant  filename,
			  Variant savename)
{

  std::cout << "Getting file\n";
  //set the basename url:
  Variant fname = mHost +Variant(":")+Variant(mPort) + filename;
  std::cout << "fname1: " << fname << std::endl;

  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, "GET");

   attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;// | EMSCRIPTEN_FETCH_SYNCHRONOUS;
   attr.onsuccess = uploadSucceeded;
   attr.onerror = uploadFailed;

   emscripten_fetch_t * fetch;


   fetch = emscripten_fetch(&attr, fname.GetString().c_str());

   std::cout << "done fetching\n";
   mStatus = fetch->status;

   //The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];

   //This can probably be stored directly to the indexDB with the
   // destinationpath property, instead of saving it to memory
   // and then writing with fwrite.

   fwrite(fetch->data,1,fetch->numBytes,mFile);
   fclose(mFile);
   emscripten_fetch_close(fetch);

  return fetch->status;

}



 //This gets an http url and saves it to the specified file.
 //this assumes you are getting a binary-formatted file.
std::string PEBLHTTP::GetHTTPText(Variant  filename)
{


  //set the basename url:
  Variant fname = mHost +Variant(":")+Variant(mPort) + Variant(filename);
   emscripten_fetch_attr_t attr;
   emscripten_fetch_attr_init(&attr);
   strcpy(attr.requestMethod, "GET");
    std::cout << "fname2: " << fname << std::endl;
   attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;

  attr.onsuccess = uploadSucceeded;
  attr.onerror = uploadFailed;


   emscripten_fetch_t *fetch;
   fetch = emscripten_fetch(&attr, fname.GetString().c_str());
   mStatus = fetch->status;

   //The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];

   printf("%lu bytes retrieved\n", (long)(fetch->numBytes));

   char *p = (char*)malloc((fetch->numBytes+ 1 ) );

    for(int i = 0; i < fetch->numBytes; ++i)
      p[i] = fetch->data[i];
    p[fetch->numBytes+1] = '\0';


    mText = std::string(p);

    free(p);
    p=NULL;

   emscripten_fetch_close(fetch);

   return mText;

}



Variant PEBLHTTP::PostHTTP(Variant pagename,
			   Variant args,
			   Variant uploadname)

{


  //set the basename url:

   emscripten_fetch_attr_t attr;
   emscripten_fetch_attr_init(&attr);
   strcpy(attr.requestMethod, "POST");

   attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY ;//| EMSCRIPTEN_FETCH_SYNCHRONOUS;










  //First, add the page arguments &a=b&c=d etc..
  //to contain it (with a 0 at the end).
  PError::AssertType(args, PEAT_LIST, "PostHTTP arguments must be a list");

  PList * dataList = (PList*)(args.GetComplexData()->GetPEBLObject().get());

  std::vector<Variant>::iterator p1 = dataList->Begin();
  std::vector<Variant>::iterator p1end = dataList->End();

  Variant arguments;
  Variant sep = "?";
  while(p1 != p1end)
    {
      std::string head = *p1;
      p1++;
      std::string value = *p1;
      p1++;
      arguments = arguments + sep + Variant(head) + Variant("=") + Variant(value);
      sep = "&";
    }

  Variant fname = mHost +Variant(":")+Variant(mPort) + pagename + arguments;
   std::cout << "fname3: " << fname << std::endl;
  /* Now specify the POST data */
  const char * const * rH;

  /*
//These are headers that probably need to be added using the requestheaders
//list.
      curl_formadd(&formpost,
		   &lastptr,
		   CURLFORM_COPYNAME, "sendfile",
		   CURLFORM_FILE, ((std::string)uploadname).c_str(),
		   CURLFORM_END);

   // Fill in the filename field
      curl_formadd(&formpost,
		   &lastptr,
		   CURLFORM_COPYNAME, "filename",
		   CURLFORM_COPYCONTENTS,  ((std::string)uploadname).c_str(),
		   CURLFORM_END);

   // Fill in the submit field too, even if this is rarely needed
      curl_formadd(&formpost,
		   &lastptr,
		   CURLFORM_COPYNAME, "submit",
		   CURLFORM_COPYCONTENTS, "send",
		   CURLFORM_END);

      //set name and port:
      curl_easy_setopt(mCurl, CURLOPT_URL, ((std::string)fname).c_str());
      curl_easy_setopt(mCurl, CURLOPT_PORT,mPort);

      curl_easy_setopt(mCurl, CURLOPT_VERBOSE, 1L);
      curl_easy_setopt(mCurl, CURLOPT_HTTPPOST, formpost);

****/



   emscripten_fetch_t * fetch;
   fetch = emscripten_fetch(&attr, fname.GetString().c_str());
   mStatus = fetch->status;

   //The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];

   char *p = (char*)malloc((fetch->numBytes+ 1 ) );

    for(int i = 0; i < fetch->numBytes; ++i)
      p[i] = fetch->data[i];
    p[fetch->numBytes+1] = '\0';


    mText = std::string(p);

    free(p);
    p=NULL;

   emscripten_fetch_close(fetch);



  return mText;

}



Variant PEBLHTTP::PostMulti(Variant pagename,
			    Variant args,
			    Variant uploadname,
			    Variant form)
{


      /* Now specify the POST data */


  //First, add the page arguments &a=b&c=d etc..
  //to contain it (with a 0 at the end).
  PError::AssertType(args, PEAT_LIST, "PostHTTP arguments must be a list");

  PList * dataList = (PList*)(args.GetComplexData()->GetPEBLObject().get());

  std::vector<Variant>::iterator p1 = dataList->Begin();
  std::vector<Variant>::iterator p1end = dataList->End();

  Variant arguments;
  Variant sep = "?";
  while(p1 != p1end)
    {
      std::string head = *p1;
      p1++;
      std::string value = *p1;
      p1++;
      arguments = arguments +  sep + (head + "=" + value);
      sep = "&";
    }

  Variant fname = mHost +Variant(":")+Variant(mPort) + pagename + arguments;

  /*

  curl_formadd(&formpost,
	       &lastptr,
	       CURLFORM_COPYNAME, ((std::string)form).c_str(),
	       CURLFORM_FILE, ((std::string)uploadname).c_str(),
	       CURLFORM_END);

  // Fill in the 'filename' field. This might differ based on server.
  curl_formadd(&formpost,
	       &lastptr,
	       CURLFORM_COPYNAME,"filename",
	       CURLFORM_COPYCONTENTS,  ((std::string)uploadname).c_str(),
	       CURLFORM_END);

  // Fill in the submit field too, even if this is rarely needed
  curl_formadd(&formpost,
	       &lastptr,
	       CURLFORM_COPYNAME, "submit",
	       CURLFORM_COPYCONTENTS, "send",
	       CURLFORM_END);


  curl_easy_setopt(mCurl, CURLOPT_HTTPPOST, formpost);



*/









  return mText;
  }



#endif
#endif
