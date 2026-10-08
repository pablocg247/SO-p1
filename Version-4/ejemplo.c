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

    else if (!strcmp(arg[0], "-l") && arg[1] == NULL) {
        printf("mauro.fernandez.perez\n");
        printf("p.carril\n");
    }
    else if (!strcmp(arg[0], "-n") && arg[1] == NULL) {
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
    else if (!strcmp(arg[0], "-d") && arg[1] == NULL)
        formato = "%d/%m/%Y";
    else if (!strcmp(arg[0], "-t") && arg[1] == NULL)
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

	if (arg[0] != NULL) {
    printf("Uso: sysinfo (no admite argumentos)\n");
    return;
	}

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

void Cmd_help(char *arg[])
{
    if (arg[0] == NULL) {
        for (int i = 0; C[i].nombre != NULL; i++) {
            printf("%s\n", C[i].help);
        }
        return;
    }

	if (arg[1] != NULL) {
    printf("Uso: help [comando]\n");
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

void Cmd_open (char * tr[])
{
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
      	else {
			fprintf(stderr, "Modo de apertura no valido: %s\n", tr[i]);
            return;
	  	}
      
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

//Funcion no terminada al no estar implementada la funcionalidad de mapeado
static int EstaMapeado(int df)
{
    (void)df;
    return 0;
}

void Cmd_close(char *tr[])
{
    int df;
    int forzar = 0;

    if (tr[0] == NULL) {
        OpenFilesList();
        return;
    }

    if (tr[1] != NULL) {
        if (!strcmp(tr[1], "-f") && tr[2] == NULL) {
            forzar = 1;
        } else {
            fprintf(stderr, "Uso: close [df [-f]]\n");
            return;
        }
    }

    if (!LeerDescriptor(tr[0], &df)) {
        return;
    }

    if (!forzar && EstaMapeado(df)) {
        fprintf(stderr, "Descriptor %d corresponde a un mapeo activo. Usa 'close %d -f' para forzar\n", df, df);
        return;
    }

    if (close(df) == -1) {
        perror("Imposible cerrar descriptor");
        return;
    }

    OpenFilesDel(df);
}

void Cmd_listopen(char *tr[])
{
	if (tr[0] != NULL) {
        fprintf(stderr, "Uso: listopen (no admite argumentos)\n");
        return;
    }
    OpenFilesList();
}

void Cmd_dup (char * tr[])
{ 
    int df, duplicado, mode;
    char aux[MAXFILENAME + 20],*p;
    OpenFile *f;

    if (tr[0]==NULL) {
        OpenFilesList();
        return;
    }

	if (tr[1] != NULL) {
        fprintf(stderr, "Uso: dup [df]\n");
        return;
    }

	if (!LeerDescriptor(tr[0], &df)) {
        return;
    }

	duplicado = dup(df);
    if (duplicado == -1) {
        perror("Imposible duplicar descriptor");
        return;
    }

    f = OpenFilesGet(df);
    if (f != NULL) {
        p = f->name;
    } else {
        p = "desconocido";
    }

    snprintf(aux, sizeof(aux), "dup %d (%s)", df, p);

    if ((mode = fcntl(duplicado, F_GETFL)) == -1) {
        perror("Imposible obtener modo del descriptor");
        close(duplicado);
        return;
    }
	if (OpenFilesAdd(duplicado, mode, aux) == -1) {
        perror("Imposible anadir a la lista de ficheros abiertos");
        close(duplicado);
    } else {
        printf("Anadida entrada a la tabla ficheros abiertos: descriptor %d (%s)\n", duplicado, aux);
    }
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
        !LeerNumero(arg[1], &numero)){
        return;
	}

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

    if (numero <= 0 ||
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
    else if (leidos != 0) {
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

char LetraTF (mode_t m)
{
     switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /* fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */ 
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
     }
}

char * ConvierteModo3 (mode_t m)
{
    char *permisos;

    if ((permisos=(char *) malloc (12))==NULL)
        return NULL;
    strcpy (permisos,"---------- ");
    
    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';
    
    return permisos;
}

int EsDirectorio (char * dir)          /*para saber si algo es directorio o no*/
{
  struct stat s;
  if (lstat(dir,&s)==-1)       /*si no puedo acceder: para mi no es directorio*/
        return 0;
  return (S_ISDIR(s.st_mode));
}

static void ListarFichero(const char *ruta, int long_mode, int link_mode, int acc_mode)
{
    struct stat s;
    char destino[MAXNOMBRE];
    char enlace_str[MAXNOMBRE + 16] = "";

    if (lstat(ruta, &s) == -1) {
        perror(ruta);
        return;
    }

    if (link_mode && S_ISLNK(s.st_mode)) {
        ssize_t n = readlink(ruta, destino, sizeof(destino) - 1);
        if (n != -1) {
            destino[n] = '\0';
            snprintf(enlace_str, sizeof(enlace_str), " -> %s", destino);
        }
    }

    if (!long_mode) {
        printf("%9jd %s%s\n", (intmax_t)s.st_size, ruta, enlace_str);
        return;
    }
	
    time_t t = acc_mode ? s.st_atime : s.st_mtime;
    struct tm *tm_info = localtime(&t);
    char fecha[32];	
    if (tm_info == NULL || strftime(fecha, sizeof(fecha), "%Y/%m/%d-%H:%M", tm_info) == 0) {
        snprintf(fecha, sizeof(fecha), "desconocida");
    }

    struct passwd *p = getpwuid(s.st_uid);
    struct group *g = getgrgid(s.st_gid);
    const char *user = (p != NULL) ? p->pw_name : "desconocido";
    const char *group = (g != NULL) ? g->gr_name : "desconocido";

    char *permisos = ConvierteModo3(s.st_mode);
    if (permisos == NULL) {
        perror("Imposible convertir permisos");
        return;
    }

    printf("%s %2lu (%lu) %s %s %s%9jd %s%s\n",
           fecha,
           (unsigned long)s.st_nlink,
           (unsigned long)s.st_ino,
           user,
           group,
           permisos,
           (intmax_t)s.st_size,
           ruta,
           enlace_str);

    free(permisos);
}

void Cmd_listfile(char *arg[])
{
    int long_mode = 0, link_mode = 0, acc_mode = 0;
    int i;

    for (i = 0; arg[i] != NULL; i++) {
        if (!strcmp(arg[i], "-long")) {
            long_mode = 1;
        } else if (!strcmp(arg[i], "-link")) {
            link_mode = 1;
        } else if (!strcmp(arg[i], "-acc")) {
            acc_mode = 1;
        } else {
            break;
        }
    }

    for (; arg[i] != NULL; i++) {
        ListarFichero(arg[i], long_mode, link_mode, acc_mode);
    }
}

static void ListarDirectorio(const char *dirpath, int reca, int recb, int hid, int long_m, int link_m, int acc_m)
{
    DIR *dir;
    struct dirent *ent;
    struct stat s;
    char ruta[4096];

    // 1. RECURSIVIDAD DESPUÉS (-recb): Primero descendemos, luego imprimimos actual
    if (recb) {
        dir = opendir(dirpath);
        if (dir == NULL) {
            perror(dirpath);
            return;
        }
        while ((ent = readdir(dir)) != NULL) {
            if (!hid && ent->d_name[0] == '.') continue;
            if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) continue;

            snprintf(ruta, sizeof(ruta), "%s/%s", dirpath, ent->d_name);
            if (lstat(ruta, &s) == 0 && S_ISDIR(s.st_mode)) {
                ListarDirectorio(ruta, reca, recb, hid, long_m, link_m, acc_m);
            }
        }
        closedir(dir);
    }

    // 2. IMPRIMIR EL DIRECTORIO ACTUAL
    printf("************ %s ************\n", dirpath);
    dir = opendir(dirpath);
    if (dir == NULL) {
        if (!recb) perror(dirpath); // Evitar imprimir el error dos veces
        return;
    }
    while ((ent = readdir(dir)) != NULL) {
        // Filtrar ocultos si no se ha pasado -hid
        if (!hid && ent->d_name[0] == '.') continue;

        // Construir la ruta completa: "directorio/fichero"
        snprintf(ruta, sizeof(ruta), "%s/%s", dirpath, ent->d_name);
        
        // ¡Reutilizamos la funcion de listfile!
        ListarFichero(ruta, long_m, link_m, acc_m);
    }
    closedir(dir);

    // 3. RECURSIVIDAD ANTES (-reca): Primero imprimimos actual, luego descendemos
    if (reca) {
        dir = opendir(dirpath);
        if (dir == NULL) return;
        while ((ent = readdir(dir)) != NULL) {
            if (!hid && ent->d_name[0] == '.') continue;
            if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) continue;

            snprintf(ruta, sizeof(ruta), "%s/%s", dirpath, ent->d_name);
            if (lstat(ruta, &s) == 0 && S_ISDIR(s.st_mode)) {
                ListarDirectorio(ruta, reca, recb, hid, long_m, link_m, acc_m);
            }
        }
        closedir(dir);
    }
}

void Cmd_list(char *arg[])
{
    int reca = 0, recb = 0, hid = 0;
    int long_mode = 0, link_mode = 0, acc_mode = 0;
    int i;
    struct stat s;

    // 1. Parsear todas las opciones posibles (empiezan por '-')
    for (i = 0; arg[i] != NULL; i++) {
        if (!strcmp(arg[i], "-reca")) reca = 1;
        else if (!strcmp(arg[i], "-recb")) recb = 1;
        else if (!strcmp(arg[i], "-hid")) hid = 1;
        else if (!strcmp(arg[i], "-long")) long_mode = 1;
        else if (!strcmp(arg[i], "-link")) link_mode = 1;
        else if (!strcmp(arg[i], "-acc")) acc_mode = 1;
        else break; // Fin de flags, comienzan las rutas
    }

    // 2. Si no se especifican rutas, se asume el directorio actual (".")
    if (arg[i] == NULL) {
        ListarDirectorio(".", reca, recb, hid, long_mode, link_mode, acc_mode);
        return;
    }

    // 3. Procesar cada ruta indicada por el usuario
    for (; arg[i] != NULL; i++) {
        if (lstat(arg[i], &s) == -1) {
            perror(arg[i]); // El archivo o directorio no existe
            continue;
        }

        if (S_ISDIR(s.st_mode)) {
            // Si es un directorio, lo exploramos
            ListarDirectorio(arg[i], reca, recb, hid, long_mode, link_mode, acc_mode);
        } else {
            // Si le pasan un archivo suelto a "list", se comporta como "listfile"
            ListarFichero(arg[i], long_mode, link_mode, acc_mode);
        }
    }
}

