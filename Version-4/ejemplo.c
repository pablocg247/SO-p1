	#include "ejemplo.h"
	//#include "openfileslist.h"
	
void AniadirAlPath(char *dir)
{
    if (dir==NULL)
        PathPrint();
    else if (PathAdd(dir)==-1)
        perror("Imposible aniadir");
}

void EliminarDelPath(char *dir)
{
    if (dir==NULL)
        PathPrint();
    else if (PathDel(dir)==-1)
        perror("Imposible eliminar");
}


void MostrarDirActual()
{
   char dir[MAXNOMBRE];
   
    if (getcwd(dir,MAXNOMBRE)==NULL)
	perror("Imposible obtener directorio");
    else
       printf ("%s\n",dir);
}

int ComprobarSegundoPlano (char *tr[])
{
    int i;
    for (i=0; tr[i]!=NULL;i++)
        if (!strcmp(tr[i],"&")){ /*& indica segundo plano*/ 
            tr[i]=NULL;         /*es el ultimo argumento*/
            return i;       /*si solo hay un & no se ejecuta nada en pplano*/
            }
    return 0;
}


void Proceso (char *tr[], int splano)
{
   pid_t pid;
   void Cmd_exec (char **);
   int background=splano || ComprobarSegundoPlano(tr);
   if ((pid=fork())==-1){
        perror ("Imposible crear proceso");
        return;
        }
  if (pid==0){  /*proceso hijo*/
    Cmd_exec (tr);
    exit(255); /*por si falla exec*/
    }
  if (!background) 
    waitpid(pid,NULL,0);
}

/*********************************************/
/*************COMANDOS DEL SHELL************************/
void Cmd_fin (char * arg[])  /*todos los cmd_ comparten prototipo*/
{                            /*reciben los mismos parametros aunque no los usen*/
    exit(0);
}

void Cmd_autores(char *arg[])
{	
    if (arg[0] == NULL) {
        printf("Mauro Fernandez Perez: mauro.fernandez.perez\n");
        printf("Pablo Carril Gontan: p.carril\n");
        return;
    }
    if (arg[1] != NULL){
		printf("Uso: authors [-l|-n]\n");
		return;
	}
    if(!strcmp(arg[0], "-l")){
		printf("mauro.fernandez.perez\n");
		printf("p.carril\n");
	}else if(!strcmp(arg[0], "-n")){
		printf("Mauro Fernandez Perez\n");
		printf("Pablo Carril Gontan\n");
	}else{
		printf("Uso: authors [-l|-n]\n");
	}
}


void Cmd_exec (char *arg[])
{
  if (execv(Ejecutable(arg[0]),arg)==-1)
	perror ("Imposible ejecutar");
}

void Cmd_splano (char *arg[])
{
  Proceso (arg,1);
}
void Cmd_pplano (char *arg[])
{
  Proceso(arg,0);
}

void Cmd_chdir (char * arg[])
{
   if (arg[0]==NULL)
      MostrarDirActual();
   else if (chdir(arg[0])==-1)
      perror("Imposible cambiar directorio");
}

void Cmd_pwd(char * arg[])
{
    MostrarDirActual();
}

void Cmd_pid (char * arg[])
{
    if (arg[0]==NULL)
        printf ("El pid del proceso es %d\n",(int) getpid());
    else
        if (!strcmp (arg[0],"-p"))
            printf ("El pid del proceso padre es %d\n",(int) getppid());
}

void Cmd_where (char *args[])
{
    if (args[0]==NULL)
        printf ("uso: where ejecutable. Indica donde ejecutable está en el path\n");
    else
        printf ("%s\n",Ejecutable (args[0]));
}


void Cmd_path(char *arg[])
{
    if (arg[0]==NULL)
        PathPrint();
    else if (!strcmp(arg[0],"-add"))
        AniadirAlPath (arg[1]);
    else if (!strcmp(arg[0],"-del"))
        EliminarDelPath (arg[1]);
    else if (!strcmp(arg[0],"-show"))
        PathPrint ();
    else if (!strcmp(arg[0],"-clear"))
        PathClear ();
    else if (!strcmp(arg[0],"-import"))
        PathAddPath ();
    else printf ("Opciones validas: -add|-del|-show|-clear|-import\n");
} 

void Cmd_importpath (char *arg[])
{
    PathAddPath();
}

void Cmd_date(char *arg[])
{
    time_t ahora;
    struct tm *fecha;
    char texto[80];
    const char *formato;

    if (arg[0] == NULL)
        formato = "%d/%m/%Y %H:%M:%S";
    else if (!strcmp(arg[0], "-d"))
        formato = "%d/%m/%Y";
    else if (!strcmp(arg[0], "-t"))
        formato = "%H:%M:%S";
    else {
        printf("Uso: date [-d|-t]\n");
        return;
    }

    ahora = time(NULL);

    if (ahora == (time_t)-1) {
        perror("Imposible obtener fecha y hora");
        return;
    }

    fecha = localtime(&ahora);

    if (fecha == NULL) {
        perror("Imposible convertir fecha y hora");
        return;
    }

    if (strftime(texto, sizeof(texto), formato, fecha) == 0) {
        fprintf(stderr, "No se pudo representar la fecha\n");
        return;
    }

    printf("%s\n", texto);
}
/*
void Cmd_sysinfo(char *arg[])
{
    struct utsname datos;

    if (uname(&datos) == -1) {
        perror("Imposible obtener informacion del sistema");
        return;
    }

    printf("%s %s %s %s %s\n",
           datos.sysname,
           datos.nodename,
           datos.release,
           datos.version,
           datos.machine);
}
*/

struct COMANDO{
  char * nombre;
  void (*funcion)(char**);
  char * help;
};

extern struct COMANDO C[];

void Cmd_help(char *arg[]){
    if (arg[0] == NULL) {
        for (int i = 0; C[i].nombre != NULL; i++) {
            printf("%s\n", C[i].help);
        }
        return;
    }

    for (int i = 0; C[i].nombre != NULL; i++) {
        if (!strcmp(arg[0], C[i].nombre)) {
            printf("%s\n", C[i].help);
            return;
        }
    }

    printf("help: comando '%s' no encontrado\n", arg[0]);
	}

void Cmd_open (char * tr[]){
	int i,df, mode=0;
    
    if (tr[0]==NULL) { 
		OpenFilesList()
        return;
    }
    for (i=1; tr[i]!=NULL; i++)
      if (!strcmp(tr[i],"cr")) mode|=O_CREAT;
      else if (!strcmp(tr[i],"ex")) mode|=O_EXCL;
      else if (!strcmp(tr[i],"ro")) mode|=O_RDONLY; 
      else if (!strcmp(tr[i],"wo")) mode|=O_WRONLY;
      else if (!strcmp(tr[i],"rw")) mode|=O_RDWR;
      else if (!strcmp(tr[i],"ap")) mode|=O_APPEND;
      else if (!strcmp(tr[i],"tr")) mode|=O_TRUNC; 
      else break;
      
    if ((df=open(tr[0],mode,0777))==-1)
        perror ("Imposible abrir fichero");
    else{
        OpenFilesAdd(df,mode,tr[0]);
        printf ("Anadida entrada a la tabla ficheros abiertos: descriptor %d (%s)\n",df,tr[0]);
}

