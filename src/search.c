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
#define FILE_PROTECTIONS  0b010000
#define FILE_ALL          0b100000



void show_file_info(const char* path, int flags){
  struct stat statbuf;
  stat(path, &statbuf);
  if (flags & FILE_LAST_ACCESS){
    printf("File last access is: %ld\n", statbuf.st_ctim.tv_sec);
  }
  if (flags & FILE_LAST_MOD){
    printf("File last modification is: %ld\n", statbuf.st_atim.tv_sec);
  }
  if (flags & FILE_SIZE){
    printf("File size is: %ld bytes\n", statbuf.st_size);
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
  if (flags & FILE_PROTECTIONS){
    char perm[10];
    mode_t mode = statbuf.st_mode;
    perm[0] = (mode & S_IRUSR) ? 'r' : '-';
    perm[1] = (mode & S_IWUSR) ? 'w' : '-';
    perm[2] = (mode & S_IXUSR) ? 'x' : '-';
    perm[3] = (mode & S_IRGRP) ? 'r' : '-';
    perm[4] = (mode & S_IWGRP) ? 'w' : '-';
    perm[5] = (mode & S_IXGRP) ? 'x' : '-';
    perm[6] = (mode & S_IROTH) ? 'r' : '-';
    perm[7] = (mode & S_IWOTH) ? 'w' : '-';
    perm[8] = (mode & S_IXOTH) ? 'x' : '-';
    perm[9] = '\0';
    printf("File permissions: %s\n", perm);
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
  while ((opt = getopt(argc, argv, "w:f:l:dtmspa")) != -1){
    switch (opt){
      case 'w': strncpy(dir, optarg, 256); break;
      case 'f': strncpy(file_name_pattern, optarg, 256); break;
      case 'l': level = atoi(optarg); break;
      case 'd': extra_flags |= FILE_LAST_ACCESS; break;
      case 't': extra_flags |= FILE_TYPE; break;
      case 'm': extra_flags |= FILE_LAST_MOD; break;
      case 's': extra_flags |= FILE_SIZE; break;
      case 'p': extra_flags |= FILE_PROTECTIONS; break;
      case 'a': extra_flags = FILE_LAST_ACCESS | FILE_LAST_MOD | FILE_TYPE | FILE_SIZE | FILE_PROTECTIONS;break;
      default:
              exit(1);
    }
  }
  
  find(dir, file_name_pattern, level, extra_flags);

  exit(0);
}
