#####################################################################
### Declaración de macros de los ficheros de codigo fuente,
### objeto, cabecera y ejecutable.
###
### El alumno debe ajustar los valores de estas macros para su programa.
###

# MIS_FUENTES indica los nombres de todos los ficheros .c desarrollados
#   por el alumno.
# OBJETOS_PROPORCIONADOS indica los nombres de todos los ficheros .o
#   proporcionados al alumno. Se puede dejar en blanco si no hay ninguno.
# EJECUTABLE indica el nombre que quiere dar el alumno a su fichero ejecutable.

# Ficheros fuente necesarios para el controlador
MIS_FUENTES1 = controlador.c

# Ficheros fuente necesarios para el reventador
MIS_FUENTES2 = reventador.c probar-claves.c

# Nombre del programa ejecutable del controlador
EJECUTABLE1 = Controlador

# Nombre del programa ejecutable del reventador
EJECUTABLE2 = Reventador

# Bibliotecas que hay que incluir en el montaje de los ejecutables
BIBLIOTECAS = -lmd

#####################################################################

#####################################################################
### Opciones para el compilador y enlazador.
###
### Probablemente el alumno no necesitara modificarlas.
###

CFLAGS =  $(INCLUDES) -g -Wall -pedantic -Wconversion -Wredundant-decls -Wunreachable-code -Wuninitialized -Wformat=2 -Wstrict-prototypes
LDFLAGS = $(BIBLIOTECAS)

#####################################################################

#####################################################################
### Declaración de reglas y dependencias

all: $(EJECUTABLE1) $(EJECUTABLE2)

todo: all

limpia: clean

.sinclude ".depend"

$(EJECUTABLE1): $(MIS_FUENTES1:C/\.c$/.o/g) $(OBJETOS_PROPORCIONADOS)
	cc $(CFLAGS) -o $@ $> $(LDFLAGS)
$(EJECUTABLE2): $(MIS_FUENTES2:C/\.c$/.o/g) $(OBJETOS_PROPORCIONADOS)
	cc $(CFLAGS) -o $@ $> $(LDFLAGS)

clean:;
	rm -f $(MIS_FUENTES1:C/\.c$/.o/g) $(MIS_FUENTES2:C/\.c$/.o/g) *core *~ *.bak $(EJECUTABLE1) $(EJECUTABLE2)

depend:;
	@cc -E -MM $(MIS_FUENTES1) $(MIS_FUENTES2) > .depend
	@echo "Lista de dependencias creada"

#####################################################################
## Uso:
# make	-> Compila y genera el ejecutable si no hay error
# make clean o make limpia -> Borra los ficheros objeto, ejecutable y cores
# make depend -> Genera la lista de dependencias fuente->objeto
#####################################################################
