#include "pf.h"
using namespace std;

PF_Manager* PF_Manager::_pf_manager = 0;


PF_Manager* PF_Manager::Instance()
{
    if(!_pf_manager)
        _pf_manager = new PF_Manager();
    
    return _pf_manager;    
}


PF_Manager::PF_Manager()
{
}


PF_Manager::~PF_Manager()
{
}

    
RC PF_Manager::CreateFile(const char *fileName)
{
  /** Open a file as write to erase one that was there before and just close it to save changes. */
  fstream file;
  file.open(fileName);
  file.close();
  return 0;
}


RC PF_Manager::DestroyFile(const char *fileName)
{
  return remove(fileName);
}


RC PF_Manager::OpenFile(const char *fileName, PF_FileHandle &fileHandle)
{
  return fileHandle.OpenFile(fileName);
}


RC PF_Manager::CloseFile(PF_FileHandle &fileHandle)
{
  return fileHandle.CloseFile();
}


PF_FileHandle::PF_FileHandle()
{
}
 

PF_FileHandle::~PF_FileHandle()
{
}


RC PF_FileHandle::ReadPage(PageNum pageNum, void *data)
{
  _file.seekg (pageNum * PF_PAGE_SIZE);
  _file.read ((char *)data, PF_PAGE_SIZE);
  return 0;
}


RC PF_FileHandle::WritePage(PageNum pageNum, const void *data)
{
  _file.seekp (pageNum * PF_PAGE_SIZE);
  _file.write ((const char *)data, PF_PAGE_SIZE);
  return 0;
}


RC PF_FileHandle::AppendPage(const void *data)
{
  return WritePage (PF_PAGE_SIZE * npages++, data);
}


unsigned PF_FileHandle::GetNumberOfPages()
{
  return npages;
}


RC PF_FileHandle::OpenFile(const char * filename)
{
  //we want the file open for both input and output, binary mode
  //set it to seek to end after opening so we know how many pages are in file
  _file.open (filename, ios_base::in | ios_base::out | ios_base::ate);
  npages = _file.tellg() / PF_PAGE_SIZE;
  return 0;
}


RC PF_FileHandle::CloseFile()
{
  _file.close();
  return 0;
}
