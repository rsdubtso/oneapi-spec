..
  Copyright 2019-2020 Intel Corporation

.. namespace:: 0

.. |f16| replace:: :any:`f16 <dnnl::memory::data_type::f16>`
.. |bf16| replace:: :any:`bf16 <dnnl::memory::data_type::bf16>`
.. |f32| replace:: :any:`f32 <dnnl::memory::data_type::f32>`
.. |s32| replace:: :any:`s32 <dnnl::memory::data_type::s32>`
.. |s8| replace:: :any:`s8 <dnnl::memory::data_type::s8>`
.. |u8| replace:: :any:`u8 <dnnl::memory::data_type::u8>`

.. |_f16| replace:: :any:`dnnl::memory::data_type::f16`
.. |_bf16| replace:: :any:`dnnl::memory::data_type::bf16`
.. |_f32| replace:: :any:`dnnl::memory::data_type::f32`
.. |_s32| replace:: :any:`dnnl::memory::data_type::s32`
.. |_s8| replace:: :any:`dnnl::memory::data_type::s8`
.. |_u8| replace:: :any:`dnnl::memory::data_type::u8`

.. |any| replace:: :any:`any <dnnl::memory::format_tag::any>`
.. |abcdef| replace:: :any:`abcdef <dnnl::memory::format_tag::abcdef>`
.. |abcde| replace:: :any:`abcde <dnnl::memory::format_tag::abcde>`
.. |abcd| replace:: :any:`abcd <dnnl::memory::format_tag::abcd>`
.. |abdc| replace:: :any:`abdc <dnnl::memory::format_tag::abdc>`
.. |abc| replace:: :any:`abc <dnnl::memory::format_tag::abc>`
.. |ab| replace:: :any:`ab <dnnl::memory::format_tag::ab>`
.. |acb| replace:: :any:`acb <dnnl::memory::format_tag::acb>`
.. |acdb| replace:: :any:`acdb <dnnl::memory::format_tag::acdb>`
.. |acdeb| replace:: :any:`acdeb <dnnl::memory::format_tag::acdeb>`
.. |a| replace:: :any:`a <dnnl::memory::format_tag::a>`
.. |bac| replace:: :any:`bac <dnnl::memory::format_tag::bac>`
.. |ba| replace:: :any:`ba <dnnl::memory::format_tag::ba>`
.. |cba| replace:: :any:`cba <dnnl::memory::format_tag::cba>`
.. |cdba| replace:: :any:`cdba <dnnl::memory::format_tag::cdba>`
.. |cdeba| replace:: :any:`cdeba <dnnl::memory::format_tag::cdeba>`
.. |decab| replace:: :any:`decab <dnnl::memory::format_tag::decab>`
.. |dhwio| replace:: :any:`dhwio <dnnl::memory::format_tag::dhwio>`
.. |goidhw| replace:: :any:`goidhw <dnnl::memory::format_tag::goidhw>`
.. |goihw| replace:: :any:`goihw <dnnl::memory::format_tag::goihw>`
.. |goiw| replace:: :any:`goiw <dnnl::memory::format_tag::goiw>`
.. |hwigo| replace:: :any:`hwigo <dnnl::memory::format_tag::hwigo>`
.. |hwio| replace:: :any:`hwio <dnnl::memory::format_tag::hwio>`
.. |io| replace:: :any:`io <dnnl::memory::format_tag::io>`
.. |ldgoi| replace:: :any:`ldgoi <dnnl::memory::format_tag::ldgoi>`
.. |ldgo| replace:: :any:`ldgo <dnnl::memory::format_tag::ldgo>`
.. |ldigo| replace:: :any:`ldigo <dnnl::memory::format_tag::ldigo>`
.. |ldio| replace:: :any:`ldio <dnnl::memory::format_tag::ldio>`
.. |ldoi| replace:: :any:`ldoi <dnnl::memory::format_tag::ldoi>`
.. |ldnc| replace:: :any:`ldnc <dnnl::memory::format_tag::ldnc>`
.. |ncdhw| replace:: :any:`ncdhw <dnnl::memory::format_tag::ncdhw>`
.. |nchw| replace:: :any:`nchw <dnnl::memory::format_tag::nchw>`
.. |ncw| replace:: :any:`ncw <dnnl::memory::format_tag::ncw>`
.. |nc| replace:: :any:`nc <dnnl::memory::format_tag::nc>`
.. |ndhwc| replace:: :any:`ndhwc <dnnl::memory::format_tag::ndhwc>`
.. |nhwc| replace:: :any:`nhwc <dnnl::memory::format_tag::nhwc>`
.. |ntc| replace:: :any:`ntc <dnnl::memory::format_tag::ntc>`
.. |nt| replace:: :any:`nt <dnnl::memory::format_tag::nt>`
.. |nwc| replace:: :any:`nwc <dnnl::memory::format_tag::nwc>`
.. |oidhw| replace:: :any:`oidhw <dnnl::memory::format_tag::oidhw>`
.. |oihw| replace:: :any:`oihw <dnnl::memory::format_tag::oihw>`
.. |oiw| replace:: :any:`oiw <dnnl::memory::format_tag::oiw>`
.. |oi| replace:: :any:`oi <dnnl::memory::format_tag::oi>`
.. |tnc| replace:: :any:`tnc <dnnl::memory::format_tag::tnc>`
.. |tn| replace:: :any:`tn <dnnl::memory::format_tag::tn>`
.. |wio| replace:: :any:`wio <dnnl::memory::format_tag::wio>`
.. |x| replace:: :any:`x <dnnl::memory::format_tag::x>`

