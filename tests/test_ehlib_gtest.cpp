#include "../include/ehlib.h"
#include <gtest/gtest.h>
#include <iostream>

class TestEhlib : public ::testing::Test 
{
protected:
	void SetUp() {}
	void TearDown() {}
};

TEST_F(TestEhlib, test_get_request)
{
	auto r = ehlib::request(
        ehlib::RequestUrl{"https://ya.ru"},
        ehlib::RequestMethod{"GET"},
		ehlib::RequestTimeout{300000},
        ehlib::RequestUserAgent{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36"}
    );

	EXPECT_TRUE(r.status_code == "200");
}

TEST_F(TestEhlib,  test_post_request)
{
	auto r = ehlib::request(
		ehlib::RequestUrl{"https://postman-echo.com/post"},
		ehlib::PostRequestBody{"This is raw POST data"},
		ehlib::RequestTimeout{300000},
		ehlib::RequestMethod{"POST"}
	);

	EXPECT_TRUE(r.status_code == "200");
}

TEST_F(TestEhlib,  test_basicauth_request)
{
	auto r = ehlib::request(
        ehlib::RequestUrl{"https://postman-echo.com/basic-auth"},
        ehlib::BasicAuth {"postman", "password"},
		ehlib::RequestTimeout{300000},
		ehlib::RequestMethod{"GET"}
	);

	EXPECT_TRUE(r.status_code == "200");
}

TEST_F(TestEhlib,  test_bearerauth_request)
{
	auto r = ehlib::request(
		ehlib::RequestUrl{"http://www.httpbin.org/bearer"},
		ehlib::BearerAuth {"ACCESS_TOKEN"},
		ehlib::RequestTimeout{300000},
		ehlib::RequestMethod{"GET"}
	);

	EXPECT_TRUE(r.status_code == "200");
}

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}