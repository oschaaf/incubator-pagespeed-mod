gurl_build_rule = """
cc_library(
    name = "googleurl_base",
    srcs = [
        'base/string16.cc',
    ],
    hdrs = [
        'base/string16.h',
        'base/basictypes.h',
    ],
    deps = [
        "@mod_pagespeed//base:log_shim", 
        #"@mod_pagespeed//base:string_util_shim",
    ],
    copts = [
        "-Wno-error=macro-redefined",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "googleurl",
    srcs = [
        'src/gurl.cc',
        'src/url_canon_etc.cc',
        'src/url_canon_fileurl.cc',
        'src/url_canon_filesystemurl.cc',
        'src/url_canon_host.cc',
        #'src/url_canon_icu.cc',
        'src/url_canon_internal.cc',
        'src/url_canon_ip.cc',
        'src/url_canon_mailtourl.cc',
        'src/url_canon_path.cc',
        'src/url_canon_pathurl.cc',
        'src/url_canon_query.cc',
        'src/url_canon_relative.cc',
        'src/url_canon_stdurl.cc',
        'src/url_parse.cc',
        'src/url_parse_file.cc',
        'src/url_util.cc',
    ],
    hdrs = [
        'src/gurl.h',
        'src/url_canon.h',
        'src/url_canon_icu.h',
        'src/url_canon_internal.h',
        'src/url_canon_internal_file.h',
        'src/url_canon_ip.h',
        'src/url_canon_stdstring.h',
        'src/url_file.h',
        'src/url_parse.h',
        'src/url_util.h',
        'src/url_parse_internal.h',
        'src/url_common.h',
        'src/url_util_internal.h',
    ],
    include_prefix="googleurl/",
    copts = [
        "-DNOTREACHED=void",
    ],
    deps = [
        ":googleurl_base",
        "@icu//:icu",
    ],
    visibility = ["//visibility:public"],
)
"""