.. |_any| replace:: :any:`dnnl::memory::format_tag::any`
.. |_abcdef| replace:: :any:`dnnl::memory::format_tag::abcdef`
.. |_abcde| replace:: :any:`dnnl::memory::format_tag::abcde`
.. |_abcd| replace:: :any:`dnnl::memory::format_tag::abcd`
.. |_abc| replace:: :any:`dnnl::memory::format_tag::abc`
.. |_ab| replace:: :any:`dnnl::memory::format_tag::ab`
.. |_acb| replace:: :any:`dnnl::memory::format_tag::acb`
.. |_acdb| replace:: :any:`dnnl::memory::format_tag::acdb`
.. |_acdeb| replace:: :any:`dnnl::memory::format_tag::acdeb`
.. |_a| replace:: :any:`dnnl::memory::format_tag::a`
.. |_bac| replace:: :any:`dnnl::memory::format_tag::bac`
.. |_ba| replace:: :any:`dnnl::memory::format_tag::ba`
.. |_cba| replace:: :any:`dnnl::memory::format_tag::cba`
.. |_cdba| replace:: :any:`dnnl::memory::format_tag::cdba`
.. |_cdeba| replace:: :any:`dnnl::memory::format_tag::cdeba`
.. |_decab| replace:: :any:`dnnl::memory::format_tag::decab`
.. |_dhwio| replace:: :any:`dnnl::memory::format_tag::dhwio`
.. |_goidhw| replace:: :any:`dnnl::memory::format_tag::goidhw`
.. |_goihw| replace:: :any:`dnnl::memory::format_tag::goihw`
.. |_goiw| replace:: :any:`dnnl::memory::format_tag::goiw`
.. |_hwigo| replace:: :any:`dnnl::memory::format_tag::hwigo`
.. |_hwio| replace:: :any:`dnnl::memory::format_tag::hwio`
.. |_io| replace:: :any:`dnnl::memory::format_tag::io`
.. |_ldgoi| replace:: :any:`dnnl::memory::format_tag::ldgoi`
.. |_ldgo| replace:: :any:`dnnl::memory::format_tag::ldgo`
.. |_ldigo| replace:: :any:`dnnl::memory::format_tag::ldigo`
.. |_ldio| replace:: :any:`dnnl::memory::format_tag::ldio`
.. |_ldnc| replace:: :any:`dnnl::memory::format_tag::ldnc`
.. |_ncdhw| replace:: :any:`dnnl::memory::format_tag::ncdhw`
.. |_nchw| replace:: :any:`dnnl::memory::format_tag::nchw`
.. |_ncw| replace:: :any:`dnnl::memory::format_tag::ncw`
.. |_nc| replace:: :any:`dnnl::memory::format_tag::nc`
.. |_ndhwc| replace:: :any:`dnnl::memory::format_tag::ndhwc`
.. |_nhwc| replace:: :any:`dnnl::memory::format_tag::nhwc`
.. |_ntc| replace:: :any:`dnnl::memory::format_tag::ntc`
.. |_nt| replace:: :any:`dnnl::memory::format_tag::nt`
.. |_nwc| replace:: :any:`dnnl::memory::format_tag::nwc`
.. |_oidhw| replace:: :any:`dnnl::memory::format_tag::oidhw`
.. |_oihw| replace:: :any:`dnnl::memory::format_tag::oihw`
.. |_oiw| replace:: :any:`dnnl::memory::format_tag::oiw`
.. |_oi| replace:: :any:`dnnl::memory::format_tag::oi`
.. |_tnc| replace:: :any:`dnnl::memory::format_tag::tnc`
.. |_tn| replace:: :any:`dnnl::memory::format_tag::tn`
.. |_wio| replace:: :any:`dnnl::memory::format_tag::wio`
.. |_x| replace:: :any:`dnnl::memory::format_tag::x`

