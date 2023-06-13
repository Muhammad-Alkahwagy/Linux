#
# Makefile
#

all: add-server add-client

%.o: %.c
	gcc -Wall -c $< `pkg-config --cflags dbus-1`
# The .o extension specifies the object file extension. 	
# The .c extension specifies the c file extension. 	
#  -Wall  :  flag enables all warning messages
# -c flag :stops the compiler after the compilation stage without performing the linking stage.
# $< :  represents the first prerequisite of the rule, which is the C source file that needs to be compiled.
# pkg-config  : is used to retrieve the appropriate compiler flags for the dbus-1 library.
# --cflags :option tells pkg-config to output the compiler flags needed to compile code that uses the dbus-1 library
#---------------------------------------------------------------------------------------------------------------------------------------------
add-server: add-server.o
	gcc add-server.o -o add-server `pkg-config --libs dbus-1`
#links the add-server executable with the dbus-1 library and creates the final executable file.

add-client: add-client.o
	gcc add-client.o -o add-client `pkg-config --libs dbus-1`

.PHONY: clean
# PHONY mean clean target is not a real file, and that the commands associated with it should always be executed, even if there is a file named clean in the directory.
clean:
	rm *.o add-server add-client

