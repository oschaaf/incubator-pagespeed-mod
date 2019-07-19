load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load(":zlib.bzl", "zlib_build_rule")
load(":hiredis.bzl", "hiredis_build_rule")
load(":jsoncpp.bzl", "jsoncpp_build_rule")
load(":icu.bzl", "icu_build_rule")
load(":libpng.bzl", "libpng_build_rule")
load(":libwebp.bzl", "libwebp_build_rule")
load(":protobuf.bzl", "protobuf_build_rule")
load(":giflib.bzl", "giflib_build_rule")
load(":optipng.bzl", "optipng_build_rule")

ENVOY_COMMIT = "master"
BROTLI_COMMIT = "882f41850b679c1ff4a3804d5515d142a5807376"
ZLIB_COMMIT = "cacf7f1d4e3d44d871b605da3b647f07d718623f"
HIREDIS_COMMIT = "010756025e8cefd1bc66c6d4ed3b1648ef6f1f95"
JSONCPP_COMMIT = "7165f6ac4c482e68475c9e1dac086f9e12fff0d0"
RE2_COMMIT = "848dfb7e1d7ba641d598cb66f81590f3999a555a"
ICU_COMMIT = "46a834e2ebcd7c5b60f49350a166d8b9e4a24c0e"
LIBPNG_COMMIT = "b78804f9a2568b270ebd30eca954ef7447ba92f7"
LIBWEBP_COMMIT = "v0.6.1"
PROTOBUF_COMMIT = "e8ae137c96444ea313485ed1118c5e43b2099cf1"
GIFLIB_COMMIT = "c9a54afc6feb1e2cd0626a49b2c9e50015e96dbe"
OPTIPNG_COMMIT = "e9a5bd640c45e99000f633a0997df89fddd20026"
GRPC_COMMIT = "08fd59f039c7cf62614ab7741b3f34527af103c7"

def mod_pagespeed_dependencies():
    http_archive(
        name = "envoy",
        strip_prefix = "envoy-%s" % ENVOY_COMMIT,
        url = "https://github.com/envoyproxy/envoy/archive/%s.tar.gz" % ENVOY_COMMIT,
    )

    http_archive(
        name = "brotli",
        strip_prefix = "brotli-%s" % BROTLI_COMMIT,
        url = "https://github.com/google/brotli/archive/%s.tar.gz" % BROTLI_COMMIT,
    )

    http_archive(
        # TODO : Rename library as per bazel naming conventions
        name = "zlib",
        strip_prefix = "zlib-%s" % ZLIB_COMMIT,
        url = "https://github.com/madler/zlib/archive/%s.tar.gz" % ZLIB_COMMIT,
        build_file_content = zlib_build_rule,
        sha256 = "6d4d6640ca3121620995ee255945161821218752b551a1a180f4215f7d124d45",
    )

    http_archive(
        name = "hiredis",
        strip_prefix = "hiredis-%s" % HIREDIS_COMMIT,
        url = "https://github.com/redis/hiredis/archive/%s.tar.gz" % HIREDIS_COMMIT,
        build_file_content = hiredis_build_rule,
        sha256 = "b239f8de6073e4eaea4be6ba4bf20516f33d7bedef1fc89287de60a0512f13bd",
    )

    http_archive(
        name = "jsoncpp",
        strip_prefix = "jsoncpp-%s" % JSONCPP_COMMIT,
        url = "https://github.com/open-source-parsers/jsoncpp/archive/%s.tar.gz" % JSONCPP_COMMIT,
        build_file_content = jsoncpp_build_rule,
        sha256 = "9757f515b42b86ebd08b13bdfde7c27ca7436186d9b01ef1fa5cbc194e1f2764",
    )

    http_archive(
        name = "re2",
        strip_prefix = "re2-%s" % RE2_COMMIT,
        url = "https://github.com/google/re2/archive/%s.tar.gz" % RE2_COMMIT,
        sha256 = "76a20451bec4e3767c3014c8e2db9ff93cbdda28e98e7bb36af41a52dc9c3dea",
    )

    http_archive(
        name = "icu",
        strip_prefix = "incubator-pagespeed-icu-%s" % ICU_COMMIT,
        url = "https://github.com/apache/incubator-pagespeed-icu/archive/%s.tar.gz" % ICU_COMMIT,
        build_file_content = icu_build_rule,
        # TODO(oschaaf): like the commits, it would be great to have the sha256 declarations at the top of the file.
        sha256 = "e596ba1ff6feb7179733b71cbc793a777a388d1f6882a4d8656b74cb381c8e22",
    )

    http_archive(
        name = "libpng",
        strip_prefix = "libpng-%s" % LIBPNG_COMMIT,
        url = "https://github.com/glennrp/libpng/archive/%s.tar.gz" % LIBPNG_COMMIT,
        build_file_content = libpng_build_rule,
        sha256 = "b82a964705b5f32fa7c0b2c5a78d264c710f8c293fe7e60763b3381f8ff38d42",
    )

    http_archive(
        name = "libwebp",
        url = "https://chromium.googlesource.com/webm/libwebp/+archive/refs/tags/%s.tar.gz" % LIBWEBP_COMMIT,
        build_file_content = libwebp_build_rule,
        # TODO(oschaaf): fix sha256, fails in CI
        # sha256 = "b350385fe4d07bb95ce72259ce4cef791fb2d1ce1d77af1acea164c6c53f2907",
    )

    http_archive(
        name = "protobuf",
        strip_prefix = "protobuf-%s" % PROTOBUF_COMMIT,
        url = "https://github.com/protocolbuffers/protobuf/archive/%s.tar.gz" % PROTOBUF_COMMIT,
        build_file_content = protobuf_build_rule,
        sha256 = "c8d3fde0c3f4c5958f31f501f84d4d313f0577609aabce2ab6f58da2e4c6fbbc",
    )

    http_archive(
        name = "giflib",
        strip_prefix = "giflib-mirror-%s" % GIFLIB_COMMIT,
        url = "https://github.com/We-Amp/giflib-mirror/archive/%s.tar.gz" % GIFLIB_COMMIT,
        build_file_content = giflib_build_rule,
        sha256 = "90452882419956d8005b0b6cc51e02488e3016e809431d794f42ab1173a5df02",
    )

    http_archive(
        name = "optipng",
        strip_prefix = "incubator-pagespeed-optipng-%s" % OPTIPNG_COMMIT,
        url = "https://github.com/apache/incubator-pagespeed-optipng/archive/%s.tar.gz" % OPTIPNG_COMMIT,
        build_file_content = optipng_build_rule,
        sha256 = "e7e937b8c3085ca82389018fcb6a8bf3cb4ba2556921826e634614e1c7e0edd2",
    )

    http_archive(
        name = "grpc",
        strip_prefix = "grpc-%s" % GRPC_COMMIT,
        url = "https://github.com/grpc/grpc/archive/%s.tar.gz" % GRPC_COMMIT,
        sha256 = "9dbb44a934d87faa8482c911e294a9f843a6c04d3936df8be116b1241bf475d9",
    )

    native.bind(
        name = "madler_zlib",
        actual = "@zlib//:zlib",
    )