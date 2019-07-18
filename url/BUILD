cc_library(
    name = "url_canon_icu",
    srcs = [
        "url_canon_icu.cc"
    ],
    visibility = ["//visibility:public"],
    copts = [
        "-Iexternal/icu/source/common",
        "-Iexternal/icu/genfiles/arch/linux/common/include",
        "-Iexternal/icu/source/i18n",
        "-Iexternal/icu/source/i18n/unicode",
        "-DNOTREACHED=void",
    ],
    deps = [
        "@mod_pagespeed//base:log_shim",
        "@googleurl//:googleurl",
        "@icu//:icu",
        #"//pagespeed/kernel/base:pagespeed_base",
    ]
)