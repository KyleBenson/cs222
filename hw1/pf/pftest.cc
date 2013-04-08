#include <fstream>
#include <iostream>
#include <cassert>
#include <cstring>

#include "pf.h"

using namespace std;


void pfTest()
{
  PF_Manager *pf = PF_Manager::Instance();
  
  /** First, we test to see that file creation is happening properly. */
  assert(pf->CreateFile("testfile") == 0);
  fstream f;
  f.open("testfile");
  assert(f.is_open());
  f.close();

  /** Now, we can open the file and test it. */
  PF_FileHandle pff;
  assert(pf->OpenFile ("testfile", pff) == 0);

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
  assert(pff.AppendPage(buf) == 0);
  assert(pff.GetNumberOfPages() == 2);
  assert(pff.ReadPage(0, buf) == 0);
  assert(strcmp(buf, msg) == 0);

  assert(pff.AppendPage(buf) == 0);
  assert(pff.GetNumberOfPages() == 3);
  assert(pff.ReadPage(0, buf) == 0);
  assert(strcmp(buf, msg) == 0);


  /** This should only need to be called thru manager, as it defers to the handle. */
  assert(pf->CloseFile (pff) == 0);

  /** Test that file deletion works. */
  pf->DestroyFile("testfile");
  f.open("testfile");
  assert(!f.is_open());
}


int main() 
{
  cout << "test..." << endl;

  pfTest();
  // other tests go here

  cout << "OK" << endl;
}
