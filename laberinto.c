#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// CONSTANTES
#define MURO '#'
#define CAMINO ' '
#define RUTA '*'
#define ENTRADA 'E'
#define SALIDA 'S'
#define MAX 50

// TIPOS DE DATOS (MOLDES)
typedef struct {
    int x, y;
} Punto;

// VARIABLES GLOBALES
char laberinto[MAX][MAX]; //Matriz laberinto
int filas = 10;    // Valor por defecto
int columnas = 10; // Valor por defecto

// Estructuras específicas para el algoritmo BFS (Resolutor)
Punto cola[MAX * MAX]; // Peor caso (anotar 2500 coordenadas para revisar). Reserva espacio en memoria.
int inicioCola = 0; //Es de donde sacamos al siguiente para atenderlo (el frente de la fila).
int finCola = 0; // Es de donde se anota el próximo que llega (el final de la fila).
Punto padres[MAX][MAX]; 
int visitado[MAX][MAX];

// 4. PROTOTIPOS (El índice de funciones)
void dibujarLaberinto(int filas, int columnas);
void inicializarLaberinto(int filas, int columnas);
void generarLaberinto_DFS(int x, int y, int filas, int columnas);
void intercambiar(int *a, int *b);
int resolverLaberinto_BFS(int inicioX, int inicioY, int filas, int columnas);
void reconstruirCamino(int x, int y);


int main(int argc, char *argv[]) {
    srand(time(NULL)); // configurador del azar
    int filas, columnas;

    // MEDICIÓN DE TIEMPO Y RESOLUCIÓN
    clock_t tiempoInicio, tiempoFin; // almacenar pulsos de la CPU
    double tiempoTotal;

    // Captura de parámetros externos
    if (argc == 3) { // calcula cuantas palabras se escribieron en la terminal
        filas = atoi(argv[1]); //segundo argumento ingresado (argv - array de textos)
        columnas = atoi(argv[2]); //tercer argumento ingresado (atoi convierte texto a integral)
    } else {
        // En el caso de que no se hayan ingresado dos números, se piden al usuario.
        printf("No se detectaron parametros externos. Por favor, ingrese dimensiones: \n");
        printf("Filas: ");
        scanf("%d", &filas);
        printf("Columnas: ");
        scanf("%d", &columnas);
    }

    // Validación de límites
    if (filas >= MAX) filas = MAX - 1;
    if (columnas >= MAX) columnas = MAX - 1;
    if (filas < 5) filas = 5;
    if (columnas < 5) columnas = 5;

    // Lógica de conversión a impares
    if (filas % 2 == 0) filas--;
    if (columnas % 2 == 0) columnas--;

    tiempoInicio = clock(); // <--- Empezamos justo antes de inicializar el laberinto (devuelve un numero entero que representa tics)

    // Preparar el bloque de muros
    inicializarLaberinto(filas, columnas); 

    // Se genera el laberinto
    generarLaberinto_DFS(1, 1, filas, columnas);

    // Definir Entrada y Salida conectadas
    laberinto[0][1] = ENTRADA;
    // laberinto[1][1] = CAMINO; // Asegura que la entrada conecte al primer nodo
    laberinto[filas - 2][columnas - 1] = SALIDA;

    // se dibuja el tablero
    dibujarLaberinto(filas, columnas);

    int exito = resolverLaberinto_BFS(0, 1, filas, columnas); // Se inicia el proceso de resolución
    tiempoFin = clock();    // <-- Paramos el reloj apenas termina la resolución

    // Calcular la diferencia en segundos
    double tiempoSegundos = ((double)(tiempoFin - tiempoInicio)) / CLOCKS_PER_SEC; // cantidad de pulsos por segundo en esta computadora

    if (exito) {
        dibujarLaberinto(filas, columnas);
        printf("\n--- LABERINTO RESUELTO (Camino mas corto) ---\n");
        printf("Salida encontrada con exito!");
    } else {
        printf("\nNo se encontro una ruta posible.");
    }


    // 4. Mostrar resultado del cronómetro
    printf("\nTiempo de ejecucion: %f segundos\n", tiempoSegundos);
    return 0;
}

//CUERPOS DE LAS FUNCIONES

// Función para mostrar el laberinto en consola
void dibujarLaberinto(int filas, int columnas) {
    system("cls");

    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            printf("%c ", laberinto[i][j]);
        }
        printf("\n");
    }
}

// Inicializa todo con MUROS
void inicializarLaberinto(int filas, int columnas) {
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            laberinto[i][j] = MURO;
        }
    }
}

