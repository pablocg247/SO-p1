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

struct COMANDO C[] = {
    {"fin", Cmd_fin, "fin\n\tTermina la ejecucion del shell."},
    {"exit", Cmd_fin, "exit\n\tTermina la ejecucion del shell."},
    {"quit", Cmd_fin, "quit\n\tTermina la ejecucion del shell."},
    {"bye", Cmd_fin, "bye\n\tTermina la ejecucion del shell."},

    {"pid", Cmd_pid, "pid [-p]\n\tMuestra el identificador de proceso (PID) del shell.\n\t-p\tMuestra el PID del proceso padre."},
    {"pwd", Cmd_pwd, "pwd\n\tMuestra la ruta absoluta del directorio de trabajo actual."},
    {"chdir", Cmd_chdir, "chdir [directorio]\n\tMuestra o cambia el directorio de trabajo actual del shell.\n\tSin argumentos, imprime el directorio actual."},

    {"autores", Cmd_autores, "autores [-l | -n]\n\tMuestra los nombres y logins de los autores del shell.\n\t-l\tMuestra unicamente los logins.\n\t-n\tMuestra unicamente los nombres."},
    {"authors", Cmd_autores, "authors [-l | -n]\n\tAlias para el comando 'autores'."},

    {"exec", Cmd_exec, "exec comando [argumentos...]\n\tSustituye la imagen del proceso actual del shell por la del programa especificado.\n\tEl shell no recuperara el control tras la ejecucion."},
    {"pplano", Cmd_pplano, "pplano comando [argumentos...]\n\tEjecuta un programa en primer plano (foreground). El shell esperara a que termine."},
    {"splano", Cmd_splano, "splano comando [argumentos...]\n\tEjecuta un programa en segundo plano (background). El shell continuara inmediatamente."},

    {"path", Cmd_path, "path [-add dir | -del dir | -show | -clear | -import]\n\tGestiona la lista de directorios donde el shell busca programas ejecutables.\n\t-add dir\tAnade un directorio a la lista.\n\t-del dir\tElimina un directorio de la lista.\n\t-show   \tMuestra la lista de rutas actual.\n\t-clear  \tVacia la lista de rutas.\n\t-import \tImporta las rutas definidas en la variable de entorno PATH."},
    {"importpath", Cmd_importpath, "importpath\n\tImporta las rutas definidas en la variable de entorno PATH. Equivalente a 'path -import'."},
    {"where", Cmd_where, "where comando\n\tBusca y muestra la ruta absoluta del ejecutable correspondiente al comando en los directorios del path."},

    {"date", Cmd_date, "date [-d | -t]\n\tMuestra la fecha y hora actual del sistema.\n\t-d\tMuestra unicamente la fecha (DD/MM/YYYY).\n\t-t\tMuestra unicamente la hora (HH:MM:SS)."},
    {"sysinfo", Cmd_sysinfo, "sysinfo\n\tMuestra informacion del sistema operativo y del hardware de la maquina (similar a uname -a)."},

    {"help", Cmd_help, "help [comando]\n\tMuestra una lista de todos los comandos disponibles en el shell.\n\tSi se especifica un comando, muestra informacion de ayuda detallada sobre el mismo."},

    {"open", Cmd_open, "open [fichero] [cr] [ex] [ro] [wo] [rw] [ap] [tr]\n\tAbre un fichero y anade su descriptor a la lista interna de ficheros abiertos.\n\tcr: O_CREAT\tex: O_EXCL\tro: O_RDONLY\two: O_WRONLY\n\trw: O_RDWR\tap: O_APPEND\ttr: O_TRUNC\n\tSin argumentos, lista los ficheros actualmente abiertos."},
    {"close", Cmd_close, "close df [-f]\n\tCierra el descriptor de fichero 'df' y lo elimina de la lista interna.\n\t-f\tFuerza el cierre incluso si el descriptor corresponde a un mapeo activo de memoria."},
    {"listopen", Cmd_listopen, "listopen\n\tMuestra la lista interna de ficheros y descriptores abiertos por el shell (similar a procstat o lsof)."},

    {"dup", Cmd_dup, "dup df\n\tDuplica el descriptor de fichero 'df' especificado y anade el nuevo descriptor resultante a la lista de ficheros abiertos."},
    {"lseek", Cmd_lseek, "lseek df offset referencia\n\tModifica el puntero de lectura/escritura del descriptor 'df'.\n\treferencia:\n\tSEEK_SET\tDesplazamiento relativo al inicio del fichero.\n\tSEEK_CUR\tDesplazamiento relativo a la posicion actual.\n\tSEEK_END\tDesplazamiento relativo al final del fichero."},
    {"readstr", Cmd_readstr, "readstr df bytes\n\tLee la cantidad especificada de 'bytes' del descriptor 'df' y los imprime por la salida estandar asumiendo que son texto."},
    {"writestr", Cmd_writestr, "writestr df cadena\n\tEscribe la 'cadena' de texto especificada en el descriptor 'df'."},

    {"makefile", Cmd_makefile, "makefile fichero\n\tCrea un fichero regular vacio con el nombre especificado."},
    {"makedir", Cmd_makedir, "makedir directorio\n\tCrea un directorio vacio con el nombre especificado."},

    {"delete", Cmd_delete, "delete objeto1 [objeto2 ...]\n\tElimina los objetos del sistema de ficheros indicados.\n\tLos directorios solo podran eliminarse si se encuentran completamente vacios."},
    {"deltree", Cmd_deltree, "deltree objeto1 [objeto2 ...]\n\tElimina los objetos indicados de forma recursiva.\n\tSi se especifica un directorio, se eliminara junto con todo su contenido (archivos y subdirectorios)."},

    {"listfile", Cmd_listfile, "listfile [-long] [-link] [-acc] f1 [f2 ...]\n\tMuestra informacion de los objetos indicados. Si se indica un directorio, se examina el propio directorio, no su contenido.\n\t-long\tMuestra informacion detallada (permisos, enlaces, inodo, propietario, tamano y fecha de modificacion/creacion).\n\t-acc \tSustituye la fecha de modificacion por la de ultimo acceso.\n\t-link\tSi el fichero es un enlace simbolico, muestra tambien el objeto al que apunta."},
    {"list", Cmd_list, "list [-reca] [-recb] [-hid] [-long] [-link] [-acc] f1 [f2 ...]\n\tLista el contenido de los directorios especificados (o del directorio actual si no se indican argumentos).\n\t-hid \tIncluye en el listado los ficheros ocultos (aquellos que comienzan por punto).\n\t-reca\tRecorrido recursivo: lista el contenido del directorio ANTES de descender a los subdirectorios.\n\t-recb\tRecorrido recursivo: lista el contenido del directorio DESPUES de descender a los subdirectorios.\n\t(-long, -link y -acc funcionan igual que en listfile)"},

    {NULL, NULL, NULL}
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
   char *tr[(MAXENTRADA/2) + 1];
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

   while (1) {
		printf("-> ");
		if (fgets(entrada, MAXENTRADA, stdin) == NULL) {
			printf("\n");
			break;
		}

		if (strchr(entrada, '\n') == NULL && !feof(stdin)) {
			int c;
			while ((c = getchar()) != '\n' && c != EOF);
			fprintf(stderr, "Error: comando demasiado largo (maximo %d caracteres)\n", MAXENTRADA - 1);
			continue;
		}

		ProcesarEntrada(entrada);
   }
}
