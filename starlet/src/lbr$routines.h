#ifndef lbr$routines_h
#define lbr$routines_h 

unsigned int  lbr$close (
			 unsigned int *library_index);

unsigned int  lbr$delete_data (
			       unsigned int *library_index,
			       unsigned int *txtrfa);

unsigned int  lbr$delete_key (
			      unsigned int *library_index,
			      unsigned int *key_name);

unsigned int  lbr$find (
			unsigned int *library_index,
			unsigned int *txtrfa);

unsigned int  lbr$flush (
			 unsigned int *library_index,
			 unsigned int block_type);

unsigned int  lbr$get_header (
			      unsigned int *library_index,
			      unsigned int *retary);

unsigned int  lbr$get_help (
			    unsigned int *library_index,
         ... 
			    );

unsigned int  lbr$get_history (
			       int *library_index,
			       int (*action_routine)(void));

unsigned int  lbr$get_index (
			     unsigned int *library_index,
			     unsigned int *index_number,
			     int (*routine_name)(void),
			     ... );
unsigned int  lbr$get_record (
			      unsigned int *library_index,
         ... 
			      );

unsigned int  lbr$ini_control (
			       unsigned int *library_index,
			       unsigned int *func,
         ... 
			       );

unsigned int  lbr$insert_key (
			      unsigned int *library_index,
			      unsigned int *key_name,
			      unsigned int *txtrfa);

unsigned int  lbr$lookup_key (
			      unsigned int *library_index,
			      unsigned int *key_name,
			      unsigned int *txtrfa);
unsigned int  lbr$open (
			unsigned int *library_index,
         ... 
			);

unsigned int  lbr$output_help (
			       int (*output_routine)(void),
         ... 
			       );

unsigned int  lbr$put_end (
			   unsigned int *library_index);

unsigned int  lbr$put_history (
			       unsigned int *library_index,
			       void *record_desc);

unsigned int  lbr$put_record (
			      unsigned int *library_index,
			      void *bufdes,
			      unsigned int *txtrfa);

unsigned int  lbr$replace_key (
			       unsigned int *library_index,
			       unsigned int *key_name,
			       unsigned int *oldrfa,
			       unsigned int *newrfa);

unsigned int  lbr$ret_rmsstv (void);

unsigned int  lbr$search (
			  unsigned int *library_index,
			  unsigned int *index_number,
			  unsigned int *rfa_to_find,
			  int (*routine_name)(void));


unsigned int  lbr$set_index (
			     unsigned int *library_index,
			     unsigned int *index_number);

unsigned int  lbr$set_locate (
			      unsigned int *library_index);


unsigned int  lbr$set_module (
			      unsigned int *library_index,
			      unsigned int *rfa,
         ... 
			      );

unsigned int  lbr$set_move (
			    unsigned int *library_index);



#endif
