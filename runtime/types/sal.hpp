/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file defines a small 'Source Annotation Language' ('SAL')
    to mark function/method parameters as input or output, 
    since methods and functions return 'void' or 'status_t'.

NOTES:
*/


#pragma once

/**
 * @brief Declares a parameter as input variable that 
 *        is supposed to be modified after use.
 * 
 * @note This is only documentation for the developer and 
 *       doesn't chance anything in the logic or mutability.
 */
#define _INOUT_


/**
 * @brief Declares a parameter as output variable.
 * 
 * @note This is only documentation for the developer and 
 *       doesn't chance anything in the logic or mutability.
 */
#define _OUT_


/**
 * @brief Declares a parameter as input variable.
 * 
 * @note This is only documentation for the developer and 
 *       doesn't chance anything in the logic or mutability.
 */
#define _IN_


/**
 * @brief Declares a function or method as public.
 * 
 * @note This is only documentation for the developer and 
 *       doesn't chance anything in the logic or mutability.
 */
#define _API_


/**
 * @brief Declares a function as public and C-API.
 * 
 * @note Always use this macro instead of `extern "C"`.
 */
#define _C_API_ extern "C"
