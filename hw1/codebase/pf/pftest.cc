#include <iostream>
#include <string>
#include <cassert>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <sys/stat.h>

#include "pf.h"

using namespace std;

const int success = 0;


// Check if a file exists
bool FileExists(string fileName)
{
    struct stat stFileInfo;

    if(stat(fileName.c_str(), &stFileInfo) == 0) return true;
    else return false;
}

int PFTest_1(PF_Manager *pf)
{
    // Functions Tested:
    // 1. CreateFile
    cout << "****In PF Test Case 1****" << endl;

    RC rc;
    string fileName = "test";

    // Create a file named "test"
    rc = pf->CreateFile(fileName.c_str());
    assert(rc == success);

    if(FileExists(fileName.c_str()))
    {
        cout << "File " << fileName << " has been created." << endl << endl;
        return 0;
    }
    else
    {
        cout << "Failed to create file!" << endl;
        return -1;
    }

    // Create "test" again, should fail
    rc = pf->CreateFile(fileName.c_str());
    assert(rc != success);

    cout << "Test Case 1 passsed!" << std::endl;
    return 0;
}

int PFTest_2(PF_Manager *pf)
{
    // Functions Tested:
    // 1. OpenFile
    // 2. AppendPage
    // 3. GetNumberOfPages
    // 4. WritePage
    // 5. ReadPage
    // 6. CloseFile
    // 7. DestroyFile
    cout << "****In PF Test Case 2****" << endl;

    RC rc;
    string fileName = "test";

    // Open the file "test"
    PF_FileHandle fileHandle;
    rc = pf->OpenFile(fileName.c_str(), fileHandle);
    assert(rc == success);
    
    // Append the first page
    // Write ASCII characters from 32 to 125 (inclusive)
    void *data = malloc(PF_PAGE_SIZE);
    for(unsigned i = 0; i < PF_PAGE_SIZE; i++)
    {
        *((char *)data+i) = i % 94 + 32;
    }
    rc = fileHandle.AppendPage(data);
    assert(rc == success);
   
    // Get the number of pages
    unsigned count = fileHandle.GetNumberOfPages();
    assert(count == (unsigned)1);

    // Update the first page
    // Write ASCII characters from 32 to 41 (inclusive)
    data = malloc(PF_PAGE_SIZE);
    for(unsigned i = 0; i < PF_PAGE_SIZE; i++)
    {
        *((char *)data+i) = i % 10 + 32;
    }
    rc = fileHandle.WritePage(0, data);
    assert(rc == success);

    // Read the page
    void *buffer = malloc(PF_PAGE_SIZE);
    rc = fileHandle.ReadPage(0, buffer);
    assert(rc == success);

    // Check the integrity
    rc = memcmp(data, buffer, PF_PAGE_SIZE);
    assert(rc == success);
 
    // Close the file "test"
    rc = pf->CloseFile(fileHandle);
    assert(rc == success);

    free(data);
    free(buffer);

    // DestroyFile
    rc = pf->DestroyFile(fileName.c_str());
    assert(rc == success);
    
    if(!FileExists(fileName.c_str()))
    {
        cout << "File " << fileName << " has been destroyed." << endl;
        cout << "Test Case 2 Passed!" << endl << endl;
        return 0;
    }
    else
    {
        cout << "Failed to destroy file!" << endl;
        return -1;
    }
}

int PFTest_3(PF_Manager *pf)
{
  cout << "****In PF Test Case 3****" << endl;

  /** First, we test to see that file creation is happening properly. */
  assert(pf->CreateFile("test") == 0);
  fstream f;
  assert(FileExists("test"));

  /** Now, we can open the file and test it. */
  PF_FileHandle pff;
  assert(pf->OpenFile ("test", pff) == 0);

  /** It should start with 0 pages. */
  assert(pff.GetNumberOfPages() == 0);
  
  /** Use this buffer to write in/read out pages. */
  char buf[PF_PAGE_SIZE];
  char * msg = "this is a test sentence to put in the page";
  memcpy(buf, msg, strlen(msg)+1);

  /** Try appending a page and make sure it was updated properly. */
  assert(pff.AppendPage(buf) == 0);
  assert(pff.GetNumberOfPages() == 1);

  assert(pff.ReadPage(0, buf) == 0);
  assert(strcmp(buf, msg) == 0);

  /** Now append two more pages, change the one in the middle, and verify everything looks good. */
  memcpy(buf, msg, strlen(msg)+1);
  assert(pff.AppendPage(buf) == 0);
  assert(pff.GetNumberOfPages() == 2);
  assert(pff.ReadPage(1, buf) == 0);
  assert(strcmp(buf, msg) == 0);

  memcpy(buf, msg, strlen(msg)+1);
  assert(pff.AppendPage(buf) == 0);
  assert(pff.GetNumberOfPages() == 3);
  assert(pff.ReadPage(2, buf) == 0);
  assert(strcmp(buf, msg) == 0);

  msg = "this is another test sentence to put in the modified page";
  memcpy(buf, msg, strlen(msg)+1);
  assert(pff.WritePage(1, buf) == 0);
  assert(pff.ReadPage(1, buf) == 0);
  assert(strcmp(buf, msg) == 0);

  /** Change the last page just to check that edge case. */
  msg = "this is yet another test sentence to put in the modified page";
  memcpy(buf, msg, strlen(msg)+1);
  assert(pff.WritePage(2, buf) == 0);
  assert(pff.ReadPage(2, buf) == 0);
  assert(strcmp(buf, msg) == 0);

  /** This should only need to be called thru manager, as it defers to the handle. */
  assert(pf->CloseFile (pff) == 0);

  /** Test that file deletion works. */
  pf->DestroyFile("test");
  assert(!FileExists("test"));
  
  cout << "Test Case 3 passsed!" << std::endl;
}

int main()
{
    PF_Manager *pf = PF_Manager::Instance();
    remove("test");
   
    PFTest_1(pf); 
    PFTest_2(pf); 
    PFTest_3(pf); 

    return 0;
}
