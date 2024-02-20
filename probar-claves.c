#include <sys/types.h>
#include <md5.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <ctype.h>
#include <assert.h>
#include "probar-claves.h"

#define DEPURAR 0				// 1=mostrar por pantalla mensajes informativos
static int depura=DEPURAR;

static void convertir_hexadecimal_binario ( const char* hexadecimal, unsigned char* binario )
{
	char entrada[3];
	int i;
	int correcto;

	assert ( strlen(hexadecimal) == 32 );
	entrada[2] = '\0';
	for ( i = 0; i < 16; i++ )
	{	entrada[0] = hexadecimal[(i<<1)];
		entrada[1] = hexadecimal[(i<<1)+1];
		correcto = (sscanf(entrada, "%hhx", &binario[i]) == 1);
		assert ( correcto );
	}
}

static int probar_1palabra(char* palabra_candidata, unsigned char* clave_resumida)
{
	MD5_CTX contexto_md5;
	uint8_t palabra_resumida[16];
	const uint8_t * const candidata = (const uint8_t*) palabra_candidata;

	MD5Init(&contexto_md5);
	MD5Update(&contexto_md5, candidata, strlen(palabra_candidata));
	MD5Final(palabra_resumida, &contexto_md5);
	if (depura) printf ("\n[Probar-clave]: %s",palabra_candidata);
	return (memcmp(palabra_resumida, clave_resumida, 16));
}

static void transformacion_mayusculas(char* palabra, int cuantas)
{
	int i;

	for (i = 0; i < cuantas; i++)
		palabra[i] = (char) toupper(palabra[i]);
}

static void transformacion_cambioletra(char* palabra, char letra_orig, char letra_nueva)
{
	unsigned int i;

	for (i = 0; i < strlen(palabra); i++)
		if (palabra[i] == letra_orig)
			palabra[i] = letra_nueva;
		
}

static int probar_combinaciones_cambioletras(char* palabra_base, char* palabra_descubierta, unsigned char* clave_resumida)
{
	// Prueba a cambiar o por 0
	strcpy(palabra_descubierta, palabra_base);
	transformacion_cambioletra(palabra_descubierta, 'o', '0');
	transformacion_cambioletra(palabra_descubierta, 'O', '0');
	if (probar_1palabra(palabra_descubierta, clave_resumida) == 0)
		return(0);
	// Prueba a cambiar i por 1
	strcpy(palabra_descubierta, palabra_base);
	transformacion_cambioletra(palabra_descubierta, 'i', '1');
	transformacion_cambioletra(palabra_descubierta, 'I', '1');
	if (probar_1palabra(palabra_descubierta, clave_resumida) == 0)
		return(0);
	// Prueba a cambiar e por 3
	strcpy(palabra_descubierta, palabra_base);
	transformacion_cambioletra(palabra_descubierta, 'e', '3');
	transformacion_cambioletra(palabra_descubierta, 'E', '3');
	if (probar_1palabra(palabra_descubierta, clave_resumida) == 0)
		return(0);
	return(1);
}

static int probar_combinaciones_mayusculas(char* palabra_base, char* palabra_descubierta, unsigned char* clave_resumida)
{
	unsigned int i;
	char palabra[MAX_PALABRA];

	// Prueba todo en mayúsculas
	strcpy(palabra_descubierta, palabra_base);
	transformacion_mayusculas(palabra_descubierta, (int) strlen(palabra_descubierta));
	if (probar_1palabra(palabra_descubierta, clave_resumida) == 0)
		return(0);
	// También prueba cambios de letras mayúsculas
	strcpy(palabra, palabra_descubierta);
	if (probar_combinaciones_cambioletras(palabra, palabra_descubierta, clave_resumida) == 0)
		return(0);
	// Prueba a poner cada letra en mayúsculas, también con cambios de letras
	for (i = 0; i < strlen(palabra_base); i++) {
		strcpy(palabra_descubierta, palabra_base);
		transformacion_mayusculas(palabra_descubierta + i, 1);
		if (probar_1palabra(palabra_descubierta, clave_resumida) == 0)
			return(0);
		strcpy(palabra, palabra_descubierta);
		if (probar_combinaciones_cambioletras(palabra, palabra_descubierta, clave_resumida) == 0)
			return(0);
	}
	return(1);
}

