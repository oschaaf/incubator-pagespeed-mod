licenses(["notice"])  # Apache 2

load(
    "//bazel:pagespeed_test.bzl",
    "pagespeed_cc_test",
    "pagespeed_cc_test_library",
)

cc_library(
    name = "cache",
    srcs = [
        "file_cache.cc",
        "purge_context.cc",
        "threadsafe_cache.cc",
        "lru_cache.cc",
        "key_value_codec.cc",
        "write_through_cache.cc",
        "compressed_cache.cc",
        "async_cache.cc",
        "purge_set.cc",
        "cache_batcher.cc",
        "in_memory_cache.cc",
        "cache_key_prepender.cc",
        "delay_cache.cc",
        "cache_stats.cc",
        "fallback_cache.cc",
        "delegating_cache_callback.cc",
    ],
    hdrs = [
        "file_cache.h",
        "cache_interface.h",
        "purge_context.h",
        "threadsafe_cache.h",
        "lru_cache.h",
        "key_value_codec.h",
        "write_through_cache.h",
        "compressed_cache.h",
        "async_cache.h",
        "purge_set.h",
        "cache_batcher.h",
        "in_memory_cache.h",
        "lru_cache_base.h",
        "cache_key_prepender.h",
        "delay_cache.h",
        "cache_stats.h",
        "fallback_cache.h",
        "delegating_cache_callback.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//gurl:url",
        "//pagespeed/kernel/base:pagespeed_base",
        "//pagespeed/kernel/thread:thread",
        "//pagespeed/kernel/util:util",
        "//third_party/rdestl:rdestl",
    ],
)

pagespeed_cc_test_library (
    name = "cache_test_base",
    srcs = [
        "cache_spammer.cc",
        "mock_time_cache.cc",
    ],
    hdrs = [
        "mock_time_cache.h",
        "cache_spammer.h",
        "cache_test_base.h",
        "cache_batcher_testing_peer.h",
    ],
    deps = [
        ":cache",
        "//pagespeed/kernel/base:kernel_test_util",
        "//pagespeed/kernel/util:util_test_base",
        "//pagespeed/kernel/base:benchmark",
    ]
)

pagespeed_cc_test(
    name = "cache_test",
    srcs = glob(["*_test.cc"]),
    deps = [
        ":cache",
        ":cache_test_base"
    ],
)