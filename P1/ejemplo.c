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
	PathClear();
    MemlistClear();
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
}

void Cmd_exec (char *arg[])
{
  if(arg[0] == NULL){
        printf("Uso: exec comando [argumentos...]\n");
        return;
    }
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

void Cmd_pid(char *arg[])
{
    if (arg[0] != NULL && !strcmp(arg[0], "-p")) {
        printf("Pid del padre del shell: %d\n", (int)getppid());
    }
    else {
        printf("Pid de shell: %d\n", (int)getpid());
    }
}

void Cmd_where (char *args[])
{
    if (args[0]==NULL)
        printf("Uso: where ejecutable. Indica donde ejecutable está en el path\n");
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
        formato = "%H:%M:%S\n%d/%m/%Y";
    else if (!strcmp(arg[0], "-d"))
        formato = "%d/%m/%Y";
    else if (!strcmp(arg[0], "-t"))
        formato = "%H:%M:%S";
    else
		return; 

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
        printf("No se pudo representar la fecha\n");
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

   printf("%s (%s), OS: %s-%s-%s\n",
           datos.nodename,
           datos.machine,
           datos.sysname,
           datos.release,
           datos.version);
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
        printf("Comandos disponibles:");

        for (int i = 0; C[i].nombre != NULL; i++) {
            printf(" %s", C[i].nombre);
        }

        printf("\n");
        return;
    }

    for (int i = 0; C[i].nombre != NULL; i++) {
        if (!strcmp(arg[0], C[i].nombre)) {
            printf("%s\n", C[i].help);
            return;
        }
    }

    printf("%s: no encontrado\n", arg[0]);
}

static int LeerNumero(const char *texto, intmax_t *numero)
{
    char *fin;

    errno = 0;
    *numero = strtoimax(texto, &fin, 10);

    if (texto == fin || *fin != '\0' || errno == ERANGE) {
        printf("Numero no valido: %s\n", texto);
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
        printf("Descriptor fuera de rango\n");
        return 0;
    }

    *df = (int)numero;
    return 1;
}

/* Nuevas funciones auxiliares*/
static int LeerDireccionMemoria(const char *texto, void **direccion)
{
    char *fin;
    uintmax_t valor;
    if (texto == NULL || texto[0] == '\0' || texto[0] == '-') {
        printf("Direccion no valida\n");
        return 0;
    }
    errno = 0;
    valor = strtoumax(texto, &fin, 16);
    if (texto == fin || *fin != '\0' || errno == ERANGE || valor > UINTPTR_MAX) {
        printf("Direccion no valida: %s\n", texto);
        return 0;
    }
    *direccion = (void *)(uintptr_t)valor;
    return 1;
}

static int LeerCantidadMemoria(const char *texto, size_t *cantidad)
{
    char *fin;
    uintmax_t valor;
    if (texto == NULL || texto[0] == '\0' || texto[0] == '-') {
        printf("Cantidad no valida\n");
        return 0;
    }
    errno = 0;
    valor = strtoumax(texto, &fin, 10);
    if (texto == fin || *fin != '\0' || errno == ERANGE || valor > SIZE_MAX) {
        printf("Cantidad no valida: %s\n", texto);
        return 0;
    }
    *cantidad = (size_t)valor;
    return 1;
}


static void IORead(char *arg[])
{
    int df;
    void *direccion;
    unsigned char *destino;
    size_t cantidad, total = 0;
    ssize_t leidos;

    if (arg[0] == NULL || arg[1] == NULL || arg[2] == NULL) {
        printf("Uso: i-o read fichero addr cont\n");
        return;
    }
    if (!LeerDireccionMemoria(arg[1], &direccion) ||
        !LeerCantidadMemoria(arg[2], &cantidad))
        return;

    df = open(arg[0], O_RDONLY);
    if (df == -1) {
        perror("Imposible abrir fichero");
        return;
    }
    destino = direccion;
    while (total < cantidad) {
        size_t pendiente = cantidad - total;
        if (pendiente > (size_t)SSIZE_MAX)
            pendiente = (size_t)SSIZE_MAX;

        leidos = read(df, destino + total, pendiente);
        if (leidos == -1) {
            if (errno == EINTR)
                continue;
            perror("Imposible leer fichero");
            close(df);
            return;
        }
        if (leidos == 0)
            break;
        total += (size_t)leidos;
    }
    if (close(df) == -1) {
        perror("Imposible cerrar fichero");
        return;
    }
    printf("Leidos %zu bytes de %s en %p\n", total, arg[0], direccion);
}