static int probar_combinaciones_palabra4(char* palabra_base, char* palabra_descubierta, unsigned char* clave_resumida)
{
	// Prueba tal cual
	if (probar_1palabra(palabra_base, clave_resumida) == 0) {
		strcpy(palabra_descubierta, palabra_base);
		return(0);
	}
	// Prueba cambios de letras
	if (probar_combinaciones_cambioletras(palabra_base, palabra_descubierta, clave_resumida) == 0)
		return(0);
	// Prueba combinaciones con mayúsculas
	if (probar_combinaciones_mayusculas(palabra_base, palabra_descubierta, clave_resumida) == 0)
		return(0);
	return(1);
}

static int probar_combinaciones_palabra3(char* palabra_base, char* palabra_descubierta, unsigned char* clave_resumida)
{
	char palabra[MAX_PALABRA];
	unsigned int i;
	// Prueba tal cual
	if (probar_combinaciones_palabra4(palabra_base, palabra_descubierta, clave_resumida) == 0)
		return(0);
	// Le da la vuelta
	for (i = 0; i < strlen(palabra_base); i++)
		palabra[strlen(palabra_base) - i - 1] = palabra_base[i];
	palabra[strlen(palabra_base)] = '\0';
	if (probar_combinaciones_palabra4(palabra, palabra_descubierta, clave_resumida) == 0)
		return(0);
	return(1);
}

static int probar_combinaciones_palabra2(char* palabra_base, char* palabra_descubierta, unsigned char* clave_resumida)
{
	char palabra[MAX_PALABRA];
	char* cP;

	cP = palabra_base;
	while (*cP != '\0') {
		*cP = (char) tolower(*cP);
		cP++;
	}
	// Prueba tal cual
	if (probar_combinaciones_palabra3(palabra_base, palabra_descubierta, clave_resumida) == 0)
		return(0);
	// La concatena dos veces
	strcpy(palabra, palabra_base);
	strcat(palabra, palabra_base);
	
	if (probar_combinaciones_palabra3(palabra, palabra_descubierta, clave_resumida) == 0)
		return(0);
	return(1);
}

int probar_combinaciones_palabra ( const char* palabra_base, char* palabra_descubierta, const char* clave_resumida )
{
	char palabra[MAX_PALABRA];
	unsigned char clave_resumida_binaria[16];
	char i;
	/* Convierte la palabra resumida desde cadena hexadecimal a datos binarios */
	convertir_hexadecimal_binario(clave_resumida, clave_resumida_binaria);

	// Prueba tal cual
	assert ( strlen(palabra_base) + 1 < MAX_PALABRA );
	strcpy(palabra, palabra_base);
	if (probar_combinaciones_palabra2(palabra, palabra_descubierta, clave_resumida_binaria) == 0)
		return(0);
	// Le pone un dígito por delante
	for (i = '0'; i <= '9'; i++) {
		palabra[0] = i;
		strcpy(palabra + 1, palabra_base);
		if (probar_combinaciones_palabra2(palabra, palabra_descubierta, clave_resumida_binaria) == 0)
			return(0);
	}
	// Le pone un dígito por detrás
	for (i = '0'; i <= '9'; i++) {
		strcpy(palabra, palabra_base);
		palabra[strlen(palabra+1)] = '\0';
		palabra[strlen(palabra)] = i;
		if (probar_combinaciones_palabra2(palabra, palabra_descubierta, clave_resumida_binaria) == 0)
			return(0);
	}
	return(1);
}

