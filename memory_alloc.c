#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <setjmp.h>
#include "cmocka.h"
#include "memory_alloc.h"

/* Initialize the memory allocator */
void memory_init() {
  m.available_blocks = DEFAULT_SIZE;
  m.first_block = 0;
  for(int i = 1; i<DEFAULT_SIZE;i++){
    m.blocks[i]=i;
  }
}


/* Return the number of consecutive blocks starting from first */

int nb_consecutive_blocks(int first) {


  if(m.first_block == NULL_BLOCK){
    return 0;
  }

  else{
    int i = first+1;
    while(m.blocks[i]-i==1 && m.blocks[i]!=NULL_BLOCK){
      i++;
    }
    return i-first;
  }
} 


/* Reorder memory blocks */
void memory_reorder() {
  /* TODO (exercise 2) */
}

/* Allocate size bytes
 * return -1 in case of an error
 */
int memory_allocate(size_t size) {
  /*Returns the index of the first available block*/

  /* Premièrement, vérifions s'il y a suffisamment de blocs disponibles.*/
  int available=m.available_blocks;
  int blocks_needed = ceil(size/8);
  int i = m.first_block;

  /*Let's separate the cases. 
    First, let us suppose that the first avalaible bloc is at index 0.
  */

  /* CAS n°1 : memory_alloc allocates all the available blocks starting from the first_block*/
  if(nb_consecutive_blocks(m.first_block)==blocks_needed && available==blocks_needed){
    initialize_buffer(i,size);
    m.first_block=NULL_BLOCK;
    return i;
  }
  /* CAS n°2 : memory_alloc alloue l'espace nécessaire à partir du premier bloc disponible.
    Le first_block change de valeur.*/
  else if(nb_consecutive_blocks(i)>=blocks_needed)
  {
    initialize_buffer(i,size);
  }
  /* CAS n°3 : memory_alloc alloue l'espace nécessaire à la fin */
  else if(){

  }

  /* CAS n°4 : memory_alloc alloue l'espace au milieu du tableau.*/
  
  while(i != NULL_BLOCK && i<DEFAULT_SIZE){
    if(nb_consecutive_blocks(i)>=blocks_needed){
      initialize_buffer(i,size);
      /*for(int j=i; j<i+blocks_needed;j++){
        m.blocks[j]=0;
        }*/
      m.first_block = m.blocks[i + blocks_needed -1];
      return i;
      break;
    }

    else {
      i++;
    }
  }
  m.error_no = E_NOMEM;
  return -1;
  }


/* Free the block of data starting at address */
void memory_free(int address, size_t size) {
  /* TODO */
}

/* Print information on the available blocks of the memory allocator */
void memory_print() {
  printf("---------------------------------\n");
  printf("\tBlock size: %lu\n", sizeof(m.blocks[0]));
  printf("\tAvailable blocks: %lu\n", m.available_blocks);
  printf("\tFirst free: %d\n", m.first_block);
  printf("\tStatus: "); memory_error_print(m.error_no);
  printf("\tContent:  ");
  for (int i = 0; i<DEFAULT_SIZE;i++){
    if(m.blocks[i]!=NULL_BLOCK){
      printf("[ %d ] -> ",i);
    }
  }
  printf("NULL_BLOCk");
  /* TODO: browse the available blocks and print their index */

  printf("\n");
  printf("---------------------------------\n");


      printf("nb_consecutive_blocks(%d) returned %d",5,nb_consecutive_blocks(5));
      printf("\n");

  }


/* print the message corresponding to error_number */
void memory_error_print(enum memory_errno error_number) {
  switch(error_number) {
  case E_SUCCESS:
    printf("Success\n");
    break;
  case E_NOMEM:
    printf("Not enough memory\n");
    break;
  case  E_SHOULD_PACK:
    printf("Not enough contiguous blocks\n");
    break;
  default:
    break;
  }
}

/* Initialize an allocated buffer with zeros */
void initialize_buffer(int start_index, size_t size) {
  char* ptr = (char*)&m.blocks[start_index];
  for(int i=0; i<size; i++) {
    ptr[i]=0;
  }
}




/************* Test functions ********************/

/* write something in the buffer */
void use_buffer(int start_index, size_t size) {
  char* ptr = (char*)&m.blocks[start_index];
  for(int i=0; i<size; i++) {
    ptr[i] = rand() % 32;
  }
}

/* write something in the buffer */
void check_empty_buffer(int start_index, size_t size) {
  char* ptr = (char*)&m.blocks[start_index];
  for(int i=0; i<size; i++) {
    assert_int_equal(ptr[i], 0); // each byte of an allocated buffer should be 0
  }
}