static void IOWrite(char *arg[])
{
    int sobrescribir = 0, i = 0, df, modo;
    void *direccion;
    const unsigned char *origen;
    size_t cantidad, total = 0;
    ssize_t escritos;

    if (arg[0] != NULL && !strcmp(arg[0], "-o")) {
        sobrescribir = 1;
        i = 1;
    }
    if (arg[i] == NULL || arg[i + 1] == NULL || arg[i + 2] == NULL) {
        printf("Uso: i-o write [-o] fichero addr cont\n");
        return;
    }
    if (!LeerDireccionMemoria(arg[i + 1], &direccion) ||
        !LeerCantidadMemoria(arg[i + 2], &cantidad))
        return;

    modo = O_WRONLY | O_CREAT;
    modo |= sobrescribir ? O_TRUNC : O_EXCL;
    df = open(arg[i], modo, 0666);
    if (df == -1) {
        perror("Imposible abrir fichero");
        return;
    }
    origen = direccion;
    while (total < cantidad) {
        size_t pendiente = cantidad - total;
        if (pendiente > (size_t)SSIZE_MAX)
            pendiente = (size_t)SSIZE_MAX;

        escritos = write(df, origen + total, pendiente);
        if (escritos == -1) {
            if (errno == EINTR)
                continue;
            perror("Imposible escribir fichero");
            close(df);
            return;
        }
        if (escritos == 0) {
            printf("No se pudo completar la escritura\n");
            close(df);
            return;
        }
        total += (size_t)escritos;
    }
    if (close(df) == -1) {
        perror("Imposible cerrar fichero");
        return;
    }
    printf("Escritos %zu bytes desde %p en %s\n", total, direccion, arg[i]);
}

void Cmd_io(char *arg[])
{
    if (arg[0] == NULL) {
        printf("Uso: i-o read fichero addr cont\n");
        printf("     i-o write [-o] fichero addr cont\n");
        return;
    }
    if (!strcmp(arg[0], "read"))
        IORead(arg + 1);
    else if (!strcmp(arg[0], "write"))
        IOWrite(arg + 1);
    else {
        printf("Uso: i-o read fichero addr cont\n");
        printf("     i-o write [-o] fichero addr cont\n");
    }
}

void Cmd_open(char *tr[])
{
    int i, df, mode = 0;

    if (tr[0] == NULL) {
        OpenFilesList();
        return;
    }

    for (i = 1; tr[i] != NULL; i++) {
        if (!strcmp(tr[i], "cr")) mode |= O_CREAT;
        else if (!strcmp(tr[i], "ex")) mode |= O_EXCL;
        else if (!strcmp(tr[i], "ro")) mode |= O_RDONLY;
        else if (!strcmp(tr[i], "wo")) mode |= O_WRONLY;
        else if (!strcmp(tr[i], "rw")) mode |= O_RDWR;
        else if (!strcmp(tr[i], "ap")) mode |= O_APPEND;
        else if (!strcmp(tr[i], "tr")) mode |= O_TRUNC;
        else
            break;
    }

    df = open(tr[0], mode, 0777);

    if (df == -1) {
        perror("Imposible abrir fichero");
        return;
    }

    if (OpenFilesAdd(df, mode, tr[0]) == -1) {
        perror("Imposible aniadir entrada a la tabla ficheros abiertos");

        if (close(df) == -1)
            perror("Imposible cerrar descriptor");

        return;
    }

    printf("Anadida entrada %d a la tabla ficheros abiertos\n", df);
}

static int EstaMapeado(int df)
{
    return MemListHasMappedFd(df);
}

