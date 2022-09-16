    /*
     *  Kernel interface includes ... this file defines values, structures,
     *  and routines that are needed to interface to the kernel ...
     */
    
    #ifndef  kernel_defs
    #define  kernel_defs 1
    
    
    #define     VOID_FP void _far *
    
    /*
     *   ... a pipe ....
     */
    
    
    typedef struct {
    
	unsigned int            pwords;
	unsigned int            pcount;
	unsigned int            head;
	unsigned int            tail;
	unsigned char           buff[0];
    } PIPE;
    #define     P_PIPE          PIPE far *
 
    
    /**********************************************************
     *  kernel shared data 
     **********************************************************/
    
    struct share_data {
    
	unsigned int far                        *fdic;
	S32                                    fdic_entries;
    };
    
    extern struct share_data far *kernel_share;
    #define     KS              (*kernel_share)
    
    /**********************************************************
     *  kernel entry points ...
     **********************************************************/
    
    /*
     * character pipe ...
     */
    
    P_PIPE  create_pipe(int);
    write_pipe(P_PIPE, unsigned int far *,int);
    read_pipe(P_PIPE, unsigned int far *,int);
    int test_pipe(P_PIPE);
    
    #endif 