#ifndef __JTOK_PRIMITIVE_H__
#define __JTOK_PRIMITIVE_H__
#ifdef __cplusplus
/* clang-format off */
extern "C"
{
/* clang-format on */
#endif /* Start C linkage */


#include "jtok.h"


/**
 * @brief Parse and fill next available jtok token as a jtok primitive
 *
 * @param parser the json parser
 * @param tokens token array (caller provided)
 * @param num_tokens max number of tokens to parse
 * @return JTOK_PARSE_STATUS_t parse status
 */
JTOK_PARSE_STATUS_t jtok_parse_primitive(jtok_parser_t *parser,
                                         jtoktok_t *tokens, size_t num_tokens);

/**
 * @brief Compare two jtok tokens with type JTOK_PRIMITIVE for equality
 *
 * @param tkn1 first token
 * @param tkn2 second token
 * @return true if equal
 * @return false if not equal
 */
bool jtok_toktokcmp_primitive(const jtoktok_t *tkn1, const jtoktok_t *tkn2);

#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __JTOK_PRIMITIVE_H__ */
