uriparser_build_rule = """

genrule(
    name = "create_config_h",
    srcs = ["src/UriConfig.h.in"],
    outs = ["config.h"],
    cmd = "touch $@",
)

genrule(
    name = "fix_UriMemory",
    srcs = ["src/UriMemory.c"],
    outs = ["src/UriMemory_fixed.c"],
    cmd = "sed 's/<config.h>/\\"config.h\\"/g' $< > $@",
)


cc_library(
    name = "uriparser",
    srcs = [
        "src/UriCommon.c",
        "src/UriCompare.c",
        "src/UriEscape.c",
        "src/UriFile.c",
        "src/UriIp4Base.c",
        "src/UriIp4.c",
        #"src/UriMemory.c",
        ":fix_UriMemory",
        "src/UriNormalizeBase.c",
        "src/UriNormalize.c",
        "src/UriParseBase.c",
        "src/UriParse.c",
        "src/UriQuery.c",
        "src/UriRecompose.c",
        "src/UriResolve.c",
        "src/UriShorten.c",        
    ],
    hdrs = [
        "src/UriCommon.h",
        "src/UriParseBase.h",
        "src/UriMemory.h",
        "src/UriNormalizeBase.h",
        "src/UriIp4Base.h",
        "include/uriparser/UriBase.h",
        "include/uriparser/UriDefsAnsi.h",
        "include/uriparser/UriDefsConfig.h",
        "include/uriparser/UriDefsUnicode.h",
        "include/uriparser/Uri.h",
        "include/uriparser/UriIp4.h",
        ":create_config_h",
    ],
    copts = [
        "-Iexternal/uriparser/src",
        "-Iexternal/uriparser/include",
    ],
    visibility = ["//visibility:public"],
)
"""
