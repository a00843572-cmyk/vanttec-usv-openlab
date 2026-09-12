# VantTec USV Open Lab - Detección de Objetos y Estimación de Profundidad Relativa

## Descripción general

Este proyecto fue desarrollado para el reto de software del VantTec USV Open Lab.

La aplicación permite detectar objetos a partir de una imagen o de una cámara web, identificar el centro de cada detección mediante coordenadas de imagen `(u, v)` y asignar un valor de profundidad relativa a cada objeto detectado.

El proyecto está implementado en C++ utilizando OpenCV y también puede compilarse y ejecutarse mediante Docker.

---

## Funcionalidades

- Detección de objetos utilizando YOLOv5.
- Visualización de bounding boxes.
- Identificación de la clase del objeto.
- Cálculo del nivel de confianza de la detección.
- Obtención del centro de la detección `(u, v)`.
- Estimación de profundidad relativa.
- Procesamiento de imágenes.
- Procesamiento mediante cámara web.
- Compatibilidad con Docker.
- Código organizado de forma modular en C++.

---

## Estructura del proyecto

```text
vanttec-usv-openlab/
├── assets/
│   ├── test.jpg
│   └── result.jpg
├── include/
│   ├── Detection.hpp
│   ├── DepthEstimator.hpp
│   └── ObjectDetector.hpp
├── models/
│   ├── classes.txt
│   └── yolo.onnx
├── src/
│   ├── DepthEstimator.cpp
│   ├── ObjectDetector.cpp
│   └── main.cpp
├── CMakeLists.txt
├── Dockerfile
├── .dockerignore
└── README.md
```

---

## Detección de objetos

Para la detección de objetos se utiliza un modelo YOLOv5 en formato ONNX.

El módulo DNN de OpenCV se utiliza para:

1. Cargar el modelo ONNX.
2. Preparar la imagen de entrada.
3. Ejecutar la inferencia.
4. Obtener los bounding boxes.
5. Calcular los niveles de confianza.
6. Aplicar Non-Maximum Suppression, o NMS.

Para cada detección válida, el programa almacena:

- ID de clase.
- Nombre de la clase.
- Nivel de confianza.
- Bounding box.
- Coordenada central `(u, v)`.
- Profundidad relativa.

El centro de cada bounding box se calcula mediante:

```text
u = x + width / 2
v = y + height / 2
```

Estas coordenadas representan la posición aproximada del centro del objeto dentro de la imagen.

---

## Estimación de profundidad relativa

La implementación actual proporciona una estimación de profundidad relativa.

No representa una distancia real en metros.

La estimación utiliza el área aparente que ocupa el bounding box del objeto dentro de la imagen.

Primero se normaliza el área del bounding box con respecto al área total de la imagen:

```text
normalized_area = bounding_box_area / image_area
```

Posteriormente se calcula la profundidad relativa mediante:

```text
relative_depth = 1 / sqrt(normalized_area)
```

Por lo tanto:

- Un objeto que ocupa una mayor área de la imagen obtiene un valor menor de profundidad relativa y se interpreta como más cercano.
- Un objeto que ocupa una menor área obtiene un valor mayor de profundidad relativa y se interpreta como más lejano.

El valor obtenido es adimensional y no debe interpretarse como metros.

---

## Limitaciones de la estimación de profundidad

Este método utiliza el tamaño aparente del objeto como aproximación de su distancia relativa.

Por esta razón presenta varias limitaciones:

- Los objetos pueden tener diferentes tamaños físicos.
- La orientación del objeto puede modificar el tamaño del bounding box.
- Las oclusiones parciales pueden afectar el cálculo.
- Los errores en la detección afectan directamente la estimación.
- La perspectiva de la cámara modifica el tamaño aparente de los objetos.
- Dos objetos de diferente tamaño físico pueden producir valores distintos aunque se encuentren aproximadamente a la misma distancia.

Para obtener una distancia métrica sería necesario incorporar información adicional, por ejemplo:

- Calibración de cámara.
- Dimensiones conocidas de los objetos.
- Visión estéreo.
- Sensores de profundidad.
- Modelos especializados de estimación monocular de profundidad.

---

## Requisitos

Para compilar el proyecto de forma nativa se requiere:

- Linux.
- C++17.
- CMake.
- OpenCV.
- Docker para ejecución mediante contenedor.

---

## Compilación

Desde la carpeta raíz del proyecto:

```bash
mkdir -p build
cd build
cmake ..
make -j2
cd ..
```

