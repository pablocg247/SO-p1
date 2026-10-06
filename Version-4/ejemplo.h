#ifndef EJEMPLO_H
#define EJEMPLO_H

#include "openfileslist.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>

#include "path.h"

#define MAXNOMBRE 1024

void Cmd_fin (char * arg[]);
void Cmd_autores(char *arg[]);
void Cmd_exec (char *arg[]);
void Cmd_splano (char *arg[]);
void Cmd_pplano (char *arg[]);
void Cmd_chdir (char * arg[]);
void Cmd_pwd(char * arg[]);
void Cmd_pid (char * arg[]);
void Cmd_path (char * arg[]);
void Cmd_importpath (char *arg[]);
void Cmd_where (char *args[]);
void Cmd_date(char *arg[]);
void Cmd_authors(char *arg[]);
void Cmd_sysinfo(char *arg[]);
void Cmd_help(char *arg[]);
void Cmd_open (char * tr[]);
void Cmd_close (char * tr[]);
void Cmd_listopen (char * tr[]);
void Cmd_dup (char * tr[]);
void Cmd_lseek(char *arg[]);
void Cmd_readstr(char *arg[]);
void Cmd_writestr(char *arg[]);
void Cmd_makefile(char *arg[]);
void Cmd_makedir(char *arg[]);
void Cmd_delete(char *arg[]);
void Cmd_deltree(char *arg[]);


#endif
