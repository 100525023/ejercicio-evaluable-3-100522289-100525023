#include <stdio.h>
#include <string.h>
#include "claves.h"

/* Llama a get_value e imprime por pantalla todos los campos que devuelve.
 * Si la clave no existe o hay algún error, imprime el código de retorno. */
static void print_get(const char *key) {
    char           value1[256];
    int            N = 0;
    float          V[32];
    struct Paquete p;

    int r = get_value((char *)key, value1, &N, V, &p);
    if (r != 0) {
        printf("get_value('%s') -> %d\n", key, r);
        return;
    }

    printf("get_value('%s') -> 0 | value1='%s' N=%d V=[", key, value1, N);
    for (int i = 0; i < N; i++)
        printf("%s%.3f", (i ? "," : ""), V[i]);
    printf("] paquete=(%d,%d,%d)\n", p.x, p.y, p.z);
}

int main(void) {

    char           key[]    = "clave5";
    char           value1[] = "ejemplo valor1";
    float          V[]      = {2.3f, 0.5f, 23.45f};
    struct Paquete p        = {10, 5, 3};

    /* destroy e idempotencia */
    printf("T1  destroy()                            -> %d  (esperado  0)\n", destroy());
    printf("T2  destroy() segunda vez                -> %d  (esperado  0)\n", destroy());

    /* exist sobre servicio vacío */
    printf("T3  exist('%s') antes de set          -> %d  (esperado  0)\n", key, exist(key));
    printf("T4  exist('no_existe')                   -> %d  (esperado  0)\n", exist("no_existe"));

    /* get y delete sobre clave inexistente */
    printf("T5  delete_key('%s') sin insertar     -> %d  (esperado -1)\n", key, delete_key(key));
    printf("T6  "); print_get(key);   /* esperado -1 */

    /* inserción básica */
    printf("T7  set_value('%s', ...)              -> %d  (esperado  0)\n", key, set_value(key, value1, 3, V, p));
    printf("T8  exist('%s') tras set              -> %d  (esperado  1)\n", key, exist(key));
    printf("T9  "); print_get(key);   /* debe mostrar los datos insertados */

    /* duplicado */
    printf("T10 set_value duplicada                  -> %d  (esperado -1)\n", set_value(key, value1, 3, V, p));

    /* N fuera de rango */
    printf("T11 set_value N=0 (fuera de rango)       -> %d  (esperado -1)\n", set_value("kN0",  "v", 0,  V, p));
    printf("T12 set_value N=33 (fuera de rango)      -> %d  (esperado -1)\n", set_value("kN33", "v", 33, V, p));
    printf("T13 set_value N=1  (minimo valido)        -> %d  (esperado  0)\n", set_value("kN1",  "v", 1,  V, p));
    printf("T14 set_value N=32 (maximo valido)        -> %d  (esperado  0)\n",
            set_value("kN32", "v", 32, (float[32]){[0]=1.0f,[31]=2.0f}, p));

    /* modify_value */
    char           value1b[] = "modificado";
    float          Vb[]      = {9.0f, 7.5f};
    struct Paquete pb        = {1, 2, 3};

    printf("T15 modify_value('%s', ...)           -> %d  (esperado  0)\n", key, modify_value(key, value1b, 2, Vb, pb));
    printf("T16 "); print_get(key);   /* debe mostrar los datos modificados */
    printf("T17 modify_value clave inexistente       -> %d  (esperado -1)\n", modify_value("no_hay", value1b, 2, Vb, pb));
    printf("T18 modify_value N=0 (fuera de rango)   -> %d  (esperado -1)\n", modify_value(key, value1b, 0, Vb, pb));
    printf("T19 modify_value N=33 (fuera de rango)  -> %d  (esperado -1)\n", modify_value(key, value1b, 33, Vb, pb));

    /* exist después de modify */
    printf("T20 exist('%s') tras modify           -> %d  (esperado  1)\n", key, exist(key));

    /* N devuelto correcto en extremos */
    char  v1out[256]; int Nout; float Vout[32]; struct Paquete pout;
    get_value("kN1",  v1out, &Nout, Vout, &pout);
    printf("T21 get kN1:  N devuelto              -> %d  (esperado  1)\n", Nout);
    get_value("kN32", v1out, &Nout, Vout, &pout);
    printf("T22 get kN32: N devuelto              -> %d  (esperado 32)\n", Nout);

    /* delete */
    printf("T23 delete_key('%s')                  -> %d  (esperado  0)\n", key, delete_key(key));
    printf("T24 exist('%s') tras delete           -> %d  (esperado  0)\n", key, exist(key));
    printf("T25 delete_key segunda vez               -> %d  (esperado -1)\n", delete_key(key));
    printf("T26 "); print_get(key);   /* esperado -1 */

    /* varias claves independientes: insertar dos, borrar una, verificar que la otra sigue */
    char  k2[] = "clave_a";
    char  k3[] = "clave_b";
    float fv[] = {1.0f};
    set_value(k2, "aaa", 1, fv, (struct Paquete){1,1,1});
    set_value(k3, "bbb", 1, fv, (struct Paquete){2,2,2});
    delete_key(k2);
    printf("T27 delete '%s', exist '%s'          -> %d  (esperado  1)\n", k2, k3, exist(k3));

    /* destroy limpia todo */
    printf("T28 destroy() limpia todo               -> %d  (esperado  0)\n", destroy());
    printf("T29 exist('%s') tras destroy          -> %d  (esperado  0)\n", k3, exist(k3));
    printf("T30 exist('kN32') tras destroy           -> %d  (esperado  0)\n", exist("kN32"));

    /* el servicio sigue operativo tras destroy */
    printf("T31 set_value tras destroy               -> %d  (esperado  0)\n", set_value(key, value1, 3, V, p));
    printf("T32 exist('%s') tras re-insercion     -> %d  (esperado  1)\n", key, exist(key));

    return 0;
}
