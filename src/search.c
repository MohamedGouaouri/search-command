#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fnmatch.h>


#define FILE_LAST_ACCESS  0b000001
#define FILE_LAST_MOD     0b000010
#define FILE_SIZE         0b000100
#define FILE_TYPE         0b001000
#define FILE_PROTECTIONs  0b010000
#define FILE_ALL          0b100000



void show_file_info(const char* path, int flags){
  struct stat statbuf;
  stat(path, &statbuf);
  if (flags & FILE_LAST_ACCESS){
    printf("File last access is: %ld\n", statbuf.st_ctim.tv_sec);
  }
  if (flags & FILE_TYPE){
    if ((statbuf.st_mode & S_IFMT) == S_IFREG)
      printf("File type is: Regular\n");
    if ((statbuf.st_mode & S_IFMT) == S_IFSOCK)
      printf("Socket file\n");
    if ((statbuf.st_mode & S_IFMT) == S_IFLNK)
      printf("Symbolic link file\n");
    if ((statbuf.st_mode & S_IFMT) == S_IFBLK)
      printf("Block device file\n");
    if ((statbuf.st_mode & S_IFMT) == S_IFCHR)
      printf("Character device\n");
  }
}




void find(const char *name, const char* file_to_find, int level, int flags)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)) || level == -1)
        // le cas particulier
        return;

    while ((entry = readdir(dir)) != NULL) {
        
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            level--;
            find(path, file_to_find, level, flags);
        } else {
          if (fnmatch(file_to_find, entry->d_name, 0) == 0){
            printf("File found in %s\n", name);
            char path_to_file[1024];
            snprintf(path_to_file, sizeof(path_to_file), "%s/%s", name, entry->d_name);
            show_file_info(path_to_file, flags);

          }
        }
    }
    closedir(dir);
}




void printUsage(){
  
}

int main(int argc, char** argv){
  int opt;
  int level;
  char file_name_pattern[256];
  char dir[256];
  int extra_flags;
  // default dir
  strncpy(dir, ".", 256);
  
  // parse options
  while ((opt = getopt(argc, argv, "w:f:l:dt")) != -1){
    switch (opt){
      case 'w': strncpy(dir, optarg, 256); break;
      case 'f': strncpy(file_name_pattern, optarg, 256); break;
      case 'l': level = atoi(optarg); break;
      case 'd': extra_flags |= FILE_LAST_ACCESS; break;
      case 't': extra_flags |= FILE_TYPE; break;
      default:
              exit(1);
    }
  }
  
  find(dir, file_name_pattern, level, extra_flags);

  exit(0);
}
