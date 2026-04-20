#include "claves.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 255  /* Longtud máxima de claves y cadenas */

/* Cada nodo de la lista representa una tupla completa. */
typedef struct nodo {
    char           key[MAXLEN + 1];
    char           value1[MAXLEN + 1];
    int            N;
    float          V[32];
    struct Paquete p;
    struct nodo   *next;
} nodo_t;

/* Cabeza de la lista NULL cuando no hay ninguna tupla guardada. */
static nodo_t *head = NULL;

/* Mutex que protege todos los accesos a la lista. */
static pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;


/* Comprueba que una cadena no sea NULL y no supere MAXLEN caracteres.
 * Devuelve 1 si es válida, 0 si no. */
static int check_str255(const char *s) {
    if (s == NULL) return 0;
    return strlen(s) <= MAXLEN;
}

/* Recorre la lista buscando el nodo con la clave indicada.
 * Hay que llamar a esta función con el mutex ya tomado.
 * Devuelve el puntero al nodo o NULL si no se encuentra. */
static nodo_t *find_node(const char *key) {
    nodo_t *cur = head;
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}


int destroy(void) {
    pthread_mutex_lock(&mx);

    /* Recorremos la lista liberndo cada nodo uno a uno. */
    nodo_t *cur = head;
    while (cur != NULL) {
        nodo_t *next = cur->next;
        free(cur);
        cur = next;
    }
    head = NULL;

    pthread_mutex_unlock(&mx);
    return 0;
}

int set_value(char *key, char *value1, int N_value2, float *V_value2,
        struct Paquete value3) {

    /* Rechazamos entradas inválidas antes de tocar la lista. */
    if (!check_str255(key) || !check_str255(value1)) return -1;
    if (N_value2 < 1 || N_value2 > 32)              return -1;
    if (V_value2 == NULL)                            return -1;

    pthread_mutex_lock(&mx);

    /* No permitimos claves duplicadas. */
    if (find_node(key) != NULL) {
        pthread_mutex_unlock(&mx);
        return -1;
    }

    nodo_t *n = (nodo_t *)malloc(sizeof(nodo_t));
    if (n == NULL) {
        pthread_mutex_unlock(&mx);
        return -1;
    }

    strncpy(n->key,    key,    MAXLEN);  n->key[MAXLEN]    = '\0';
    strncpy(n->value1, value1, MAXLEN);  n->value1[MAXLEN] = '\0';

    /* Iniciamos todo el vector a cero para que las posiciones sin usar
     * no contengan basura, y luego copiamos los N elementos reales. */
    n->N = N_value2;
    for (int i = 0; i < 32;       i++) n->V[i] = 0.0f;
    for (int i = 0; i < N_value2; i++) n->V[i] = V_value2[i];

    n->p = value3;

    /* Insertar al principio de la lista es O(1) y es suficiente aquí. */
    n->next = head;
    head    = n;

    pthread_mutex_unlock(&mx);
    return 0;
}

int get_value(char *key, char *value1, int *N_value2, float *V_value2,
        struct Paquete *value3) {

    if (!check_str255(key)) return -1;
    if (value1 == NULL || N_value2 == NULL || V_value2 == NULL || value3 == NULL)
        return -1;

    pthread_mutex_lock(&mx);

    nodo_t *n = find_node(key);
    if (n == NULL) {
        pthread_mutex_unlock(&mx);
        return -1;
    }

    /* Copiamos los datos al espacio que el llamador ha reservado. */
    strncpy(value1, n->value1, MAXLEN);
    value1[MAXLEN] = '\0';

    *N_value2 = n->N;
    for (int i = 0; i < n->N; i++) V_value2[i] = n->V[i];
    *value3 = n->p;

    pthread_mutex_unlock(&mx);
    return 0;
}

int modify_value(char *key, char *value1, int N_value2, float *V_value2,
        struct Paquete value3) {

    if (!check_str255(key) || !check_str255(value1)) return -1;
    if (N_value2 < 1 || N_value2 > 32)              return -1;
    if (V_value2 == NULL)                            return -1;

    pthread_mutex_lock(&mx);

    nodo_t *n = find_node(key);
    if (n == NULL) {
        pthread_mutex_unlock(&mx);
        return -1;
    }

    strncpy(n->value1, value1, MAXLEN);
    n->value1[MAXLEN] = '\0';

    /* Igual que en set_value: reiniciamos el vector antes de copiar. */
    n->N = N_value2;
    for (int i = 0; i < 32;       i++) n->V[i] = 0.0f;
    for (int i = 0; i < N_value2; i++) n->V[i] = V_value2[i];

    n->p = value3;

    pthread_mutex_unlock(&mx);
    return 0;
}

int delete_key(char *key) {
    if (!check_str255(key)) return -1;

    pthread_mutex_lock(&mx);

    nodo_t *cur  = head;
    nodo_t *prev = NULL;

    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            /* Desconectamos el nodo de la lista antes de liberarlo. */
            if (prev == NULL)
                head = cur->next;
            else
                prev->next = cur->next;
            free(cur);
            pthread_mutex_unlock(&mx);
            return 0;
        }
        prev = cur;
        cur  = cur->next;
    }

    pthread_mutex_unlock(&mx);
    return -1;  /* La clave no existía */
}

int exist(char *key) {
    if (!check_str255(key)) return -1;

    pthread_mutex_lock(&mx);
    nodo_t *n = find_node(key);
    pthread_mutex_unlock(&mx);

    return (n != NULL) ? 1 : 0;
}
