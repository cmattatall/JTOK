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
 *

 */
JTOK_PARSE_STATUS_t jtok_parse_object(jtok_parser_t *parser, jtok_tkn_t *tokens,
                                      size_t num_tokens);


/**
 * @brief Compare token objects for equality
 *
 * @param pool1 pool of allocated tokens (must contain obj1)
 * @param tkn1 first object
 * @param pool2 pool of allocated tokens (must contain obj2)
 * @param tkn2 second object
 * @return true if equal
 * @return false if ont equal
 *
 * @note Objects are equal if all of their children are equal.
 *       This does not mean the children are ordered the same.
 */
bool jtok_toktokcmp_object(const jtok_tkn_t *pool1, const jtok_tkn_t *obj1,
                           const jtok_tkn_t *pool2, const jtok_tkn_t *obj2);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __JTOK_OBJECT_H__ */
