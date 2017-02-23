/*
 * tc_fixes.h
 *
 * Created: 11/5/2016 8:05:03 PM
 *  Author: Andrew Kaster
 */ 


#ifndef TC_FIXES_H_
#define TC_FIXES_H_

#include <compiler.h>

#ifndef LINUX_DEV

#define TC_WGMODE_NORMAL_gc     TC_TC0_WGMODE_NORMAL_gc
#define TC_WGMODE_FRQ_gc        TC_TC0_WGMODE_NORMAL_gc
#define TC_WGMODE_SS_gc         TC_TC0_WGMODE_SS_gc
#define TC_WGMODE_DS_T_gc       TC_TC0_WGMODE_DS_T_gc
#define TC_WGMODE_DS_TB_gc      TC_TC0_WGMODE_DS_TB_gc
#define TC_WGMODE_DS_B_gc       TC_TC0_WGMODE_DS_B_gc
#define TC_CLKSEL_t             TC_TC0_CLKSEL_t
#define TC_CLKSEL_DIV1024_gc    TC_TC0_CLKSEL_DIV1024_gc
#define TC_CLKSEL_DIV256_gc     TC_TC0_CLKSEL_DIV256_gc
#define TC_CLKSEL_DIV64_gc      TC_TC0_CLKSEL_DIV64_gc
#define TC_CLKSEL_DIV8_gc       TC_TC0_CLKSEL_DIV8_gc
#define TC_CLKSEL_DIV4_gc       TC_TC0_CLKSEL_DIV4_gc
#define TC_CLKSEL_DIV2_gc       TC_TC0_CLKSEL_DIV2_gc
#define TC_CLKSEL_DIV1_gc       TC_TC0_CLKSEL_DIV1_gc
#define TC_CLKSEL_OFF_gc        TC_TC0_CLKSEL_OFF_gc
#define TC_CMD_RESTART_gc       TC_TC0_CMD_RESTART_gc
#define TC_CMD_RESET_gc         TC_TC0_CMD_RESET_gc
#define TC_CMD_UPDATE_gc        TC_TC0_CMD_UPDATE_gc
#define TC_EVSEL_t              TC_TC0_EVSEL_t
#define TC_EVACT_t              TC_TC0_EVACT_t

#endif /* LINUX_DEV */

#endif /* TC_FIXES_H_ */
