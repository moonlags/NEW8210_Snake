/*
 * scan.h
 *
 *  Created on: 2018年6月15日
 *      Author: horizon
 */

#ifndef INCLUDE_SCAN_H_
#define INCLUDE_SCAN_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern int bar_scan_init(void);
extern int bar_scan_start(void);
extern int bar_scan_end(void);
extern int bar_scan_read(void *data, size_t nbytes);
extern int bar_scan_exit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* INCLUDE_SCAN_H_ */
