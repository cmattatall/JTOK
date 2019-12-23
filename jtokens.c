/* 
jsmn.c
JSON Parsing Module. 
Source is JSMN Source with a few modifications from Carl
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h> /* included for character type checking */
#include "jtokens.h"

/**
 * Allocates a fresh unused token from the token pull.
 */
static jtok_t *alloc_token(token_parser *parser,
													 jtok_t *tokens, size_t num_tokens)
{
	jtok_t *tok;
	if (parser->toknext >= num_tokens)
	{
		return NULL;
	}
	tok = &tokens[parser->toknext++];
	tok->start = tok->end = -1;
	tok->size = 0;
	tok->parent = -1;
	return tok;
}

/**
 * Fills token type and boundaries.
 */
static void fill_token(jtok_t *token, jtokType_t type,
											 word_t start, word_t end)
{
	token->type = type;
	token->start = start;
	token->end = end;
	token->size = 0;
}

/**
	 * Fills next available token with JSON primitive.
 */
static parseRetval_t tokenize_primitive(token_parser *parser, const char *json,
																				size_t len, jtok_t *tokens, size_t num_tokens)
{
	jtok_t *token;
	parseRetval_t retval = PARSE_OK;
	word_t start = parser->pos;
	for (; retval == PARSE_OK && parser->pos < len; parser->pos++)
	{
		if (json[parser->pos] == '\0')
		{
			/* reached end of json string before end of token */
			parser->pos = start;
			printf("returning TOKERR_PART from within tokenize_primitive. rest of json is %s> <\n", &json[parser->pos]);
			retval = TOKERR_PART; /* only have a partial json */
			break;
		}
		switch (json[parser->pos])
		{
		/* found end of the token */
		case '\t': /* tab (whitespace) */
		case '\r': /* carriage return */
		case '\n': /* linefeed */
		case ' ':	/* space (whitespace) */
		case ',':	/* start of new key */
		case ']':	/* end of array */
		case '}':	/* end of object */
			if (tokens == NULL)
			{
				parser->pos--;
				retval = PARSE_OK;
			}
			else
			{
				token = alloc_token(parser, tokens, num_tokens);
				if (token == NULL)
				{
					parser->pos = start;
					retval = TOKERR_NOMEM;
				}
				else
				{
					fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
					token->parent = parser->toksuper;
					parser->pos--;
					retval = PARSE_OK;
				}
			}
		case '\'':
		case '\"':
			/* something strange is going on in this case. end quote without a start quote 
				* This would also catch the case wherein my logic 
				* 		interprets a string as a primitive for some reason 
				*/
			retval = TOKERR_INVAL;
			break;
		}
		if (iscntrl(json[parser->pos])) /* control characters are errors */
		{
			parser->pos = start;
			return TOKERR_INVAL;
		}
	}
	return retval;
}

static parseRetval_t tokenize_string(token_parser *parser,
																		 const char *json,
																		 size_t len, jtok_t *tokens,
																		 size_t num_tokens)
{
	parseRetval_t retval; /* track parsing status */
	jtok_t *token;				/* tkn index */

	/* advance index to parser's index and skip starting quote */
	word_t start = parser->pos++;
	for (retval = PARSE_OK; retval == PARSE_OK && parser->pos < len; parser->pos++)
	{
		/* ctype.h :: iscntrl returns 0 for NONCONTROL CHARS */
		if (iscntrl(json[parser->pos]) == 0)
		{
			switch (json[parser->pos])
			{
			case '\0':
				parser->pos = start;
				retval = TOKERR_PART;
				break;
			case '\'': /* dont permit single quoting */
				parser->pos = start;
				retval = TOKERR_INVAL;
				break;
			case '\"': /* found end of the "string" */
				if (tokens != NULL)
				{
					/* get a new token */
					if ((token = alloc_token(parser, tokens, num_tokens)) == NULL)
					{
						/* error if we couldnt allocate */
						parser->pos = start;
						retval = TOKERR_NOMEM;
						break;
					}
					else
					{
						/* populate the token fields */
						/* use start + 1 so the starting index isnt the >"< character */
						fill_token(token, JSMN_STRING, start + 1, parser->pos);
						token->parent = parser->toksuper;
						break;
					}
				}
				break;
			case '\\': /* escape character found */
				if (++(parser->pos) < len)
				{
					switch (json[parser->pos])
					{
					case '\"':
					case '/':
					case '\\':
					case 'b': /* uncertain if backspace escape is safe.. */
					case 'f':
					case 'r':
					case 'n':
					case 't':
						break;
					default: /* invalid escape character */
						parser->pos = start;
						retval = TOKERR_INVAL;
						break;
					}
				}
				break;
			default: /* do nothing for every other printable character */
				break;
			}
		}
		else /* control characters are errors */
		{
			parser->pos = start;
			retval = TOKERR_INVAL;
			break;
		}
	}

	//todo: REMOVE POST DEBUG. i'd use ifdef bug embedded platforms dont have printf :(
	printf("before return from tokenize_string, status = %s\n", parseStatusMsg[retval]);
	return retval;
}

