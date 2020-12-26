#ifndef __JTOK_ARRAY_H__
#define __JTOK_ARRAY_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */

#include "jtok.h"

/**
 * @brief Parse and fill the next available jtok token as a jtok array,
 * recursing into sub-objects
 *
 * @param parser the json parser
 * @param tokens token array (caller provided)
 * @param num_tokens max number of tokens to parser
 * @return JTOK_PARSE_STATUS_t parser status
 */
JTOK_PARSE_STATUS_t jtok_parse_array(jtok_parser_t *parser, jtok_tkn_t *tokens,
                                     size_t num_tokens);

/**
 * @brief Compare two jtok tokens with type JTOK_ARRAY for equality
 *
 * @param pool1 pool of allocated tokens that contains tkn1
 * @param tkn1 first token
 * @param pool2 pool of allocated tokens that contains tkn2
 * @param tkn2 second token
 * @return true if equal
 * @return false if not equal
 */
bool jtok_toktokcmp_array(const jtok_tkn_t *pool1, const jtok_tkn_t *tkn1,
                          const jtok_tkn_t *pool2, const jtok_tkn_t *tkn2);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __JTOK_ARRAY_H__ */
