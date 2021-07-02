## search-command
A simple command for Linux system similar to find command designed as a homework assignment

## How to use it

First you need to clone the repo:
```bash
git clone https://github.com/MohamedGouaouri/search-command
cd search-command
```
### Build
```bash
make
```

### How to works
The general usecase is the following:
``` bash
search -w <dir> -l <level> -f <file name pattern> -dtmsp|a
```
where:
``` 
-w <dir> to specify the directory to start searching from
-l <level> to specify the level
-f file name pattern
-d for file last access time
-t for file type
-m for file last modification time
-s for file size
-p to show file permissions
-a to show all the above information
```
### Example
```
export $PATH=$PATH:./search
.search -w . -l 2 -a -f "example*"
```
