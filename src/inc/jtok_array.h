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
 * @param depth the parse nesting depth
 * @return JTOK_PARSE_STATUS_t parser status
 */
JTOK_PARSE_STATUS_t jtok_parse_array(jtok_parser_t *parser, int depth);

/**
 * @brief Compare two jtok tokens with type JTOK_ARRAY for equality
 *
 * @param tkn1 first token
 * @param tkn2 second token
 * @return true if equal
 * @return false if not equal
 */
bool jtok_toktokcmp_array(const jtok_tkn_t *tkn1, const jtok_tkn_t *tkn2);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __JTOK_ARRAY_H__ */
