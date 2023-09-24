#ifndef CSON_H
#define CSON_H

#ifndef CVEC_IMPLEMENTATION
#define CVEC_IMPLEMENTATION
#endif

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CVEC
typedef struct {
    size_t element_count;  // Current number of elements in the vector
    size_t element_size;   // Size of individual elements
    char* data;            // Pointer to the actual data (char* for generic data storage)
    size_t element_capacity; // Total capacity of the vector in elements
} CVec;

void CVec_init(CVec* vec, size_t element_size, size_t element_capacity);
void CVec_grow(CVec* vec);
void CVec_push_back(CVec* vec, void* element);
bool CVec_get(const CVec* vec, size_t index, void* element);
bool CVec_pop_back(CVec* vec, void* element);
void CVec_free(CVec* vec);

// CVEC implementation
#ifdef CVEC_IMPLEMENTATION
void CVec_init(CVec* vec, size_t element_size, size_t element_capacity) {
    vec->element_count = 0;
    vec->element_size = element_size;
    vec->element_capacity = element_capacity;
    vec->data = (char*) malloc(element_size * element_capacity);

    if (vec->data == NULL) {
        fprintf(stderr, "Memory allocation failed in CVec_init\n");
        exit(1);
    }
}

void CVec_grow(CVec* vec) {
    size_t new_capacity = vec->element_capacity * 2;
    char* new_data = (char*) realloc(vec->data, vec->element_size * new_capacity);

    if (new_data == NULL) {
        fprintf(stderr, "Memory reallocation failed in CVec_grow\n");
        exit(1);
    }

    vec->data = new_data;
    vec->element_capacity = new_capacity;
}


void CVec_push_back(CVec* vec, void* element) {
    if (vec->element_count == vec->element_capacity) {
        // If the capacity is reached, grow the vector
        CVec_grow(vec);
    }

    // Calculate the offset to copy the element
    size_t offset = vec->element_count * vec->element_size;

    // Copy the element to the end of the data array
    memcpy(vec->data + offset, element, vec->element_size);

    // Increment the element count
    vec->element_count++;
}

// Function to retrieve an element from the CVec at the specified index
bool CVec_get(const CVec* vec, size_t index, void* element) {
    if (index >= vec->element_count) {
        return false; // Index out of bounds
    }

    // Calculate the offset to access the element
    size_t offset = index * vec->element_size;

    // Copy the element to the provided pointer
    memcpy(element, vec->data + offset, vec->element_size);

    return true; // Get was successful
}

// Function to remove and copy the element at the back of the CVec to a provided pointer
bool CVec_pop_back(CVec* vec, void* element) {
    if (vec->element_count == 0) {
        return false; // Cannot pop from an empty vector
    }

    // Calculate the index of the last element
    size_t last_index = vec->element_count - 1;

    // Use CVec_get to retrieve and copy the last element
    if (!CVec_get(vec, last_index, element)) {
        return false; // Get operation failed
    }

    // Decrement the element count
    vec->element_count--;

    return true; // Pop back was successful
}

// Function to free the memory used by a CVec
void CVec_free(CVec* vec) {
    free(vec->data);
    vec->data = NULL; // Set data pointer to NULL to avoid double-free
    vec->element_count = 0;
    vec->element_capacity = 0;
    vec->element_size = 0;
}
#endif // CVEC_IMPLEMENTATION



// CSON
#define CSON_DEFAULT_MEMBLOCK_SIZE 16

// string view
typedef struct {
  char *str;
  size_t len;
} CSON_SV;

CSON_SV *CSON_SV_new(const char *str);
void CSON_SV_init(CSON_SV *sv, char *str);
bool CSON_SV_eq(CSON_SV *A, CSON_SV *B);

// tokenizing
typedef enum {
  CSON_TOKENTYPE_WS,
  CSON_TOKENTYPE_COLON,
  CSON_TOKENTYPE_COMMA,
  CSON_TOKENTYPE_STRING,
  CSON_TOKENTYPE_CURLY_OPEN,
  CSON_TOKENTYPE_CURLY_CLOSE,
  CSON_TOKENTYPE_SQUARE_OPEN,
  CSON_TOKENTYPE_SQUARE_CLOSE,
  CSON_TOKENTYPE_NUMBER,
  CSON_TOKENTYPE_WORD,
  CSON_TOKENTYPE_UNKNOWN,
  CSON_TOKENTYPE_EOF
} CSON_TokenType;

