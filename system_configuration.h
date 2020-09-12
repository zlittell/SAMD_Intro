/* 
 * File:   system_configuration.h
 * Author: zlitt
 *
 * Created on September 11, 2020, 2:22 PM
 */

#ifndef SYSTEM_CONFIGURATION_H
#define	SYSTEM_CONFIGURATION_H

#ifdef	__cplusplus
extern "C" {
#endif

void configureClocks(void);
void init_TC2(void);
void init_IO(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_CONFIGURATION_H */

