#ifndef __SMALL_JSON_PARSER_H__
#define __SMALL_JSON_PARSER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// # SmallJSONParser #
//
// This is a simple, one-file JSON parser in C. It is designed for highly
// resource-constrained systems. It uses no memory allocation, and can stream
// data, so that the whole file does not need to reside in memory.
//
// It does not actually convert the JSON structure into a native data structure
// or tree. Instead, it works more like a tokeniser, and finds each primitive
// value in turn, and also reports when an object or array starts or ends.
//
// The application then follows along with the structure, and extracts the data
// it needs as it encounters it. The implementation also includes a set of
// helper functions to make this easier.
//
// ## Simplifications ##
//
// In order to keep code size down, the parser actually parses a simplified JSON
// syntax. Any well-formed JSON data will be parsed correctly by the simplified
// grammar, but many kinds of JSON data that a strict parser would not accept
// will be accepted by this parser.
//
// * `:` and `,` are ignored entirely.
// * `]` and `}` are treated as interchangeable.
// * Any bareword of any length starting with `t` is treated as a `true` value.
// * Any bareword of any length starting with `f` is treated as a `false` value.
// * Any bareword of any length starting with `n` is treated as a `null` value.
// * Number parsing may be less strict that the spec requires.
//
// For instance, this can be parsed as valid data: `{"a" t "b" f "c" [1 2 3}}`.
//
// ## API documentation ##
//
// The API contains several layers, all quite simple.




// ### Minimal API ###
//
// At this level, the parser is supplied with a buffer of memory containing JSON
// data, and the parsing function is called repeatedly, returning one token per
// call. When the end of buffer is reached, the parser may return a partial
// token (for instance, if the buffer ends in the middle of a string value).
//
// Once the end of the buffer has been released, a new buffer with the following
// data can be supplied, and the parser will carry in where it left off, and
// return the remaining data of any partial token it previously returned.
//
// If the whole JSON structure can be fit into one buffer, partial tokens do not
// need to be handled.
//
// Value data is returned in tokens simply as start and end pointers into the
// original data. You can access this data directly yourself, or you can use the
// token parsing API functions, defined further down, to parse string and
// number values.

#define StringJSONToken 1
#define NumberJSONToken 2
#define StartObjectJSONToken 3
#define EndObjectJSONToken 4
#define StartArrayJSONToken 5
#define EndArrayJSONToken 6
#define TrueJSONToken 7
#define FalseJSONToken 8
#define NullJSONToken 9
#define OutOfDataJSONToken 10
#define ParseErrorJSONToken 11

#define JSONTokenFlagMask 0xc000
#define PartialJSONTokenFlag 0x8000
#define TruncatedJSONTokenFlag 0x4000

typedef struct JSONParser
{
	const uint8_t *currentbyte,*end;
	int state,partialtokentype;
} JSONParser;

typedef struct JSONToken
{
	unsigned int typeandflags;
	const uint8_t *start,*end;
} JSONToken;

// #### Functions ####

// Initialise a `JSONParser` structure. Call this before starting parsing, or to
// restart a parser from scratch. `self` is an uninitialised `JSONParser`.
void InitialiseJSONParser(JSONParser *self);

// Provide input for the `JSONParser` `self`. `bytes` is a pointer to the bytes
// of JSON data, and `length` is the number of bytes.
void ProvideJSONInput(JSONParser *self,const uint8_t *bytes,size_t length);

// Find the next token from the JSON data being parsed by the `JSONParser`
// `self`, The `start` field of the returned `JSONToken` structure is a pointer
// to the first byte of the value for this token. The `end` filed points to one
// past the end of the value. `typeandflags` contains a combination of the
// token type and flags for the token.
//
// If the end of the data provided to the the `JSONParser` is reached while
// looking for the next token, the type of the token will be
// `OutOfDataJSONToken`.
//
// If the end of the data provided to the the `JSONParser` is reached while
// looking for the end of a found token, the returned token will be marked as
// partial, and the value will contain only as much of the token's value as
// was available.
//
// If a partial token is returned, then the next call to this function after
// providing more data through the `ProvideJSONInput` function will be a token
// of the same type, whose `start` and `end` pointers contain only the newly
// available data, not the earlier data. The value will have to be manually
// reconstructed if it is of interest.
JSONToken NextJSONToken(JSONParser *self);

// Returns the type of the `JSONToken` `token`. This will be one of the
// `*JSONToken` values.
static inline int JSONTokenType(JSONToken token) { return token.typeandflags&~JSONTokenFlagMask; }

// Checks if the `JSONToken` `token` is partial, i.e., if it the token extends
// past the end of the current buffer.
static inline bool IsJSONTokenPartial(JSONToken token) { return token.typeandflags&PartialJSONTokenFlag; }




