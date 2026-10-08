#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


#include "ejemplo.h"


#define MAXENTRADA 2048

struct COMANDO{
  char * nombre;
  void (*funcion)(char**);
  char * help;
};

/**************************SHELL**************************/

struct COMANDO C[]={
   {"fin",Cmd_fin,"fin: termina el shell"},
   {"exit",Cmd_fin,"exit: termina el shell"},
   {"quit",Cmd_fin,"quit: termina el shell"},
   {"bye",Cmd_fin,"bye: termina el shell"},
   {"pid",Cmd_pid,"pid: muestra el PID del shell \n" "pid -p: muestra el PID del proceso padre"},
   {"pwd", Cmd_pwd,"pwd: muestra el directorio actual"},
   {"chdir",Cmd_chdir,"chdir: muestra el directorio actual\n" "chdir directorio: cambia al directorio indicado"},
   {"autores",Cmd_autores,"autores: muestra nombres y logins\n" "autores -l: muestra solo los logins\n" "autores -n: muestra solo los nombres"},
   {"authors",Cmd_autores,"authors: muestra nombres y logins\n" "authors -l: muestra solo los logins\n" "authors -n: muestra solo los nombres"},
   {"exec",Cmd_exec,"exec programa [argumentos]: sustituye el shell por el programa"},
   {"pplano",Cmd_pplano,"pplano programa [argumentos]: ejecuta un programa en primer plano"},
   {"splano",Cmd_splano,"splano programa [argumentos]: ejecuta en segundo plano"},
   {"path",Cmd_path,"path: muestra las rutas de busqueda\n" "path -add directorio: anade una ruta\n" "path -del directorio: elimina una ruta\n" "path -show: muestra las rutas\n" "path -clear: vacia las rutas\n" "path -import: importa las rutas del entorno"},
   {"importpath",Cmd_importpath,"importpath: importa las rutas del entorno"},
   {"where",Cmd_where,"where programa: busca su ruta"},
   {"date",Cmd_date,"date: muestra fecha y hora\n" "date -d: muestra solo la fecha\n" "date -t: muestra solo la hora"},
   {"sysinfo",Cmd_sysinfo,"sysinfo: muestra informacion del sistema y de la maquina"},
   {"help",Cmd_help,"help: muestra los comandos disponibles\n" "help comando: muestra la ayuda del comando indicado"},
   {"open",Cmd_open,"open: muestra los ficheros abiertos\n" "open fichero [modos]: abre un fichero\n" "modos: cr ex ro wo rw ap tr"},
   {"close",Cmd_close,""},
   {"listopen",Cmd_listopen,""},
   {"dup",Cmd_dup,""},
   {"lseek", Cmd_lseek, "lseek df pos ref: cambia la posicion\n" "Referencias: SEEK_SET, SEEK_CUR, SEEK_END"},
   {"readstr", Cmd_readstr, "readstr df cont: lee bytes y los muestra como texto"},
   {"writestr", Cmd_writestr, "writestr df str: escribe una cadena sin espacios"},
   {"makefile", Cmd_makefile, "makefile nombre: crea un fichero vacio"},
   {"makedir", Cmd_makedir, "makedir nombre: crea un directorio"},
   {"delete", Cmd_delete, "delete nombre1 nombre2 ...: elimina ficheros, enlaces y directorios vacios"},
   {"deltree", Cmd_deltree, "deltree nombre1 nombre2 ...: elimina directorios con todo su contenido"},
   {"listfile",Cmd_listfile,""},
   {"list",Cmd_list,""},
   {NULL,NULL,NULL},
  };

void DecidirComando (char *tr[])
{
  int i;

  if (tr[0]==NULL) return; /*superfluo, por si cambio otras cosas */
  for (i=0; C[i].nombre!=NULL; i++)
    if (!strcmp(C[i].nombre,tr[0])){
        (*C[i].funcion)(tr+1);
	    return;
    }
  Cmd_pplano(tr); /*si no es un comando, es un  ejecutable externo: en primer plano*/
}

int TrocearCadena(char * cadena, char * trozos[])
{
  int i=1;
  if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
      return 0;
  while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
     i++;
  return i;
}	
void ProcesarEntrada(char * entrada)
{
   char *tr[MAXENTRADA/2];
   if (TrocearCadena(entrada,tr)==0) /*no hay nada*/
	return;
   DecidirComando(tr);
}

int  main(int argc, char *argv[], char *ent[])
{
   char entrada[MAXENTRADA];

   if (argv[1]==NULL)
        printf ("Ejecutando con path vacio: %s -p para importar el path\n",argv[0]);
   else if (!strcmp(argv[1],"-p"))
        Cmd_importpath(NULL);

   while (1){
      printf ("-> ");
      fgets(entrada,MAXENTRADA,stdin);
      ProcesarEntrada(entrada);
   }
}
