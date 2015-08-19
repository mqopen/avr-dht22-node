/*! \file avrlibdefs.h \brief AVRlib global defines and macros. */
//*****************************************************************************
//
// File Name    : 'avrlibdefs.h'
// Title        : AVRlib global defines and macros include file
// Author       : Pascal Stang
// Created      : 7/12/2001
// Revised      : 9/30/2002
// Version      : 1.1
// Target MCU   : Atmel AVR series
// Editor Tabs  : 4
//
//  Description : This include file is designed to contain items useful to all
//                  code files and projects, regardless of specific implementation.
//
// This code is distributed under the GNU Public License
//      which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************


#ifndef __AVRLIBDEFS_H__
#define __AVRLIBDEFS_H__

// MIN/MAX/ABS macros
#define MIN(a,b)            ((a<b)?(a):(b))
#define MAX(a,b)            ((a>b)?(a):(b))
#define ABS(x)              ((x>0)?(x):(-x))

// constants
#define PI      3.14159265359

#endif
