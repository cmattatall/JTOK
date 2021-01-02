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
 * @return JTOK_PARSE_STATUS_t parser status
 */
JTOK_PARSE_STATUS_t jtok_parse_object(jtok_parser_t *parser);


/**
 * @brief Compare token objects for equality
 *
 * @param tkn1 first object
 * @param tkn2 second object
 * @return true if equal
 * @return false if ont equal
 *
 * @note Objects are equal if all of their children are equal.
 *       This does not mean the children are ordered the same.
 */
bool jtok_toktokcmp_object(const jtok_tkn_t *obj1, const jtok_tkn_t *obj2);


#ifdef __cplusplus
/* clang-format off */
}
/* clang-format on */
#endif /* End C linkage */
#endif /* __JTOK_OBJECT_H__ */
