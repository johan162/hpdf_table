/* 
 * File:   hpdf_errstr.h
 * Author: ljp
 *
 * Created on den 25 juli 2015, 15:33
 */

#ifndef HPDF_ERRSTR_H
#define	HPDF_ERRSTR_H

#ifdef	__cplusplus
extern "C" {
#endif

const char *
hpdf_errstr(const HPDF_STATUS err_code);

#ifdef	__cplusplus
}
#endif

#endif	/* HPDF_ERRSTR_H */

