# cson.h
cson.h is a single header json parsing library written in c

## Disclaimer

This library as of now is not fully JSON compliant on the following points.
- No handling of special charachters in strings like \n, \r, etc.
- Exponents for numbers are currently not supported.

## To run tests

Test are implemented using [utest.h by sheredom](https://github.com/sheredom/utest.h).

Simply run `make` to run the tests.

## Usage

Below is a basic example of how to get a value from a json object using a key string.

Please note that all string arguments must be zero terminated.

```C
#include <stdio.h>
#include "cson.h"

int main(void){
	CSON* cson;
	CSON_Result res = CSON_parse(&cson,"{\"key\":\"value\"}");
	if(res == CSON_ERROR){
		// handle parser error
	}
	const char* value = CSON_get_string(CSON_get_by_key(cson,"key"));
	printf("%s\n",value);
	CSON_free(cson);
	return 0;
}
```

### Accessing elements from containers

The following functions can be used to access elements from objects and arrays.

```C
CSON* CSON_get_by_key(CSON* cson, const char* key); // access elements from objects
CSON* CSON_get_by_index(CSON* cson, size_t index);  // access elements from arrays
```

### Type checking

As there is no way to check the json types at compile time, you can use the following functions to check the types at runtime.

```C
bool CSON_is_null(CSON *cson);
bool CSON_is_bool(CSON *cson);
bool CSON_is_string(CSON *cson);
bool CSON_is_number(CSON *cson);
bool CSON_is_array(CSON *cson);
bool CSON_is_object(CSON *cson);
bool CSON_is_container(CSON *cson); // returns true for both arrays and objects
```

### Getters

The following functions can be used to retrieve the actual value from the CSON element.

```C
bool CSON_get_bool(CSON *cson);
const char *CSON_get_string(CSON *cson);
double CSON_get_double(CSON *cson); // returns number as double
```

## TODO

- make CSON JSON compliant
- add ways to get more information about parsing errors
- implement api for creating and editing the JSON DOM 
- implement encoding functionality
