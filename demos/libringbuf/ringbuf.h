#ifndef _ringbuf_h_
#define _ringbuf_h_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ringbuf_unit_s {
    struct ringbuf_unit_s 	*prev;
    struct ringbuf_unit_s 	*next;
    long 					size;					// Compatible with 64-bit machines
    uint8_t 				data[0];
} ringbuf_unit_t;

typedef struct ringbuf_s {
    uint8_t 				*ptr;
    int 					capacity;
    #define RB_MAX_READ_NUM	32
    ringbuf_unit_t 			*r[RB_MAX_READ_NUM];    // Pointer array, supports multi-path simultaneous reading of ringbuf
    ringbuf_unit_t 			*w; 					// write position
} ringbuf_t;

typedef struct ringbuf_rlink_s {
    ringbuf_t 				*rb;
    int 					index; 					// read index num
} ringbuf_rlink_t;

/* input: mem, capacity
 * ouput: rb, ringbuf struct
 * note: mem need to be allocated by yourself before.
 */
int ringbuf_create(ringbuf_t **rb, const void* mem, const int capacity);
/* input: rb
 * ouput:
 * note: rb->ptr need to be free by yourself after
 */
int ringbuf_destroy(ringbuf_t *rb);
/* input: rb
 * ouput: rbrl, read ringbuf handle
 * func: create a read handle
 */
int ringbuf_read_add(ringbuf_t *rb, ringbuf_rlink_t *rbrl);
/* input: 	rb
 *		  	index: =0:latest, <0:before(previous), >0:after(next) 
 * ouput: 	rbrl, read ringbuf handle
 * return:	0:success, <0:error, -2:out range oldest, -3:out range latest
 * func: seek read link
 */
int ringbuf_read_seek(ringbuf_rlink_t *rbrl, int index);
int ringbuf_read_del(ringbuf_rlink_t *rbrl);
int ringbuf_capacity_get(const ringbuf_t *rb);
/* input: rb, size
 * ouput: p, write mem address
 * func: get write data mem address
 */
int ringbuf_write_get_unit(ringbuf_t *rb, unsigned char **p, int size);
/* input: rb, size
 * ouput: 
 * func: update write ringbuf link table
 */
int ringbuf_write_put_unit(ringbuf_t *rb, int size);
/* input: rbrl
 * ouput: p, read data mem address; size, data size
 * func: get read data mem address
 */
int ringbuf_read_get_unit(ringbuf_rlink_t *rbrl, unsigned char **p, int *size);
/* input: rbrl
 * ouput: 
 * func: update read ringbuf link table
 */
int ringbuf_read_put_unit(ringbuf_rlink_t *rbrl);

/* input: rbrl
 * ouput: 
 * func: get read remain data number
 * return: number of remain unit
 */
int ringbuf_read_get_remain(ringbuf_rlink_t *rbrl);



#ifdef __cplusplus
}
#endif
#endif /* !_ringbuf_h_ */
