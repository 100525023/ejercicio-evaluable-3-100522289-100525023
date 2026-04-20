#ifndef CLAVES_H
#define CLAVES_H

struct Paquete {
   int x;
   int y;
   int z;
};

/**
 * @brief Esta llamada permite inicializar el servicio de elementos clave-valor1-valor2-valor3.
 * Mediante este servicio se destruyen todas las tuplas que estuvieran almacenadas previamente.
 *
 * @return int La función devuelve 0 en caso de éxito y -1 en caso de error.
 */
int destroy(void);

/**
 * @brief Este servicio inserta el elemento <key, value1, value2, value3>.
 * El vector correspondiente al valor 2 vendrá dado por la dimensión del vector (N_Value2) y
 * el vector en si (V_value2).
 * El servicio devuelve 0 si se insertó con éxito y -1 en caso de error.
 */
int set_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3);

/**
 * @brief Este servicio permite obtener los valores asociados a la clave key.
 * La cadena de caracteres asociada se devuelve en value1.
 * En N_Value2 se devuelve la dimensión del vector asociado al valor 2 y en V_value2 las componentes del vector.
 * Tanto value1 como V_value2 tienen que tener espacio reservado para poder almacenar el máximo número
 * de elementos posibles.
 */
int get_value(char *key, char *value1, int *N_value2, float *V_value2, struct Paquete *value3);

/**
 * @brief Este servicio permite modificar los valores asociados a la clave key.
 */
int modify_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3);

/**
 * @brief Este servicio permite borrar el elemento cuya clave es key.
 */
int delete_key(char *key);

/**
 * @brief Este servicio permite determinar si existe un elemento con clave key.
 */
int exist(char *key);

#endif