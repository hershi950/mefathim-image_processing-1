#pragma once

#ifndef __c_linkage_h
#define __c_linkage_h


#ifdef __cplusplus
	#define C_LINKAGE_BEGIN extern "C" {
	#define C_LINKAGE_END }
#else
	#define C_LINKAGE_BEGIN
	#define C_LINKAGE_END 
	#define false 0
	#define true 1
#endif


#ifndef __func__
#define __func__ __FUNCTION__
#endif

#define	MIN(a, b)	((a) < (b) ? (a) : (b))
#define	MAX(a, b)	((a) > (b) ? (a) : (b))

#define NELM(x) (sizeof(x)/sizeof(x[0]))

#endif

