Código y memoria de la sección de informática de mi trabajo fin de grado, Visualización realista por Monte-Carlo. La memoria está contenida en memoria\libro.pdf. A continuación se incluyen unas breves aclaraciones sobre el código:

Información básica de la estructura de la entrega de este trabajo. Se adjunta la memoria en un pdf y una carpeta con la implementación del código creado para los experimentos. Este código es, en su mayoría, obra de Peter Shirley, tal y como se ha especificado en la memoria del trabajo. Si se quiere acceder al trabajo original tal y como se escribió, se puede encontrar aquí: [Ray Tracing in One Weekend Github](https://github.com/RayTracing/raytracing.github.io), el que se ha escogido para este trabajo es la parte de Rest of Your Life, en concreto. El código se incluye aquí porque es necesario para ejecutar el código, y no hemos visto problema ya que el copyright original es CC0.

Es por esta razón que la mayor parte de los ficheros no están comentados ni documentados, ya que solo son obra original:
- xz_rect_sa.h
- ellipse.h
- ellipsessa.h
- rectangleMap.h
- Parte de main.cc

Y los archivos auxiliares para hacer las gráficas:
- graficas.py
- make_rsme_data.cpp

Cómo utilizar este código:
Se tiene que compilar mediante la orden

g++ -O3 -o main main.cc

Recomendamos optimización con -O3 debido a que el ejecutable necesita hacer muchos cálculos, y es lento. Para ejecutar el main debe pasarse la salida estándar a un fichero ppm, de la forma:

main > imagen.ppm

Esto genera una imagen que podemos visualizar con un programa correspondiente. GIMP funciona y es el que se ha usado en la creación de la memoria.

Antonio Checa.
