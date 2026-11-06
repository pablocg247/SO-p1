/*
    Miembros:   Mauro Fernandez Perez   Pablo Carril Gontan
    Logins:     mauro.fernandez.perez   p.carril
*/


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
    {"fin", Cmd_fin,
        "fin: Termina el shell."},

    {"exit", Cmd_fin,
        "exit: Termina el shell."},

    {"quit", Cmd_fin,
        "quit: Termina el shell."},

    {"bye", Cmd_fin,
        "bye: Termina el shell."},

    {"pid", Cmd_pid,
        "pid: [-p] Muestra el PID del shell.\n"
        "\t-p: muestra el PID del proceso padre."},

    {"chdir", Cmd_chdir,
        "chdir: [dir] Cambia el directorio actual; sin argumentos, lo muestra.\n"
        "\tdir: directorio al que se quiere cambiar."},

    {"autores", Cmd_autores,
        "autores: [-l|-n] Muestra los nombres y logins de los autores.\n"
        "\t-l: muestra solo los logins.\n"
        "\t-n: muestra solo los nombres."},

    {"authors", Cmd_autores,
        "authors: [-l|-n] Muestra los nombres y logins de los autores.\n"
        "\t-l: muestra solo los logins.\n"
        "\t-n: muestra solo los nombres."},

    {"exec", Cmd_exec,
        "exec: programa [args...] Sustituye el shell por el programa si la ejecucion tiene exito.\n"
        "\tprograma: nombre o ruta del programa que se quiere ejecutar.\n"
        "\targs...: argumentos que se pasan al programa."},

    {"pplano", Cmd_pplano,
        "pplano: programa [args...] [&] Ejecuta un programa y espera a que termine.\n"
        "\tprograma: nombre o ruta del programa que se quiere ejecutar.\n"
        "\targs...: argumentos que se pasan al programa.\n"
        "\t&: al final, ejecuta el programa en segundo plano, sin esperar."},

    {"splano", Cmd_splano,
        "splano: programa [args...] Ejecuta un programa en segundo plano, sin esperar a que termine.\n"
        "\tprograma: nombre o ruta del programa que se quiere ejecutar.\n"
        "\targs...: argumentos que se pasan al programa."},

    {"path", Cmd_path,
        "path: [-add dir|-del dir|-show|-clear|-import] Gestiona las rutas de busqueda; sin argumentos, las muestra.\n"
        "\t-add dir: anade un directorio a las rutas de busqueda.\n"
        "\t-del dir: elimina un directorio de las rutas de busqueda.\n"
        "\t-show: muestra las rutas de busqueda.\n"
        "\t-clear: vacia la lista de rutas de busqueda.\n"
        "\t-import: incorpora las rutas de la variable de entorno PATH."},

    {"importpath", Cmd_importpath,
        "importpath: Incorpora las rutas de la variable de entorno PATH a la lista de busqueda."},

    {"where", Cmd_where,
        "where: programa Busca un ejecutable y muestra su ruta; si no lo encuentra, muestra el nombre recibido.\n"
        "\tprograma: nombre del ejecutable que se quiere localizar."},

    {"date", Cmd_date,
        "date: [-d|-t] Muestra la hora y la fecha en lineas separadas.\n"
        "\t-d: muestra solo la fecha, en formato dia/mes/anio.\n"
        "\t-t: muestra solo la hora, en formato horas:minutos:segundos."},

    {"sysinfo", Cmd_sysinfo,
        "sysinfo: Muestra el nombre de la maquina, su arquitectura y la informacion del sistema operativo."},

    {"help", Cmd_help,
        "help: [cmd] Muestra los comandos internos disponibles o la ayuda de uno concreto.\n"
        "\tcmd: nombre del comando cuya ayuda se quiere consultar."},

    {"open", Cmd_open,
        "open: [fichero [modos...]] Abre un fichero y lo registra; sin argumentos, muestra los descriptores registrados.\n"
        "\tfichero: nombre o ruta del fichero que se quiere abrir.\n"
        "\tcr: crea el fichero si no existe (O_CREAT).\n"
        "\tex: junto con cr, falla si el fichero ya existe (O_EXCL).\n"
        "\tro: abre para lectura; es el acceso por defecto (O_RDONLY).\n"
        "\two: abre para escritura (O_WRONLY).\n"
        "\trw: abre para lectura y escritura (O_RDWR).\n"
        "\tap: situa cada escritura al final del fichero (O_APPEND).\n"
        "\ttr: trunca el fichero al abrirlo para escritura (O_TRUNC)."},

    {"close", Cmd_close,
        "close: [df [-f]] Cierra un descriptor y lo elimina de la lista; sin argumentos, muestra la lista.\n"
        "\tdf: descriptor que se quiere cerrar; si es negativo, muestra la lista.\n"
        "\t-f: fuerza el cierre si el descriptor corresponde a un mapeo activo."},

    {"listopen", Cmd_listopen,
        "listopen: Muestra los descriptores registrados, incluidos los heredados, con su nombre, modo y posicion cuando sea consultable."},

    {"dup", Cmd_dup,
        "dup: [df] Duplica un descriptor, compartiendo la posicion del fichero; sin argumentos, muestra la lista.\n"
        "\tdf: descriptor que se quiere duplicar; si es negativo, muestra la lista."},

    {"lseek", Cmd_lseek,
        "lseek: df pos [ref] Cambia la posicion de lectura y escritura de un descriptor.\n"
        "\tdf: descriptor del fichero cuya posicion se quiere cambiar.\n"
        "\tpos: desplazamiento en bytes respecto a la referencia indicada.\n"
        "\tref: SEEK_SET desde el inicio, SEEK_CUR desde la posicion actual o SEEK_END desde el final; por defecto, SEEK_SET."},

    {"readstr", Cmd_readstr,
        "readstr: df cont Lee desde la posicion actual y muestra la cantidad de bytes leidos y el texto.\n"
        "\tdf: descriptor del que se quiere leer.\n"
        "\tcont: cantidad maxima de bytes que se quieren leer; admite cero."},

    {"writestr", Cmd_writestr,
        "writestr: df texto... Escribe texto sin anadir un salto de linea ni un terminador nulo.\n"
        "\tdf: descriptor abierto con permiso de escritura.\n"
        "\ttexto...: palabras que se escriben separadas por un espacio."},

    {"makefile", Cmd_makefile,
        "makefile: [nombre] Crea un fichero vacio; sin argumentos, muestra el directorio actual.\n"
        "\tnombre: ruta del fichero que se quiere crear; falla si ya existe."},

    {"makedir", Cmd_makedir,
        "makedir: [nombre] Crea un directorio; sin argumentos, muestra el directorio actual.\n"
        "\tnombre: ruta del directorio que se quiere crear; su directorio padre debe existir."},

    {"delete", Cmd_delete,
        "delete: [nombres...] Elimina ficheros, enlaces o directorios vacios; sin argumentos, muestra el directorio actual.\n"
        "\tnombres...: rutas de los elementos que se quieren eliminar."},

    {"deltree", Cmd_deltree,
        "deltree: [nombres...] Elimina ficheros o directorios con todo su contenido; sin argumentos, muestra el directorio actual.\n"
        "\tnombres...: rutas de los elementos que se quieren eliminar recursivamente."},

    {"listfile", Cmd_listfile,
        "listfile: [-long] [-link] [-acc] [nombres...] Muestra informacion de los elementos indicados; sin nombres, muestra el directorio actual.\n"
        "\t-long: muestra permisos, enlaces, inodo, propietario, grupo, tamanio y fecha.\n"
        "\t-link: muestra tambien el destino de los enlaces simbolicos.\n"
        "\t-acc: usa la fecha del ultimo acceso en el formato largo.\n"
        "\tnombres...: rutas que se quieren consultar; para un directorio, muestra sus datos, no su contenido."},

    {"list", Cmd_list,
        "list: [-reca] [-recb] [-hid] [-long] [-link] [-acc] [nombres...] Lista ficheros o contenidos de directorios; sin nombres, muestra el directorio actual.\n"
        "\t-reca: recorre los subdirectorios despues de listar el directorio actual.\n"
        "\t-recb: recorre los subdirectorios antes de listar el directorio actual; tiene prioridad sobre -reca.\n"
        "\t-hid: incluye las entradas ocultas.\n"
        "\t-long: muestra permisos, enlaces, inodo, propietario, grupo, tamanio y fecha.\n"
        "\t-link: muestra tambien el destino de los enlaces simbolicos.\n"
        "\t-acc: usa la fecha del ultimo acceso en el formato largo.\n"
        "\tnombres...: rutas que se quieren listar; usa . para listar el contenido del directorio actual."},
        
    {"read", Cmd_read,
        "read: df addr cont Lee bytes de un descriptor y los guarda en memoria.\n"
        "\tdf: descriptor abierto para lectura.\n"
        "\taddr: direccion de destino en hexadecimal.\n"
        "\tcont: numero de bytes en decimal."},

    {"write", Cmd_write,
        "write: df addr cont Escribe bytes de memoria en un descriptor.\n"
        "\tdf: descriptor abierto para escritura.\n"
        "\taddr: direccion de origen en hexadecimal.\n"
        "\tcont: numero de bytes en decimal."},

    {"recurse", Cmd_recurse,
        "recurse: n Ejecuta una funcion recursiva y muestra sus direcciones.\n"
        "\tn: valor inicial no negativo; la recursion desciende hasta cero."},

    {"memfill", Cmd_memfill,
      "memfill: addr cont hcode Rellena memoria con el byte indicado.\n"
      "\taddr: direccion inicial en hexadecimal.\n"
      "\tcont: numero de bytes en decimal.\n"
      "\thcode: byte decimal o hexadecimal con prefijo 0x (0 a 255)."},
      
    {"i-o", Cmd_io,
      "i-o: read|write ... Transfiere bytes entre un fichero y memoria.\n"
      "\tread fichero addr cont: lee del fichero hacia memoria.\n"
      "\twrite [-o] fichero addr cont: escribe desde memoria al fichero.\n"
      "\t-o: permite sobrescribir y vacia el contenido anterior.\n"
      "\tfichero: nombre o ruta del fichero.\n"
      "\taddr: direccion de memoria en hexadecimal.\n"
      "\tcont: cantidad de bytes en decimal."},

    {"memdump", Cmd_memdump,
      "memdump: addr cont Vuelca en pantalla el contenido de memoria en hexadecimal y caracteres.\n"
      "\taddr: direccion inicial en hexadecimal.\n"
      "\tcont: numero de bytes en decimal."},

    {"allocate", Cmd_allocate,
      "allocate: [-malloc|-shared|-createshared|-mmap]... Asigna un bloque de memoria; sin argumentos, muestra la lista.\n"
      "\t-malloc tam: asigna un bloque malloc de tamano tam.\n"
      "\t-createshared cl tam: crea y asigna un bloque de memoria compartida de clave cl y tamano tam.\n"
      "\t-shared cl: asigna un bloque de memoria compartida ya existente de clave cl.\n"
      "\t-mmap fich perm: mapea el fichero fich en memoria con permisos perm."},

    {"deallocate", Cmd_deallocate,
      "deallocate: [-malloc|-shared|-delkey|-mmap|addr]... Desasigna un bloque de memoria; sin argumentos, muestra la lista.\n"
      "\t-malloc tam: desasigna el bloque malloc de tamano tam.\n"
      "\t-shared cl: desasigna el bloque de memoria compartida de clave cl.\n"
      "\t-mmap fich: desmapea el fichero mapeado fich.\n"
      "\t-delkey cl: elimina del sistema la clave de memoria compartida cl sin desmapear.\n"
      "\taddr: desasigna el bloque de memoria situado en la direccion addr."},

    {"memory", Cmd_memory,
      "memory: [-blocks|-funcs|-vars|-all|-pmap] Muestra informacion sobre la memoria del proceso.\n"
      "\t-blocks: muestra la lista de bloques de memoria asignados.\n"
      "\t-funcs: muestra las direcciones de funciones del programa y de libreria.\n"
      "\t-vars: muestra las direcciones de variables locales, globales y estaticas.\n"
      "\t-all: muestra la informacion de -blocks, -funcs y -vars.\n"
      "\t-pmap: muestra el mapa de memoria del proceso mediante pmap o equivalente."},

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
   if (OpenFilesInit() == -1) {
		OpenFilesClear();
		return EXIT_FAILURE;
   }	

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
			printf("Error: comando demasiado largo (maximo %d caracteres)\n", MAXENTRADA - 1);
			continue;
		}

		ProcesarEntrada(entrada);
   }
	MemListClear();
    OpenFilesClear();
    PathClear();
}