.. |normalization_flags_none| replace:: :any:`none <dnnl::normalization_flags::none>`
.. |use_global_stats| replace:: :any:`use_global_stats <dnnl::normalization_flags::use_global_stats>`
.. |use_scaleshift| replace:: :any:`use_scaleshift <dnnl::normalization_flags::use_scaleshift>`
.. |fuse_norm_relu| replace:: :any:`fuse_norm_relu <dnnl::normalization_flags::fuse_norm_relu>`

.. |_normalization_flags_none| replace:: :any:`dnnl::normalization_flags::none`
.. |_use_global_stats| replace:: :any:`dnnl::normalization_flags::use_global_stats`
.. |_use_scaleshift| replace:: :any:`dnnl::normalization_flags::use_scaleshift`
.. |_fuse_norm_relu| replace:: :any:`dnnl::normalization_flags::fuse_norm_relu`

.. |backward| replace:: :any:`backward <dnnl::prop_kind::backward>`
.. |backward_data| replace:: :any:`backward_data <dnnl::prop_kind::backward_data>`
.. |forward_inference| replace:: :any:`forward_inference <dnnl::prop_kind::forward_inference>`
.. |forward_training| replace:: :any:`forward_training <dnnl::prop_kind::forward_training>`

.. |_backward| replace:: :any:`dnnl::prop_kind::backward`
.. |_backward_data| replace:: :any:`dnnl::prop_kind::backward_data`
.. |_forward_inference| replace:: :any:`dnnl::prop_kind::forward_inference`
.. |_forward_training| replace:: :any:`dnnl::prop_kind::forward_training`

