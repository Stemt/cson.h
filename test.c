#define CSON_IMPLEMENTATION
#include "cson.h"

#include "utest.h"


UTEST_MAIN()

// bare element parsing tests
UTEST(CSON_Test_element, parse_empty_array) {
	CSON* cson;
	CSON_parse(&cson,"[]");
	ASSERT_EQ(cson->type, CSON_ARRAY);
}

UTEST(CSON_Test_element, parse_empty_object) {
	CSON* cson;
	CSON_parse(&cson,"{}");
	ASSERT_EQ(cson->type, CSON_OBJECT);
}

UTEST(CSON_Test_element, parse_string) {
	CSON* cson;
	CSON_parse(&cson,"\"test\"");
	ASSERT_EQ(cson->type, CSON_STRING);
}

UTEST(CSON_Test_element, parse_number) {
	CSON* cson;
	CSON_parse(&cson,"42");
	ASSERT_EQ(cson->type, CSON_NUMBER);
}

UTEST(CSON_Test_element, parse_true) {
	CSON* cson;
	CSON_parse(&cson,"true");
	ASSERT_EQ(cson->type, CSON_TRUE);
}

UTEST(CSON_Test_element, parse_false) {
	CSON* cson;
	CSON_parse(&cson,"false");
	ASSERT_EQ(cson->type, CSON_FALSE);
}

UTEST(CSON_Test_element, parse_null) {
	CSON* cson;
	CSON_parse(&cson,"null");
	ASSERT_EQ(cson->type, CSON_NULL);
}

// getter tests
UTEST(CSON_Test_getters, get_string){
	CSON* cson;
	CSON_parse(&cson, "\"test\"");
	char* str = "test";
	ASSERT_TRUE(CSON_is_string(cson));
	ASSERT_EQ(strcmp(CSON_get_string(cson),str),0);
}

UTEST(CSON_Test_getters, get_int){
	CSON* cson;
	CSON_parse(&cson, "42");
	ASSERT_TRUE(CSON_is_number(cson));
	int value = CSON_get_number(cson);
	ASSERT_EQ(value,42);
}

UTEST(CSON_Test_getters, get_true){
	CSON* cson;
	CSON_parse(&cson, "true");
	ASSERT_TRUE(CSON_get_bool(cson));
}

UTEST(CSON_Test_getters, get_false){
	CSON* cson;
	CSON_parse(&cson, "false");
	ASSERT_TRUE(!CSON_get_bool(cson));
}

UTEST(CSON_Test_getters, check_null){
	CSON* cson;
	CSON_parse(&cson, "null");
	ASSERT_TRUE(CSON_is_null(cson));
}

UTEST(CSON_Test_getters, get_array_element){
	CSON* cson;
	CSON_parse(&cson, "[true]");
	ASSERT_TRUE(CSON_is_array(cson));
	ASSERT_TRUE(CSON_get_bool(CSON_get_by_index(cson,0)));
}

UTEST(CSON_Test_getters, get_object_element){
	CSON* cson;
	CSON_parse(&cson, "{\"state\":false}");
	ASSERT_TRUE(CSON_is_object(cson));
	ASSERT_FALSE(CSON_get_bool(CSON_get_by_key(cson,"state")));
}

// nested object tests
UTEST(CSON_Test_nested, array_elements){
	CSON* cson;
	CSON_parse(&cson, "[\"true\",true,false,null,32,[],{}]");
	ASSERT_TRUE(CSON_is_array(cson));
	ASSERT_TRUE(CSON_is_string(CSON_get_by_index(cson,0)));
	ASSERT_TRUE(CSON_is_bool(CSON_get_by_index(cson,1)));
	ASSERT_TRUE(CSON_is_bool(CSON_get_by_index(cson,2)));
	ASSERT_TRUE(CSON_is_null(CSON_get_by_index(cson,3)));
	ASSERT_TRUE(CSON_is_number(CSON_get_by_index(cson,4)));
	ASSERT_TRUE(CSON_is_array(CSON_get_by_index(cson,5)));
	ASSERT_TRUE(CSON_is_object(CSON_get_by_index(cson,6)));
}

UTEST(CSON_Test_nested, object_elements){
	CSON* cson;
	CSON_parse(&cson,"{"
			"\"true\":true,"
			"\"false\":false,"
			"\"null\":null,"
			"\"string\":\"string\","
			"\"number\":42,"
			"\"array\":[],"
			"\"object\":{}"
			"}");
	ASSERT_TRUE_MSG(CSON_is_object(cson),"is object");
	ASSERT_TRUE_MSG(CSON_is_string(CSON_get_by_key(cson,"string")),"is string");
	ASSERT_TRUE_MSG(CSON_is_bool(CSON_get_by_key(cson,"true")),"is true");
	ASSERT_TRUE_MSG(CSON_is_bool(CSON_get_by_key(cson,"false")),"is false");
	ASSERT_TRUE_MSG(CSON_is_null(CSON_get_by_key(cson,"null")),"is null");
	ASSERT_TRUE_MSG(CSON_is_number(CSON_get_by_key(cson,"number")),"is number");
	ASSERT_TRUE_MSG(CSON_is_array(CSON_get_by_key(cson,"array")),"is array");
	ASSERT_TRUE_MSG(CSON_is_object(CSON_get_by_key(cson,"object")),"is object");
}


