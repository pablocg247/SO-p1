	#include "ejemplo.h"
	
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
    OpenFilesClear();
    exit(0);
}

void Cmd_autores(char *arg[])
{
    if (arg[0] == NULL) {
        printf("Mauro Fernandez Perez: mauro.fernandez.perez\n");
        printf("Pablo Carril Gontan: p.carril\n");
    }

    else if (!strcmp(arg[0], "-l")) {
        printf("mauro.fernandez.perez\n");
        printf("p.carril\n");
    }
    else if (!strcmp(arg[0], "-n")) {
        printf("Mauro Fernandez Perez\n");
        printf("Pablo Carril Gontan\n");
    }
    else {
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
		OpenFilesList();
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
        if(OpenFilesAdd(df,mode,tr[0]) == -1){
			perror("Imposible aniadir entrada a la tabla ficheros abiertos");
			close(df);
			}
        else {printf ("Anadida entrada a la tabla ficheros abiertos: descriptor %d (%s)\n",df,tr[0]);}
	}
}

void Cmd_close (char *tr[])
{ 
    int df;
    
    if (tr[0]==NULL || (df=atoi(tr[0]))<0) {
		OpenFilesList();
        return;
    }

    
    if (close(df)==-1)
        perror("Imposible cerrar descriptor");
    else
       OpenFilesDel(df);
}

void Cmd_listopen(char *tr[])
{
    OpenFilesList();
}

void Cmd_dup (char * tr[])
{ 
    int df, duplicado;
    char aux[MAXFILENAME],*p;
    OpenFile *f;

    if (tr[0]==NULL || (df=atoi(tr[0]))<0) {
        OpenFilesList();
        return;
    }
    
	f = OpenFilesGet(df);
    if (f == NULL){
        printf("Imposible duplicar fichero\n");
        return;
    }
    p = f->name;

    duplicado = dup(df);
    if (duplicado == -1) {
        perror("Imposible duplicar descriptor");
        return;
    }

    sprintf(aux, "dup %d (%s)", df, p);

    int modo = fcntl(duplicado, F_GETFL);
    if (OpenFilesAdd(duplicado, modo, aux) == -1) {
        perror("Imposible anadir a la lista de ficheros abiertos");
        close(duplicado);
    } else {
        printf("Anadida entrada a la tabla ficheros abiertos: descriptor %d (%s)\n", duplicado, aux);
    }
}

static int LeerNumero(const char *texto, intmax_t *numero)
{
    char *fin;

    errno = 0;
    *numero = strtoimax(texto, &fin, 10);

    if (texto == fin || *fin != '\0' || errno == ERANGE) {
        fprintf(stderr, "Numero no valido: %s\n", texto);
        return 0;
    }

    return 1;
}


static int LeerDescriptor(const char *texto, int *df)
{
    intmax_t numero;

    if (!LeerNumero(texto, &numero))
        return 0;

    if (numero < 0 || numero > INT_MAX) {
        fprintf(stderr, "Descriptor fuera de rango\n");
        return 0;
    }

    *df = (int)numero;
    return 1;
}

void Cmd_lseek(char *arg[])
{
    int df, referencia;
    intmax_t numero;
    off_t posicion, resultado;

    if (arg[0] == NULL || arg[1] == NULL ||
        arg[2] == NULL || arg[3] != NULL) {
        printf("Uso: lseek df pos SEEK_SET|SEEK_CUR|SEEK_END\n");
        return;
    }

    if (!LeerDescriptor(arg[0], &df) ||
        !LeerNumero(arg[1], &numero))
        return;

    posicion = (off_t)numero;

    if ((intmax_t)posicion != numero) {
        fprintf(stderr, "Posicion fuera de rango\n");
        return;
    }

    if (!strcmp(arg[2], "SEEK_SET"))
        referencia = SEEK_SET;
    else if (!strcmp(arg[2], "SEEK_CUR"))
        referencia = SEEK_CUR;
    else if (!strcmp(arg[2], "SEEK_END"))
        referencia = SEEK_END;
    else {
        printf("Referencia no valida: usa SEEK_SET, SEEK_CUR o SEEK_END\n");
        return;
    }

    resultado = lseek(df, posicion, referencia);

    if (resultado == (off_t)-1)
        perror("Imposible cambiar posicion");
    else
        printf("Posicion actual: %jd\n", (intmax_t)resultado);
}

void Cmd_writestr(char *arg[])
{
    int df;
    size_t longitud, total = 0;
    ssize_t escritos;

    if (arg[0] == NULL || arg[1] == NULL || arg[2] != NULL) {
        printf("Uso: writestr df str (str sin espacios)\n");
        return;
    }

    if (!LeerDescriptor(arg[0], &df))
        return;

    longitud = strlen(arg[1]);

    while (total < longitud) {
        escritos = write(df, arg[1] + total, longitud - total);

        if (escritos == -1) {
            if (errno == EINTR)
                continue;

            perror("Imposible escribir");
            return;
        }

        if (escritos == 0) {
            fprintf(stderr, "No se pudo completar la escritura\n");
            return;
        }

        total += (size_t)escritos;
    }

    printf("Escritos %zu bytes\n", total);
}