.. |binary_add| replace:: :any:`binary_add <dnnl::algorithm::binary_add>`
.. |binary_max| replace:: :any:`binary_max <dnnl::algorithm::binary_max>`
.. |binary_min| replace:: :any:`binary_min <dnnl::algorithm::binary_min>`
.. |binary_mul| replace:: :any:`binary_mul <dnnl::algorithm::binary_mul>`
.. |convolution_auto| replace:: :any:`convolution_auto <dnnl::algorithm::convolution_auto>`
.. |convolution_direct| replace:: :any:`convolution_direct <dnnl::algorithm::convolution_direct>`
.. |convolution_winograd| replace:: :any:`convolution_winograd <dnnl::algorithm::convolution_winograd>`
.. |deconvolution_direct| replace:: :any:`deconvolution_direct <dnnl::algorithm::deconvolution_direct>`
.. |deconvolution_winograd| replace:: :any:`deconvolution_winograd <dnnl::algorithm::deconvolution_winograd>`
.. |eltwise_abs| replace:: :any:`eltwise_abs <dnnl::algorithm::eltwise_abs>`
.. |eltwise_bounded_relu| replace:: :any:`eltwise_bounded_relu <dnnl::algorithm::eltwise_bounded_relu>`
.. |eltwise_clip| replace:: :any:`eltwise_clip <dnnl::algorithm::eltwise_clip>`
.. |eltwise_elu_use_dst_for_bwd| replace:: :any:`eltwise_elu_use_dst_for_bwd <dnnl::algorithm::eltwise_elu_use_dst_for_bwd>`
.. |eltwise_elu| replace:: :any:`eltwise_elu <dnnl::algorithm::eltwise_elu>`
.. |eltwise_exp_use_dst_for_bwd| replace:: :any:`eltwise_exp_use_dst_for_bwd <dnnl::algorithm::eltwise_exp_use_dst_for_bwd>`
.. |eltwise_exp| replace:: :any:`eltwise_exp <dnnl::algorithm::eltwise_exp>`
.. |eltwise_gelu_erf| replace:: :any:`eltwise_gelu_erf <dnnl::algorithm::eltwise_gelu_erf>`
.. |eltwise_gelu_tanh| replace:: :any:`eltwise_gelu_tanh <dnnl::algorithm::eltwise_gelu_tanh>`
.. |eltwise_gelu| replace:: :any:`eltwise_gelu <dnnl::algorithm::eltwise_gelu>`
.. |eltwise_linear| replace:: :any:`eltwise_linear <dnnl::algorithm::eltwise_linear>`
.. |eltwise_logistic_use_dst_for_bwd| replace:: :any:`eltwise_logistic_use_dst_for_bwd <dnnl::algorithm::eltwise_logistic_use_dst_for_bwd>`
.. |eltwise_logistic| replace:: :any:`eltwise_logistic <dnnl::algorithm::eltwise_logistic>`
.. |eltwise_log| replace:: :any:`eltwise_log <dnnl::algorithm::eltwise_log>`
.. |eltwise_pow| replace:: :any:`eltwise_pow <dnnl::algorithm::eltwise_pow>`
.. |eltwise_relu_use_dst_for_bwd| replace:: :any:`eltwise_relu_use_dst_for_bwd <dnnl::algorithm::eltwise_relu_use_dst_for_bwd>`
.. |eltwise_relu| replace:: :any:`eltwise_relu <dnnl::algorithm::eltwise_relu>`
.. |eltwise_soft_relu| replace:: :any:`eltwise_soft_relu <dnnl::algorithm::eltwise_soft_relu>`
.. |eltwise_sqrt_use_dst_for_bwd| replace:: :any:`eltwise_sqrt_use_dst_for_bwd <dnnl::algorithm::eltwise_sqrt_use_dst_for_bwd>`
.. |eltwise_sqrt| replace:: :any:`eltwise_sqrt <dnnl::algorithm::eltwise_sqrt>`
.. |eltwise_square| replace:: :any:`eltwise_square <dnnl::algorithm::eltwise_square>`
.. |eltwise_swish| replace:: :any:`eltwise_swish <dnnl::algorithm::eltwise_swish>`
.. |eltwise_tanh_use_dst_for_bwd| replace:: :any:`eltwise_tanh_use_dst_for_bwd <dnnl::algorithm::eltwise_tanh_use_dst_for_bwd>`
.. |eltwise_tanh| replace:: :any:`eltwise_tanh <dnnl::algorithm::eltwise_tanh>`
.. |lbr_gru| replace:: :any:`lbr_gru <dnnl::algorithm::lbr_gru>`
.. |lrn_across_channels| replace:: :any:`lrn_across_channels <dnnl::algorithm::lrn_across_channels>`
.. |lrn_within_channel| replace:: :any:`lrn_within_channel <dnnl::algorithm::lrn_within_channel>`
.. |pooling_avg_exclude_padding| replace:: :any:`pooling_avg_exclude_padding <dnnl::algorithm::pooling_avg_exclude_padding>`
.. |pooling_avg_include_padding| replace:: :any:`pooling_avg_include_padding <dnnl::algorithm::pooling_avg_include_padding>`
.. |pooling_avg| replace:: :any:`pooling_avg <dnnl::algorithm::pooling_avg>`
.. |pooling_max| replace:: :any:`pooling_max <dnnl::algorithm::pooling_max>`
.. |resampling_linear| replace:: :any:`resampling_linear <dnnl::algorithm::resampling_linear>`
.. |resampling_nearest| replace:: :any:`resampling_nearest <dnnl::algorithm::resampling_nearest>`
.. |undef| replace:: :any:`undef <dnnl::algorithm::undef>`
.. |vanilla_gru| replace:: :any:`vanilla_gru <dnnl::algorithm::vanilla_gru>`
.. |vanilla_lstm| replace:: :any:`vanilla_lstm <dnnl::algorithm::vanilla_lstm>`
.. |vanilla_rnn| replace:: :any:`vanilla_rnn <dnnl::algorithm::vanilla_rnn>`

