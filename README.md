![GRAPHOS](res/img/graphos.png)

Open-source photogrammetric software for 3D reconstruction.

![gui](res/img/gui.png)

Construir la Imagen Docker:

docker build -t graphos .

Ejecutar un Contenedor con Soporte de GPU:

docker run --gpus all --name GRAPHOS -it graphos