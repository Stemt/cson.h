# cson.h
cson.h is a single header json parsing library written in c

## To run tests

Simply run `make`

## Usage

Below is a basic example of how to get a value from a json object using a key string.

```C
#include <stdio.h>
#include "cson.h"

int main(void){
	CSON* cson;
	CSON_Result res = CSON_parse(cson,"{\"key\":\"value\"}");
	if(res == CSON_ERROR){
		// handle parser error
	}
	const char* value = CSON_get_string(CSON_get_by_key(cson,"key"));
	printf("%s\n",value);
	return 0;
}
```
