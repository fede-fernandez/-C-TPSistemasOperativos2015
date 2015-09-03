Para agregar la libreria :

Crear shared library

    File -> New -> C Project
    Configurar el proyecto con los siguiente valores

Project name: commonsDeAsedio 
Project type: Shared Library / Empty Project
Toolchains: Linux GCC

    Finish

y copien dentro la carpeta commonsDeAsedio del repositorio remoto
-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Configurar shared library

    Click derecho en el proyecto -> Properties -> C/C++ Build -> Settings -> Tool Settings -> Miscellaneous
    Tildar "Position Independent Code (-fPIC)
    OK
---------------------------------------------------------------------------------------------------------------------------------------------------------------------
Configurar proyecto para usar la shared library

    Click derecho en el proyecto -> Properties -> C/C++ General -> Paths and Symbols -> References
    Tildar commonsDeAsedio
    OK
    Incluir el archivo de la shared library en el proyecto: #include <commonsDeAsedio/headerQueSeQuieraUsar.h>
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
Hacer que Linkee con las commonsDeAsedio (OJO! cuando haya que ejecutarlo en linea de comandos no va a servir de nada esto, esto solamente es paar ejecutar desde Eclipse)

    Click derecho en el proyecto -> Run as -> Run Configurations... -> Pestaña Environment -> Boton New..

Name: LD_LIBRARY_PATH
Value: ${workspace_loc}/commonsDeAsedio/Debug

    OK
    Apply
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
Hacer que linkee nuestro proyecto con las commons en consola <--------------------
                                                                                                                                            |
cd workspace/commonsDeAsedio/Debug                                                                   |
                                                                                                                                            |  Este paso es solo para linkear nuestras commons por si se realizaron cambios 
make clean                                                                                                                        |                                         
                                                                                                                                            |
make <------------------------------------------------------------------------------------------------------

cd workspace/nombreDeProyecto/Debug

make clean 

make

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/commonsDeAsedio/Debug/

Ahora ya se puede ejecutar nuestro programa haciendo ./nombreDeProyecto





