gurl_build_rule = """
cc_library(
    name = "url",
    srcs = [
        "gurl.cc",
#        "origin.cc",
        "scheme_host_port.cc",
        "third_party/mozilla/url_parse.cc",
        "url_canon.cc",
        "url_canon_etc.cc",
        "url_canon_filesystemurl.cc",
        "url_canon_fileurl.cc",
        "url_canon_host.cc",
        "url_canon_internal.cc",
        "url_canon_ip.cc",
        "url_canon_mailtourl.cc",
        "url_canon_path.cc",
        "url_canon_pathurl.cc",
        "url_canon_query.cc",
        "url_canon_relative.cc",
#        "url_canon_stdstring.cc",
        "url_canon_stdurl.cc",
        "url_constants.cc",
        "url_parse_file.cc",
        "url_util.cc",
    ],
    hdrs = [
        "url_constants.h",
        "url_file.h",
        "url_parse_internal.h",
        "url_util.h",
        "gurl.h",
        "url_util_internal.h",
#        "origin.h",
        "scheme_host_port.h",
        "third_party/mozilla/url_parse.h",
        "url_canon.h",
        "url_canon_internal.h",
        "url_canon_internal_file.h",
        "url_canon_ip.h",
        "url_canon_stdstring.h",
    ],
    include_prefix="url/",
    deps = ["@mod_pagespeed//base:log_shim", "@mod_pagespeed//base:string_util_shim"],
    visibility = ["//visibility:public"],
)
"""