/**
 * Parse JSON string and fill tokens.
 */

tokenizationResult_t jtokenize(token_parser *parser,
															 const char *js,
															 size_t len,
															 jtok_t *tokens,
															 uword_t num_tokens)
{
	tokenizationResult_t res;
	word_t i;
	jtok_t *token;

	for (res.status = PARSE_OK; res.status == PARSE_OK && parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
	{
		char c;
		jtokType_t type;

		c = js[parser->pos];
		switch (c)
		{

		case '{': /* found start of parent object */
		case '[':
			res.count++;
			if (tokens != NULL)
			{
				token = alloc_token(parser, tokens, num_tokens);
				if (token == NULL)
				{
					res.status = TOKERR_NOMEM;
				}
				else
				{
					/* link to parent (and update parent size)*/
					if (parser->toksuper != -1)
					{
						tokens[parser->toksuper].size++;
						token->parent = parser->toksuper;
					}

					/* determine type of parent object */
					token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
					token->start = parser->pos;
					parser->toksuper = parser->toknext - 1;
				}
			}
			break;
		case '}': /* found end of parent object */
		case ']':
			if (tokens != NULL)
			{
				type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
				if (parser->toknext < 1)
				{
					res.status = TOKERR_INVAL;
				}
				else
				{
					token = &tokens[parser->toknext - 1];
					for (;;)
					{
						/* safety check for unpopulated token */
						if (token->start != -1 && token->end == -1)
						{
							if (token->type != type) /* validate token type */
							{
								res.status = TOKERR_INVAL;
							}
							else /* update superior node and set token's end idx */
							{
								token->end = parser->pos + 1;
								parser->toksuper = token->parent;
							}
							break;
						}

						/* jump to parent if it exists */
						if (token->parent != -1)
						{
							token = &tokens[token->parent];
						}
					}
				}
			}
			break;
		case '\"': /* found the starting quote of a string key/val */

			printf("before tokenizing string, token_status == %d\n", res.status);

			if ((res.status = tokenize_string(parser, js, len, tokens, num_tokens)) == PARSE_OK)
			{

				res.count++;
				if (parser->toksuper != -1 && tokens != NULL)
				{
					tokens[parser->toksuper].size++;
				}
			}
			break;
		case '\t':
		case '\r':
		case '\n':
		case ' ':
			break;
		case ':':
			parser->toksuper = parser->toknext - 1;
			break;
		case ',':
			if (tokens != NULL &&
					tokens[parser->toksuper].type != JSMN_ARRAY &&
					tokens[parser->toksuper].type != JSMN_OBJECT)
			{
				parser->toksuper = tokens[parser->toksuper].parent;
			}
			break;
		/* primitives are: numbers and booleans */
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 't':
		case 'f':
		case 'n':
			/* And they must not be keys of the object */
			if (tokens != NULL)
			{
				jtok_t *t = &tokens[parser->toksuper];
				if (t->type == JSMN_OBJECT || (t->type == JSMN_STRING && t->size != 0))
				{
					res.status = TOKERR_INVAL;
				}
			}

			printf("before tokenizing primitive, token_status == %d\n", res.status);
			if ((res.status = tokenize_primitive(parser, js, len, tokens, num_tokens)) == PARSE_OK)
			{
				res.count++;
				if (parser->toksuper != -1 && tokens != NULL)
				{
					tokens[parser->toksuper].size++;
				}
			}
			else
			{
				printf("after tokenizing primitive, token_status == %d\n", res.status);
			}

			break;
		default: /* unexpected character */
			res.status = TOKERR_INVAL;
			break;
		}
	}

	/* go through and make sure every token has a start + end index */
	/* if not, we didnt actually receive a full JSON */
	for (i = parser->toknext - 1; i >= 0; i--)
	{
		/* Unmatched opened object or array */
		if (tokens[i].start != -1 && tokens[i].end == -1)
		{
			printf("returning TOKERR_PART FROM inside jtokenize. check for start != -1 and end != -1 failed!\n");
			res.status = TOKERR_PART;
			break;
		}
		else
		{
			printf("token[%d].start  = %d\n"
						 "token[%d].end    = % d\n"
						 "token[%d].size   = % d\n"
						 "token[%d].parent = % d\n\n",
						 i, tokens[i].start, 
						 i, tokens[i].end, 
						 i, tokens[i].size, 
						 i, tokens[i].parent);
		}
	}
	return res;
}

/**
 * Creates a new parser based over a given  buffer with an array of tokens
 * available.
 */
void jtok_init(token_parser *parser)
{
	parser->pos = 0;
	parser->toknext = 0;
	parser->toksuper = -1;
}