// ### Pull API ###
//
// To facilitate streaming data through the parser, a slightly higher-level API
// is provided on top of the minimal API. With this API, you provide a callback
// function that loads and provides data to the parser as needed. You also
// provide a memory buffer that is used to reconstruct partial tokens across
// buffer boundaries.
//
// If a token needs reconstructed, the start and end pointers of the returned
// token will be pointing into the reconstruction buffer instead of into the
// original data.
//
// As the reconstruction buffer is of fixed size, if a value is too big to fit
// into the provided memory, it is truncated to fit, and any remaining data is
// discarded. If this is a problem, you should use the minimal API to handle
// reconstructing tokens yourself, or just load the entire JSON struture into
// memory so there are no buffer boundaries to deal with.

// #### Functions ####

// An input provider callback function. `parser` is the `JSONParser` structure
// that needs more input, and `context` is the arbitrary context pointer
// provided to `InitialiseJSONProvider`.
//
// The function should fetch more data, then call `ProvideJSONInput` on
// `parser`, then return `true`. Alternatively, it should return `false` to
// indicate that there is no more input available.
typedef bool JSONInputProviderCallbackFunction(JSONParser *parser,void *context);

typedef struct JSONProvider {
	JSONInputProviderCallbackFunction *callback;
	void *context;
	uint8_t *buffer;
	size_t buffersize;
} JSONProvider;

// Initialise a `JSONProvider` structure. `JSONParser` is responsible for
// streaming input to a `JSONParser`, by using the function supplied through
// `callback`. `context` is an arbitrary pointer that is passed on to the
// callback as extra data.
//
// It is also responsible for providing a memory buffer for reassembling
// partial tokens when needed. This buffer is passed in through the `buffer`
// argument, and its size through `buffersize`. Make sure this buffer is large
// enough to fit the largest value you are interested in.
//
// Up to one byte less than the available buffer size will be used when
// reconstructing the value, in order to leave space for a possible terminating
// zero byte added by `UnescapeJSONStringTokenInPlace()`.
static inline void InitialiseJSONProvider(JSONProvider *self,
JSONInputProviderCallbackFunction *callback,void *context,uint8_t *buffer,size_t buffersize)
{
	self->callback=callback;
	self->context=context;
	self->buffer=buffer;
	self->buffersize=buffersize;
}

// Find the next token from the JSON data being parsed by the `JSONParser`
// `self`, and provided by the `JSONProvider` `provider`. The `start` field of
// the returned `JSONToken` structure is a pointer to the first byte of the
// value for this token. The `end` filed points to one past the end of the
// value. `typeandflags` contains a combination of the token type and flags for
// the token.
//
// If the end of the stream provided by the `JSONProvider` is reached while
// looking for the next token, the type of the token will be
// `OutOfDataJSONToken`.
//
// If the value of a token extends across the buffer edges as provided by
// `JSONProvider`, the token value will be reassembled in the buffer supplied by
// the `JSONProvider`, and the `start` and `end` values will point into this
// buffer instead of into the actual JSON data.
//
// If there is not enough space to reassemble the token value, it will be
// flagged as truncated.
JSONToken NextJSONTokenWithProvider(JSONParser *self,JSONProvider *provider);

// Checks if the `JSONToken` `token` is truncated, i.e., if it needed to be
// reassembled by `NextJSONTokenWithProvider()` but there was not enough buffer
// space to do so.
static inline bool IsJSONTokenTruncated(JSONToken token) { return token.typeandflags&TruncatedJSONTokenFlag; }




// ### Token parsing API ###
//
// These functions help with extracting data from string and number values.
// Numbers can be parsed both as integers ~or floating-point~ values. Strings can
// be processed to expand escape codes, including Unicode escapes which are
// converted into UTF-8. String escape expansion can either be done in-place or
// into a separate buffer. If it is done in-place, the original data buffer is
// modified, so make sure this is possible and acceptable.

// #### Functions ####

// Unescape a JSON string token `token`, handing both escape codes such as \n
// and \r, and Unicode escapes like \u2619, which are converted to UTF-8. The
// result is stored in `unescapedbuffer`, and terminated with a zero byte.
// This buffer must be at least as long  as the length of the token, plus one
// for the terminating zero byte. You can use
// `SizeOfUnescapingBufferForJSONStringToken()` to calculate this size.
//
// The end of the unescaped string will be written to `end`, if not `NULL`.
bool UnescapeJSONStringToken(JSONToken token,uint8_t *unescapedbuffer,uint8_t **end);

// The number of bytes needed to unescape the token `token`, including the
// terminating zero byte.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static size_t SizeOfUnescapingBufferForJSONStringToken(JSONToken token)
{
	return token.end-token.start+1;
}
#pragma GCC diagnostic pop

// Unescape a JSON string token `token`, handing both escape codes such as \n
// and \r, and Unicode escapes like \u2619, which are converted to UTF-8. The
// result overwrites the existing data, and a zero byte is added for
// termination. The result is pointed to by `start` after the conversion is
// finished.
//
// This function overwrites data provided by `ProvideJSONInput()`, so make sure
// this is possible and acceptable before calling it.
bool UnescapeJSONStringTokenInPlace(JSONToken *token);

