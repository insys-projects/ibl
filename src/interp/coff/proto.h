/*****************************************************************************/
/* PROTO.H - Prototypes for loader required functions.                       */
/*****************************************************************************/

/*--------------------------------------------------------------------------*/
/*  LOADER REQUIRED FUNCTIONS			                            */
/*--------------------------------------------------------------------------*/
/* extern int   mem_write        (unsigned char *, unsigned int, T_ADDR, 
			       unsigned char);  - moved to osal.h */
extern int   set_reloc_amount (void);
extern void  lookup_sym       (int indx, SYMENT *sym, AUXENT *aux);
extern int   load_syms        (int need_reloc);
extern void *myalloc          (unsigned int size);
extern void *mralloc          (void *p, unsigned int size);
extern void  load_msg         (const char *format);
