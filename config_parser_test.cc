#include "gtest/gtest.h"
#include "config_parser.h"
#include <sstream>
#include <string>

class NginxStringConfigTest : public ::testing::Test {
 protected:
  bool ParseString(const std::string& config_string) {
    std::stringstream config_stream(config_string);
    return parser_.Parse(&config_stream, &out_config_);
  }
  NginxConfigParser parser_;
  NginxConfig out_config_;
};

TEST_F(NginxStringConfigTest, SimpleConfig) {
  EXPECT_TRUE(ParseString("foo bar;"));
  // We can also add expectations about the contents of the statements:
  EXPECT_EQ(1, out_config_.statements_.size());
  EXPECT_EQ("foo", out_config_.statements_.at(0)->tokens_.at(0));
  EXPECT_EQ("bar", out_config_.statements_.at(0)->tokens_.at(1));
}

TEST_F(NginxStringConfigTest, ParseEmptyStringConfig) {
  EXPECT_FALSE(ParseString(""));
}

TEST_F(NginxStringConfigTest, MultipleStatementConfig) {
  EXPECT_TRUE(ParseString("foo bar; bar foo; hello world;"));
  EXPECT_EQ(3, out_config_.statements_.size());
  EXPECT_EQ("foo", out_config_.statements_.at(0)->tokens_.at(0));
  EXPECT_EQ("bar", out_config_.statements_.at(0)->tokens_.at(1));
  EXPECT_EQ("bar", out_config_.statements_.at(1)->tokens_.at(0));
  EXPECT_EQ("foo", out_config_.statements_.at(1)->tokens_.at(1));
  EXPECT_EQ("hello", out_config_.statements_.at(2)->tokens_.at(0));
  EXPECT_EQ("world", out_config_.statements_.at(2)->tokens_.at(1));
}

TEST_F(NginxStringConfigTest, BracesConfig) {
  EXPECT_TRUE(ParseString("braces {foo bar;}"));
  EXPECT_EQ(1, out_config_.statements_.size());
  EXPECT_EQ("braces", out_config_.statements_.at(0)->tokens_.at(0));
  EXPECT_EQ("foo", out_config_.statements_.at(0)->child_block_->statements_.at(0)->tokens_.at(0));
  EXPECT_EQ("bar", out_config_.statements_.at(0)->child_block_->statements_.at(0)->tokens_.at(1));
}

TEST_F(NginxStringConfigTest, NestedBracesConfig) {
  EXPECT_TRUE(ParseString("braces {foo bar {hello world;}}"));
  EXPECT_EQ(1, out_config_.statements_.size());
  EXPECT_EQ("braces", out_config_.statements_.at(0)->tokens_.at(0));
  EXPECT_EQ("foo", out_config_.statements_.at(0)->child_block_->statements_.at(0)->tokens_.at(0));
  EXPECT_EQ("bar", out_config_.statements_.at(0)->child_block_->statements_.at(0)->tokens_.at(1));
  EXPECT_EQ("hello", out_config_.statements_.at(0)->child_block_->statements_.at(0)->child_block_->statements_.at(0)->tokens_.at(0));
  EXPECT_EQ("world", out_config_.statements_.at(0)->child_block_->statements_.at(0)->child_block_->statements_.at(0)->tokens_.at(1));
}
