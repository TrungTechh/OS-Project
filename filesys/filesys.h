// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  The disk is simulated using the native UNIX 
//	file system (in a file named "DISK"). 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.  
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"
#include "debug.h"

#define FILE_MAX 10
//#define MAX_PROCESS 10

#ifdef FILESYS_STUB 		// Temporarily implement file system calls as 
				// calls to UNIX, until the real file system
				// implementation is available
class FileSystem {
  public:

  	OpenFile** openFile; 
	char** openFileName;
    FileSystem() {
        openFile = new OpenFile*[FILE_MAX];
        openFileName = new char*[FILE_MAX];
        for (int i = 0; i < FILE_MAX; i++) {
            openFile[i] = NULL;
            openFileName[i] = NULL;
	    this->Create("stdin", 0);
	    this->Create("stdout", 0);
	    openFileName[0] = "stdin";
	    openFile[0] = this->Open("stdin");
	    openFileName[1] = "stdout";
	    openFile[1] = this->Open("stdout");
        }
        
    }
    
	~FileSystem() {
		for (int i = 0; i < FILE_MAX; i++) {
            if (openFile[i] != NULL)
                delete openFile[i];

            if (openFileName[i] != NULL)
                openFileName[i] = NULL;
        }

        delete[] openFile;
        delete[] openFileName;
	}
	
    bool Create(char *name, int initialSize) {
        int fileDescriptor = OpenForWrite(name);

        if (fileDescriptor == -1) return FALSE;
        Close(fileDescriptor);
        return TRUE;
    }
	
    OpenFile* Open(char *name,int type = 0){
		int fileDescriptor = OpenForReadWrite(name, FALSE);

	  	if (fileDescriptor == -1) return NULL;
	  	return new OpenFile(fileDescriptor,type);
	}
	int FindFreeSlot() {
        for (int i = 2; i < FILE_MAX; i++) {
            if (openFile[i] == NULL) return i;
        }
        return -1;
    }

	bool isOpen(char* filename) {
        for(int i = 2; i < FILE_MAX; i++) {
            if(openFileName[i] != NULL && !strcmp(filename, openFileName[i])) {
                return true;
            }
        }

        return false;
    }

	bool Close(int id){
		if (id < 2 || id > FILE_MAX) return FALSE;
        if (openFile[id]) {
			delete openFileName[id];
            openFileName[id] = NULL;
			delete openFile[id];
            openFile[id] = NULL;
			DEBUG(dbgSys, "\nClosing file with ID - " << id << " successfully!");
            return TRUE;
        }
		DEBUG(dbgSys, "\nFile with ID - " << id << " has already closed!");
        return FALSE;
	}

    bool Remove(char *name) { return Unlink(name) == 0; }

	int Read(char* buffer, int charCount, int index){
		if (index < 0 ||index >= FILE_MAX) return -1;
        if (openFile[index] == NULL) return -1;
        int result = openFile[index]->Read(buffer, charCount);
        if (result != charCount) return -2;
        return result;
	}

	int Write(char* buffer, int charCount, int index){
		if (index < 0 ||index >= FILE_MAX) return -1;
        if (openFile[index] == NULL) return -1;
        
        return openFile[index]->Write(buffer, charCount);
	}
	
	int Seek(int pos, int id){
		if (id <= 1 || id >= FILE_MAX) return -1;
        if (openFile[id] == NULL) return -1;
        // use seek(-1) to move to the end of file
        if (pos == -1) pos = openFile[id]->Length();
        if (pos < 0 || pos > openFile[id]->Length()) return -1;
        return openFile[id]->Seek(pos);
	}

};

#else // FILESYS
class FileSystem {
  public:
    FileSystem(bool format);		// Initialize the file system.
					// Must be called *after* "synchDisk" 
					// has been initialized.
    					// If "format", there is nothing on
					// the disk, so initialize the directory
    					// and the bitmap of free blocks.

    bool Create(char *name, int initialSize);  	
					// Create a file (UNIX creat)

    OpenFile* Open(char *name, int type = 0); 	// Open a file (UNIX open)

	int FindFreeSlot();

    bool isOpen(char* filename);

    bool Remove(char *name);  		// Delete a file (UNIX unlink)

    void List();			// List all the files in the file system

    void Print();			// List all the files and their contents

  private:
   OpenFile* freeMapFile;		// Bit map of free disk blocks,
					// represented as a file
   OpenFile* directoryFile;		// "Root" directory -- list of 
					// file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