void Cmd_close(char *tr[])
{
    int df;
    int forzar;
    intmax_t numero;

    if (tr[0] == NULL) {
        OpenFilesList();
        return;
    }

    if (!LeerNumero(tr[0], &numero))
        return;

    if (numero < 0) {
        OpenFilesList();
        return;
    }

    if (numero > INT_MAX) {
        printf("Descriptor fuera de rango\n");
        return;
    }

    df = (int)numero;

    forzar = tr[1] != NULL && !strcmp(tr[1], "-f");

    if (!forzar && EstaMapeado(df)) {
        printf("Descriptor %d corresponde a un mapeo activo. " "Usa 'close %d -f' para forzar\n", df, df);
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
    OpenFilesList();
}

void Cmd_dup(char *tr[])
{
    int df, duplicado, mode;
    intmax_t numero;
    char aux[MAXFILENAME + 40];
    const char *nombre;
    OpenFile *f;

    if (tr[0] == NULL) {
        OpenFilesList();
        return;
    }

    if (!LeerNumero(tr[0], &numero))
        return;

    if (numero < 0) {
        OpenFilesList();
        return;
    }

    if (numero > INT_MAX) {
        printf("Descriptor fuera de rango\n");
        return;
    }

    df = (int)numero;

    duplicado = dup(df);

    if (duplicado == -1) {
        perror("Imposible duplicar descriptor");
        return;
    }

    f = OpenFilesGet(df);
    nombre = (f != NULL) ? f->name : "desconocido";

    snprintf(aux, sizeof(aux), "duplicado de %d (%s)", df, nombre);

    mode = fcntl(duplicado, F_GETFL);

    if (mode == -1) {
        perror("Imposible obtener modo del descriptor");

        if (close(duplicado) == -1)
            perror("Imposible cerrar descriptor");

        return;
    }

    if (OpenFilesAdd(duplicado, mode, aux) == -1) {
        perror("Imposible anadir a la lista de ficheros abiertos");

        if (close(duplicado) == -1)
            perror("Imposible cerrar descriptor");
    }
}

void Cmd_lseek(char *arg[])
{
    int df, referencia = SEEK_SET;
    intmax_t numero;
    off_t posicion, resultado;

    if (arg[0] == NULL || arg[1] == NULL) {
        printf("Parametros incorrectos\n");
        return;
    }

    if (!LeerDescriptor(arg[0], &df) ||
        !LeerNumero(arg[1], &numero)) {
        return;
    }

    posicion = (off_t)numero;

    if ((intmax_t)posicion != numero) {
        printf("Posicion fuera de rango\n");
        return;
    }

    if (arg[2] != NULL) {
        if (!strcmp(arg[2], "SEEK_CUR"))
            referencia = SEEK_CUR;
        else if (!strcmp(arg[2], "SEEK_END"))
            referencia = SEEK_END;
    }

    resultado = lseek(df, posicion, referencia);

    if (resultado == (off_t)-1) {
        printf("Error al intentar posicionar el descriptor %d " "en el offset  %jd: %s\n", df, (intmax_t)posicion, strerror(errno));
    }
    else {
        printf("Descriptor %d posicionado en %jd\n", df, (intmax_t)resultado);
    }
}

void Cmd_writestr(char *arg[])
{
    int df;
    size_t longitud, total;
    ssize_t escritos;
    const char *texto;

    if (arg[0] == NULL || arg[1] == NULL) {
        printf("Parametros incorrectos\n");
        return;
    }

    if (!LeerDescriptor(arg[0], &df))
        return;

    for (int i = 1; arg[i] != NULL; i++) {

        for (int parte = 0; parte < 2; parte++) {
            if (parte == 0) {
                texto = arg[i];
            }
            else {
                if (arg[i + 1] == NULL)
                    break;

                texto = " ";
            }

            longitud = strlen(texto);
            total = 0;

            while (total < longitud) {
                escritos = write(df, texto + total, longitud - total);

                if (escritos == -1) {
                    if (errno == EINTR)
                        continue;

                    perror("Imposible escribir");
                    return;
                }

                if (escritos == 0) {
                    printf("No se pudo completar la escritura\n");
                    return;
                }

                total += (size_t)escritos;
            }
        }
    }
}

void Cmd_readstr(char *arg[])
{
    int df;
    intmax_t numero;
    size_t cantidad;
    ssize_t leidos;
    char *texto;

    if (arg[0] == NULL || arg[1] == NULL) {
        printf("Parametros incorrectos\n");
        return;
    }

    if (!LeerDescriptor(arg[0], &df) ||
        !LeerNumero(arg[1], &numero))
        return;

    if (numero < 0 ||
        (uintmax_t)numero > (uintmax_t)SSIZE_MAX ||
        (uintmax_t)numero > (uintmax_t)(SIZE_MAX - 1)) {
        printf("Cantidad fuera de rango\n");
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

        printf("Leidos %zd bytes del descriptor %d\n", leidos, df);
        printf("%s\n", texto);
    }

    free(texto);
}

void Cmd_makefile(char *arg[])
{
    int df;

    if (arg[0] == NULL) {
        MostrarDirActual();
        return;
    }

    df = open(arg[0], O_WRONLY | O_CREAT | O_EXCL, 0777);

    if (df == -1) {
        perror(arg[0]);
        return;
    }

    if (close(df) == -1)
        perror("Imposible cerrar el fichero creado");
}

void Cmd_makedir(char *arg[])
{
    if (arg[0] == NULL) {
        MostrarDirActual();
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
        MostrarDirActual();
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
        MostrarDirActual();
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

    if ((permisos = (char *) malloc(12)) == NULL)
        return NULL;
    strcpy(permisos, "---------- ");

    permisos[0] = LetraTF(m);
    if (m & S_IRUSR) permisos[1] = 'r';
    if (m & S_IWUSR) permisos[2] = 'w';
    if (m & S_IXUSR) permisos[3] = 'x';
    if (m & S_IRGRP) permisos[4] = 'r';
    if (m & S_IWGRP) permisos[5] = 'w';
    if (m & S_IXGRP) permisos[6] = 'x';
    if (m & S_IROTH) permisos[7] = 'r';
    if (m & S_IWOTH) permisos[8] = 'w';
    if (m & S_IXOTH) permisos[9] = 'x';
    if (m & S_ISUID) permisos[3] = (m & S_IXUSR) ? 's' : 'S';
    if (m & S_ISGID) permisos[6] = (m & S_IXGRP) ? 's' : 'S';
    if (m & S_ISVTX) permisos[9] = (m & S_IXOTH) ? 't' : 'T';
    
    return permisos;
}

static void ListarFichero(const char *ruta, const char *nombre, int long_mode, int link_mode, int acc_mode)
{
    struct stat s;
    char destino[PATH_MAX];
    char enlace_str[PATH_MAX + 5] = "";

    if (lstat(ruta, &s) == -1) {
        perror(ruta);
        return;
    }

    if (link_mode && S_ISLNK(s.st_mode)) {
        ssize_t n = readlink(ruta, destino, sizeof(destino) - 1);

        if (n == -1) {
            perror(ruta);
            return;
        }

        destino[n] = '\0';
        snprintf(enlace_str, sizeof(enlace_str), " -> %s", destino);
    }

    if (!long_mode) {
        printf("%8jd  %s%s\n", (intmax_t)s.st_size, nombre, enlace_str);
        return;
    }

    time_t t = acc_mode ? s.st_atime : s.st_mtime;
    struct tm *tm_info = localtime(&t);
    char fecha[32];

    if (tm_info == NULL) {
        perror("Imposible convertir fecha");
        return;
    }

    if (strftime(fecha, sizeof(fecha),
                 "%Y/%m/%d-%H:%M", tm_info) == 0) {
        printf("No se pudo representar la fecha\n");
        return;
    }

    struct passwd *p = getpwuid(s.st_uid);
    struct group *g = getgrgid(s.st_gid);

    char uid[32], gid[32];

    snprintf(uid, sizeof(uid), "%ju", (uintmax_t)s.st_uid);
    snprintf(gid, sizeof(gid), "%ju", (uintmax_t)s.st_gid);

    const char *user = (p != NULL) ? p->pw_name : uid;
    const char *group = (g != NULL) ? g->gr_name : gid;

    char *permisos = ConvierteModo3(s.st_mode);

    if (permisos == NULL) {
        perror("Imposible convertir permisos");
        return;
    }

    printf("%s %3ju (%8ju) %8s %8s %s%8jd %s%s\n",
           fecha,
           (uintmax_t)s.st_nlink,
           (uintmax_t)s.st_ino,
           user,
           group,
           permisos,
           (intmax_t)s.st_size,
           nombre,
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
        }
        else if (!strcmp(arg[i], "-link")) {
            link_mode = 1;
        }
        else if (!strcmp(arg[i], "-acc")) {
            acc_mode = 1;
        }
        else {
            break;
        }
    }

    if (arg[i] == NULL) {
        MostrarDirActual();
        return;
    }

    for (; arg[i] != NULL; i++) {
        ListarFichero(arg[i], arg[i], long_mode, link_mode, acc_mode);
    }
}

int EsDirectorio (char * dir)          /*para saber si algo es directorio o no*/
{
  struct stat s;
  if (lstat(dir,&s)==-1)       /*si no puedo acceder: para mi no es directorio*/
        return 0;
  return (S_ISDIR(s.st_mode));
}

static void ListarDirectorio(const char *dirpath,
                            int reca, int recb, int hid,
                            int long_m, int link_m, int acc_m)
{
    DIR *dir;
    struct dirent *ent;
    struct stat datos;
    char ruta[4096];
    int len;

    if (recb)
        reca = 0;

    /* Recorrer subdirectorios antes del listado */
    if (recb) {
        dir = opendir(dirpath);

        if (dir == NULL) {
            perror(dirpath);
            return;
        }

        while (1) {
            errno = 0;
            ent = readdir(dir);

            if (ent == NULL) {
                if (errno != 0)
                    perror(dirpath);
                break;
            }

            if (!hid && ent->d_name[0] == '.')
                continue;

            if (!strcmp(ent->d_name, ".") ||
                !strcmp(ent->d_name, ".."))
                continue;

            len = snprintf(ruta, sizeof(ruta),
                           "%s/%s", dirpath, ent->d_name);

            if (len < 0 || (size_t)len >= sizeof(ruta)) {
                printf("No se pudo construir la ruta: %s/%s\n",
                       dirpath, ent->d_name);
                continue;
            }

            if (lstat(ruta, &datos) == -1) {
                perror(ruta);
                continue;
            }

            if (S_ISDIR(datos.st_mode))
                ListarDirectorio(ruta, reca, recb, hid,
                                 long_m, link_m, acc_m);
        }

        if (closedir(dir) == -1)
            perror(dirpath);
    }

    /* Mostrar el contenido del directorio */
    dir = opendir(dirpath);

    if (dir == NULL) {
        perror(dirpath);
        return;
    }

    printf("************%s\n", dirpath);

    while (1) {
        errno = 0;
        ent = readdir(dir);

        if (ent == NULL) {
            if (errno != 0)
                perror(dirpath);
            break;
        }

        if (!hid && ent->d_name[0] == '.')
            continue;

        len = snprintf(ruta, sizeof(ruta),
                       "%s/%s", dirpath, ent->d_name);

        if (len < 0 || (size_t)len >= sizeof(ruta)) {
            printf("No se pudo construir la ruta: %s/%s\n",
                   dirpath, ent->d_name);
            continue;
        }

        ListarFichero(ruta, ent->d_name,
                     long_m, link_m, acc_m);
    }

    if (closedir(dir) == -1)
        perror(dirpath);

    /* Recorrer subdirectorios después del listado */
    if (reca) {
        dir = opendir(dirpath);

        if (dir == NULL) {
            perror(dirpath);
            return;
        }

        while (1) {
            errno = 0;
            ent = readdir(dir);

            if (ent == NULL) {
                if (errno != 0)
                    perror(dirpath);
                break;
            }

            if (!hid && ent->d_name[0] == '.')
                continue;

            if (!strcmp(ent->d_name, ".") ||
                !strcmp(ent->d_name, ".."))
                continue;

            len = snprintf(ruta, sizeof(ruta),
                           "%s/%s", dirpath, ent->d_name);

            if (len < 0 || (size_t)len >= sizeof(ruta)) {
                printf("No se pudo construir la ruta: %s/%s\n",
                       dirpath, ent->d_name);
                continue;
            }

            if (lstat(ruta, &datos) == -1) {
                perror(ruta);
                continue;
            }

            if (S_ISDIR(datos.st_mode))
                ListarDirectorio(ruta, reca, recb, hid,
                                 long_m, link_m, acc_m);
        }

        if (closedir(dir) == -1)
            perror(dirpath);
    }
}

void Cmd_list(char *arg[])
{
    int reca = 0, recb = 0, hid = 0;
    int long_mode = 0, link_mode = 0, acc_mode = 0;
    int i;

    for (i = 0; arg[i] != NULL; i++) {
        if (!strcmp(arg[i], "-reca"))
            reca = 1;
        else if (!strcmp(arg[i], "-recb"))
            recb = 1;
        else if (!strcmp(arg[i], "-hid"))
            hid = 1;
        else if (!strcmp(arg[i], "-long"))
            long_mode = 1;
        else if (!strcmp(arg[i], "-link"))
            link_mode = 1;
        else if (!strcmp(arg[i], "-acc"))
            acc_mode = 1;
        else
            break;
    }

    if (recb)
        reca = 0;

    if (arg[i] == NULL) {
        MostrarDirActual();
        return;
    }

    for (; arg[i] != NULL; i++) {
        if (EsDirectorio(arg[i])) {
            ListarDirectorio(arg[i], reca, recb, hid,
                             long_mode, link_mode, acc_mode);
        }
        else {
            ListarFichero(arg[i], arg[i],
                          long_mode, link_mode, acc_mode);
        }
    }
}

/*Nuevo*/ 

static void Recursiva(int n)
{
    char automatico[TAMANO_RECURSION];
    static char estatico[TAMANO_RECURSION];
    printf("parametro:%3d(%p) array %p, arr estatico %p\n",
           n, (void *)&n, (void *)automatico, (void *)estatico);
    if (n > 0)
        Recursiva(n - 1);
}

void Cmd_recurse(char *arg[])
{
    intmax_t numero;
    if (arg[0] == NULL) {
        printf("Uso: recurse n\n");
        return;
    }
    if (!LeerNumero(arg[0], &numero))
        return;
    if (numero < 0 || numero > INT_MAX) {
        printf("Numero de recursiones fuera de rango\n");
        return;
    }
    Recursiva((int)numero);
}

static void LlenarMemoria(void *p, size_t cont, unsigned char byte)
{
    unsigned char *arr = p;
    for (size_t i = 0; i < cont; i++)
        arr[i] = byte;
}

void Cmd_memfill(char *arg[])
{
    void *direccion;
    size_t cantidad;
    char *fin;
    uintmax_t valor;
    if (arg[0] == NULL || arg[1] == NULL || arg[2] == NULL) {
        printf("Uso: memfill addr cont hcode\n");
        return;
    }
    if (!LeerDireccionMemoria(arg[0], &direccion) ||
        !LeerCantidadMemoria(arg[1], &cantidad))
        return;
    if (arg[2][0] == '\0' || arg[2][0] == '-') {
        printf("Byte hexadecimal no valido\n");
        return;
    }
    errno = 0;
    valor = strtoumax(arg[2], &fin, 16);
    if (arg[2] == fin || *fin != '\0' || errno == ERANGE || valor > UCHAR_MAX) {
        printf("Byte hexadecimal fuera de rango: %s\n", arg[2]);
        return;
    }
    LlenarMemoria(direccion, cantidad, (unsigned char)valor);
    printf("Llenados %zu bytes de memoria en %p con 0x%02x\n",
           cantidad, direccion, (unsigned int)valor);
}

void Cmd_read(char *arg[])
{
    int df;
    void *direccion;
    size_t cantidad;
    ssize_t leidos;
    if (arg[0] == NULL || arg[1] == NULL || arg[2] == NULL) {
        printf("Uso: read df addr cont\n");
        return;
    }
    if (!LeerDescriptor(arg[0], &df) ||
        !LeerDireccionMemoria(arg[1], &direccion) ||
        !LeerCantidadMemoria(arg[2], &cantidad))
        return;
    if ((uintmax_t)cantidad > (uintmax_t)SSIZE_MAX) {
        printf("Cantidad fuera de rango para read\n");
        return;
    }
    do {
        leidos = read(df, direccion, cantidad);
    } while (leidos == -1 && errno == EINTR);
    if (leidos == -1) {
        perror("Imposible leer del descriptor");
        return;
    }
    printf("Leidos %jd bytes del descriptor %d en %p\n",
           (intmax_t)leidos, df, direccion);
}

void Cmd_write(char *arg[])
{
    int df;
    void *direccion;
    size_t cantidad, total = 0;
    ssize_t escritos;
    const unsigned char *datos;
    if (arg[0] == NULL || arg[1] == NULL || arg[2] == NULL) {
        printf("Uso: write df addr cont\n");
        return;
    }
    if (!LeerDescriptor(arg[0], &df) ||
        !LeerDireccionMemoria(arg[1], &direccion) ||
        !LeerCantidadMemoria(arg[2], &cantidad))
        return;
    if ((uintmax_t)cantidad > (uintmax_t)SSIZE_MAX) {
        printf("Cantidad fuera de rango para write\n");
        return;
    }
    datos = direccion;
    while (total < cantidad) {
        escritos = write(df, datos + total, cantidad - total);
        if (escritos == -1) {
            if (errno == EINTR)
                continue;
            perror("Imposible escribir en el descriptor");
            return;
        }
        if (escritos == 0) {
            printf("No se pudo completar la escritura\n");
            return;
        }
        total += (size_t)escritos;
    }
    printf("Escritos %zu bytes desde %p en el descriptor %d\n", total, direccion, df);
}

void Cmd_memdump(char *arg[])
{
    void *direccion;
    size_t cantidad, desplazamiento = 0;
    const unsigned char *datos;

    if (arg[0] == NULL || arg[1] == NULL) {
        printf("Uso: memdump addr cont\n");
        return;
    }

    if (!LeerDireccionMemoria(arg[0], &direccion) ||
        !LeerCantidadMemoria(arg[1], &cantidad))
        return;

    datos = direccion;

    printf("Volcando %zu bytes desde %p\n", cantidad, direccion);

    while (desplazamiento < cantidad) {
        size_t bytes = cantidad - desplazamiento;

        if (bytes > 16)
            bytes = 16;

        printf("%p: ", (const void *)(datos + desplazamiento));

        for (size_t i = 0; i < bytes; i++) {
            printf("%02x ",
                   (unsigned int)datos[desplazamiento + i]);
        }

        for (size_t i = bytes; i < 16; i++)
            printf("   ");

        printf(" |");

        for (size_t i = 0; i < bytes; i++) {
            unsigned char c = datos[desplazamiento + i];

            printf("%c", isprint(c) ? c : '.');
        }

        printf("|\n");

        desplazamiento += bytes;
    }
}

/******************** ALLOCATE Y DEALLOCATE ********************/

static void *ObtenerMemoriaShmget(key_t clave, size_t tam)
{
    void *p;
    int aux, id, flags = 0777;
    struct shmid_ds s;

    if (tam)
        flags = flags | IPC_CREAT | IPC_EXCL;
    if (clave == IPC_PRIVATE) {
        errno = EINVAL;
        return NULL;
    }
    if ((id = shmget(clave, tam, flags)) == -1)
        return NULL;
    if ((p = shmat(id, NULL, 0)) == (void *)-1) {
        aux = errno;
        if (tam)
            shmctl(id, IPC_RMID, NULL);
        errno = aux;
        return NULL;
    }
    shmctl(id, IPC_STAT, &s);
    if (MemListAddShared(p, s.shm_segsz, clave) == -1) {
        aux = errno;
        shmdt(p);
        if (tam)
            shmctl(id, IPC_RMID, NULL);
        errno = aux;
        return NULL;
    }
    return p;
}

static void *MapearFichero(char *fichero, int protection)
{
    int df, map = MAP_PRIVATE, modo = O_RDONLY;
    struct stat s;
    void *p;
    char desc_mapeo[MAXFILENAME + 32];

    if (protection & PROT_WRITE)
        modo = O_RDWR;
    if (stat(fichero, &s) == -1 || (df = open(fichero, modo)) == -1)
        return NULL;
    if ((p = mmap(NULL, s.st_size, protection, map, df, 0)) == MAP_FAILED) {
        int aux = errno;
        close(df);
        errno = aux;
        return NULL;
    }
    if (MemListAddMmap(p, s.st_size, fichero, df) == -1) {
        int aux = errno;
        munmap(p, s.st_size);
        close(df);
        errno = aux;
        return NULL;
    }
    snprintf(desc_mapeo, sizeof(desc_mapeo), "Mapeo de %s", fichero);
    OpenFilesAdd(df, modo, desc_mapeo);
    return p;
}

static void do_AllocateMalloc(char *tr[])
{
    size_t tam;
    void *p;

    if (tr[0] == NULL) {
        MemListPrint(MEM_MALLOC);
        return;
    }
    if (!LeerCantidadMemoria(tr[0], &tam))
        return;

    if (tam == 0) {
        printf("No se asignan bloques de 0 bytes\n");
        return;
    }
    p = malloc(tam);
    if (p == NULL) {
        printf("Imposible obtener memoria con malloc: %s\n", strerror(errno));
        return;
    }
    if (MemListAddMalloc(p, tam) == -1) {
        printf("Imposible insertar en lista: %s\n", strerror(errno));
        free(p);
        return;
    }
    printf("Asignados %llu bytes en %p\n", (unsigned long long)tam, p);
}

static void do_AllocateCreateShared(char *tr[])
{
    key_t cl;
    size_t tam;
    void *p;
    intmax_t num_clave;

    if (tr[0] == NULL || tr[1] == NULL) {
        MemListPrint(MEM_SHARED);
        return;
    }

    if (!LeerNumero(tr[0], &num_clave))
        return;
    if (num_clave < INT_MIN || num_clave > INT_MAX) {
        printf("Clave fuera de rango válido\n");
        return;
    }
    cl = (key_t)num_clave;

    if (!LeerCantidadMemoria(tr[0], &tam))
        return;

    if (tam == 0) {
        printf("No se asignan bloques de 0 bytes\n");
        return;
    }
    if ((p = ObtenerMemoriaShmget(cl, tam)) != NULL)
        printf("Asignados %llu bytes en %p\n", (unsigned long long)tam, p);
    else
        printf("Imposible asignar memoria compartida clave %lu: %s\n",
               (unsigned long)cl, strerror(errno));
}

static void do_AllocateShared(char *tr[])
{
    key_t cl;
    void *p;
    intmax_t num_clave;

    if (tr[0] == NULL) {
        MemListPrint(MEM_SHARED);
        return;
    }
    if (!LeerNumero(tr[0], &num_clave))
        return;
    if (num_clave < INT_MIN || num_clave > INT_MAX) {
        printf("Clave fuera de rango válido\n");
        return;
    }
    cl = (key_t)num_clave;

    if ((p = ObtenerMemoriaShmget(cl, 0)) != NULL)
        printf("Memoria compartida de clave %lu  en %p\n", (unsigned long)cl, p);
    else
        printf("Imposible asignar memoria compartida clave %lu: %s\n",
               (unsigned long)cl, strerror(errno));
}

static void do_AllocateMmap(char *arg[])
{
    char *perm;
    void *p;
    int protection = 0;

    if (arg[0] == NULL) {
        MemListPrint(MEM_MMAP);
        return;
    }
    if ((perm = arg[1]) != NULL && strlen(perm) < 4) {
        if (strchr(perm, 'r') != NULL) protection |= PROT_READ;
        if (strchr(perm, 'w') != NULL) protection |= PROT_WRITE;
        if (strchr(perm, 'x') != NULL) protection |= PROT_EXEC;
    }
    if ((p = MapearFichero(arg[0], protection)) == NULL)
        perror("Imposible mapear fichero");
    else
        printf("fichero %s mapeado en %p\n", arg[0], p);
}

void Cmd_allocate(char *tr[])
{
    if (tr[0] == NULL) {
        MemListPrint(MEM_ALL);
        return;
    }
    if (!strcmp(tr[0], "-malloc"))
        do_AllocateMalloc(tr + 1);
    else if (!strcmp(tr[0], "-createshared"))
        do_AllocateCreateShared(tr + 1);
    else if (!strcmp(tr[0], "-shared"))
        do_AllocateShared(tr + 1);
    else if (!strcmp(tr[0], "-mmap"))
        do_AllocateMmap(tr + 1);
    else
        printf("uso: allocate [-malloc|-shared|-createshared|-mmap] ....\n");
}

static void do_DeallocateMalloc(char *tr[])
{
    size_t tam;
    MemBlock *b;

    if (tr[0] == NULL) {
        MemListPrint(MEM_MALLOC);
        return;
    }
    tam = (size_t)strtoull(tr[0], NULL, 10);
    b = MemListGetByMallocSize(tam);
    if (b == NULL) {
        printf("No hay bloque de ese tamano asignado con malloc\n");
        return;
    }
    free(b->addr);
    if (MemListDelByAddr(b->addr) == -1)
        printf("Imposible quitar de lista: %s\n", strerror(errno));
}

static void do_DeallocateShared(char *tr[])
{
    key_t cl;
    MemBlock *b;

    if (tr[0] == NULL) {
        MemListPrint(MEM_SHARED);
        return;
    }
    cl = (key_t)strtoul(tr[0], NULL, 10);
    b = MemListGetBySharedKey(cl);
    if (b == NULL) {
        printf("No hay bloque de esa clave mapeado en el proceso\n");
        return;
    }
    shmdt(b->addr);
    if (MemListDelByAddr(b->addr) == -1)
        printf("Imposible quitar de lista: %s\n", strerror(errno));
}

static void do_DeallocateMmap(char *tr[])
{
    MemBlock *b;

    if (tr[0] == NULL) {
        MemListPrint(MEM_MMAP);
        return;
    }
    b = MemListGetByMmapFile(tr[0]);
    if (b == NULL) {
        printf("Fichero %s no mapeado\n", tr[0]);
        return;
    }
    if (munmap(b->addr, b->size) == -1) {
        printf("Imposible desmapear %s en %p: %s\n",
               b->name, b->addr, strerror(errno));
        return;
    }
    if (MemListDelByAddr(b->addr) == -1)
        printf("Imposible quitar de lista: %s\n", strerror(errno));
}

static void do_SharedDelkey(char *args[])
{
    key_t clave;
    int id;
    char *key = args[0];

    if (key == NULL || (clave = (key_t)strtoul(key, NULL, 10)) == IPC_PRIVATE) {
        printf("      delkey necesita clave_valida\n");
        return;
    }
    if ((id = shmget(clave, 0, 0666)) == -1) {
        perror("shmget: imposible obtener memoria compartida");
        return;
    }
    if (shmctl(id, IPC_RMID, NULL) == -1)
        perror("shmctl: imposible eliminar memoria compartida");
}

static void *CadenatoPointer(char *s)
{
    void *p = NULL;
    sscanf(s, "%p", &p);
    return p;
}

static void do_DeallocateAddr(char *str_addr)
{
    void *p = CadenatoPointer(str_addr);
    MemBlock *b = MemListGetByAddr(p);

    if (b == NULL) {
        printf("Direccion %p no asignada con malloc, shared o mmap\n", p);
        return;
    }
    if (b->type == MEM_MALLOC) {
        free(b->addr);
    } else if (b->type == MEM_SHARED) {
        shmdt(b->addr);
    } else if (b->type == MEM_MMAP) {
        if (munmap(b->addr, b->size) == -1) {
            printf("Imposible desmapear %s en %p: %s\n",
                   b->name, b->addr, strerror(errno));
            return;
        }
    }
    if (MemListDelByAddr(b->addr) == -1)
        printf("Imposible quitar de lista: %s\n", strerror(errno));
}

void Cmd_deallocate(char *tr[])
{
    if (tr[0] == NULL) {
        MemListPrint(MEM_ALL);
        return;
    }
    if (!strcmp(tr[0], "-malloc"))
        do_DeallocateMalloc(tr + 1);
    else if (!strcmp(tr[0], "-shared"))
        do_DeallocateShared(tr + 1);
    else if (!strcmp(tr[0], "-mmap"))
        do_DeallocateMmap(tr + 1);
    else if (!strcmp(tr[0], "-delkey"))
        do_SharedDelkey(tr + 1);
    else
        do_DeallocateAddr(tr[0]);
}

/* Variables globales (inicializadas y no inicializadas) */
int var_global_init1 = 10;
int var_global_init2 = 20;
int var_global_ni1;

/* Variables estáticas (inicializadas y no inicializadas) */
static int var_static_init1 = 30;
static int var_static_init2 = 40;
static int var_static_ni1;

static void Do_Vars(void)
{
    /* Variables locales / automáticas (inicializadas y no inicializadas) */
    auto int var_local_init1 = 1;
    auto int var_local_init2 = 2;
    auto int var_local_ni1;

    printf("Variables locales   %18p,%18p,%18p\n",
           (void *)&var_local_init1, (void *)&var_local_init2, (void *)&var_local_ni1);
    printf("Variables globales  %18p,%18p,%18p\n",
           (void *)&var_global_init1, (void *)&var_global_init2, (void *)&environ);
    printf("Var (N.I.)globales  %18p,%18p,%18p\n",
           (void *)&var_global_ni1, (void *)&var_global_init1 /* relleno seguro */, (void *)&var_global_init2);
    printf("Variables staticas  %18p,%18p,%18p\n",
           (void *)&var_static_init1, (void *)&var_static_init2, (void *)&var_static_ni1);
    printf("Var (N.I.)staticas  %18p,%18p,%18p\n",
           (void *)&var_static_ni1, (void *)&var_static_init1 /* relleno seguro */, (void *)&var_static_init2);
}

static void Do_Funcs(void)
{
    printf("Funciones programa  %18p,%18p,%18p\n",
           (void *)&Cmd_memory, (void *)&Cmd_allocate, (void *)&Cmd_deallocate);
    printf("Funciones libreria  %18p,%18p,%18p\n",
           (void *)&printf, (void *)&malloc, (void *)&fork);
}

static void Do_Pmap(void)
{
    pid_t pid;
    char elpid[32];
    char *argv[4] = {"pmap", elpid, NULL};

    sprintf(elpid, "%d", (int)getpid());
    if ((pid = fork()) == -1) {
        perror("Imposible crear proceso");
        return;
    }
    if (pid == 0) {
        if (execvp(argv[0], argv) == -1)
            perror("cannot execute pmap (linux, solaris)");

        argv[0] = "procstat"; argv[1] = "vm"; argv[2] = elpid; argv[3] = NULL;
        if (execvp(argv[0], argv) == -1)
            perror("cannot execute procstat (FreeBSD)");

        argv[0] = "procmap"; argv[1] = elpid; argv[2] = NULL;
        if (execvp(argv[0], argv) == -1)
            perror("cannot execute procmap (OpenBSD)");

        argv[0] = "vmmap"; argv[1] = "-interleave"; argv[2] = elpid; argv[3] = NULL;
        if (execvp(argv[0], argv) == -1)
            perror("cannot execute vmmap (Mac-OS)");
        exit(1);
    }
    waitpid(pid, NULL, 0);
}

void Cmd_memory(char *arg[])
{
    int blocks = 0, funcs = 0, vars = 0, pmap = 0;
    int i;

    if (arg[0] == NULL) {
        MemListPrint(MEM_ALL);
        return;
    }

    for (i = 0; arg[i] != NULL; i++) {
        if (!strcmp(arg[i], "-blocks"))
            blocks = 1;
        else if (!strcmp(arg[i], "-funcs"))
            funcs = 1;
        else if (!strcmp(arg[i], "-vars"))
            vars = 1;
        else if (!strcmp(arg[i], "-all")) {
            blocks = 1;
            funcs = 1;
            vars = 1;
        }
        else if (!strcmp(arg[i], "-pmap"))
            pmap = 1;
        else {
            printf("Opcion %s no contemplada\n", arg[i]);
            return;
        }
    }

    if (blocks)
        MemListPrint(MEM_ALL);
    if (vars)
        Do_Vars();
    if (funcs)
        Do_Funcs();
    if (pmap)
        Do_Pmap();
}