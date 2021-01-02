#ifndef __JTOK_SHARED_H__
#define __JTOK_SHARED_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include <stdlib.h>

#include "jtok.h"

#define JTOK_ASCII_CHAR_LOWEST_VALUE 32   /* ' ' space */
#define JTOK_ASCII_CHAR_HIGHEST_VALUE 127 /* DEL */


#define HEXCHAR_ESCAPE_SEQ_COUNT 4 /* can escape 4 hex chars such as \uffea */

/**
 * @brief Allocate fresh token from the token pool
 *
 * @param parser
 * @param tokens
 * @param num_tokens
 * @return jtok_tkn_t*
 */
jtok_tkn_t *jtok_alloc_token(jtok_parser_t *parser);

/**
 * @brief Fill jtok_token type and boundaries
 *
 * @param token the jtok token to populate
 * @param type the token type
 * @param start stard index
 * @param end end index
 *
 * @return 0 on success, 1 on failure
 */
int jtok_fill_token(jtok_tkn_t *token, JTOK_TYPE_t type, int start, int end);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __JTOK_SHARED_H__ */