typedef struct {
  CSON_SV sv;
  CSON_TokenType type;
} CSON_Token;

typedef struct {
  CSON_SV sv;
} CSON_Tokenizer;

CSON_Tokenizer *CSON_Tokenizer_new(char *cstr);
void CSON_Tokenizer_free(CSON_Tokenizer *tokenizer);
CSON_TokenType CSON_Tokenizer_identify_token_type(char c);
bool CSON_Tokenizer_is_char_valid_given_token(char c, CSON_TokenType type);
CSON_Token CSON_Tokenizer_consume(CSON_Tokenizer *tokenizer);
CSON_Token CSON_Tokenizer_peek(CSON_Tokenizer *tokenizer);

// parsing
typedef enum {
  CSON_SUCCES,
  CSON_ERROR,
} CSON_Result;

typedef enum {
  CSON_ARRAY = 0,
  CSON_OBJECT = 1,
  CSON_STRING = 2,
  CSON_NUMBER = 3,
  CSON_TRUE = 4,
  CSON_FALSE = 5,
  CSON_NULL = 6,
} CSON_Type;

typedef struct {
  CSON_Type type;
} CSON;

CSON_Result CSON_parse(CSON **cson, char *cstr);
CSON_Result CSON_parse_element(CSON **element, CSON_Tokenizer *tokenizer);
CSON_Result CSON_parse_object(CSON **element, CSON_Tokenizer *tokenizer);
CSON_Result CSON_parse_array(CSON **element, CSON_Tokenizer *tokenizer);
CSON_Result CSON_parse_string(CSON **element, CSON_Tokenizer *tokenizer);
CSON_Result CSON_parse_number(CSON **element, CSON_Tokenizer *tokenizer);
CSON_Result CSON_parse_literal(CSON **element, CSON_Tokenizer *tokenizer);

void CSON_free(CSON *cson);

// checkers
bool CSON_is_null(CSON *cson);
bool CSON_is_bool(CSON *cson);
bool CSON_is_string(CSON *cson);
bool CSON_is_number(CSON *cson);
bool CSON_is_array(CSON *cson);
bool CSON_is_object(CSON *cson);
bool CSON_is_container(CSON *cson);

// getters
bool CSON_get_bool(CSON *cson);
const char *CSON_get_string(CSON *cson);
double CSON_get_double(CSON *cson);
CSON *CSON_get_by_index(CSON *cson, size_t index);
CSON *CSON_get_by_key(CSON *cson, const char *key);

// setters
void CSON_set_null(CSON *cson);
void CSON_set_bool(CSON *cson, bool b);
void CSON_set_string(CSON *cson, const char *cstr);

typedef struct {
  CSON cson;
} CSON_Literal;
CSON_Literal *CSON_Literal_new();
void CSON_Literal_free(CSON_Literal *literal);

typedef struct {
  CSON cson;
  double value;
} CSON_Number;

CSON_Number *CSON_Number_new(double value);
void CSON_Number_free(CSON_Number *number);

typedef struct {
  CSON cson;
  CSON_SV sv;
} CSON_String;

CSON_String *CSON_String_from_sv(CSON_SV sv);
void CSON_String_free(CSON_String *string);

typedef struct {
  CSON cson;
  CVec data;
} CSON_Array;

CSON_Array *CSON_Array_new();
void CSON_Array_free(CSON_Array *array);
void CSON_Array_append(CSON_Array *array, CSON *value);

typedef struct {
  CSON cson;
  CVec keys;
  CVec data;
} CSON_Object;

CSON_Object *CSON_Object_new();
void CSON_Object_free(CSON_Object *object);
void CSON_Object_insert(CSON_Object *object, CSON_String *key, CSON *value);

#ifdef CSON_IMPLEMENTATION

// genralized

