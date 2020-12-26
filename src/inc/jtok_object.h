#ifndef __JTOK_OBJECT_H__
#define __JTOK_OBJECT_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include <stdbool.h>

#include "jtok.h"

/**
 * @brief Parse and fill the next available jtok token as a jtok object,
 * recursing into sub-objects
 *
 * @param parser the json parser
 * @param tokens token array (caller provided)
 * @param num_tokens max number of tokens to parser
 * @return JTOK_PARSE_STATUS_t parser status
 */
JTOK_PARSE_STATUS_t jtok_parse_object(jtok_parser_t *parser, jtok_tkn_t *tokens,
                                      size_t num_tokens);


bool jtok_toktokcmp_object(const jtok_tkn_t *tkn1, const jtok_tkn_t *tkn2);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __JTOK_OBJECT_H__ */
