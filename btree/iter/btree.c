/*
 * Binárny vyhľadávací strom — iteratívna varianta
 *
 * S využitím dátových typov zo súboru btree.h, zásobníkov zo súborov stack.h a
 * stack.c a pripravených kostier funkcií implementujte binárny vyhľadávací
 * strom bez použitia rekurzie.
 */

#include "../btree.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializácia stromu.
 *
 * Užívateľ musí zaistiť, že incializácia sa nebude opakovane volať nad
 * inicializovaným stromom. V opačnom prípade môže dôjsť k úniku pamäte (memory
 * leak). Keďže neinicializovaný ukazovateľ má nedefinovanú hodnotu, nie je
 * možné toto detegovať vo funkcii.
 */
void bst_init(bst_node_t **tree) {
    *tree = NULL;
}

/*
 * Nájdenie uzlu v strome.
 *
 * V prípade úspechu vráti funkcia hodnotu true a do premennej value zapíše
 * hodnotu daného uzlu. V opačnom prípade funckia vráti hodnotu false a premenná
 * value ostáva nezmenená.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
    stack_bst_t s1;
    stack_bst_init(&s1);

    stack_bst_push(&s1, tree);
    while (!stack_bst_empty(&s1)) {
        bst_node_t *el = stack_bst_top(&s1);
        stack_bst_pop(&s1);
        // skip
        if (el == NULL) continue;

        // nasli sme
        if (el->key == key) {
            *value = el->value;
            return true;
        } else if (key < el->key && el->left) {  // nenasli sme, pridaj do stacku lavy alebo pravy podstrom
            stack_bst_push(&s1, el->left);
        } else if (key > el->key && el->right) {
            stack_bst_push(&s1, el->right);
        }
    }

    return false;
}

/*
 * Vloženie uzlu do stromu.
 *
 * Pokiaľ uzol so zadaným kľúčom v strome už existuje, nahraďte jeho hodnotu.
 * Inak vložte nový listový uzol.
 *
 * Výsledný strom musí spĺňať podmienku vyhľadávacieho stromu — ľavý podstrom
 * uzlu obsahuje iba menšie kľúče, pravý väčšie.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
    bst_node_t *newNode;
    // vytvorame novy node, ak nepotrebujem, tak vymazem
    newNode = malloc(sizeof(bst_node_t));
    // kontrola mallocu
    if (newNode == NULL) exit(1);
    newNode->key = key;
    newNode->value = value;
    newNode->left = NULL;
    newNode->right = NULL;

    if (*tree == NULL) {
        *tree = newNode;
        return;
    }

    stack_bst_t s1;
    stack_bst_init(&s1);

    // najdi kde ulozit
    bst_node_t *ptr = *tree;

    while (ptr != NULL) {
        if (ptr->key == key) {
            // nasli sme, prepiseme hodnotu
            ptr->value = value;
            free(newNode);
            return;
        }

        // idem vlavo, hladam najposlednejsi
        if (ptr->key > key) {
            if (ptr->left == NULL) {
                // vkladame do laveho
                ptr->left = newNode;
                return;
            } else {
                ptr = ptr->left;
            }

        }
            // idem vpravo, hladam najposlednejsi
        else { // ptr-key < key
            if (ptr->right == NULL) {
                // vkladame do praveho
                ptr->right = newNode;
                return;
            } else {
                ptr = ptr->right;
            }
        }
    }
}

/*
 * Pomocná funkcia ktorá nahradí uzol najpravejším potomkom.
 *
 * Kľúč a hodnota uzlu target budú nahradené kľúčom a hodnotou najpravejšieho
 * uzlu podstromu tree. Najpravejší potomok bude odstránený. Funkcia korektne
 * uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkcia predpokladá že hodnota tree nie je NULL.
 *
 * Táto pomocná funkcia bude využitá pri implementácii funkcie bst_delete.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
    bst_node_t *max;
    bst_node_t *beforeMax;
    beforeMax = NULL;
    max = *tree;

    // najdi najvacsiu polozku pod tree
    while (max != NULL) {
        if (max->right == NULL) {
            break;
        }
        beforeMax = max;
        max = max->right;
    }

    target->value = max->value;
    target->key = max->key;

    if (!beforeMax) {  // je root s jednym potomkom
        if (max->left) {
            // ma lavy podstrom -> napoj ho na parenta
            target->left = max->left;
        } else {
            target->left = NULL;
        }

    } else {
        // zanorene
        if (max->left) {
            beforeMax->right = max->left;
        } else {
            beforeMax->right = NULL;
        }
    }
    free(max);

    // kontrola, moze mat lavy podstrom -> vtedy pripoj lavy podstrom k beforeMax->right


}

/*
 * Odstránenie uzlu v strome.
 *
 * Pokiaľ uzol so zadaným kľúčom neexistuje, funkcia nič nerobí.
 * Pokiaľ má odstránený uzol jeden podstrom, zdedí ho otec odstráneného uzla.
 * Pokiaľ má odstránený uzol oba podstromy, je nahradený najpravejším uzlom
 * ľavého podstromu. Najpravejší uzol nemusí byť listom!
 * Funkcia korektne uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkciu implementujte iteratívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key) {
    bst_node_t *ptr = *tree;
    bst_node_t *before_ptr = NULL;

    while (ptr != NULL) {
        if (key < ptr->key) {
            before_ptr = ptr;
            ptr = ptr->left;
            continue;
        } else if (key > ptr->key) {
            before_ptr = ptr;
            ptr = ptr->right;
            continue;
        }

        // nasiel hodnotu
        // nema podstromy -> vymaz
        if (ptr->left == NULL && ptr->right == NULL) {
            if (before_ptr == NULL) {
                // je to koren a je prazdny
                free(*tree);
                *tree = NULL;
            } else if (before_ptr->left == ptr) {
                // bol to lavy podstrom
                free(before_ptr->left);
                before_ptr->left = NULL;
            } else if (before_ptr->right == ptr) {
                // bol to pravy podstrom
                free(before_ptr->right);
                before_ptr->right = NULL;
            }
            return;
        }

        // ma oba podstromy
        if (ptr->left != NULL && ptr->right != NULL) {
            // najdi najpravejsieho v lavom podstrome (najblizsia hodnota) a premaz
            bst_replace_by_rightmost(ptr, &ptr->left);
            return;
        }

        // ma jeden podstrom
        if (ptr->right == NULL) { // ma iba lavy podstrom
            if (!before_ptr) {
                // je root
                *tree = (*tree)->left;
            } else if (before_ptr->left == ptr) { // ak bol naviazany zlava -> presmeruj pointer
                before_ptr->left = ptr->left;
            } else { // bol naviazany zprava -> presmeruj pointer
                before_ptr->right = ptr->left;
            }
            free(ptr);
            ptr = NULL;
            return;
        }

        // ma iba pravy podstrom
        if (!before_ptr) {
            // je root
            *tree = (*tree)->right;
        } else if (before_ptr->left == ptr) { // bol naviazany zlava
            before_ptr->left = ptr->right;
        } else {
            before_ptr->right = ptr->right;
        }
        free(ptr);
        ptr = NULL;
        return;
    }
}

/*
 * Zrušenie celého stromu.
 *
 * Po zrušení sa celý strom bude nachádzať v rovnakom stave ako po
 * inicializácii. Funkcia korektne uvoľní všetky alokované zdroje rušených
 * uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree) {
    stack_bst_t s1;
    stack_bst_init(&s1);

    stack_bst_push(&s1, *tree);
    while (!stack_bst_empty(&s1)) {
        bst_node_t *el = stack_bst_top(&s1);
        stack_bst_pop(&s1);
        if (el == NULL) continue;

        stack_bst_push(&s1, el->left);
        stack_bst_push(&s1, el->right);
        free(el);
        el = NULL;
    }

    *tree = NULL;
}

/*
 * Pomocná funkcia pre iteratívny preorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu.
 * Nad spracovanými uzlami zavola bst_print_node a uloží ich do zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit) {
    bst_node_t *ptr = tree;

    while (ptr != NULL) {
        stack_bst_push(to_visit, ptr);
        bst_print_node(ptr);
        ptr = ptr->left;
    }
}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_preorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree) {
    stack_bst_t s1;
    stack_bst_init(&s1);

    bst_leftmost_preorder(tree, &s1);
    while (!stack_bst_empty(&s1)) {
        bst_node_t *el = stack_bst_top(&s1);
        stack_bst_pop(&s1);
        bst_leftmost_preorder(el->right, &s1);
    }
}

/*
 * Pomocná funkcia pre iteratívny inorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {
    bst_node_t *ptr = tree;

    while (ptr != NULL) {
        stack_bst_push(to_visit, ptr);
        ptr = ptr->left;
    }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_inorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree) {
    stack_bst_t s1;
    stack_bst_init(&s1);
    bst_leftmost_inorder(tree, &s1);

    // BAC
    while (!stack_bst_empty(&s1)) {
        bst_node_t *el = stack_bst_top(&s1);
        stack_bst_pop(&s1);
        bst_print_node(el);
        bst_leftmost_inorder(el->right, &s1);
    }
}

/*
 * Pomocná funkcia pre iteratívny postorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov. Do zásobníku bool hodnôt ukladá informáciu že uzol
 * bol navštívený prvý krát.
 *
 * Funkciu implementujte iteratívne pomocou zásobníkov uzlov a bool hodnôt a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit) {
    bst_node_t *ptr = tree;

    while (ptr != NULL) {
        stack_bst_push(to_visit, ptr);
        stack_bool_push(first_visit, true);
        ptr = ptr->left;
    }
}

/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_postorder a
 * zásobníkov uzlov a bool hodnôt bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree) {
    bool from_left;
    stack_bst_t s1;
    stack_bool_t s1bool;
    stack_bst_init(&s1);
    stack_bool_init(&s1bool);

    bst_leftmost_postorder(tree, &s1, &s1bool);

    while (!stack_bst_empty(&s1)) {
        bst_node_t *ptr = stack_bst_top(&s1);
        from_left = stack_bool_top(&s1bool);
        stack_bool_pop(&s1bool);

        if (from_left) {
            stack_bool_push(&s1bool, false);
            bst_leftmost_postorder(ptr->right, &s1, &s1bool);
        } else {
            stack_bst_pop(&s1);
            bst_print_node(ptr);
        }
    }
}