void CSON_free(CSON *cson) {
  switch (cson->type) {
  case CSON_TRUE:
  case CSON_FALSE:
  case CSON_NULL: {
    CSON_Literal_free((CSON_Literal *)cson);
    return;
  }
  case CSON_STRING: {
    CSON_String_free((CSON_String *)cson);
    return;
  }
  case CSON_ARRAY: {
    CSON_Array_free((CSON_Array *)cson);
    return;
  }
  case CSON_OBJECT: {
    CSON_Object_free((CSON_Object *)cson);
    return;
  }
  case CSON_NUMBER: {
    CSON_Number_free((CSON_Number *)cson);
    return;
  }
  default: {
    fprintf(stderr, "CSON: attempt to free unkown CSON type %d", cson->type);
    exit(EXIT_FAILURE);
  }
  }
}

// checkers
bool CSON_is_string(CSON *cson) {
  assert(cson && "provided null pointer");
  return cson->type == CSON_STRING;
}

bool CSON_is_number(CSON *cson) {
  assert(cson && "provided null pointer");
  return cson->type == CSON_NUMBER;
}

bool CSON_is_bool(CSON *cson) {
  assert(cson && "provided null pointer");
  return cson->type == CSON_TRUE || cson->type == CSON_FALSE;
}

bool CSON_is_null(CSON *cson) {
  assert(cson && "provided null pointer");
  return cson->type == CSON_NULL;
}

bool CSON_is_array(CSON *cson) {
  assert(cson && "provided null pointer");
  return cson->type == CSON_ARRAY;
}

bool CSON_is_object(CSON *cson) {
  assert(cson && "provided null pointer");
  return cson->type == CSON_OBJECT;
}

bool CSON_is_container(CSON *cson) {
  assert(cson && "provided null pointer");
  return CSON_is_array(cson) || CSON_is_object(cson);
}

// getters
const char *CSON_get_string(CSON *cson) {
  assert(CSON_is_string(cson) &&
         "attempted to get string from non string type");
  return ((CSON_String *)cson)->sv.str;
}

double CSON_get_number(CSON *cson) {
  assert(CSON_is_number(cson) &&
         "attempted to get number from non number type");
  return ((CSON_Number *)cson)->value;
}

bool CSON_get_bool(CSON *cson) {
  assert(CSON_is_bool(cson) && "attempted to get number from non number type");
  return cson->type == CSON_TRUE;
}

CSON *CSON_get_by_index(CSON *cson, size_t index) {
  assert(CSON_is_array(cson) &&
         "attempted to get by index from non array type");
  CSON *element = NULL;
  CVec_get(&((CSON_Array *)cson)->data, index, &element);
  return element;
}

CSON *CSON_get_by_key(CSON *cson, const char *key) {
  assert(CSON_is_object(cson) &&
         "attempted to get by key from non object type");
  CSON_Object *object = (CSON_Object *)cson;
  CSON_SV *given_key = CSON_SV_new(key);
  CSON_String *current_key;
  CSON *value = NULL;
  size_t i = 0;
  while (i < object->keys.element_count) {
    CVec_get(&((CSON_Object *)cson)->keys, i, &current_key);
    if (CSON_SV_eq(given_key, &current_key->sv)) {
      CVec_get(&((CSON_Object *)cson)->data, i, &value);
      return value;
    }
    i++;
  }
  return NULL;
}

// string view
CSON_SV *CSON_SV_new(const char *cstr) {
  size_t len = strlen(cstr);
  char *str_buf = malloc(len);
  assert(str_buf && "No ram?");
  memcpy(str_buf, cstr, len);
  CSON_SV *sv = malloc(sizeof(CSON_SV));
  assert(sv && "No ram?");
  *sv = (CSON_SV){.str = str_buf, .len = len};
  return sv;
}

void CSON_SV_init(CSON_SV *sv, char *cstr) {
  *sv = (CSON_SV){.str = cstr, .len = strlen(cstr)};
}

bool CSON_SV_eq(CSON_SV *A, CSON_SV *B) {
  if (A->len != B->len) {
    return false;
  }
  return memcmp(A->str, B->str, A->len) == 0;
}