/* allocate size bytes (or nb_pages pages), and free the allocated buffer
 * available_blocks is the number of available blocks at the beginning
 */
void test_alloc_free(size_t size, int nb_pages, int available_blocks) {
  /* allocate nb_pages pages */
  int res = memory_allocate(size);
  assert_int_equal(m.error_no, E_SUCCESS); // memory_allocate should be successfull
  assert_int_not_equal(res, -1); 		   // memory_allocate should return something != -1
  assert_int_equal(m.available_blocks, available_blocks-nb_pages); // all the blocks but nb_pages should be available
  assert(res >= 0 && res < DEFAULT_SIZE);

  use_buffer(res, size);

  /* free the allocated buffer */
  memory_free(res, size);
  assert_int_equal(m.available_blocks, available_blocks); // all the blocks should be available
}

/* allocate size bytes */
int test_alloc(size_t size) {
  int res = memory_allocate(size);
  assert_int_equal(m.error_no, E_SUCCESS); // memory_allocate should be successfull
  assert_int_not_equal(res, -1); 		   // memory_allocate should return something != -1
  assert_true(res >= 0 && res < DEFAULT_SIZE);
  check_empty_buffer(res, size);
  use_buffer(res, size);
  return res;
}

/* free the buffer located at address addr */
void test_memory_free(int addr, size_t size) {
  memory_free(addr, size);
}

void test_exo1_memory_init() {
 /* initialize memory */
  memory_init();
  assert_int_equal(m.available_blocks, DEFAULT_SIZE);   // all the blocks should be available
}

void test_exo1_memory_print() {
  test_exo1_memory_init();
  memory_print();
}

/* allocate size bytes (eg. nb_pages blocks) */
void test_alloc_blocks(size_t size, int nb_pages) {
  test_exo1_memory_init();

  int allocated_blocks;
  /* allocate a few blocks */
  allocated_blocks = test_alloc(size); /* allocate 1 byte */
  assert_int_equal(m.available_blocks, DEFAULT_SIZE-nb_pages);
  assert_in_range(allocated_blocks, 0, DEFAULT_SIZE);
} 

/* allocate 1 byte */
void test_exo1_alloc_one_byte() {
  test_alloc_blocks(1, 1);
}

/* allocate 1 page */
void test_exo1_alloc_one_page() {
  test_alloc_blocks(sizeof(memory_page_t), 1);
}

/* allocate 2 pages */
void test_exo1_alloc_two_pages() {
  test_alloc_blocks(2*sizeof(memory_page_t), 2);
}

/* allocate a few blocks and free them */
void test_exo1_free_blocks() {
  test_exo1_memory_init();

  int allocated_blocks[DEFAULT_SIZE];
  /* allocate a few blocks */
  allocated_blocks[0] = test_alloc(1); /* allocate 1 byte */
  assert_int_equal(m.available_blocks, DEFAULT_SIZE-1);

  allocated_blocks[1] = test_alloc(sizeof(memory_page_t)); /* allocate 1 memory block */
  assert_int_equal(m.available_blocks, DEFAULT_SIZE-2);

  allocated_blocks[2] = test_alloc(2*sizeof(memory_page_t)); /* allocate 2 memory blocks */
  assert_int_equal(m.available_blocks, DEFAULT_SIZE-4);

  test_memory_free(allocated_blocks[0], 1); /* free 1 byte */

  test_memory_free(allocated_blocks[1], sizeof(memory_page_t)); /* free 1 block */

  test_memory_free(allocated_blocks[2], 2*sizeof(memory_page_t)); /* free 2 blocks */
  assert_int_equal(m.available_blocks, DEFAULT_SIZE);

  /* allocate a few blocks and free them */
  test_alloc_free(1, 1, DEFAULT_SIZE);                       /* allocate 1 byte */
  test_alloc_free(sizeof(memory_page_t), 1, DEFAULT_SIZE);   /* allocate 1 memory page */
  test_alloc_free(2*sizeof(memory_page_t), 2, DEFAULT_SIZE); /* allocate 2 memory page */
}

/* allocate multiple blocks and free them */
void test_exo1_multiple_alloc() {
  test_exo1_memory_init();

  /* test multiple allocations */
  int allocated_blocks[DEFAULT_SIZE];
  for(int i=0; i<DEFAULT_SIZE; i++) {
    allocated_blocks[i] = test_alloc(1); /* allocate 1 byte */
    assert_int_equal(m.available_blocks, DEFAULT_SIZE-(i+1));
  }
  // the available blocks should be something like:
  // NULL_BLOCK
  // memory_print();

  /* free all the allocated blocks */
  for(int i=0; i<DEFAULT_SIZE; i++) {
    test_memory_free(allocated_blocks[i], 1);
    assert_int_equal(m.available_blocks, i+1);
  }
}

