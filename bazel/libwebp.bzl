libwebp_build_rule = """
cc_library(
    name = "libwebp",
    deps = [":libwebp_dec", ":libwebp_enc"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "libwebp_dec",
    srcs = [
        "src/dec/alpha_dec.c",
        "src/dec/buffer_dec.c",
        "src/dec/frame_dec.c",
        "src/dec/idec_dec.c",
        "src/dec/io_dec.c",
        "src/dec/quant_dec.c",
        "src/dec/tree_dec.c",
        "src/dec/vp8_dec.c",
        "src/dec/vp8l_dec.c",
        "src/dec/webp_dec.c",
    ],
    hdrs = [
        "src/dec/alphai_dec.h",
        "src/dec/common_dec.h",
        "src/dec/vp8_dec.h",
        "src/dec/vp8i_dec.h",
        "src/dec/vp8li_dec.h",
        "src/dec/webpi_dec.h",
    ],
    deps = [
        'libwebp_dsp',
        'libwebp_dsp_neon',
        'libwebp_mux',
        'libwebp_utils',  
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "libwebp_dsp",
    visibility = ["//visibility:public"],
    srcs =[
        'src/dsp/alpha_processing.c',
        'src/dsp/alpha_processing_mips_dsp_r2.c',
        'src/dsp/alpha_processing_sse2.c',
        'src/dsp/alpha_processing_sse41.c',
        'src/dsp/cost.c',
        'src/dsp/cost_mips32.c',
        'src/dsp/cost_mips_dsp_r2.c',
        'src/dsp/cost_sse2.c',
        'src/dsp/cpu.c',
        'src/dsp/dec.c',
        'src/dsp/dec_clip_tables.c',
        'src/dsp/dec_mips32.c',
        'src/dsp/dec_mips_dsp_r2.c',
        'src/dsp/dec_msa.c',
        'src/dsp/dec_sse2.c',
        'src/dsp/dec_sse41.c',
        'src/dsp/enc.c',
        'src/dsp/enc_avx2.c',
        'src/dsp/enc_mips32.c',
        'src/dsp/enc_mips_dsp_r2.c',
        'src/dsp/enc_msa.c',
        'src/dsp/enc_sse2.c',
        'src/dsp/enc_sse41.c',
        'src/dsp/filters.c',
        'src/dsp/filters_mips_dsp_r2.c',
        'src/dsp/filters_msa.c',
        'src/dsp/filters_sse2.c',
        'src/dsp/lossless.c',
        'src/dsp/lossless_enc.c',
        'src/dsp/lossless_enc_mips32.c',
        'src/dsp/lossless_enc_mips_dsp_r2.c',
        'src/dsp/lossless_enc_msa.c',
        'src/dsp/lossless_enc_sse2.c',
        'src/dsp/lossless_enc_sse41.c',
        'src/dsp/lossless_mips_dsp_r2.c',
        'src/dsp/lossless_msa.c',
        'src/dsp/lossless_sse2.c',
        'src/dsp/rescaler.c',
        'src/dsp/rescaler_mips32.c',
        'src/dsp/rescaler_mips_dsp_r2.c',
        'src/dsp/rescaler_msa.c',
        'src/dsp/rescaler_sse2.c',
        'src/dsp/ssim.c',
        'src/dsp/ssim_sse2.c',
        'src/dsp/upsampling.c',
        'src/dsp/upsampling_mips_dsp_r2.c',
        'src/dsp/upsampling_msa.c',
        'src/dsp/upsampling_sse2.c',
        'src/dsp/yuv.c',
        'src/dsp/yuv_mips32.c',
        'src/dsp/yuv_mips_dsp_r2.c',
        'src/dsp/yuv_sse2.c',        
    ],
    hdrs = [
        'src/dsp/dsp.h',  
        'src/webp/types.h',
        'src/enc/vp8i_enc.h',
        'src/enc/cost_enc.h',
        'src/dec/common_dec.h',
        'src/dsp/common_sse2.h',
        'src/utils/bit_writer_utils.h',
        'src/utils/thread_utils.h',
        'src/utils/utils.h',
        'src/webp/encode.h',
        'src/dsp/lossless.h',
        'src/utils/rescaler_utils.h',
        'src/dsp/yuv.h',
        'src/dec/vp8i_dec.h',
        'src/dec/vp8li_dec.h',
        'src/dec/vp8_dec.h',
        'src/dec/webpi_dec.h',
        'src/webp/decode.h',
        'src/enc/histogram_enc.h',
        'src/utils/bit_reader_utils.h',
        'src/utils/color_cache_utils.h',
        'src/utils/huffman_utils.h',
        'src/webp/format_constants.h',
        'src/utils/random_utils.h',
        'src/utils/endian_inl_utils.h',
        'src/enc/backward_references_enc.h',
        'src/dsp/lossless_common.h',
    ]
)

cc_library(
    name = "libwebp_dsp_neon",
    srcs = [
        'src/dsp/alpha_processing_neon.c',
        'src/dsp/dec_neon.c',
        'src/dsp/enc_neon.c',
        'src/dsp/filters_neon.c',
        'src/dsp/lossless_enc_neon.c',
        'src/dsp/lossless_neon.c',
        'src/dsp/rescaler_neon.c',
        'src/dsp/upsampling_neon.c',
        'src/dsp/yuv_neon.c',        
    ],
    hdrs = [
        'src/dsp/dsp.h',
        'src/webp/types.h',
        'src/dsp/yuv.h',
        'src/dec/vp8_dec.h',
        'src/webp/decode.h',
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "libwebp_mux",
    srcs = [
        'src/mux/anim_encode.c',
        'src/mux/muxedit.c',
        'src/mux/muxinternal.c',
        'src/mux/muxread.c',        
    ],
    hdrs = [
        'src/mux/muxi.h',
        'src/dec/vp8i_dec.h',
        'src/dec/vp8li_dec.h',
        'src/dec/common_dec.h',
        'src/dec/webpi_dec.h',
        'src/utils/rescaler_utils.h',
        'src/webp/types.h',
        'src/dec/vp8_dec.h',
        'src/webp/decode.h',
        'src/utils/bit_reader_utils.h',
        'src/utils/color_cache_utils.h',
        'src/mux/animi.h',
        'src/utils/huffman_utils.h',
        'src/webp/format_constants.h',
        'src/utils/random_utils.h',
        'src/utils/thread_utils.h',
        'src/dsp/dsp.h',
        'src/webp/mux.h',
        'src/webp/mux_types.h',
        'src/utils/utils.h',
        'src/webp/encode.h',
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "libwebp_utils",
    visibility = ["//visibility:public"],
    srcs = [
        'src/utils/bit_reader_utils.c',
        'src/utils/bit_writer_utils.c',
        'src/utils/color_cache_utils.c',
        'src/utils/filters_utils.c',
        'src/utils/huffman_encode_utils.c',
        'src/utils/huffman_utils.c',
        'src/utils/quant_levels_dec_utils.c',
        'src/utils/quant_levels_utils.c',
        'src/utils/random_utils.c',
        'src/utils/rescaler_utils.c',
        'src/utils/thread_utils.c',
        'src/utils/utils.c',
    ],
    hdrs = [
        'src/utils/quant_levels_dec_utils.h',
        'src/utils/quant_levels_utils.h',
        'src/utils/thread_utils.h',
        'src/utils/random_utils.h',
        'src/utils/color_cache_utils.h',
        'src/dsp/dsp.h',
        'src/utils/utils.h',
        'src/webp/types.h',
        'src/webp/decode.h',
        'src/webp/encode.h',
        'src/utils/huffman_utils.h',
        'src/utils/filters_utils.h',
        'src/utils/huffman_encode_utils.h',
        'src/utils/bit_writer_utils.h',
        'src/webp/format_constants.h',
        'src/utils/bit_reader_inl_utils.h',
        'src/utils/bit_reader_utils.h',
        'src/utils/endian_inl_utils.h',
        'src/utils/rescaler_utils.h',
    ],
)

cc_library(
    name = "libwebp_enc",
    visibility = ["//visibility:public"],
    srcs = [
        'src/enc/alpha_enc.c',
        'src/enc/analysis_enc.c',
        'src/enc/backward_references_cost_enc.c',
        'src/enc/backward_references_enc.c',
        'src/enc/config_enc.c',
        'src/enc/cost_enc.c',
        'src/enc/delta_palettization_enc.c',
        'src/enc/filter_enc.c',
        'src/enc/frame_enc.c',
        'src/enc/histogram_enc.c',
        'src/enc/iterator_enc.c',
        'src/enc/near_lossless_enc.c',
        'src/enc/picture_csp_enc.c',
        'src/enc/picture_enc.c',
        'src/enc/picture_psnr_enc.c',
        'src/enc/picture_rescale_enc.c',
        'src/enc/picture_tools_enc.c',
        'src/enc/predictor_enc.c',
        'src/enc/quant_enc.c',
        'src/enc/syntax_enc.c',
        'src/enc/token_enc.c',
        'src/enc/tree_enc.c',
        'src/enc/vp8l_enc.c',
        'src/enc/webp_enc.c',        
    ],
    hdrs = [
        'src/enc/vp8i_enc.h',
        'src/enc/vp8li_enc.h',
        'src/webp/encode.h',
        'src/dsp/lossless_common.h',
        'src/enc/backward_references_enc.h',
        'src/dsp/lossless.h',
        'src/enc/histogram_enc.h',
        'src/dsp/yuv.h',
        'src/enc/cost_enc.h',
        'src/enc/delta_palettization_enc.h',
    ],
    deps = [
        'libwebp_dsp',
        'libwebp_dsp_neon',
        'libwebp_mux',
        'libwebp_utils',  
    ],
)

"""