// literal
CSON_Literal *CSON_Literal_new(CSON_Type type) {
  CSON_Literal *literal = malloc(sizeof(CSON_Literal));
  assert(literal && "No ram?");
  literal->cson.type = type;
  return literal;
}

void CSON_Literal_free(CSON_Literal *literal) { free(literal); }

// number
CSON_Number *CSON_Number_new(double value) {
  CSON_Number *number = malloc(sizeof(CSON_Number));
  assert(number && "No ram?");
  number->cson.type = CSON_NUMBER;
  number->value = value;
  return number;
}

void CSON_Number_free(CSON_Number *number) { free(number); }

// string
CSON_String *CSON_String_from_sv(CSON_SV sv) {
  CSON_String *string = malloc(sizeof(CSON_String));
  assert(string && "No ram?");
  string->sv.str =
      malloc(sv.len + 1); // allocate for string and zero termination
  assert(string->sv.str && "No ram?");
  memcpy(string->sv.str, sv.str, sv.len);
  string->sv.str[sv.len] = '\0';
  string->sv.len = sv.len;
  string->cson.type = CSON_STRING;
  return string;
}

void CSON_String_free(CSON_String *string) {
  free(string->sv.str);
  free(string);
}

// array
CSON_Array *CSON_Array_new() {
  CSON_Array *array = malloc(sizeof(CSON_Array));
  assert(array && "No ram?");
  array->cson.type = CSON_ARRAY;
  CVec_init(&array->data, sizeof(CSON *), CSON_DEFAULT_MEMBLOCK_SIZE);
  return array;
}

void CSON_Array_free(CSON_Array *array) {
  CSON *value;
  CVec_pop_back(&array->data, &value);
  while (array->data.element_count > 0) {
    free(value);
    CVec_pop_back(&array->data, &value);
  }
  free(array);
}

void CSON_Array_append(CSON_Array *array, CSON *value) {
  CVec_push_back(&array->data, &value);
}

// object
CSON_Object *CSON_Object_new() {
  CSON_Object *object = malloc(sizeof(CSON_Object));
  assert(object && "No ram?");
  object->cson.type = CSON_OBJECT;
  CVec_init(&object->keys, sizeof(CSON_String *), CSON_DEFAULT_MEMBLOCK_SIZE);
  CVec_init(&object->data, sizeof(CSON *), CSON_DEFAULT_MEMBLOCK_SIZE);
  return object;
}

void CSON_Object_free(CSON_Object *object) {
  CSON *key;
  CVec_pop_back(&object->keys, &key);
  CSON *value;
  CVec_pop_back(&object->data, &value);
  while (object->data.element_count > 0) {
    free(key);
    free(value);
    CVec_pop_back(&object->keys, &key);
    CVec_pop_back(&object->data, &value);
  }
  free(object);
}

void CSON_Object_insert(CSON_Object *object, CSON_String *key, CSON *value) {
  CVec_push_back(&object->keys, &key);
  CVec_push_back(&object->data, &value);
}

// tokenizer
CSON_Tokenizer *CSON_Tokenizer_new(char *cstr) {
  CSON_Tokenizer *tokenizer = malloc(sizeof(tokenizer));
  assert(tokenizer && "No ram?");
  CSON_SV_init(&tokenizer->sv, cstr);
  return tokenizer;
}

void CSON_Tokenizer_free(CSON_Tokenizer *tokenizer) { free(tokenizer); }

CSON_TokenType CSON_Tokenizer_identify_token_type(char c) {
  switch (c) {
  case ' ':
  case '\t':
  case '\n':
  case '\r':
    return CSON_TOKENTYPE_WS;
  case '{':
    return CSON_TOKENTYPE_CURLY_OPEN;
  case '}':
    return CSON_TOKENTYPE_CURLY_CLOSE;
  case '[':
    return CSON_TOKENTYPE_SQUARE_OPEN;
  case ']':
    return CSON_TOKENTYPE_SQUARE_CLOSE;
  case ',':
    return CSON_TOKENTYPE_COMMA;
  case ':':
    return CSON_TOKENTYPE_COLON;
  case '\"':
    return CSON_TOKENTYPE_STRING;
  case '\0':
    return CSON_TOKENTYPE_EOF;
  default:
    break;
  }
  if ((c >= '0' && c <= '9') || c == '-') {
    return CSON_TOKENTYPE_NUMBER;
  } else if ((c >= 'A' && c <= 'Z') || (c >= 'a' || c <= 'z')) {
    return CSON_TOKENTYPE_WORD;
  }
  return CSON_TOKENTYPE_UNKNOWN;
}

