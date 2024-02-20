/* Módulo para reventar claves conociendo su codificación MD5
 */
#ifndef PROBAR_CLAVES_H_INCLUDED
#define PROBAR_CLAVES_H_INCLUDED

#define MAX_PALABRA 256 /* longitud máxima de las palabras a procesar */

/* Prueba variaciones de la palabra base para intentar encontrar una palabra cuyo
 * resumen MD5 se corresponda con la recibida en clave_resumida.
 *	- palabra_base: Parámetro de entrada que contiene la palabra base a probar.
 *	- palabra_descubierta: Parámetro de salida que contendrá la clave (si se descubre).
 *	- clave_resumida: Parámetro de entrada que contiene la codificación MD5 de la clave.
 *	                  Debe tener una longitud de 32 caracteres y cada uno de ellos
 *			  debe corresponder a un dígito hexadecimal.
 * devuelve 0 si ha descifrado la clave, en cuyo caso palabra_descubierta contendrá
 * la clave descubierta, y devuelve un valor distinto de 0 si no ha sido capaz de
 * descifrar la clave, en cuyo caso el valor de palabra_descubierta quedará indeterminado.
 */

extern int probar_combinaciones_palabra (
	const char* palabra_base,
	char* palabra_descubierta,
	const char* clave_resumida
);

#endif