.. |_binary_add| replace:: :any:`dnnl::algorithm::binary_add`
.. |_binary_max| replace:: :any:`dnnl::algorithm::binary_max`
.. |_binary_min| replace:: :any:`dnnl::algorithm::binary_min`
.. |_binary_mul| replace:: :any:`dnnl::algorithm::binary_mul`
.. |_convolution_auto| replace:: :any:`dnnl::algorithm::convolution_auto`
.. |_convolution_direct| replace:: :any:`dnnl::algorithm::convolution_direct`
.. |_convolution_winograd| replace:: :any:`dnnl::algorithm::convolution_winograd`
.. |_deconvolution_direct| replace:: :any:`dnnl::algorithm::deconvolution_direct`
.. |_deconvolution_winograd| replace:: :any:`dnnl::algorithm::deconvolution_winograd`
.. |_eltwise_abs| replace:: :any:`dnnl::algorithm::eltwise_abs`
.. |_eltwise_bounded_relu| replace:: :any:`dnnl::algorithm::eltwise_bounded_relu`
.. |_eltwise_clip| replace:: :any:`dnnl::algorithm::eltwise_clip`
.. |_eltwise_elu_use_dst_for_bwd| replace:: :any:`dnnl::algorithm::eltwise_elu_use_dst_for_bwd`
.. |_eltwise_elu| replace:: :any:`dnnl::algorithm::eltwise_elu`
.. |_eltwise_exp_use_dst_for_bwd| replace:: :any:`dnnl::algorithm::eltwise_exp_use_dst_for_bwd`
.. |_eltwise_exp| replace:: :any:`dnnl::algorithm::eltwise_exp`
.. |_eltwise_gelu_erf| replace:: :any:`dnnl::algorithm::eltwise_gelu_erf`
.. |_eltwise_gelu_tanh| replace:: :any:`dnnl::algorithm::eltwise_gelu_tanh`
.. |_eltwise_gelu| replace:: :any:`dnnl::algorithm::eltwise_gelu`
.. |_eltwise_linear| replace:: :any:`dnnl::algorithm::eltwise_linear`
.. |_eltwise_logistic_use_dst_for_bwd| replace:: :any:`dnnl::algorithm::eltwise_logistic_use_dst_for_bwd`
.. |_eltwise_logistic| replace:: :any:`dnnl::algorithm::eltwise_logistic`
.. |_eltwise_log| replace:: :any:`dnnl::algorithm::eltwise_log`
.. |_eltwise_pow| replace:: :any:`dnnl::algorithm::eltwise_pow`
.. |_eltwise_relu_use_dst_for_bwd| replace:: :any:`dnnl::algorithm::eltwise_relu_use_dst_for_bwd`
.. |_eltwise_relu| replace:: :any:`dnnl::algorithm::eltwise_relu`
.. |_eltwise_soft_relu| replace:: :any:`dnnl::algorithm::eltwise_soft_relu`
.. |_eltwise_sqrt_use_dst_for_bwd| replace:: :any:`dnnl::algorithm::eltwise_sqrt_use_dst_for_bwd`
.. |_eltwise_sqrt| replace:: :any:`dnnl::algorithm::eltwise_sqrt`
.. |_eltwise_square| replace:: :any:`dnnl::algorithm::eltwise_square`
.. |_eltwise_swish| replace:: :any:`dnnl::algorithm::eltwise_swish`
.. |_eltwise_tanh_use_dst_for_bwd| replace:: :any:`dnnl::algorithm::eltwise_tanh_use_dst_for_bwd`
.. |_eltwise_tanh| replace:: :any:`dnnl::algorithm::eltwise_tanh`
.. |_lbr_gru| replace:: :any:`dnnl::algorithm::lbr_gru`
.. |_lrn_across_channels| replace:: :any:`dnnl::algorithm::lrn_across_channels`
.. |_lrn_within_channel| replace:: :any:`dnnl::algorithm::lrn_within_channel`
.. |_pooling_avg_exclude_padding| replace:: :any:`dnnl::algorithm::pooling_avg_exclude_padding`
.. |_pooling_avg_include_padding| replace:: :any:`dnnl::algorithm::pooling_avg_include_padding`
.. |_pooling_avg| replace:: :any:`dnnl::algorithm::pooling_avg`
.. |_pooling_max| replace:: :any:`dnnl::algorithm::pooling_max`
.. |_resampling_linear| replace:: :any:`dnnl::algorithm::resampling_linear`
.. |_resampling_nearest| replace:: :any:`dnnl::algorithm::resampling_nearest`
.. |_undef| replace:: :any:`dnnl::algorithm::undef`
.. |_vanilla_gru| replace:: :any:`dnnl::algorithm::vanilla_gru`
.. |_vanilla_lstm| replace:: :any:`dnnl::algorithm::vanilla_lstm`
.. |_vanilla_rnn| replace:: :any:`dnnl::algorithm::vanilla_rnn`