/* test allocation when the system is running out of memory */
void test_exo1_out_of_memory() {
  test_exo1_memory_init();

  int allocated_blocks[DEFAULT_SIZE];
  /* First, use all the memory */
  for(int i=0; i<DEFAULT_SIZE; i++) {
    allocated_blocks[i] = test_alloc(1); /* allocate 1 byte */
    // these allocations should succeed
    assert_int_equal(m.available_blocks, DEFAULT_SIZE-(i+1));
  }
  assert_int_equal(m.available_blocks, 0); // no more memory

  /* Now, try to allocate one more byte */
  allocated_blocks[1] = memory_allocate(1);
  assert_int_equal(allocated_blocks[1], -1); // memory_allocate should return an error
  assert_int_equal(m.error_no, E_NOMEM); // memory_allocate should set error_no to E_NOMEM
  assert_int_equal(m.available_blocks, 0); // no more memory
}


/* Run a few tests for exercise 2.
 *
 * If you implemented correctly the functions, all these tests should be successfull
 * Of course this test suite may not cover all the tricky cases, and you are free to add
 * your own tests.
 */
void test_exo2_reorder() {
  test_exo1_memory_init();

  /* test multiple allocations */
  int allocated_blocks[DEFAULT_SIZE];
  for(int i=0; i<DEFAULT_SIZE; i++) {
    allocated_blocks[i] = test_alloc(1); /* allocate 1 byte */
    assert_int_equal(m.available_blocks, DEFAULT_SIZE-(i+1));
  }
  assert_int_equal(m.available_blocks, 0);

  /* free half of the allocated blocks */
  int freed_pages=0;
  for(int i=0; i<DEFAULT_SIZE; i+=2) {
    test_memory_free(allocated_blocks[i], 1);
    freed_pages++;
    assert_int_equal(m.available_blocks, freed_pages);
  }
  // the available blocks should be:
  // [0] -> [2] -> [4] -> [6] -> [8] -> [10] -> [12] -> [14] -> NULL_BLOCK
  //  memory_print();

  int res = memory_allocate(sizeof(memory_page_t)*2);
  // allocation should fail as there's no 2 consecutive blocks
  assert_int_equal(res, -1);
  assert_int_equal(m.error_no, E_SHOULD_PACK);

  // free the remaining allocated blocks
  for(int i=1; i<DEFAULT_SIZE; i+=2) {
    test_memory_free(allocated_blocks[i], 1);
    freed_pages++;
    assert_int_equal(m.available_blocks, freed_pages);
  }
  // the available blocks should be something like:
  // [15] -> [13] -> [11] -> [9] -> [7] -> [5] -> [3] -> [1] -> [0] -> [2] -> [4] -> [6] -> [8] -> [10] -> [12] -> [14] -> NULL_BLOCK
  memory_print();

  // Now, there are 16 available blocks (but probably randomly distributed)
  // This call may trigger the memory reordering function, and successfully allocate 2 blocks
  res = memory_allocate(sizeof(memory_page_t)*2);
  // allocation should fail as there's no 2 consecutive blocks
  assert_int_equal(m.error_no, E_SUCCESS);

  // the available blocks should be something like:
  // [2] -> [3] -> [4] -> [5] -> [6] -> [7] -> [8] -> [9] -> [10] -> [11] -> [12] -> [13] -> [14] -> [15] -> NULL_BLOCK
  //memory_print();
}

int main(int argc, char**argv) {
  const struct CMUnitTest tests[] = {
    /* a few tests for exercise 1.
     *
     * If you implemented correctly the functions, all these tests should be successfull
     * Of course this test suite may not cover all the tricky cases, and you are free to add
     * your own tests.
     */
    cmocka_unit_test(test_exo1_memory_init),
    cmocka_unit_test(test_exo1_memory_print),
    cmocka_unit_test(test_exo1_alloc_one_byte),
    cmocka_unit_test(test_exo1_alloc_one_page),
    cmocka_unit_test(test_exo1_alloc_two_pages),
    cmocka_unit_test(test_exo1_free_blocks),
    cmocka_unit_test(test_exo1_multiple_alloc),
    cmocka_unit_test(test_exo1_out_of_memory),

    /* Run a few tests for exercise 2.
     *
     * If you implemented correctly the functions, all these tests should be successfull
     * Of course this test suite may not cover all the tricky cases, and you are free to add
     * your own tests.
     */

    cmocka_unit_test(test_exo2_reorder)
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
