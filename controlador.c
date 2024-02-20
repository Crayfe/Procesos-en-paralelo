/*
 Programa controlador.c
 Autor: Cristian Ayuso Ferrón
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sysexits.h>
#include <signal.h>


//Variables globales para que sean accesibles por la interrupcion control c
FILE *fd_ex;
int ex_created=0;
char *f_exclusivo;
FILE *fd;
pid_t *hijo;
long *array_octetos;

//Funciones auxiliares y manejadores de interrupciones
long octetos_fichero(char *);
long *reparte_octetos(int , char *, long);
int palabras_totales(char *);
void controlC_handler(int);


int main(int argc, char *argv[]){	
	signal(SIGINT, controlC_handler);
	char *nombre_programa=malloc(strlen(argv[0])-2);//no se conoce el tamano del nombre del programa en tiempo de ejecucion, se asigna memoria dinamica
	memcpy(nombre_programa, &argv[0][2], strlen(argv[0]));// el nombre sale como ./Controlador y así no vale, hay que quitar el ./ para crear el directorio
	char *directorio_temporal="/tmp/";
	char *extension=".lock";
	f_exclusivo = malloc(strlen(directorio_temporal)+strlen(nombre_programa)+strlen(extension));
	sprintf(f_exclusivo,"%s%s%s",directorio_temporal,nombre_programa,extension); // al final debería quedar de la forma: /tmp/Controlador.lock
	free(nombre_programa);//esta cadena ya no sirve, se libera
	//printf("%s\n", f_exclusivo);
	fd_ex = fopen(f_exclusivo, "wx");
	if(fd_ex == NULL){
	  printf("Ya hay un proceso controlador en ejecucion, esperando 5 segundos a ver si termina...\n");
	  sleep(5);
	  fd_ex = fopen(f_exclusivo, "wx");
	  if(fd_ex == NULL){
	    printf("No hubo suerte, vuelve a intentarlo mas tarde :(\n");
	    exit(1);
	  }
	}
	ex_created=1;
	char *clave_cif = argv[1];
	char *f_diccionario = argv[2];
	int n_reventadores = atoi(argv[3]);
	char *f_reventador = argv[4];
	char *f_resultado = argv[5];
	
	printf("\n\n/// PROGRAMA CONTROLADOR FASE 3///\n\n");
	printf("\nParametros introducidos:\n1) Clave cifrada: %s\n2) Fichero diccionario: %s\n3) Numero de reventadores: %i\n4) Ejecutable reventador: %s\n5) Archivo de resultado: %s\n", clave_cif, f_diccionario, n_reventadores, f_reventador, f_resultado );
	fd = fopen(f_resultado, "w");
	fprintf(fd,"%s\n", clave_cif);
	fclose(fd);
	printf("\n\n>>> ASIGNANDO PORCIONES A REVENTADOR/ES >>>\n\n");
	int n_palabras =palabras_totales(f_diccionario);
	if(n_palabras < n_reventadores){
	  n_reventadores = n_palabras;
	  printf("No hay palabra para tanto reventador, el maximo de reventadores sera de  %i\n",n_reventadores);
	}
	long total_octetos = octetos_fichero(f_diccionario);
	long octetos_reventador = total_octetos/n_reventadores;
	array_octetos = reparte_octetos(n_reventadores, f_diccionario, total_octetos);
	printf("Octetos del diccionario %s: %li\n", f_diccionario, total_octetos);
	if(n_reventadores == 1){
		printf("Octeto inicial del reventador: 0\nOcteto final del reventador: %li\n", total_octetos);
	}else{
		printf("Octetos promedio por reventador: %li\n\n", octetos_reventador);
	}
	
	hijo = malloc(sizeof(pid_t)*(unsigned long)n_reventadores);
	char oct_ini[sizeof(long)+1] = "";
	char oct_fin[sizeof(long)+1] = "";
	for(int i =0; i< n_reventadores; i++){
	  printf("Reventador #%i:\n",i+1);
	  sprintf(oct_ini,"%li",array_octetos[2*i]);
	  sprintf(oct_fin,"%li",array_octetos[(2*i)+1]);
	  printf("Octeto inicial: %s\nOcteto final: %s\n\n", oct_ini, oct_fin);
	  hijo[i]= fork();
	  if(hijo[i] == 0){
	    char *params[7];
	    params[0] ="Reventador";
	    params[1]= f_diccionario;
	    params[2]= oct_ini;
	    params[3]= oct_fin;
	    params[4]= clave_cif;
	    params[5]= f_resultado;
	    params[6]=NULL;
	    execv(f_reventador, params);
	    perror("execv");
	    exit(0);
	  }
	}
	int found=0;
	int estado;
	int hijos_vivos = n_reventadores;
	while(hijos_vivos > 0 && !found){
	  pid_t hijo_favorito = wait(&estado);
	  printf("Proceso con pid: %i finalizado con estado %i\n",hijo_favorito, WEXITSTATUS(estado));
	  --hijos_vivos;
	  if(WEXITSTATUS(estado) == 0){
	      found = 1;
	      for(int i =0; i<n_reventadores; i++){
		if(hijo[i]==hijo_favorito || hijo[i]==0){
		  //no hacer nada, ya terminó.
		}else{
		  if(kill(hijo[i], SIGTERM)==-1)
		    printf("Error matando proceso hijo\n");
		  else
		    printf("El proceso con pid: %i ha sido forzado a finalizar\n",hijo[i]);
		}
	      }
	  }else{
	    for(int i =0; i<n_reventadores; i++){
	      if(hijo[i]==hijo_favorito)
		hijo[i]=0; //lo borra de la lista porque ya terminó
	    }
	  }
	  
	}

	if(found){
	  fd = fopen(f_resultado, "r");
	  char palabra[256]="";
	  fgets(palabra, 256, fd);
	  fgets(palabra, 256, fd);//La clave encontrada será la línea siguiente a la clave previamente introducida
	  printf("Clave encontrada: %s\n",palabra);
	  fclose(fd);
	}else{
	  fd = fopen(f_resultado, "a");
	  fprintf(fd,"No se encontro ninguna coincidencia.\n");
	  printf("No se encontro ninguna clave.\n");
	  fclose(fd);
	}
    fclose(fd_ex);
    remove("/tmp/Controlador.lock");
    free(array_octetos);
    free(hijo);
   printf("Fin del programa.\n");
return 0;
}
//funcion auxiliar que contempla la casuistica de que no hayan suficientes palabras a repartir entre los reventadores (tratandose de un ataque de diccionario muy raro sería)
int palabras_totales(char *f_dic){
  int palabras_totales=0;
    char letra;
  FILE *aux;
  aux = fopen(f_dic, "r");
  do{
    letra = (char)fgetc(aux);
    if(letra == '\n')
    ++palabras_totales;
  }while(letra != EOF);
  fclose(aux);
  return palabras_totales;
  
}
//funcion auxiliar para determinar cuanto ocupa el diccionario en octetos
long octetos_fichero(char *f_dic){
  long n_octetos = 0;
  char letra;
  FILE *aux;
  aux = fopen(f_dic, "r");
  do{
    letra = (char)fgetc(aux);
    ++n_octetos;
  }while(letra != EOF);
  fclose(aux);
  return n_octetos;
}
//Funcion auxiliar para hacer el reparto de octetos entre los distintos reventadores. 
//Para facilitar su manipulacion se creara un array dinamico de octetos de tal forma que los octetos de cada reventador i corresponden a los índices (2*i) y (2*i)+1
long *reparte_octetos(int n_reventadores, char *f_dic, long octetos_totales){
  long octetos_promedio = octetos_totales/n_reventadores;
  long *indice_octetos = malloc(sizeof(long)*(unsigned long)n_reventadores*2);
  indice_octetos[0]=0;//se conoce a priori la posicion del octeto inicial
  indice_octetos[2*n_reventadores-1]=octetos_totales;//idem para el octeto final
  char letra;
  FILE *aux;
  aux = fopen(f_dic, "r");
  long cursor=0;
  if(n_reventadores > 1){
	for(int i=0; i<n_reventadores-1; i++){
	      if(i==0){
		cursor = octetos_promedio;
	      }else{
		cursor = indice_octetos[2*i]+octetos_promedio;
	      }
	      fseek(aux, cursor, SEEK_SET);
	      letra = (char)fgetc(aux);
	      if(letra == '\n'){
		indice_octetos[(2*i)+1] = cursor;
		indice_octetos[(2*i)+2] = cursor+1;
	      }else{
		int count=0;
		do{   
		  letra = (char)fgetc(aux);
		  count++;
		}while(letra != '\n');
		cursor=cursor+count;
		indice_octetos[(2*i)+1] = cursor;
		indice_octetos[(2*i)+2] = cursor+1;
	      }
	}
  }
  fclose(aux);
  return indice_octetos;
}
//Esta implementación no se pedia en la practica, pero viene bien para hacer pruebas a la hora de intentar ejecutar otro programa Controlador cuando ya hay uno en ejecucion. 
//Al usar CTRL + C se interrumpe todo el programa y hay que liberar la memoria dinamica y eliminar el fichero exclusivo si este fue creado 
void controlC_handler(){
  if(ex_created){
    fclose(fd_ex);
    fclose(fd);
    remove(f_exclusivo);
    free(f_exclusivo);
    free(array_octetos);
    free(hijo);
  }
    exit(1);
}
