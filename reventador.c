/*
 Programa reventador.c
 Autor: Cristian Ayuso Ferrón
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "probar-claves.h"

//Variables globales para que sean accesibles por la interrupcion SIGTERM
FILE *fd;
char *palabra_descubierta;

void kill_handler(){
    fclose(fd);
    free(palabra_descubierta);
    exit(1);
}
int main(int argc, char *argv[]){
	signal(SIGTERM, kill_handler);
	char *f_diccionario;
	long oct_ini;
	long oct_fin;
	char *clave_cif;
	char *f_resultado;
	//printf("\n\n/// PROGRAMA REVENTADOR ///\n\n");	
	f_diccionario = argv[1];
	oct_ini = atoi(argv[2]);
	oct_fin = atoi(argv[3]);
	clave_cif = argv[4];
	f_resultado = argv[5];
	//printf("Parametros introducidos:\n1) Fichero del diccionario: %s\n2) Octeto inicial: %li\n3) Octeto final: %li\n4) Clave cifrada: %s\n5) Fichero resultado: %s\n", f_diccionario, oct_ini, oct_fin, clave_cif, f_resultado );
	//printf("\n\n>>> RECORRIENDO DICCIONARIO DESDE OCTETO %li A %li >>>\n\n", oct_ini, oct_fin);
	fd = fopen(f_diccionario, "r");
	//printf("Posicion: %li", ftell(fd));
	fseek(fd, oct_ini, SEEK_SET);
	long puntero=0;
	palabra_descubierta=malloc(MAX_PALABRA);
	int clave_vacia = 1;
	do{
	    char palabra[MAX_PALABRA]="";
	    fgets(palabra, MAX_PALABRA, fd);
	    clave_vacia = probar_combinaciones_palabra(palabra, palabra_descubierta, clave_cif);
	    if(!clave_vacia){
	      fd = fopen(f_resultado, "a");
	      fprintf(fd,"%s\n", palabra_descubierta);
	      fclose(fd);
	      free(palabra_descubierta);
	      exit(0);
	    }
	    puntero = ftell(fd);
	}while(puntero < oct_fin-1);
	fclose(fd);
	free(palabra_descubierta);
	exit(1);
}