// --- FUNCIÓN DE GENERACIÓN ---
void generarLaberinto_DFS(int x, int y, int filas, int columnas) {
    // Marcamos la celda actual como CAMINO (visitado)
    laberinto[x][y] = CAMINO;

    // Direcciones: Arriba, Abajo, Izquierda, Derecha
    int dX[] = {-2, 2, 0, 0};
    int dY[] = {0, 0, -2, 2};

    // Creamos un orden aleatorio para explorar las 4 direcciones
    int orden[] = {0, 1, 2, 3}; //24 opciones posibles una vez terminada la ejecución del for
    for (int i = 0; i < 4; i++) {
        int r = rand() % 4;
        intercambiar(&orden[i], &orden[r]);
    }

    // Intentamos movernos en las 4 direcciones
    for (int i = 0; i < 4; i++) {
        int dir = orden[i];
        int nuevoX = x + dX[dir];
        int nuevoY = y + dY[dir];

        // ¿Es un movimiento válido? (Dentro de límites y es un MURO sin visitar)
        if (nuevoX > 0 && nuevoX < filas - 1 && nuevoY > 0 && nuevoY < columnas - 1 && laberinto[nuevoX][nuevoY] == MURO) {
            
            // "Rompemos" el muro intermedio
            laberinto[x + dX[dir] / 2][y + dY[dir] / 2] = CAMINO;
            
            // Llamada recursiva (seguimos excavando desde la nueva celda)
            generarLaberinto_DFS(nuevoX, nuevoY, filas, columnas);
        }
    }
}
// Función auxiliar para mezclar las direcciones
void intercambiar(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int resolverLaberinto_BFS(int inicioX, int inicioY, int filas, int columnas) {
    // 1. LIMPIEZA INICIAL: Ponemos todo en 0 y los padres en -1
    for(int i=0; i < filas; i++) {
        for(int j=0; j < columnas; j++) {
            visitado[i][j] = 0;
            padres[i][j] = (Punto){-1, -1}; // -1 significa "no tengo padre aún"
        }
    }

    // 2. EMPEZAR: Ponemos la entrada en la cola y la marcamos
    inicioCola = 0;
    finCola = 0;
    cola[finCola++] = (Punto){inicioX, inicioY};
    visitado[inicioX][inicioY] = 1;

    // Direcciones de movimiento (esta vez de 1 en 1, no de 2 en 2)
    int dX[] = {1, -1, 0, 0}; 
    int dY[] = {0, 0, 1, -1};

    // 3. EL BUCLE PRINCIPAL: Mientras haya gente en la fila esperando...
    while (inicioCola < finCola) {
        Punto actual = cola[inicioCola++]; // Sacamos al primero ("Atendemos")

        // ¿Llegamos a la salida?
        if (laberinto[actual.x][actual.y] == SALIDA) {
            reconstruirCamino(actual.x, actual.y); // ¡Dibujamos el éxito!
            return 1;
        }

        // 4. EXPLORAR: Miramos los 4 vecinos del punto actual
        for (int i = 0; i < 4; i++) {
            int nX = actual.x + dX[i];
            int nY = actual.y + dY[i];

            // ¿Es un lugar válido, no visitado y caminable?
            if (nX >= 0 && nX < filas && nY >= 0 && nY < columnas &&
                !visitado[nX][nY] && (laberinto[nX][nY] == CAMINO || laberinto[nX][nY] == SALIDA)) {
                
                visitado[nX][nY] = 1;      // Lo marcamos para no repetir
                padres[nX][nY] = actual;   // ¡Anotamos quién es su padre!
                cola[finCola++] = (Punto){nX, nY}; // Lo mandamos al final de la fila
            }
        }
    }
    return 0; // Si la fila se vacía y no vimos la SALIDA, no hay camino.
}

void reconstruirCamino(int x, int y) {
    // 1. Empezamos mirando al "padre" de la celda donde está la salida
    Punto actual = padres[x][y];

    // 2. El bucle sigue el rastro de migas de pan
    while (actual.x != -1) {
        // Si llegamos a la entrada, nos detenemos para no borrar el símbolo '['
        if (laberinto[actual.x][actual.y] == ENTRADA) {
            break;
        }

        // Marcamos la posición como parte de la ruta óptima
        laberinto[actual.x][actual.y] = RUTA;

        // "Saltamos" al padre del punto actual para seguir retrocediendo
        actual = padres[actual.x][actual.y];
    }
}