/* vim:set et sw=4 sts=4 ff=unix cino=:0: */
/*---------------------------------------------------------------------------------

FileName:

    kerr.h

Abstract:

    defination the Error Code, this number is defined for each task.

    If the ec is negative, it is a error, or else is not a error.

    The Real ErrorCode is (MainErroCode & __LINES__)

Author:

    Yu Wenbin <kamasamikon@yahoo.com.cn>

Environment:

    Kernel_mode

Revision History:

    2003/11/17 11:48 : Create
    2004/02/04 14:11 : Change the error code to contain the line number
                       Add NG, OK, REC, LEC Macros

    2004/02/10 17:10 : Add CEC

---------------------------------------------------------------------------------*/

#ifndef __K_ERR_H__
#define __K_ERR_H__

#ifdef __cplusplus
extern "C" {
#endif

#define EC_SUCCESS             0x0000 << 16
#define EC_PENDING             0x0001 << 16
#define EC_CANCELLED           0x0002 << 16
#define EC_IGNORED             0x0003 << 16

/*-----------------------------------------------------------
 * The rqst should be process by default handler after usr-handler
 */
#define EC_GO_DEFAULT        0x0004 << 16

#define EC_FAILED              0x8000 << 16
#define EC_BAB_REQUEST         0x8001 << 16
#define EC_BAD_REQUEST         0x8002 << 16
#define EC_BAD_WINDOW          0x8003 << 16
#define EC_BAD_WIN_STATUS      0x8004 << 16
#define EC_MEMORY              0x8005 << 16
#define EC_WIN_STALL           0x8006 << 16

#define EC_BAD_ADDR         0x8007 << 16

#define EC_BEYOND_EDGE           0x8006 << 16
#define EC_STR_TRUNCATION      0x8007 << 16

/*-----------------------------------------------------------
 * The ec is error or not:
 */
#define NG(_x_) ((_x_) & 0x80000000)
#define OK(_x_) (!NG(_x_))

/*-----------------------------------------------------------
 * Real Error Code:
 */
#define REC(_x_) ((_x_) & 0xFFFF0000)

/*-----------------------------------------------------------
 * Line from Error Code:
 */
#define LEC(_x_) ((_x_) & 0x0000FFFF)

/*-----------------------------------------------------------
 * Combined Error Code:
 */
#define CEC(_x_) (REC(_x_) & __LINE__)


#ifdef __cplusplus
}
#endif


#endif /* __K_ERR_H__ */