CSON_Token CSON_Tokenizer_consume(CSON_Tokenizer *tokenizer) {
  CSON_Token token = CSON_Tokenizer_peek(tokenizer);
  tokenizer->sv.str += token.sv.len;
  if (token.type == CSON_TOKENTYPE_STRING) {
    tokenizer->sv.str += 2; // increment by two to account for quote chars of
                            // STRING type (quotes are not included in string)
  }
  return token;
}

CSON_Token CSON_Tokenizer_peek(CSON_Tokenizer *tokenizer) {
  CSON_Token token = {0};
  token.sv.str = tokenizer->sv.str;
  token.type = CSON_Tokenizer_identify_token_type(token.sv.str[0]);

  switch (token.type) {
  case CSON_TOKENTYPE_EOF:
    return token;
  case CSON_TOKENTYPE_CURLY_OPEN:
  case CSON_TOKENTYPE_CURLY_CLOSE:
  case CSON_TOKENTYPE_SQUARE_OPEN:
  case CSON_TOKENTYPE_SQUARE_CLOSE:
  case CSON_TOKENTYPE_COMMA:
  case CSON_TOKENTYPE_COLON: {
    token.sv.len = 1;
    return token;
  }
  case CSON_TOKENTYPE_WS: {
    size_t i = 1;
    while (CSON_Tokenizer_identify_token_type(token.sv.str[i]) ==
           CSON_TOKENTYPE_WS) {
      i++;
    }
    token.sv.len = i;
    return token;
  }
  case CSON_TOKENTYPE_STRING: {
    token.sv.str++;
    size_t i = 1;
    while (token.sv.str[i] != '\"') {
      if (token.sv.str[i] == '\0') {
        token.type = CSON_TOKENTYPE_EOF;
        token.sv.len = 0;
        return token;
      }
      i++;
    }
    token.sv.len = i;
    return token;
  }
  case CSON_TOKENTYPE_NUMBER: {
    size_t i = 1;
    char c = token.sv.str[i];
    while (((c >= '0' && c <= '9') || c == '.') && c != '\0') {
      i++;
      c = token.sv.str[i];
    }
    token.sv.len = i;
    return token;
  }
  case CSON_TOKENTYPE_WORD: {
    size_t i = 1;
    char c = token.sv.str[i];
    while (CSON_Tokenizer_identify_token_type(c) == CSON_TOKENTYPE_WORD) {
      i++;
      c = token.sv.str[i];
    }
    token.sv.len = i;
    return token;
  }
  case CSON_TOKENTYPE_UNKNOWN:
    break;
  }
  // should not ever be triggered
  fprintf(stderr, "CSON: unkown token encountered starting with %c\n",
          tokenizer->sv.str[0]);
  exit(EXIT_FAILURE);
}
// parsing
CSON_Result CSON_parse(CSON **cson, char *cstr) {
  CSON_Tokenizer *tokenizer = CSON_Tokenizer_new(cstr);
  CSON_Result res = CSON_parse_element(cson, tokenizer);
  CSON_Tokenizer_free(tokenizer);
  return res;
}

