# Proyecto Servidor WEB / Integrantes:

-   Franco Araya - franco.araya@alumnos.uv.cl
-   Cindy Ramírez - cindy.ramirez@alumnos.uv.cl
-   Jeremy Castro - jeremy.castroe@alumnos.uv.cl

## Instalación

Para la compilación del programa ingrese a la carpeta ```bin ```:

```bash
$ cd bin
```

Estructura de las carpetas:
```
├── README.md
├── bin
│   ├── Makefile
│   └── config.json
├── documentos
│   └── Informe Tarea 2.pdf
├── src
│   ├── Makefile
│   ├── includes
│   │   ├── YASL.h
│   │   └── json.hpp
│   └── servidorWeb.cc
├── www-data
│   ├── index.html
│   ├── pagina1.html
│   ├── pagina2.html
│   └── pagina3.html
└── www-error
    ├── 400.html
    └── 404.html
    
```

Ejecute lo siguiente para la compilación:

```bash
$ make clean
$ make
```

Finalmente ejecutar:

```bash
$ ./ServidorWeb
```

## En caso de error en el puerto

Modificar el ```puerto``` en el archivo ```config.json```dentro de la carpeta ```bin```

```json
{
    "ip"            : "127.0.0.1",
    "puerto"        : 7200,
    "root_dir"      : "../www-data",
    "notFoundFile"  : "../www-error"
}
```

Cuando se tenga el servidor corriendo, en el navegador colocar la ```ìp``` de la configuración y el puerto.
```bash
http://127.0.0.1:7200
```

El servidor tiene a disposición 4 páginas:

```bash
├── index.html
├── pagina1.html
├── pagina2.html
└── pagina3.html
```
En el ejemplo anterior se redirige a ```index.html```
Y para navegar por las otras páginas:
```bash
http://127.0.0.1:7200/paginaX.html
```

Donde ```X``` es el número de la página.

En caso de que lo solicitado no exista, se retorna un ```error 404``` a su navegador.
