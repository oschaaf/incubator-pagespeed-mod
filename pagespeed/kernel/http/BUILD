licenses(["notice"])  # Apache 2

load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
    "envoy_cc_test",
    "envoy_cc_test_library",
    "envoy_package",
)

envoy_package()

envoy_cc_library(
    name = "http",
    srcs = [
        "google_url.cc",
        "query_params.cc",
    ],
    hdrs = [
        "google_url.h",
        "query_params.h",
    ],
    copts = [
        "-Wno-error=unused-parameter",
        "-Wno-unused-parameter",
    ],
    repository = "@envoy",
    visibility = ["//visibility:public"],
    deps = [
        "//pagespeed/kernel/base:pagespeed_base",
        "@glog",
        "@uriparser",
    ],
)

envoy_cc_test_library(
    name = "http_test_base",
    srcs = [
    ],
    hdrs = [
    ],
    copts = [
        "-Wno-error=unused-parameter",
        "-Wno-unused-parameter",
    ],
    repository = "@envoy",
    deps = [
        ":http",
    ],
)

envoy_cc_test(
    name = "http_test",
    copts = [
        "-Wno-error=unused-private-field",
        "-Wno-error=unused-parameter",
        "-Wno-unused-parameter",
    ],
    repository = "@envoy",
    deps = [
        ":http",
        ":http_test_base",
        "//pagespeed/kernel/base:pagespeed_gtest",
    ],
)