void Cmd_readstr(char *arg[])
{
    int df;
    intmax_t numero;
    size_t cantidad;
    ssize_t leidos;
    char *texto;

    if (arg[0] == NULL || arg[1] == NULL || arg[2] != NULL) {
        printf("Uso: readstr df cont\n");
        return;
    }

    if (!LeerDescriptor(arg[0], &df) ||
        !LeerNumero(arg[1], &numero))
        return;

    if (numero < 0 ||
        (uintmax_t)numero > (uintmax_t)SSIZE_MAX ||
        (uintmax_t)numero > (uintmax_t)(SIZE_MAX - 1)) {
        fprintf(stderr, "Cantidad fuera de rango\n");
        return;
    }

    cantidad = (size_t)numero;
    texto = malloc(cantidad + 1);

    if (texto == NULL) {
        perror("Imposible reservar memoria");
        return;
    }

    do {
        leidos = read(df, texto, cantidad);
    } while (leidos == -1 && errno == EINTR);

    if (leidos == -1) {
        perror("Imposible leer");
    }
    else {
        texto[leidos] = '\0';
        printf("%s\n", texto);
    }

    free(texto);
}


void Cmd_makefile(char *arg[])
{
    int df;

    if (arg[0] == NULL || arg[1] != NULL) {
        printf("Uso: makefile nombre\n");
        return;
    }

    df = open(arg[0], O_WRONLY | O_CREAT | O_EXCL, 0666);

    if (df == -1) {
        perror(arg[0]);
        return;
    }

    if (close(df) == -1)
        perror("Imposible cerrar el fichero creado");
}


void Cmd_makedir(char *arg[])
{
    if (arg[0] == NULL || arg[1] != NULL) {
        printf("Uso: makedir nombre\n");
        return;
    }

    if (mkdir(arg[0], 0777) == -1)
        perror(arg[0]);
}


static char *CopiarRuta(const char *ruta)
{
    char *copia = strdup(ruta);
    size_t n;

    if (copia == NULL) {
        perror("Imposible copiar ruta");
        return NULL;
    }

    n = strlen(copia);

    while (n > 1 && copia[n - 1] == '/')
        copia[--n] = '\0';

    return copia;
}


void Cmd_delete(char *arg[])
{
    struct stat datos;
    int i, resultado;
    char *ruta;

    if (arg[0] == NULL) {
        printf("Uso: delete nombre1 nombre2 ...\n");
        return;
    }

    for (i = 0; arg[i] != NULL; i++) {
        ruta = CopiarRuta(arg[i]);

        if (ruta == NULL)
            continue;

        if (lstat(ruta, &datos) == -1) {
            perror(ruta);
        }
        else {
            if (S_ISDIR(datos.st_mode))
                resultado = rmdir(ruta);
            else
                resultado = unlink(ruta);

            if (resultado == -1)
                perror(ruta);
        }

        free(ruta);
    }
}


static int BorrarArbol(const char *ruta)
{
    struct stat datos;
    DIR *directorio;
    struct dirent *entrada;
    char *hijo;
    size_t tam;
    int correcto = 1;

    if (lstat(ruta, &datos) == -1) {
        perror(ruta);
        return 0;
    }

    if (!S_ISDIR(datos.st_mode)) {
        if (unlink(ruta) == -1) {
            perror(ruta);
            return 0;
        }

        return 1;
    }

    directorio = opendir(ruta);

    if (directorio == NULL) {
        perror(ruta);
        return 0;
    }

    while (1) {
        errno = 0;
        entrada = readdir(directorio);

        if (entrada == NULL) {
            if (errno != 0) {
                perror(ruta);
                correcto = 0;
            }

            break;
        }

        if (!strcmp(entrada->d_name, ".") ||
            !strcmp(entrada->d_name, ".."))
            continue;

        tam = strlen(ruta) + strlen(entrada->d_name) + 2;
        hijo = malloc(tam);

        if (hijo == NULL) {
            perror("Imposible reservar memoria");
            correcto = 0;
            break;
        }

        snprintf(hijo, tam, "%s/%s", ruta, entrada->d_name);

        if (!BorrarArbol(hijo))
            correcto = 0;

        free(hijo);
    }

    if (closedir(directorio) == -1) {
        perror(ruta);
        correcto = 0;
    }

    if (correcto && rmdir(ruta) == -1) {
        perror(ruta);
        correcto = 0;
    }

    return correcto;
}


void Cmd_deltree(char *arg[])
{
    int i;
    char *ruta;

    if (arg[0] == NULL) {
        printf("Uso: deltree nombre1 nombre2 ...\n");
        return;
    }

    for (i = 0; arg[i] != NULL; i++) {
        ruta = CopiarRuta(arg[i]);

        if (ruta == NULL)
            continue;

        BorrarArbol(ruta);
        free(ruta);
    }
}