El ejecutable se genera en:

```text
build/usv_vision
```

---

## Modo imagen

Para procesar una imagen se utiliza:

```bash
./build/usv_vision --image assets/test.jpg
```

El resultado procesado se guarda en:

```text
assets/result.jpg
```

La imagen resultante muestra:

- Bounding box.
- Clase detectada.
- Nivel de confianza.
- Centro de la detección.
- Profundidad relativa.

La terminal también muestra información sobre cada detección.

Ejemplo:

```text
Class: bottle | Confidence: 0.43 | Center: (u, v) | Relative depth: value
```

---

## Modo cámara

Para utilizar la cámara web predeterminada:

```bash
./build/usv_vision --camera 0
```

El programa procesa continuamente los frames obtenidos desde la cámara.

Cada frame pasa por el detector de objetos y posteriormente se calcula la profundidad relativa de cada detección.

Para cerrar el programa se puede presionar:

```text
Q
```

o:

```text
ESC
```

---

## Docker

El proyecto también puede compilarse y ejecutarse dentro de un contenedor Docker.

### Construcción de la imagen

Desde la carpeta raíz:

```bash
docker build -t usv-vision .
```

---

## Prueba mediante Docker

Para ejecutar la prueba predeterminada:

```bash
docker run --rm usv-vision
```

También puede ejecutarse de forma explícita el modo imagen:

```bash
docker run --rm usv-vision \
    ./build/usv_vision --image assets/test.jpg
```

---

## Cámara web mediante Docker

En un sistema Linux, el dispositivo de cámara puede compartirse con el contenedor mediante:

```bash
docker run --rm \
    --device=/dev/video0:/dev/video0 \
    usv-vision \
    ./build/usv_vision --camera 0
```

La visualización de la ventana de OpenCV desde Docker puede requerir adicionalmente acceso al sistema gráfico del host.

---

## Manejo de errores

El programa incluye validaciones básicas para detectar errores comunes.

Entre ellos:

- Imagen inexistente o inválida.
- Cámara no disponible.
- Frame vacío.
- Modelo de detección no encontrado.
- Archivo de clases no encontrado.
- Salida inesperada del modelo YOLO.

Cuando ocurre alguno de estos errores, el programa muestra un mensaje en la terminal.

---

## Tecnologías utilizadas

- C++17.
- OpenCV.
- OpenCV DNN.
- YOLOv5.
- ONNX.
- CMake.
- Docker.

---

## Flujo de procesamiento

El flujo general de la aplicación es:

```text
Imagen / Cámara
       ↓
Preprocesamiento con OpenCV
       ↓
Detección de objetos con YOLOv5
       ↓
Bounding box + clase + confianza
       ↓
Obtención del centro (u, v)
       ↓
Estimación de profundidad relativa
       ↓
Visualización y resultado final
```

---

## Uso dentro de un vehículo autónomo

La información obtenida por este módulo puede utilizarse posteriormente como entrada para otros componentes del sistema de navegación.

Por ejemplo:

```text
Percepción
    ↓
Detección de objetos
    ↓
Posición aproximada del objeto
    ↓
Profundidad relativa
    ↓
Planeación de trayectoria
    ↓
Evasión de obstáculos
```

La salida de este proyecto representa una primera aproximación a un módulo de percepción para un vehículo autónomo de superficie.

---

## Posibles mejoras

El proyecto puede ampliarse posteriormente mediante:

- Calibración de la cámara.
- Estimación de distancia métrica.
- Uso de cámaras estéreo.
- Integración de cámaras de profundidad.
- Modelos de profundidad monocular.
- Mejoras en el preprocesamiento de imágenes.
- Letterboxing para preservar la relación de aspecto durante la entrada a YOLO.
- Integración con ROS 2.
- Publicación de detecciones mediante mensajes ROS 2.
- Integración con módulos de navegación.
- Integración con planeación de trayectoria.
- Integración con sistemas de evasión de obstáculos.

---

## Resultado actual

La implementación actual permite detectar objetos y asociar cada detección con información básica de posición dentro de la imagen.

Para cada objeto detectado se obtiene:

```text
Clase
Confianza
Bounding box
Centro (u, v)
Profundidad relativa
```

Durante las pruebas realizadas con una imagen que contiene una botella, el modelo fue capaz de detectar correctamente el objeto y generar una imagen de salida anotada.

El proyecto también puede compilarse y ejecutarse correctamente dentro de Docker.

