/******************************************************************************
 * bar_scanner.h
 *
 *
 * DESCRIPTION: - 用户层 扫描头 操作函数声明
 *
 * Modification history
 * ----------------------------------------------------------------------------
 * Date         Version  Author       History
 * ----------------------------------------------------------------------------
 * alex.chen	2013-05-29 ,  Written
 ******************************************************************************/

#ifndef __SCANNER_USER_H__
#define __SCANNER_USER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int bar_open(const char *pathname, int oflags);
int bar_close(int fd);
ssize_t bar_scan(int fd, uint32_t onoff);
ssize_t bar_read(int fd, void *buf, size_t nbytes);
ssize_t bar_read_timeout(int fd, void *buf, size_t nbytes, uint32_t timeoutms);
ssize_t bar_get_ver(int fd, int8_t type[32], int8_t ver[128]);

/******************************************************************************
 * Function:	bar_get_bright_status
 *
 * DESCRIPTION:	获取扫描头照明灯的状态
 *
 * Input:		fd     - 打开扫描头的句柄
 *
 * Output:		status   - 用于保存读取出来的照明灯状态,0表示关闭，非零0表示打开
 *
 * Returns:		0	:	成功
 *             	其它:	错误的返回码
 *
 ******************************************************************************/
int bar_get_bright_status(int fd, uint32_t *status);

/******************************************************************************
 * Function:	bar_set_bright_status
 *
 * DESCRIPTION:	设置扫描头照明灯的状态
 *
 * Input:		fd     - 打开扫描头的句柄
 *              status   -用于设置的照明灯状态，0表示关闭，非零表示打开
 *
 * Output:		
 *
 * Returns:		0	:	成功
 *             	其它:	错误的返回码
 *
 ******************************************************************************/
int bar_set_bright_status(int fd, uint32_t status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


