#!/usr/bin/python
import socket               # Import socket module
import time
import os

def checkmultifile(filename):
	if os.path.isfile(filename):
		posp = filename.rfind('.')
		filename = filename[:posp]+'(1)'+filename[posp:]

		while os.path.isfile(filename):	
			pospara1 = filename.rfind('(')
		#	print pospara1
			pospara2 = filename.rfind(')')
			posnum = int(filename[pospara1+1:pospara2])
			numnew = posnum+1
			filename = filename[:pospara1+1]+str(numnew)+filename[pospara2:]	
	return filename
	
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)         # Create a socket object
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


host = ''		 # Get local machine name
port = 12345                # Reserve a port for your service.
s.bind((host, port))        # Bind to the port

s.listen(5)                 # Now wait for client connection.
while True:
	#
	c, addr = s.accept()     # Establish connection with client.
	print 'Got connection from', addr
	#os.mkdir(str(addr[0]))
	fname = c.recv(32)        #file name received
#	print fname
	starpos = fname.find('*')
	if starpos > 0:
		fname = fname[:starpos]
	#print fname
	#cutpos = fname.find(".csv")
	
	#create directory	
	subjectcode = fname[:fname.find('_')]
	path = subjectcode+'/'
	if os.path.isdir(subjectcode)== False:
		os.mkdir(subjectcode)
		logfile = open(path+'logging','w')
		logfile.write('Subcode,Time,File,Address\n')
	else:
		logfile = open(path+'logging','a')
	
	fname = checkmultifile(path+fname)
#	print fname

	for i in range(0,2):
		if i ==0:
			openfile = open(fname,'wb')
		else:
			cutpos = fname.find('.csv')
			openfile = open(fname[:cutpos],'wb')

		sizef = c.recv(16)
#		print sizef
		#find END of message
		indEND = sizef.find("END")
		sizef1 = sizef[:indEND]
		
		#length of sizef1
		lensi1 = len(sizef1)
		
		sizef1 = long(sizef1)
		#nl = sizef1/1024
		#nl = nl+1
		#rem = sizef1%1024
		#print nl
		openfile.write(sizef[indEND+3:])

		begint = time.time()
		timeout = 10
		sizesum = len(sizef[indEND+3:])
#		while time.time()-begint < timeout:
		while sizesum < sizef1:
#			i = i+1
			mindata = min(1024,sizef1-sizesum)
			data = c.recv(mindata)
			
			#print data[len(data)-20:]
#			print "size total = ",sizesum
#			print data
			sizesum = sizesum+len(data)
			#pos = data.find("!DONE!")
			#if pos >0:
				
			if data[len(data)-6:] == "!DONE!":
				openfile.write(data[:len(data)-6])
				break
			openfile.write(data)

#		print long(sizef1)
	
		logfile.write(subjectcode+','+time.asctime()+','+fname+','+str(addr[0])+'\n')
		openfile.close()
	
	logfile.close()
	c.send('Thank you for connecting')
	c.close()                # Close the connection
