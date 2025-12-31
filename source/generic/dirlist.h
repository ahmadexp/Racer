class direntry
{
public:
  char filename[MAXPATHLEN];
  bool folder;
  bool statfailed;
  int extrainfoint;
  direntry(char* filename2);
};

direntry::direntry(char* filename2)
{
#ifdef _WIN32
  sprintf_s(filename, sizeof(filename), "%s", filename2);
#else
  snprintf(filename, sizeof(filename), "%s", filename2);
#endif
  folder = 0;
  extrainfoint = 0;
}

const int maxdirentries = 500;
direntry *direntries[maxdirentries];
int n_direntries = 0;

void deletedirentry(int number)
{
  delete direntries[number];
  direntries[number] = NULL;
  if (number < n_direntries-1)
  {
    for(int a = number; a < n_direntries; a++)
    {
      direntries[a] = direntries[a+1];
    }
  }
  direntries[n_direntries-1] = NULL;
  n_direntries = n_direntries-1;
}

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

void dirlist(char* directory)
{
  while(n_direntries) deletedirentry(0);
  
#ifdef _WIN32
  char filename[MAXPATHLEN];
  WIN32_FIND_DATAA findFileData;
  HANDLE hFind = FindFirstFileA(directory, &findFileData);
  if(hFind != INVALID_HANDLE_VALUE)
  {
    bool endoflist = 0;
    while(endoflist == 0)
    {
      sprintf_s(filename, sizeof(filename), "%s", findFileData.cFileName);

      direntries[n_direntries] = new direntry((char*)filename);
      n_direntries++;
      
      if(findFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
      {
        direntries[n_direntries-1]->folder = 1;
      }
      
      if(FindNextFileA(hFind, &findFileData) == 0)
      {
        endoflist = 1;
      }
    }
    FindClose(hFind);
  }
#else
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (directory)) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      if (n_direntries >= maxdirentries) break;
      
      direntries[n_direntries] = new direntry(ent->d_name);
      
      // Check if it's a directory
      struct stat st;
      char path[MAXPATHLEN];
      snprintf(path, sizeof(path), "%s/%s", directory, ent->d_name);
      if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
          direntries[n_direntries]->folder = 1;
        }
      }
      
      n_direntries++;
    }
    closedir (dir);
  }
#endif
}