CSON_Result CSON_parse_element(CSON **element, CSON_Tokenizer *tokenizer) {
  CSON_Token token = CSON_Tokenizer_consume(tokenizer);
  switch (token.type) {
  case CSON_TOKENTYPE_CURLY_OPEN: {
    return CSON_parse_object(element, tokenizer);
  } break;
  case CSON_TOKENTYPE_SQUARE_OPEN: {
    return CSON_parse_array(element, tokenizer);
  } break;
  case CSON_TOKENTYPE_STRING: {
    *element = (CSON *)CSON_String_from_sv(token.sv);
    return CSON_SUCCES;
  } break;
  case CSON_TOKENTYPE_NUMBER: {
    double d;
    sscanf(token.sv.str, "%lf", &d);
    *element = (CSON *)CSON_Number_new(d);
    return CSON_SUCCES;
  } break;
  case CSON_TOKENTYPE_WORD: {
    CSON_SV sv_true;
    CSON_SV_init(&sv_true, "true");
    CSON_SV sv_false;
    CSON_SV_init(&sv_false, "false");
    CSON_SV sv_null;
    CSON_SV_init(&sv_null, "null");

    if (CSON_SV_eq(&token.sv, &sv_true)) {
      *element = (CSON *)CSON_Literal_new(CSON_TRUE);
    } else if (CSON_SV_eq(&token.sv, &sv_false)) {
      *element = (CSON *)CSON_Literal_new(CSON_FALSE);
    } else if (CSON_SV_eq(&token.sv, &sv_null)) {
      *element = (CSON *)CSON_Literal_new(CSON_NULL);
    } else {
      return CSON_ERROR;
    }
    return CSON_SUCCES;
  } break;
  default:
    break;
  }
  return CSON_ERROR;
}

CSON_Result CSON_parse_array(CSON **element, CSON_Tokenizer *tokenizer) {
  CSON_Array *array = CSON_Array_new();
  CSON_Token token = CSON_Tokenizer_peek(tokenizer);
  if (token.type == CSON_TOKENTYPE_SQUARE_CLOSE) {
    CSON_Tokenizer_consume(tokenizer);
  }
  CSON_Result res;
  while (token.type != CSON_TOKENTYPE_SQUARE_CLOSE) {

    // parse value
    CSON *value = NULL;
    res = CSON_parse_element(&value, tokenizer);
    if (res == CSON_ERROR) {
      goto PARSE_ERROR;
    }

    CSON_Array_append(array, value);

    // check for more
    token = CSON_Tokenizer_consume(tokenizer);
    switch (token.type) {
    case CSON_TOKENTYPE_COMMA:
      // token = CSON_Tokenizer_consume(tokenizer);
    case CSON_TOKENTYPE_SQUARE_CLOSE:
      continue;
    default:
      goto PARSE_ERROR;
    }
  }
  *element = (CSON *)array;
  return CSON_SUCCES;
PARSE_ERROR:
  CSON_Array_free(array);
  return CSON_ERROR;
}

CSON_Result CSON_parse_object(CSON **element, CSON_Tokenizer *tokenizer) {
  CSON_Object *object = CSON_Object_new();
  CSON_Token token = CSON_Tokenizer_peek(tokenizer);
  if (token.type == CSON_TOKENTYPE_CURLY_CLOSE) {
    CSON_Tokenizer_consume(tokenizer);
  }
  CSON_Result res;
  while (token.type != CSON_TOKENTYPE_CURLY_CLOSE) {
    // parse key
    CSON_String *key = NULL;
    res = CSON_parse_element((CSON **)&key, tokenizer);
    if (res == CSON_ERROR || key->cson.type != CSON_STRING) {
      goto PARSE_ERROR;
    }

    // colon seperator
    token = CSON_Tokenizer_consume(tokenizer);
    if (token.type != CSON_TOKENTYPE_COLON) {
      goto PARSE_ERROR;
    }

    // parse value
    CSON *value = NULL;
    res = CSON_parse_element(&value, tokenizer);
    if (res == CSON_ERROR) {
      goto PARSE_ERROR;
    }

    CSON_Object_insert(object, key, value);

    // check for more
    token = CSON_Tokenizer_consume(tokenizer);
    switch (token.type) {
    case CSON_TOKENTYPE_COMMA:
    case CSON_TOKENTYPE_CURLY_CLOSE:
      continue;
    default:
      goto PARSE_ERROR;
    }
  }
  *element = (CSON *)object;
  return CSON_SUCCES;
PARSE_ERROR:
  CSON_Object_free(object);
  return CSON_ERROR;
}

#endif // CSON_IMPLEMENTATION

#endif // CSON_H
