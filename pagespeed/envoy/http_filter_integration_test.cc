#include "test/integration/http_integration.h"

namespace Envoy {
class HttpFilterPageSpeedIntegrationTest : public HttpIntegrationTest,
                                        public testing::TestWithParam<Network::Address::IpVersion> {
public:
  HttpFilterPageSpeedIntegrationTest()
      : HttpIntegrationTest(Http::CodecClient::Type::HTTP1, GetParam(), realTime()) {}
  /**
   * Initializer for an individual integration test.
   */
  void SetUp() override { initialize(); }

  void initialize() override {
    config_helper_.addFilter("{ name: pagespeed, config: { key: via, val: pagespeed-filter } }");
    HttpIntegrationTest::initialize();
  }
};

INSTANTIATE_TEST_SUITE_P(IpVersions, HttpFilterPageSpeedIntegrationTest,
                        testing::ValuesIn(TestEnvironment::getIpVersionsForTest()));

// TODO(oschaaf): this test hangs. Seems to be a test-only issue.
TEST_P(HttpFilterPageSpeedIntegrationTest, DISABLED_Test1) {
  Http::TestRequestHeaderMapImpl headers{{":method", "GET"}, {":path", "/"}, {":authority", "host"}};

  IntegrationCodecClientPtr codec_client;
  FakeHttpConnectionPtr fake_upstream_connection;
  FakeStreamPtr request_stream;

  codec_client = makeHttpConnection(lookupPort("http"));
  auto response = codec_client->makeHeaderOnlyRequest(headers);
  ASSERT_TRUE(fake_upstreams_[0]->waitForHttpConnection(*dispatcher_, fake_upstream_connection,
                                                        std::chrono::milliseconds(1000)));
  ASSERT_TRUE(fake_upstream_connection->waitForNewStream(*dispatcher_, request_stream));
  ASSERT_TRUE(request_stream->waitForEndStream(*dispatcher_));
  response->waitForEndStream();

  //EXPECT_EQ("pagespeed-filter",
  //             request_stream->headers().get(Http::LowerCaseString("via"))->value().getStringView());

  codec_client->close();
}
} // namespace Envoy