// Quickly compare whether the string value in the `JSONToken` `token` is equal
// to a given string or not. The string value is not unescaped in any way.
bool FastIsJSONStringEqual(JSONToken token,const char *string);

// Quickly compare whether the string value in the `JSONToken` `token` is equal
// to a given string for which the length is already known, or not. The string
// value is not unescaped in any way.
bool FastIsJSONStringEqualWithLength(JSONToken token,const char *string,size_t length);

// Parse the number value of the `JSONToken` `token`, assuming it is an
// integer. The result is stored in `result`, and the function returns `true`
// if conversion succeeded, else `false`.
bool ParseNumberTokenAsInteger(JSONToken token,int *result);

// ### Structure parsing API ###
//
// These functions help when writing code to parse the higher-level structure
// of the stream of tokens that the token parser outputs. The serve two main
// functions, encoding the expectations on future output, for instance that
// the upcoming value token after a key token in a dictionary should be a
// number, and skipping values that we are not interested in.
//
// All functions come in two varieties, one that takes a `JSONProvider` and
// one that does not. The ones that do not simply leave out this parameter,
// and the `WithProvider` part of the function name. They are not documented
// separately, and should just be assumed to exist for each function in this
// section. It should be noted that they are mainly only useful if the whole
// JSON data has been provided to the `JSONParser`, as there is no handling
// of buffer boundaries.
//
// Each function returns `bool` signifying success. Normal usage of these
// functions would be to check the result of each one, and return from the
// function or goto an error handler if any of them return false.
//
// #### Functions ####

// Find the next token from the JSON data being parsed by the `JSONParser`,
// and check that it is of type `expectedtype`. The token is stored in the
// location pointed to by `token`.
bool ExpectJSONTokenOfTypeWithProvider(JSONParser *self,JSONProvider *provider,int expectedtype,JSONToken *token);

// Skip the next JSON value. If it is an object or array, all contained values
// will be skipped also.
bool SkipJSONValueWithProvider(JSONParser *self,JSONProvider *provider);

// Find the next token from the JSON data being parsed by the `JSONParser`,
// check that it is of type `expectedtype`, then skip it. If it is an object or
// array, all contained values will be skipped also.
bool ExpectAndSkipJSONValueOfTypeWithProvider(JSONParser *self,JSONProvider *provider,int expectedtype);

// Skip all following JSON values until the end of the current object is
// reached. Any objects or arrays encountered will also be skipped in full.
bool SkipUntilEndOfJSONObjectWithProvider(JSONParser *self,JSONProvider *provider);

// Skip all following JSON values until the end of the current object is
// reached. Any objects or arrays encountered will also be skipped in full.
bool SkipUntilEndOfJSONArrayWithProvider(JSONParser *self,JSONProvider *provider);

// Skip through the keys and values of a JSON object until a key with value
// `key` is reached. No expanding of escape codes is done on the keys before
// comparing. Objects and arrays are skipped in full.
//
// This function assumes you have already encountered the `StartObjectJSONToken`
// token for this array.
bool SkipUntilJSONObjectKeyWithProvider(JSONParser *self,JSONProvider *provider,const char *key);

// Find the next token from the JSON data being parsed by the `JSONParser`,
// and check that it is the start of an object. If it is, skip through the keys
// and values of a JSON object until a key with value `key` is reached. No
// expanding of escape codes is done on the keys before comparing. Objects
// and arrays are skipped in full.
bool ExpectAndSkipUntilJSONObjectKeyWithProvider(JSONParser *self,JSONProvider *provider,const char *key);

static inline bool ExpectJSONTokenOfType(JSONParser *self,int expectedtype,JSONToken *token) { return ExpectJSONTokenOfTypeWithProvider(self,NULL,expectedtype,token); }
static inline bool SkipJSONValue(JSONParser *self) { return SkipJSONValueWithProvider(self,NULL); }
static inline bool ExpectAndSkipJSONValueOfType(JSONParser *self,int expectedtype) { return ExpectAndSkipJSONValueOfTypeWithProvider(self,NULL,expectedtype); }
static inline bool SkipUntilEndOfJSONObject(JSONParser *self) { return SkipUntilEndOfJSONObjectWithProvider(self,NULL); }
static inline bool SkipUntilEndOfJSONArray(JSONParser *self) { return SkipUntilEndOfJSONArrayWithProvider(self,NULL); }
static inline bool SkipUntilJSONObjectKey(JSONParser *self,const char *key) { return SkipUntilJSONObjectKeyWithProvider(self,NULL,key); }
static inline bool ExpectAndSkipUntilJSONObjectKey(JSONParser *self,const char *key) { return ExpectAndSkipUntilJSONObjectKeyWithProvider(self,NULL,key); }

// ## License ##
//
// This code is released into the public domain with no warranties. If that is
// not suitable, it is also available under the
// [CC0 license](http://creativecommons.org/publicdomain/zero/1.0/).

#ifdef __cplusplus
}
#endif

#endif
