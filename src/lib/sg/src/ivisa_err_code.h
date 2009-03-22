/* vim:set et sw=4 sts=4: */
/*---------------------------------------------------------------------------------

FileName:

iverrcode.h

Abstract:

If the Error Code is negative, it is a error, or else is not a error.

---------------------------------------------------------------------------------*/

#ifndef __IVERRCODE_H__
#define __IVERRCODE_H__

#define IVEC_SUCCESS            (kint)0     /* operation sucess */
#define IVEC_INGNORE            (kint)1     /* operation already done, second skipped */
#define    IVEC_INPROGRESS            (kint)2        /* Operation in progress */


#define IVEC_FAILURE            (kint)-1    /* normal error */
#define IVEC_BUFSIZE            (kint)-2    /* caller provide memory not enough error */
#define IVEC_MEMORY             (kint)-3    /* can not allocate memory */
#define IVEC_NOTIMPL            (kint)-4    /* the feature not implement */
#define IVEC_TIMEOUT            (kint)-5    /* time out occur duiring operation */
#define IVEC_BADPARAM           (kint)-6    /* bad parameters input */
#define IVEC_INITIALIZED        (kint)-7    /* the module is alread initialized */
#define IVEC_NOTINIT            (kint)-8    /* the module is not initialized */
#define IVEC_FILEOPENED         (kint)-9    /* the file was already opened */
#define IVEC_FILENOTOPEN        (kint)-10   /* the file is not in open stat */
#define IVEC_CANCELLED          (kint)-11   /* User Cancelled */

#endif /* __IVERRCODE_H__ */
