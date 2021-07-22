#include "stdafx_nc_runtime_test.h"
#include "nc_runtime/json_setting_loader.h"

TEST(JsonSettingLoaderTest, basic)
{
    JsonSettingLoader o;
    o.loadFile("test_data/config.json");
    auto root = o.root();
    auto v = root.stringValueForKey("user.name");
    ASSERT_TRUE(v.hasValue());
    EXPECT_STREQ(v.value(), "Alexander the Great");

    EXPECT_FALSE(root.stringValueForKey("nonexist").hasValue());
    EXPECT_FALSE(root.stringValueForKey("user.nonexist").hasValue());
    EXPECT_STREQ(root.stringValueForKey("user.nonexist").Or("simba"), "simba");
}

TEST(JsonSettingLoaderTest, nonexistFile)
{
    JsonSettingLoader o;
    EXPECT_FALSE(o.loadFile("non-exist.json"));
    auto root = o.root();
    auto v = root.stringValueForKey("lua.main");
    ASSERT_FALSE(v.hasValue());
}