.. |DNNL_ARG_SRC_0| replace:: :c:macro:`DNNL_ARG_SRC_0`
.. |DNNL_ARG_SRC| replace:: :c:macro:`DNNL_ARG_SRC`
.. |DNNL_ARG_SRC_LAYER| replace:: :c:macro:`DNNL_ARG_SRC_LAYER`
.. |DNNL_ARG_FROM| replace:: :c:macro:`DNNL_ARG_FROM`
.. |DNNL_ARG_SRC_1| replace:: :c:macro:`DNNL_ARG_SRC_1`
.. |DNNL_ARG_SRC_ITER| replace:: :c:macro:`DNNL_ARG_SRC_ITER`
.. |DNNL_ARG_SRC_2| replace:: :c:macro:`DNNL_ARG_SRC_2`
.. |DNNL_ARG_SRC_ITER_C| replace:: :c:macro:`DNNL_ARG_SRC_ITER_C`
.. |DNNL_ARG_DST_0| replace:: :c:macro:`DNNL_ARG_DST_0`
.. |DNNL_ARG_DST| replace:: :c:macro:`DNNL_ARG_DST`
.. |DNNL_ARG_TO| replace:: :c:macro:`DNNL_ARG_TO`
.. |DNNL_ARG_DST_LAYER| replace:: :c:macro:`DNNL_ARG_DST_LAYER`
.. |DNNL_ARG_DST_1| replace:: :c:macro:`DNNL_ARG_DST_1`
.. |DNNL_ARG_DST_ITER| replace:: :c:macro:`DNNL_ARG_DST_ITER`
.. |DNNL_ARG_DST_2| replace:: :c:macro:`DNNL_ARG_DST_2`
.. |DNNL_ARG_DST_ITER_C| replace:: :c:macro:`DNNL_ARG_DST_ITER_C`
.. |DNNL_ARG_WEIGHTS_0| replace:: :c:macro:`DNNL_ARG_WEIGHTS_0`
.. |DNNL_ARG_WEIGHTS| replace:: :c:macro:`DNNL_ARG_WEIGHTS`
.. |DNNL_ARG_SCALE_SHIFT| replace:: :c:macro:`DNNL_ARG_SCALE_SHIFT`
.. |DNNL_ARG_WEIGHTS_LAYER| replace:: :c:macro:`DNNL_ARG_WEIGHTS_LAYER`
.. |DNNL_ARG_WEIGHTS_1| replace:: :c:macro:`DNNL_ARG_WEIGHTS_1`
.. |DNNL_ARG_WEIGHTS_ITER| replace:: :c:macro:`DNNL_ARG_WEIGHTS_ITER`
.. |DNNL_ARG_WEIGHTS_PEEPHOLE| replace:: :c:macro:`DNNL_ARG_WEIGHTS_PEEPHOLE`
.. |DNNL_ARG_WEIGHTS_PROJECTION| replace:: :c:macro:`DNNL_ARG_WEIGHTS_PROJECTION`
.. |DNNL_ARG_BIAS| replace:: :c:macro:`DNNL_ARG_BIAS`
.. |DNNL_ARG_MEAN| replace:: :c:macro:`DNNL_ARG_MEAN`
.. |DNNL_ARG_VARIANCE| replace:: :c:macro:`DNNL_ARG_VARIANCE`
.. |DNNL_ARG_WORKSPACE| replace:: :c:macro:`DNNL_ARG_WORKSPACE`
.. |DNNL_ARG_SCRATCHPAD| replace:: :c:macro:`DNNL_ARG_SCRATCHPAD`
.. |DNNL_ARG_DIFF_SRC_0| replace:: :c:macro:`DNNL_ARG_DIFF_SRC_0`
.. |DNNL_ARG_DIFF_SRC| replace:: :c:macro:`DNNL_ARG_DIFF_SRC`
.. |DNNL_ARG_DIFF_SRC_LAYER| replace:: :c:macro:`DNNL_ARG_DIFF_SRC_LAYER`
.. |DNNL_ARG_DIFF_SRC_1| replace:: :c:macro:`DNNL_ARG_DIFF_SRC_1`
.. |DNNL_ARG_DIFF_SRC_ITER| replace:: :c:macro:`DNNL_ARG_DIFF_SRC_ITER`
.. |DNNL_ARG_DIFF_SRC_2| replace:: :c:macro:`DNNL_ARG_DIFF_SRC_2`
.. |DNNL_ARG_DIFF_SRC_ITER_C| replace:: :c:macro:`DNNL_ARG_DIFF_SRC_ITER_C`
.. |DNNL_ARG_DIFF_DST_0| replace:: :c:macro:`DNNL_ARG_DIFF_DST_0`
.. |DNNL_ARG_DIFF_DST| replace:: :c:macro:`DNNL_ARG_DIFF_DST`
.. |DNNL_ARG_DIFF_DST_LAYER| replace:: :c:macro:`DNNL_ARG_DIFF_DST_LAYER`
.. |DNNL_ARG_DIFF_DST_1| replace:: :c:macro:`DNNL_ARG_DIFF_DST_1`
.. |DNNL_ARG_DIFF_DST_ITER| replace:: :c:macro:`DNNL_ARG_DIFF_DST_ITER`
.. |DNNL_ARG_DIFF_DST_2| replace:: :c:macro:`DNNL_ARG_DIFF_DST_2`
.. |DNNL_ARG_DIFF_DST_ITER_C| replace:: :c:macro:`DNNL_ARG_DIFF_DST_ITER_C`
.. |DNNL_ARG_DIFF_WEIGHTS_0| replace:: :c:macro:`DNNL_ARG_DIFF_WEIGHTS_0`
.. |DNNL_ARG_DIFF_WEIGHTS| replace:: :c:macro:`DNNL_ARG_DIFF_WEIGHTS`
.. |DNNL_ARG_DIFF_SCALE_SHIFT| replace:: :c:macro:`DNNL_ARG_DIFF_SCALE_SHIFT`
.. |DNNL_ARG_DIFF_WEIGHTS_LAYER| replace:: :c:macro:`DNNL_ARG_DIFF_WEIGHTS_LAYER`
.. |DNNL_ARG_DIFF_WEIGHTS_1| replace:: :c:macro:`DNNL_ARG_DIFF_WEIGHTS_1`
.. |DNNL_ARG_DIFF_WEIGHTS_ITER| replace:: :c:macro:`DNNL_ARG_DIFF_WEIGHTS_ITER`
.. |DNNL_ARG_DIFF_WEIGHTS_PEEPHOLE| replace:: :c:macro:`DNNL_ARG_DIFF_WEIGHTS_PEEPHOLE`
.. |DNNL_ARG_DIFF_WEIGHTS_PROJECTION| replace:: :c:macro:`DNNL_ARG_DIFF_WEIGHTS_PROJECTION`
.. |DNNL_ARG_DIFF_BIAS| replace:: :c:macro:`DNNL_ARG_DIFF_BIAS`
.. |DNNL_ARG_ATTR_OUTPUT_SCALES| replace:: :c:macro:`DNNL_ARG_ATTR_OUTPUT_SCALES`
.. |DNNL_ARG_MULTIPLE_SRC| replace:: :c:macro:`DNNL_ARG_MULTIPLE_SRC`
.. |DNNL_ARG_MULTIPLE_DST| replace:: :c:macro:`DNNL_ARG_MULTIPLE_DST`
.. |DNNL_ARG_ATTR_ZERO_POINTS| replace:: :c:macro:`DNNL_ARG_ATTR_ZERO_POINTS`
.. |DNNL_RUNTIME_DIM_VAL| replace:: :c:macro:`DNNL_RUNTIME_DIM_VAL`
.. |DNNL_RUNTIME_SIZE_VAL| replace:: :c:macro:`DNNL_RUNTIME_SIZE_VAL`
.. |DNNL_RUNTIME_F32_VAL| replace:: :c:macro:`DNNL_RUNTIME_F32_VAL`
.. |DNNL_RUNTIME_S32_VAL| replace:: :c:macro:`DNNL_RUNTIME_S32_VAL`

