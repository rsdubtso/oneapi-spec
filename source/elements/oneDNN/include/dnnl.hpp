/*******************************************************************************
* Copyright 2016-2020 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/// @file
/// oneDNN API

#ifndef DNNL_HPP
#define DNNL_HPP

/// @cond DO_NOT_DOCUMENT_THIS
#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <vector>
#include <unordered_map>

#include <CL/sycl.hpp>
/// @endcond

#include "dnnl.h"

/// @addtogroup dnnl_api oneDNN API
/// @{

/// oneDNN namespace
namespace dnnl {

/// @addtogroup dnnl_api_utils Utilities
/// Utility types and definitions.
/// @{

/// The exception class.
struct error : public std::exception {};

/// @} dnnl_api_utils

struct stream;
struct memory;
struct primitive_desc_base;

/// @addtogroup dnnl_api_primitives Primitives
/// Compute primitives
/// @{

/// @addtogroup dnnl_api_primitives_common Common
/// Common operations to create, destroy and inspect primitives
/// @{

/// Base class for all computational primitives.
struct primitive {
    /// Kinds of primitives supported by the library.
    enum class kind {
        /// Undefined primitive
        undef,
        /// A reorder primitive.
        reorder,
        /// A shuffle primitive.
        shuffle,
        /// A (out-of-place) tensor concatenation primitive.
        concat,
        /// A summation primitive.
        sum,
        /// A convolution primitive.
        convolution,
        /// A deconvolution primitive.
        deconvolution,
        /// An element-wise primitive.
        eltwise,
        /// A softmax primitive.
        softmax,
        /// A pooling primitive.
        pooling,
        /// An LRN primitive.
        lrn,
        /// A batch normalization primitive.
        batch_normalization,
        /// A layer normalization primitive.
        layer_normalization,
        /// An inner product primitive.
        inner_product,
        /// An RNN primitive.
        rnn,
        /// A binary primitive.
        binary,
        /// A logsoftmax primitive.
        logsoftmax,
        /// A matmul (matrix multiplication) primitive.
        matmul,
        /// A resampling primitive.
        resampling,
    };

    /// Default constructor. Constructs an empty object.
    primitive();

    /// Constructs a primitive from a primitive descriptor.
    ///
    /// @param pd Primitive descriptor.
    primitive(const primitive_desc_base &pd);

    /// Returns the kind of the primitive.
    ///
    /// @returns The primitive kind.
    inline kind get_kind() const;

    /// Executes computations specified by the primitive in a specified stream.
    ///
    /// Arguments are passed via an arguments map containing <index, memory
    /// object> pairs. The index must be one of the `DNNL_ARG_*` values such
    /// as `DNNL_ARG_SRC`, and the memory must have a memory descriptor
    /// matching the one returned by
    /// #dnnl::primitive_desc_base::query_md(#query::exec_arg_md, index)
    /// unless using dynamic shapes (see #DNNL_RUNTIME_DIM_VAL).
    ///
    /// @param stream Stream object. The stream must belong to the same engine
    ///     as the primitive.
    /// @param args Arguments map.
    void execute(const stream &stream,
            const std::unordered_map<int, memory> &args) const;

    /// Executes computations specified by the primitive in a specified stream.
    ///
    /// Arguments are passed via an arguments map containing <index, memory
    /// object> pairs. The index must be one of the `DNNL_ARG_*` values such
    /// as `DNNL_ARG_SRC`, and the memory must have a memory descriptor
    /// matching the one returned by
    /// #dnnl::primitive_desc::query_md(#query::exec_arg_md, index) unless
    /// using dynamic shapes (see #DNNL_RUNTIME_DIM_VAL).
    ///
    /// @param stream Stream object. The stream must belong to the same engine
    ///     as the primitive.
    /// @param args Arguments map.
    /// @param deps Optional vector with `cl::sycl::event` dependencies.
    ///
    cl::sycl::event execute_sycl(const stream &stream,
            const std::unordered_map<int, memory> &args,
            const std::vector<cl::sycl::event> &deps = {}) const;

    /// Assignment operator.
    primitive &operator=(const primitive &rhs);
};

/// @} dnnl_api_primitives_common

/// @addtogroup dnnl_api_attributes
///
/// A container for parameters that extend primitives behavior.
///
/// Attributes can also contain Post-ops, which are computations executed
/// after the primitive.
///
/// @{

/// Scratchpad mode.
enum class scratchpad_mode {
    /// The library manages the scratchpad allocation. There may be multiple
    /// implementation-specific policies that can be configured via mechanisms
    /// that fall outside of the scope of this specification.
    library,
    /// The user manages the scratchpad allocation by querying and providing
    /// the scratchpad memory to primitives. This mode is thread-safe as long
    /// as the scratchpad buffers are not used concurrently by two primitive
    /// executions.
    user,
};

/// Propagation kind.
enum class prop_kind {
    /// Undefined propagation kind.
    undef,
    /// Forward data propagation (training mode). In this mode, primitives
    /// perform computations necessary for subsequent backward propagation.
    forward_training,
    /// Forward data propagation (inference mode). In this mode, primitives
    /// perform only computations that are necessary for inference and omit
    /// computations that are necessary only for backward propagation.
    forward_inference,
    /// Forward data propagation,
    /// alias for #dnnl::prop_kind::forward_inference.
    forward_scoring,
    /// Forward data propagation,
    /// alias for #dnnl::prop_kind::forward_training.
    forward,
    /// Backward propagation (with respect to all parameters).
    backward,
    /// Backward data propagation.
    backward_data,
    /// Backward weights propagation.
    backward_weights,
    /// Backward bias propagation.
    backward_bias
};

/// Kinds of algorithms.
enum class algorithm {
    /// Undefined algorithm
    undef,
    /// Convolution algorithm that is chosen to be either direct or Winograd
    /// automatically
    convolution_auto,
    /// Direct convolution
    convolution_direct,
    /// Winograd convolution
    convolution_winograd,
    /// Direct deconvolution
    deconvolution_direct,
    /// Winograd deconvolution
    deconvolution_winograd,
    /// Elementwise: rectified linear unit (ReLU)
    eltwise_relu,
    /// Elementwise: hyperbolic tangent non-linearity (tanh)
    eltwise_tanh,
    /// Elementwise: exponential linear unit (ELU)
    eltwise_elu,
    /// Elementwise: square
    eltwise_square,
    /// Elementwise: abs
    eltwise_abs,
    /// Elementwise: square root
    eltwise_sqrt,
    /// Elementwise: swish (\f$x \cdot sigmoid(a \cdot x)\f$)
    eltwise_swish,
    /// Elementwise: linear
    eltwise_linear,
    /// Elementwise: bounded_relu
    eltwise_bounded_relu,
    /// Elementwise: soft_relu
    eltwise_soft_relu,
    /// Elementwise: logistic
    eltwise_logistic,
    /// Elementwise: exponent
    eltwise_exp,
    /// Elementwise: gelu
    /// alias for #dnnl::algorithm::eltwise_gelu_tanh
    eltwise_gelu,
    /// Elementwise: tanh-based gelu
    eltwise_gelu_tanh,
    /// Elementwise: erf-based gelu
    eltwise_gelu_erf,
    /// Elementwise: natural logarithm
    eltwise_log,
    /// Elementwise: clip
    eltwise_clip,
    /// Elementwise: pow
    eltwise_pow,
    /// Elementwise: rectified linar unit (ReLU) (dst for backward)
    eltwise_relu_use_dst_for_bwd,
    /// Elementwise: hyperbolic tangent non-linearity (tanh) (dst for backward)
    eltwise_tanh_use_dst_for_bwd,
    /// Elementwise: exponential linear unit (ELU) (dst for backward)
    eltwise_elu_use_dst_for_bwd,
    /// Elementwise: square root (dst for backward)
    eltwise_sqrt_use_dst_for_bwd,
    /// Elementwise: logistic (dst for backward)
    eltwise_logistic_use_dst_for_bwd,
    /// Elementwise: exponent (dst for backward)
    eltwise_exp_use_dst_for_bwd,
    /// Local response normalization (LRN) across multiple channels
    lrn_across_channels,
    /// LRN within a single channel
    lrn_within_channel,
    /// Max pooling
    pooling_max,
    /// Average pooling exclude padding,
    /// alias for #dnnl::algorithm::pooling_avg_include_padding
    pooling_avg,
    /// Average pooling include padding
    pooling_avg_include_padding,
    /// Average pooling exclude padding
    pooling_avg_exclude_padding,
    /// RNN cell
    vanilla_rnn,
    /// LSTM cell
    vanilla_lstm,
    /// GRU cell
    vanilla_gru,
    /// GRU cell with linear before reset. Differs from original GRU
    /// in how the new memory gate is calculated:
    /// \f$c_t = tanh(W_c*x_t + b_{c_x} + r_t*(U_c*h_{t-1}+b_{c_h})) \f$
    /// LRB GRU expects 4 bias tensors on input:
    /// \f$[b_{u}, b_{r}, b_{c_x}, b_{c_h}]\f$
    lbr_gru,
    /// Binary add
    binary_add,
    /// Binary mul
    binary_mul,
    /// Binary max
    binary_max,
    /// Binary min
    binary_min,
    /// Nearest Neighbor resampling method
    resampling_nearest,
    /// Linear (Bilinear, Trilinear) resampling method
    resampling_linear,
};

/// @} dnnl_api_attributes

/// @addtogroup dnnl_api_primitives_common
/// @{

/// Flags for normalization primitives (can be combined via '|')
enum class normalization_flags : unsigned {
    /// Use no normalization flags. If specified, the library computes mean and
    /// variance on forward propagation for training and inference, outputs them
    /// on forward propagation for training, and computes the respective
    /// derivatives on backward propagation.
    none,

    /// Use global statistics. If specified, the library uses mean and
    /// variance provided by the user as an input on forward propagation and
    /// does not compute their derivatives on backward propagation. Otherwise,
    /// the library computes mean and variance on forward propagation for
    /// training and inference, outputs them on forward propagation for
    /// training, and computes the respective derivatives on backward
    /// propagation.
    use_global_stats = 0x1u,

    /// Use scale and shift parameters. If specified, the user is expected to
    /// pass scale and shift as inputs on forward propagation. On backward
    /// propagation of type #dnnl::prop_kind::backward, the library computes
    /// their derivatives. If not specified, the scale and shift parameters
    /// are not used by the library in any way.
    use_scale_shift = 0x2u,

    /// Fuse normalization with ReLU. On training, normalization will require
    /// the workspace to implement backward propagation. On inference, the
    /// workspace is not required and behavior is the same as when normalization
    /// is fused with ReLU using the post-ops API.
    fuse_norm_relu = 0x4u,
};

/// Bitwise OR operation for batch normalization flags.
normalization_flags operator|(normalization_flags lhs, normalization_flags rhs);

/// @} dnnl_api_primitives_common

/// @addtogroup dnnl_api_rnn
/// @{

/// RNN cell flags.
enum class rnn_flags : unsigned {
    /// Undefined RNN flags
    undef,
};

/// Bitwise OR operation for RNN cell flags.
rnn_flags operator|(rnn_flags lhs, rnn_flags rhs);

/// A direction of RNN primitive execution.
enum class rnn_direction {
    /// Unidirectional execution of RNN primitive from left to right.
    unidirectional_left2right,
    /// Unidirectional execution of RNN primitive from right to left.
    unidirectional_right2left,
    /// Bidirectional execution of RNN primitive with concatenation of the
    /// results.
    bidirectional_concat,
    /// Bidirectional execution of RNN primitive with summation of the
    /// results.
    bidirectional_sum,
    /// Alias for #dnnl::rnn_direction::unidirectional_left2right
    unidirectional = unidirectional_left2right,
};

/// @} dnnl_api_rnn

/// @addtogroup dnnl_api_primitives_common
/// @{

/// Primitive descriptor query specification.
enum class query {
    /// no query
    undef,

    /// execution engine
    engine,
    /// primitive kind
    primitive_kind,

    /// number of inputs expected
    num_of_inputs_s32,
    /// number of outputs expected
    num_of_outputs_s32,

    /// runtime estimation (seconds), unimplemented
    time_estimate_f64,
    /// memory required for scratchpad (bytes)
    memory_consumption_s64,

    /// scratchpad engine
    ///
    /// engine to be used for creating scratchpad memory
    scratchpad_engine,

    /// reorder source engine
    reorder_src_engine,
    /// reorder destination engine
    reorder_dst_engine,

    /// implementation name
    impl_info_str,

    /// propagation kind
    prop_kind,

    /// operation descriptor
    op_d,
    /// convolution descriptor
    convolution_d,
    /// deconvolution descriptor
    deconvolution_d,
    /// shuffle descriptor
    shuffle_d,
    /// eltwise descriptor
    eltwise_d,
    /// softmax descriptor
    softmax_d,
    /// pooling descriptor
    pooling_d,
    /// lrn descriptor
    lrn_d,
    /// batch normalization descriptor
    batch_normalization_d,
    /// layer normalization descriptor
    layer_normalization_d,
    /// inner product descriptor
    inner_product_d,
    /// rnn descriptor
    rnn_d,
    /// binary descriptor
    binary_d,
    /// logsoftmax descriptor
    logsoftmax_d,
    /// matmul descriptor
    matmul_d,
    /// resampling descriptor
    resampling_d,

    /// source memory desc
    src_md,
    /// source gradient (diff) memory desc
    diff_src_md,
    /// weights memory descriptor desc
    weights_md,
    /// weights gradient (diff) memory desc
    diff_weights_md,
    /// destination memory desc
    dst_md,
    /// destination gradient (diff) memory desc
    diff_dst_md,
    /// workspace memory desc
    workspace_md,
    /// scratchpad memory desc
    scratchpad_md,
    /// memory desc of an execute argument
    exec_arg_md,
};

/// @} dnnl_api_primitives_common

/// @} dnnl_api_primitives

/// @addtogroup dnnl_api_engine Engine
///
/// An abstraction of a computational device: a CPU, a specific GPU
/// card in the system, etc. Most primitives are created to execute
/// computations on one specific engine. The only exceptions are reorder
/// primitives that transfer data between two different engines.
///
/// @{

/// An execution engine.
struct engine {
    /// Kinds of engines.
    enum class kind {
        /// An unspecified engine
        any,
        /// CPU engine
        cpu,
        /// GPU engine
        gpu,
    };

    /// Constructs an empty engine. An empty engine cannot be used in any
    /// operations.
    engine();

    /// Returns the number of engines of a certain kind.
    ///
    /// @param kind The kind of engines to count.
    /// @returns The number of engines of the specified kind.
    static size_t get_count(kind kind);

    /// Constructs an engine.
    ///
    /// @param kind The kind of engine to construct.
    /// @param index The index of the engine. Must be less than the value
    ///     returned by #get_count() for this particular kind of engine.
    engine(kind kind, size_t index);

    /// Constructs an engine from SYCL device and context objects.
    ///
    /// @param kind The kind of engine to construct.
    /// @param dev SYCL device.
    /// @param ctx SYCL context.
    engine(kind kind, const cl::sycl::device &dev,
            const cl::sycl::context &ctx);

    /// Returns the kind of the engine.
    /// @returns The kind of the engine.
    kind get_kind() const;

    /// Returns the underlying SYCL context object.
    cl::sycl::context get_sycl_context() const;

    /// Returns the underlying SYCL device object.
    cl::sycl::device get_sycl_device() const;
};

/// @} dnnl_api_engine

/// @addtogroup dnnl_api_stream Stream
///
/// An encapsulation of execution context tied to a particular engine.
///
/// @{

/// A container for stream attributes.
struct stream_attr : public handle<dnnl_stream_attr_t> {
    using handle::handle;

    /// Constructs default (empty) stream attributes.
    stream_attr();

    /// Constructs stream attributes for a stream that runs on an engine of a
    /// particular kind.
    ///
    /// @param kind Target engine kind.
    stream_attr(engine::kind kind);
};

/// An execution stream.
struct stream {
    /// Stream flags. Can be combined using the bitwise OR operator.
    enum class flags : unsigned {
        /// Default order execution. Either in-order or out-of-order depending
        /// on the engine runtime.
        default_order,
        /// In-order execution.
        in_order,
        /// Out-of-order execution.
        out_of_order,
        /// Default stream configuration.
        default_flags,
    };

    /// Constructs an empty stream. An empty stream cannot be used in any
    /// operations.
    stream();

    /// Constructs a stream for the specified engine and with behavior
    /// controlled by the specified flags.
    ///
    /// @param engine Engine to create the stream on.
    /// @param flags Flags controlling stream behavior.
    /// @param attr Stream attributes.
    stream(const engine &engine, flags flags = flags::default_flags,
            const stream_attr &attr = stream_attr());

    /// Constructs a stream for the specified engine and the SYCL queue.
    ///
    /// @param engine Engine object to use for the stream.
    /// @param queue SYCL queue to use for the stream.
    stream(const engine &engine, cl::sycl::queue &queue);

    /// Returns the underlying SYCL queue object.
    /// @returns SYCL queue object.
    cl::sycl::queue get_sycl_queue() const;

    /// Waits for all primitives executing in the stream to finish.
    /// @returns The stream itself.
    stream &wait();
};

/// @} dnnl_api_stream

/// @addtogroup dnnl_api_memory Memory
///
/// A container that describes and stores data. Memory objects can contain
/// data of various types and formats. There are two levels of abstraction:
///
/// 1. **Memory descriptor** -- engine-agnostic logical description of data
///     (number of dimensions, dimension sizes, and data type), and,
///     optionally, the information about the physical format of data in
///     memory. If this information is not known yet, a memory descriptor can
///     be created with #dnnl::memory::format_tag::any. This allows
///     compute-intensive primitives to choose the best format for
///     computation. The user is responsible for reordering the data into the
///     chosen format when formats do not match.
///
///     A memory descriptor can be initialized either by specifying dimensions
///     and a memory format tag or strides for each of them, or by
///     manipulating the dnnl_memory_desc_t structure directly.
///
///
///     The user can query the amount of memory required by a memory
///     descriptor using the #dnnl::memory::desc::get_size() function. The
///     size of data in general cannot be computed as the product of
///     dimensions multiplied by the size of the data type. So users are
///     required to use this function for better code portability.
///
///     Two memory descriptors can be compared using the equality and
///     inequality operators.  The comparison is especially useful when
///     checking whether it is necessary to reorder data from the user's data
///     format to a primitive's format.
///
/// 2. **Memory object** -- an engine-specific object that handles the data
///     and its description (a memory descriptor). With CPU engine or with
///     USM, the data handle is simply a pointer to @c void. The data handle
///     can be queried using #dnnl::memory::get_data_handle() and set using
///     #dnnl::memory::set_data_handle(). The underlying SYCL buffer, when
///     used, can be queried using #dnnl::memory::get_sycl_buffer and and set
///     using #dnnl::memory::set_sycl_buffer. A memory object can also be
///     queried for the underlying memory descriptor and for its engine using
///     #dnnl::memory::get_desc() and dnnl::memory::get_engine().
///
/// Along with ordinary memory descriptors with all dimensions being positive,
/// the library supports *zero-volume*  memory descriptors with one or more
/// dimensions set to zero. This is used to support the NumPy\* convention.
/// If a zero-volume memory is passed to a primitive, the primitive typically
/// does not perform any computations with this memory. For example:
///
/// - A concatenation primitive would ignore all memory object with zeroes in
///   the concat dimension / axis.
///
/// - A forward convolution with a source memory object with zero in the
///   minibatch dimension would always produce a destination memory object
///   with a zero in the minibatch dimension and perform no computations.
///
/// - However, a forward convolution with a zero in one of the weights
///   dimensions is ill-defined and is considered to be an error by the
///   library because there is no clear definition of what the output values
///   should be.
///
/// Data handle of a zero-volume memory is never accessed.
///
/// @{

/// Memory object.
///
/// A memory object encapsulates a handle to a memory buffer allocated on a
/// specific engine, tensor dimensions, data type, and memory format, which is
/// the way tensor indices map to offsets in linear memory space. Memory
/// objects are passed to primitives during execution.
struct memory {
    /// Integer type for representing dimension sizes and indices.
    using dim = int64_t;
    /// Vector of dimensions. Implementations are free to force a limit on the
    /// vector's length.
    using dims = std::vector<dim>;

    /// Data type specification.
    enum class data_type {
        /// Undefined data type (used for empty memory descriptors).
        undef,
        /// [16-bit/half-precision floating point](https://en.wikipedia.org/wiki/Half-precision_floating-point_format).
        f16,
        /// non-standard
        /// [16-bit floating point with 7-bit mantissa](https://en.wikipedia.org/wiki/Bfloat16_floating-point_format).
        bf16,
        /// [32-bit/single-precision floating point](https://en.wikipedia.org/wiki/Single-precision_floating-point_format).
        f32,
        /// 32-bit signed integer.
        s32,
        /// 8-bit signed integer.
        s8,
        /// 8-bit unsigned integer.
        u8,
    };

    /// Memory format tag specification.
    ///
    /// Memory format tags can be further divided into two categories:
    ///
    ///  - Domain-agnostic names, i.e. names that do not depend on the tensor
    ///    usage in the specific primitive. These names use letters from `a`
    ///    to `f` to denote logical dimensions and form the order in which the
    ///    dimensions are laid in memory. For example,
    ///    #dnnl::memory::format_tag::ab is used to denote a 2D tensor where the
    ///    second logical dimension (denoted as `b`) is the innermost, i.e.
    ///    has stride = 1, and the first logical dimension (`a`) is laid out in
    ///    memory with stride equal to the size of the second dimension. On the
    ///    other hand, #dnnl::memory::format_tag::ba is the transposed version
    ///    of the same tensor: the outermost dimension (`a`) becomes the
    ///    innermost one.
    ///
    ///  - Domain-specific names, i.e. names that make sense only in the
    ///    context of a certain domain, such as CNN. These names are
    ///    aliases to the corresponding domain-agnostic tags and used mostly
    ///    for convenience. For example, #dnnl::memory::format_tag::nc
    ///    is used to denote 2D CNN activations tensor memory format, where
    ///    the channels dimension is the innermost one and the batch dimension
    ///    is the outermost one. Moreover, #dnnl::memory::format_tag::nc is
    ///    an alias for #dnnl::memory::format_tag::ab, because for
    ///    CNN primitives the logical dimensions of activations tensors come
    ///    in order: batch, channels, spatial.  In other words, batch
    ///    corresponds to the first logical dimension (`a`), and channels
    ///    correspond to the second one (`b`).
    ///
    /// The following domain-specific notation applies to memory format tags:
    ///  - @c 'n' denotes the mini-batch dimension
    ///  - @c 'c' denotes a channels dimension
    ///  - When there are multiple channel dimensions (for example,
    ///    in convolution weights tensor), @c 'i' and @c 'o' denote dimensions
    ///    of input and output channels
    ///  - @c 'g' denotes a groups dimension for convolution weights
    ///  - @c 'd', @c 'h', and @c 'w' denote spatial depth, height, and width
    ///    respectively
    ///
    enum class format_tag {
        /// Undefined memory format tag
        undef,
        /// Placeholder memory format tag. Used to instruct the primitive to
        /// select a format automatically.
        any,

        /// plain 1D tensor
        a,

        /// plain 2D tensor
        ab,
        /// permuted 2D tensor
        ba,

        /// plain 3D tensor
        abc,
        /// permuted 3D tensor
        acb,
        /// permuted 3D tensor
        bac,
        /// permuted 3D tensor
        bca,
        /// permuted 3D tensor
        cba,

        /// plain 4D tensor
        abcd,
        /// permuted 4D tensor
        abdc,
        /// permuted 4D tensor
        acdb,
        /// permuted 4D tensor
        bacd,
        /// permuted 4D tensor
        bcda,
        /// permuted 4D tensor
        cdba,
        /// permuted 4D tensor
        dcab,

        /// plain 5D tensor
        abcde,
        /// permuted 5D tensor
        abdec,
        /// permuted 5D tensor
        acbde,
        /// permuted 5D tensor
        acdeb,
        /// permuted 5D tensor
        bcdea,
        /// permuted 5D tensor
        cdeba,
        /// permuted 5D tensor
        decab,
        /// plain 6D tensor
        abcdef,
        /// plain 6D tensor
        acbdef,
        /// plain 6D tensor
        defcab,

        /// 1D tensor; an alias for #dnnl::memory::format_tag::a
        x = a,
        /// 2D CNN activations tensor; an alias for #dnnl::memory::format_tag::ab
        nc = ab,
        /// 2D CNN activations tensor; an alias for #dnnl::memory::format_tag::ba
        cn = ba,
        /// 2D RNN statistics tensor; an alias for #dnnl::memory::format_tag::ab
        tn = ab,
        /// 2D RNN statistics tensor; an alias for #dnnl::memory::format_tag::ba
        nt = ba,
        /// 3D CNN activations tensor; an alias for #dnnl::memory::format_tag::abc
        ncw = abc,
        /// 3D CNN activations tensor; an alias for #dnnl::memory::format_tag::acb
        nwc = acb,
        /// 4D CNN activations tensor; an alias for #dnnl::memory::format_tag::abcd
        nchw = abcd,
        /// 4D CNN activations tensor; an alias for #dnnl::memory::format_tag::acdb
        nhwc = acdb,
        /// 4D CNN activations tensor; an alias for #dnnl::memory::format_tag::bcda
        chwn = bcda,
        /// 5D CNN activations tensor; an alias for #dnnl::memory::format_tag::abcde
        ncdhw = abcde,
        /// 5D CNN activations tensor; an alias for #dnnl::memory::format_tag::acdeb
        ndhwc = acdeb,

        /// 2D CNN weights tensor; an alias for #dnnl::memory::format_tag::ab
        oi = ab,
        /// 2D CNN weights tensor; an alias for #dnnl::memory::format_tag::ba
        io = ba,
        /// 3D CNN weights tensor; an alias for #dnnl::memory::format_tag::abc
        oiw = abc,
        /// 3D CNN weights tensor; an alias for #dnnl::memory::format_tag::acb
        owi = acb,
        /// 3D CNN weights tensor; an alias for #dnnl::memory::format_tag::cba
        wio = cba,
        /// 3D CNN weights tensor; an alias for #dnnl::memory::format_tag::bca
        iwo = bca,
        /// 4D CNN weights tensor; an alias for #dnnl::memory::format_tag::abcd
        oihw = abcd,
        /// 4D CNN weights tensor; an alias for #dnnl::memory::format_tag::cdba
        hwio = cdba,
        /// 4D CNN weights tensor; an alias for #dnnl::memory::format_tag::acdb
        ohwi = acdb,
        /// 4D CNN weights tensor; an alias for #dnnl::memory::format_tag::bcda
        ihwo = bcda,
        /// 4D CNN weights tensor; an alias for #dnnl::memory::format_tag::bacd
        iohw = bacd,
        /// 5D CNN weights tensor; an alias for #dnnl::memory::format_tag::abcde
        oidhw = abcde,
        /// 5D CNN weights tensor; an alias for #dnnl::memory::format_tag::cdeba
        dhwio = cdeba,
        /// 5D CNN weights tensor; an alias for #dnnl::memory::format_tag::acdeb
        odhwi = acdeb,
        /// 5D CNN weights tensor; an alias for #dnnl::memory::format_tag::bcdea
        idhwo = bcdea,

        /// 4D CNN weights tensor with groups; an alias for #dnnl::memory::format_tag::abcd
        goiw = abcd,
        /// 4D CNN weights tensor with groups; an alias for #dnnl::memory::format_tag::dcab
        wigo = dcab,
        /// 5D CNN weights tensor with groups; an alias for #dnnl::memory::format_tag::abcde
        goihw = abcde,
        /// 5D CNN weights tensor with groups; an alias for #dnnl::memory::format_tag::decab
        hwigo = decab,
        /// 5D CNN weights tensor with groups; an alias for #dnnl::memory::format_tag::acbde
        giohw = acbde,
        /// 6D CNN weights tensor with groups; an alias for #dnnl::memory::format_tag::abcdef
        goidhw = abcdef,
        /// 6D CNN weights tensor with groups; an alias for #dnnl::memory::format_tag::abcdef
        giodhw = acbdef,
        /// 6D CNN weights tensor with groups; an alias for #dnnl::memory::format_tag::defcab
        dhwigo = defcab,

        /// 3D RNN data tensor in the format (seq_length, batch, input channels).
        tnc = abc,
        /// 3D RNN data tensor in the format (batch, seq_length, input channels).
        ntc = bac,
        /// 4D RNN states tensor in the format (num_layers, num_directions,
        /// batch, state channels).
        ldnc = abcd,
        /// 5D RNN weights tensor in the format (num_layers, num_directions,
        ///  input_channels, num_gates, output_channels).
        ///
        ///  - For LSTM cells, the gates order is input, forget, candidate
        ///    and output gate.
        ///  - For GRU cells, the gates order is update, reset and output gate.
        ldigo = abcde,
        /// 5D RNN weights tensor in the format (num_layers, num_directions,
        /// num_gates, output_channels, input_channels).
        ///
        ///  - For LSTM cells, the gates order is input, forget, candidate
        ///    and output gate.
        ///  - For GRU cells, the gates order is update, reset and output gate.
        ldgoi = abdec,
        /// 4D LSTM projection tensor in the format (num_layers, num_directions,
        /// num_channels_in_hidden_state, num_channels_in_recurrent_projection).
        ldio = abcd,
        /// 4D LSTM projection tensor in the format (num_layers, num_directions,
        /// num_channels_in_recurrent_projection, num_channels_in_hidden_state).
        ldoi = abdc,
        /// 4D RNN bias tensor in the format (num_layers, num_directions,
        /// num_gates, output_channels).
        ///
        ///  - For LSTM cells, the gates order is input, forget, candidate
        ///    and output gate.
        ///  - For GRU cells, the gates order is update, reset and output gate.
        ldgo = abcd,
    };

    /// A memory descriptor.
    struct desc {
        /// Constructs a zero (empty) memory descriptor. Such a memory
        /// descriptor can be used to indicate absence of an argument.
        desc();

        /// Constructs a memory descriptor.
        ///
        /// @note
        ///     The logical order of dimensions corresponds to the `abc...`
        ///     format tag, and the physical meaning of the dimensions depends
        ///     both on the primitive that would operate on this memory and
        ///     the operation context.
        ///
        /// @param dims Tensor dimensions.
        /// @param data_type Data precision/type.
        /// @param format_tag Memory format tag.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case a
        ///     zero memory descriptor will be constructed. This flag is
        ///     optional and defaults to false.
        desc(const memory::dims &dims, data_type data_type,
                format_tag format_tag, bool allow_empty = false);

        /// Constructs a memory descriptor by strides.
        ///
        /// @note
        ///     The logical order of dimensions corresponds to the `abc...`
        ///     format tag, and the physical meaning of the dimensions depends
        ///     both on the primitive that would operate on this memory and
        ///     the operation context.
        ///
        /// @param dims Tensor dimensions.
        /// @param data_type Data precision/type.
        /// @param strides Strides for each dimension.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case a
        ///     zero memory descriptor will be constructed. This flag is
        ///     optional and defaults to false.
        desc(const memory::dims &dims, data_type data_type,
                const memory::dims &strides, bool allow_empty = false);

        /// Constructs a memory descriptor for a region inside an area
        /// described by this memory descriptor.
        //
        /// @param dims Sizes of the region.
        /// @param offsets Offsets to the region from the encompassing
        ///     memory object in each dimension.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case a
        ///     zero memory descriptor will be returned. This flag is optional
        ///     and defaults to false.
        /// @returns A memory descriptor for the region.
        desc submemory_desc(const memory::dims &dims,
                const memory::dims &offsets, bool allow_empty = false) const;

        /// Constructs a memory descriptor by reshaping an existing one. The
        /// new memory descriptor inherits the data type.
        ///
        /// The operation ensures that the transformation of the physical memory
        /// format corresponds to the transformation of the logical dimensions.
        /// If such transformation is impossible, the function either throws an
        /// exception (default) or returns a zero memory descriptor depending on
        /// the `allow_empty` flag.
        ///
        /// The reshape operation can be described as a combination of the
        /// following basic operations:
        /// 1. Add a dimension of size `1`. This is always possible.
        /// 2. Remove a dimension of size `1`. This is possible only if the
        ///    dimension has no padding (i.e.
        ///    `padded_dims[dim] == dims[dim] && dims[dim] == 1`).
        /// 3. Split a dimension into multiple ones. This is possible only if
        ///    the size of the dimension is exactly equal to the product of the
        ///    split ones and the dimension does not have padding (i.e.
        ///    `padded_dims[dim] = dims[dim]`).
        /// 4. Joining multiple consecutive dimensions into a single one. As in
        ///    the cases above, this requires that the dimensions do not have
        ///    padding and that the memory format is such that in physical
        ///    memory these dimensions are dense and have the same order as
        ///    their logical counterparts. This also assumes that these
        ///    dimensions are not blocked.
        ///    - Here, dense means:
        ///      `stride for dim[i] == (stride for dim[i + 1]) * dim[i + 1]`;
        ///    - And same order means:
        ///      `i < j` if and only if `stride for dim[i] < stride for dim[j]`.
        ///
        /// @warning
        ///     Some combinations of physical memory layout and/or offsets or
        ///     dimensions may result in a failure to make a reshape.
        ///
        /// @param dims New dimensions. The product of dimensions must
        ///     remain constant.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case a
        ///     zero memory descriptor will be returned. This flag is optional
        ///     and defaults to false.
        /// @returns A new memory descriptor with new dimensions.
        desc reshape(const memory::dims &dims, bool allow_empty = false) const;

        /// Constructs a memory descriptor by permuting axes in an existing
        /// one.
        ///
        /// The physical memory layout representation is adjusted accordingly
        /// to maintain the consistency between the logical and physical parts
        /// of the memory descriptor. The new memory descriptor inherits the
        /// data type.
        ///
        /// The logical axes will be permuted in the following manner:
        /// @code
        /// for (i = 0; i < ndims(); i++)
        ///     new_desc.dims()[permutation[i]] = dims()[i];
        /// @endcode
        ///
        /// Example:
        /// @code
        ///     std::vector<int> permutation = {1, 0}; // swap the first and
        ///                                            // the second axes
        ///     dnnl::memory::desc in_md(
        ///             {2, 3}, data_type, memory::format_tag::ab);
        ///     dnnl::memory::desc expect_out_md(
        ///             {3, 2}, data_type, memory::format_tag::ba);
        ///
        ///     assert(in_md.permute_axes(permutation) == expect_out_md);
        /// @endcode
        ///
        /// @param permutation Axes permutation.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case a
        ///     zero memory descriptor will be returned. This flag is optional
        ///     and defaults to false.
        /// @returns A new memory descriptor with new dimensions.
        desc permute_axes(const std::vector<int> &permutation,
                bool allow_empty = false) const;

        /// Returns dimensions of the memory descriptor.
        ///
        /// Potentially expensive due to the data copy involved.
        /// @returns A copy of the dimensions vector.
        memory::dims dims() const;

        /// Returns the data type of the memory descriptor.
        /// @returns The data type.
        memory::data_type data_type() const;

        /// Returns size of the memory descriptor in bytes.
        /// @returns The number of bytes required to allocate a memory buffer
        ///     for the memory object described by this memory descriptor
        ///     including the padding area.
        size_t get_size() const;

        /// Checks whether the memory descriptor is zero (empty).
        /// @returns @c true if the memory descriptor describes an empty
        ///     memory and @c false otherwise.
        bool is_zero() const;

        /// An equality operator.
        /// @param other Another memory descriptor.
        /// @returns Whether this and the other memory descriptors have
        ///     the same format tag, dimensions, strides, etc.
        bool operator==(const desc &other) const;

        /// An inequality operator.
        /// @param other Another memory descriptor.
        /// @returns Whether this and the other memory descriptors describe
        ///     different memory.
        bool operator!=(const desc &other) const;
    };

    /// Default constructor.
    ///
    /// Constructs an empty memory object, which can be used to indicate
    /// absence of a parameter.
    memory();

    /// Constructs a memory object.
    ///
    /// Unless @p handle is equal to DNNL_MEMORY_NONE, the constructed memory
    /// object will have the underlying buffer set. In this case, the buffer
    /// will be initialized as if #dnnl::memory::set_data_handle() had been
    /// called.
    ///
    /// @sa memory::set_data_handle()
    ///
    /// @param md Memory descriptor.
    /// @param engine Engine to store the data on.
    /// @param handle Handle of the memory buffer to use as an underlying
    ///     storage.
    ///     - A pointer to the user-allocated buffer. In this case the library
    ///       doesn't own the buffer.
    ///     - The DNNL_MEMORY_ALLOCATE special value. Instructs the library to
    ///       allocate the buffer for the memory object. In this case the
    ///       library owns the buffer.
    ///     - DNNL_MEMORY_NONE to create dnnl_memory without an underlying
    ///       buffer.
    memory(const desc &md, const engine &engine, void *handle);

    /// Constructs a memory object from a SYCL buffer.
    ///
    /// @param md Memory descriptor.
    /// @param engine Engine to store the data on.
    /// @param buf A SYCL buffer.
    template <typename T, int ndims = 1>
    memory(const desc &md, const engine &engine,
            cl::sycl::buffer<T, ndims> &buf);

    /// Constructs a memory object.
    ///
    /// The underlying storage for the memory will be allocated by the library.
    ///
    /// @param md Memory descriptor.
    /// @param engine Engine to store the data on.
    memory(const desc &md, const engine &engine);

    /// Returns the associated memory descriptor.
    desc get_desc() const;

    /// Returns the associated engine.
    engine get_engine() const;

    /// Returns the underlying memory buffer.
    ///
    /// On the CPU engine, or when using USM, this is a pointer to the
    /// allocated memory.
    void *get_data_handle() const;

    /// Sets the underlying memory buffer.
    ///
    /// This function may write zero values to the memory specified by the @p
    /// handle if the memory object has a zero padding area. This may be time
    /// consuming and happens each time this function is called. The
    /// operation is always blocking and the stream parameter is a hint.
    ///
    /// @note
    ///     Even when the memory object is used to hold values that stay
    ///     constant during the execution of the program (pre-packed weights
    ///     during inference, for example), the function will still write
    ///     zeroes to the padding area if it exists. Hence, the @p handle
    ///     parameter cannot and does not have a const qualifier.
    ///
    /// @param handle Memory buffer to use as the underlying storage. On the
    ///     CPU engine or when USM is used, the data handle is a pointer to
    ///     the actual data. For OpenCL it is a cl_mem. It must have at least
    ///     get_desc().get_size() bytes allocated.
    /// @param stream Stream to use to execute padding in.
    void set_data_handle(void *handle, const stream &stream) const;

    /// Sets the underlying memory buffer.
    ///
    /// See documentation for
    /// #dnnl::memory::set_data_handle(void *, const stream &) const
    /// for more information.
    ///
    /// @param handle Memory buffer to use as the underlying storage. For the
    ///     CPU engine, the data handle is a pointer to the actual data. For
    ///     OpenCL it is a cl_mem. It must have at least get_desc().get_size()
    ///     bytes allocated.
    void set_data_handle(void *handle) const;

    /// Returns the underlying SYCL buffer object.
    ///
    /// @tparam T Type of the requested buffer.
    /// @tparam ndims Number of dimensions of the requested buffer.
    /// @param offset Offset within the returned buffer at which the memory
    ///               object's data starts. Only meaningful for 1D buffers.
    template <typename T, int ndims = 1>
    cl::sycl::buffer<T, ndims> get_sycl_buffer(size_t *offset = nullptr) const;

    /// Sets the underlying buffer to the given SYCL buffer.
    ///
    /// @tparam T Type of the buffer.
    /// @tparam ndims Number of dimensions of the buffer.
    /// @param buf SYCL buffer.
    template <typename T, int ndims>
    void set_sycl_buffer(cl::sycl::buffer<T, ndims> &buf);
};

/// @} dnnl_api_memory

/// @addtogroup dnnl_api_primitives
/// @{
/// @addtogroup dnnl_api_attributes Attributes
///
/// A container for parameters that extend primitives behavior.
///
/// @{

/// Post-ops.
///
/// Post-ops are computations executed after the main primitive computations
/// and are attached to the primitive via primitive attributes.
///
struct post_ops {
    /// Constructs an empty sequence of post-ops.
    post_ops();

    /// Returns the number of post-ops entries.
    int len() const;

    /// Returns the primitive kind of post-op at entry with a certain index.
    /// @param index Index of the post-op to return the kind for.
    /// @returns Primitive kind of the post-op at the specified index.
    primitive::kind kind(int index) const;

    /// Appends an accumulation (sum) post-op. Prior to accumulating the
    /// result, the previous value would be multiplied by a scaling factor
    /// @p scale.
    ///
    /// The kind of this post-op is #dnnl::primitive::kind::sum.
    ///
    /// This feature may improve performance for cases like residual learning
    /// blocks, where the result of convolution is accumulated to the
    /// previously computed activations. The parameter @p scale may be used
    /// for the integer-based computations when the result and previous
    /// activations have different logical scaling factors.
    ///
    /// In the simplest case when the accumulation is the only post-op,
    /// the computations would be `dst[:] := scale * dst[:] + op(...)`
    /// instead of `dst[:] := op(...)`.
    ///
    /// @note
    ///     This post-op executes in-place and does not change the
    ///     destination layout.
    ///
    /// @param scale Scaling factor.
    void append_sum(float scale = 1.);

    /// Returns the parameters of an accumulation (sum) post-op.
    ///
    /// @param index Index of the sum post-op.
    /// @param scale Scaling factor of the sum post-op.
    void get_params_sum(int index, float &scale) const;

    /// Appends an elementwise post-op.
    ///
    /// The kind of this post-op is #dnnl::primitive::kind::eltwise.
    ///
    /// In the simplest case when the elementwise is the only post-op, the
    /// computations would be `dst[:] := scale * eltwise_op (op(...))` instead
    /// of `dst[:] <- op(...)`, where eltwise_op is configured with the given
    /// parameters.
    ///
    /// @param scale Scaling factor.
    /// @param algorithm Elementwise algorithm.
    /// @param alpha Alpha parameter for the elementwise algorithm.
    /// @param beta Beta parameter for the elementwise algorithm.
    void append_eltwise(
            float scale, algorithm algorithm, float alpha, float beta);

    /// Returns parameters of an elementwise post-up.
    ///
    /// @param index Index of the post-op.
    /// @param scale Output scaling factor.
    /// @param algorithm Output elementwise algorithm kind.
    /// @param alpha Output alpha parameter for the elementwise algorithm.
    /// @param beta Output beta parameter for the elementwise algorithm.
    void get_params_eltwise(int index, float &scale, algorithm &algorithm,
            float &alpha, float &beta) const;
};

/// Primitive attributes.
struct primitive_attr {
    /// Constructs default (empty) primitive attributes.
    primitive_attr();

    /// Returns the scratchpad mode.
    scratchpad_mode get_scratchpad_mode() const;

    /// Sets scratchpad mode.
    ///
    /// @param mode Specified scratchpad mode.
    void set_scratchpad_mode(scratchpad_mode mode);

    /// Returns output scaling factors correspondence mask and values.
    ///
    /// @param mask Scaling factors correspondence mask that defines the
    ///     correspondence between the output tensor dimensions and the @p
    ///     scales vector. The set i-th bit indicates that a dedicated output
    ///     scaling factor is used for each index along that dimension. The
    ///     mask value of 0 implies a common output scaling factor for the
    ///     whole output tensor.
    /// @param scales Vector of output scaling factors.
    void get_output_scales(int &mask, std::vector<float> &scales) const;

    /// Sets output scaling factors correspondence mask and values.
    ///
    /// Example usage:
    /// @code
    ///     int mb = 32, oc = 32,
    ///         oh = 14, ow = 14; // convolution output params
    ///     // unique output scales per output channel
    ///     vector<float> scales = { ... };
    ///     int oc_dim = 1; // mb_dim = 0, channel_dim = 1, height_dim = 2, ...
    ///
    ///     // construct a convolution descriptor
    ///     dnnl::convolution::desc conv_d;
    ///
    ///     dnnl::primitive_attr attr;
    ///     attr.set_output_scales(attr, oc, 1 << oc_dim, scales);
    ///
    ///     dnnl::primitive_desc conv_pd(conv_d, attr, engine);
    /// @endcode
    ///
    /// @note
    ///     The order of dimensions does not depend on how elements are laid
    ///     out in memory. For example:
    ///     - for a 2D CNN activations tensor the order is always (n, c)
    ///     - for a 4D CNN activations tensor the order is always (n, c, h, w)
    ///     - for a 5D CNN weights tensor the order is always
    ///        (g, oc, ic, kh, kw)
    ///
    /// @param mask Defines the correspondence between the output tensor
    ///     dimensions and the @p scales vector. The set i-th bit indicates
    ///     that a dedicated scaling factor is used for each index along that
    ///     dimension. Set the mask to 0 to use a common output scaling factor
    ///     for the whole output tensor.
    /// @param scales Constant vector of output scaling factors. If the
    ///     scaling factors are known at the time of this call, the following
    ///     equality must hold:
    ///     \f$scales.size() = \prod\limits_{d \in mask} output.dims[d].\f$
    ///     Violations can only be detected when the attributes
    ///     are used to create a primitive descriptor.
    ///     If the scaling factors are not known at the time of the call,
    ///     this vector must contain a single #DNNL_RUNTIME_F32_VAL value and
    ///     the output scaling factors must be passed at execution time as an
    ///     argument with index #DNNL_ARG_ATTR_OUTPUT_SCALES.
    void set_output_scales(int mask, const std::vector<float> &scales);
    /// Returns scaling factors correspondence mask and values for a given
    /// memory argument.
    ///
    /// @param arg Parameter argument index as passed to the
    ///     primitive::execute() call.
    /// @param mask Scaling factors correspondence mask that defines the
    ///     correspondence between the output tensor dimensions and the @p
    ///     scales vector. The set i-th bit indicates that a dedicated scaling
    ///     factor is used for each index along that dimension. Set the mask to
    ///     0 to use a common scaling factor for the whole output tensor.
    /// @param scales Output vector of scaling factors.
    void get_scales(int arg, int &mask, std::vector<float> &scales) const;

    /// Sets scaling factors for primitive operations for a given memory
    /// argument.
    ///
    /// @sa dnnl::primitive_attr::set_output_scales
    ///
    /// @param arg Parameter argument index as passed to the
    ///     primitive::execute() call.
    /// @param mask Scaling factors correspondence mask that defines the
    ///     correspondence between the tensor dimensions and the @p scales
    ///     vector. The set i-th bit indicates that a dedicated scaling factor
    ///     is used for each index along that dimension. Set the mask to 0 to
    ///     use a common scaling factor for the whole output tensor.
    /// @param scales Constant vector of scaling factors. The following equality
    ///     must hold:
    ///     \f$scales.size() = \prod\limits_{d \in mask} argument.dims[d].\f$
    void set_scales(int arg, int mask, const std::vector<float> &scales);

    /// Returns zero points correspondence mask and values.
    ///
    /// @param arg Parameter argument index as passed to the
    ///     primitive::execute() call.
    /// @param mask Zero points correspondence mask that defines the
    ///     correspondence between the output tensor dimensions and the @p
    ///     zero_points vector. The set i-th bit indicates that a dedicated
    ///     zero point is used for each index along that dimension. Set the
    ///     mask to 0 to use a common zero point for the whole output tensor.
    /// @param zero_points Output vector of zero points.
    void get_zero_points(
            int arg, int &mask, std::vector<int32_t> &zero_points) const;

    /// Sets zero points for primitive operations for a given memory argument.
    ///
    /// @sa dnnl::primitive_attr::set_output_scales
    ///
    /// @param arg Parameter argument index as passed to the
    ///     primitive::execute() call.
    /// @param mask Zero point correspondence mask that defines the
    ///     correspondence between the tensor dimensions and the @p
    ///     zero_points vector. The set i-th bit indicates that a dedicated
    ///     zero point is used for each index along that dimension. Set the
    ///     mask to 0 to use a common zero point for the whole output tensor.
    /// @param zero_points Constant vector of zero points. If the zero points
    ///     are known at the time of this call, the following equality must
    ///     hold: \f$zero\_points.size() = \prod\limits_{d \in mask}
    ///     argument.dims[d].\f$ If the zero points are not known at the time
    ///     of the call, this vector must contain a single
    ///     #DNNL_RUNTIME_F32_VAL value and the zero points must be passed at
    ///     execution time as an argument with index
    ///     #DNNL_ARG_ATTR_ZERO_POINTS.
    void set_zero_points(
            int arg, int mask, const std::vector<int32_t> &zero_points);

    /// Returns post-ops previously set via set_post_ops().
    ///
    /// @returns Post-ops.
    const post_ops get_post_ops() const;

    /// Sets post-ops.
    ///
    /// @note
    ///     There is no way to check whether the post-ops would be supported
    ///     by the target primitive. Any error will be reported
    ///     by the respective primitive descriptor constructor.
    ///
    /// @param ops Post-ops object to copy post-ops from.
    void set_post_ops(const post_ops ops);

    /// Sets quantization scale and shift parameters for RNN data tensors.
    ///
    /// For performance reasons, the low-precision configuration of the RNN
    /// primitives expect input activations to have the unsigned 8-bit integer
    /// data type. The scale and shift parameters are used to quantize
    /// floating-point data to unsigned integer and must be passed to the RNN
    /// primitive using attributes.
    ///
    /// The quantization formula is `scale * (data + shift)`.
    ///
    /// Example usage:
    /// @code
    ///     // RNN parameters
    ///     int l = 2, t = 2, mb = 32, sic = 32, slc = 32, dic = 32, dlc = 32;
    ///     // Activations quantization parameters
    ///     float scale = 2.0f, shift = 0.5f;
    ///
    ///     primitive_attr attr;
    ///
    ///     // Set scale and shift for int8 quantization of activation
    ///     attr.set_rnn_data_qparams(scale, shift);
    ///
    ///     // Create and configure rnn op_desc
    ///     vanilla_rnn_forward::desc rnn_d(/* arguments */);
    ///     vanilla_rnn_forward::primitive_desc rnn_d(rnn_d, attr, engine);
    /// @endcode
    ///
    /// @note
    ///     Quantization scale and shift are common for src_layer, src_iter,
    ///     dst_iter, and dst_layer.
    ///
    /// @param scale The value to scale the data by.
    /// @param shift The value to shift the data by.
    void set_rnn_data_qparams(float scale, float shift);

    /// Sets quantization scaling factors for RNN weights tensors. The
    /// low-precision configuration of the RNN primitives expect input weights
    /// to use the signed 8-bit integer data type. The scaling factors are
    /// used to quantize floating-point data to signed integer and must be
    /// passed to RNN primitives using attributes.
    ///
    /// @note
    ///     The dimension order is always native and does not depend on the
    ///     actual layout used. For example, five-dimensional weights always
    ///     have (l, d, i, g, o) logical dimension ordering.
    ///
    /// @note
    ///     Quantization scales are common for weights_layer and
    ///     weights_iteration
    ///
    /// @param mask Scaling factors correspondence mask that defines the
    ///     correspondence between the output tensor dimensions and the @p
    ///     scales vector. The set i-th bit indicates that a dedicated scaling
    ///     factor should be used each index along that dimension. Set the
    ///     mask to 0 to use a common scaling factor for the whole output
    ///     tensor.
    /// @param scales Constant vector of output scaling factors. The following
    ///     equality must hold:
    ///     \f$scales.size() = \prod\limits_{d \in mask} weights.dims[d].\f$
    ///     Violations can only be detected when the attributes are used to
    ///     create a primitive descriptor.
    void set_rnn_weights_qparams(int mask, const std::vector<float> &scales);
};

/// @} dnnl_api_attributes

/// @addtogroup dnnl_api_primitives_common
/// @{

/// Base class for all primitive descriptors.
struct primitive_desc_base {
    /// Default constructor. Produces an empty object.
    primitive_desc_base();

    /// Returns the engine of the primitive descriptor.
    /// @returns The engine of the primitive descriptor.
    engine get_engine() const;

    /// Returns implementation name.
    /// @returns The implementation name.
    const char *impl_info_str() const;

    /// Returns a memory::dim value (same as int64_t).
    /// @param what The value to query.
    /// @returns The result of the query.
    memory::dim query_s64(query what) const;

    /// Returns a memory descriptor.
    ///
    /// @note
    ///     There are also convenience methods
    ///     #dnnl::primitive_desc_base::src_desc(),
    ///     #dnnl::primitive_desc_base::dst_desc(), and others.
    ///
    /// @param what The kind of parameter to query; can be
    ///     #dnnl::query::src_md, #dnnl::query::dst_md, etc.
    /// @param idx Index of the parameter. For example, convolution bias can
    ///     be queried with what = #dnnl::query::weights_md and idx = 1.
    /// @returns The requested memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///     parameter of the specified kind or index.
    memory::desc query_md(query what, int idx = 0) const;

    /// Returns a source memory descriptor.
    /// @param idx Source index.
    /// @returns Source memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///     source parameter with index @p pdx.
    memory::desc src_desc(int idx) const;

    /// Returns a destination memory descriptor.
    /// @param idx Destination index.
    /// @returns Destination memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///     destination parameter with index @p pdx.
    memory::desc dst_desc(int idx) const;

    /// Returns a weights memory descriptor.
    /// @param idx Weights index.
    /// @returns Weights memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///     weights parameter with index @p pdx.
    memory::desc weights_desc(int idx) const;

    /// Returns a diff source memory descriptor.
    /// @param idx Diff source index.
    /// @returns Diff source memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///     diff source parameter with index @p pdx.
    memory::desc diff_src_desc(int idx) const;

    /// Returns a diff destination memory descriptor.
    /// @param idx Diff destination index.
    /// @returns Diff destination memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///     diff destination parameter with index @p pdx.
    memory::desc diff_dst_desc(int idx) const;

    /// Returns a diff weights memory descriptor.
    /// @param idx Diff weights index.
    /// @returns Diff weights memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///     diff weights parameter with index @p pdx.
    memory::desc diff_weights_desc(int idx) const;

    // Separate versions without the index argument for documentation
    // purposes.

    /// Returns a source memory descriptor.
    /// @returns Source memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///     source parameter.
    memory::desc src_desc() const;

    /// Returns a destination memory descriptor.
    /// @returns Destination memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///     destination parameter.
    memory::desc dst_desc() const;

    /// Returns a weights memory descriptor.
    /// @returns Weights memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///     weights parameter.
    memory::desc weights_desc() const;

    /// Returns a diff source memory descriptor.
    /// @returns Diff source memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///     diff source memory with.
    memory::desc diff_src_desc() const;

    /// Returns a diff destination memory descriptor.
    /// @returns Diff destination memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///     diff destination parameter.
    memory::desc diff_dst_desc() const;

    /// Returns a diff weights memory descriptor.
    /// @returns Diff weights memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///     diff weights parameter.
    memory::desc diff_weights_desc() const;

    /// Returns the workspace memory descriptor.
    /// @returns Workspace memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not require
    ///     workspace parameter.
    memory::desc workspace_desc() const;
    /// Returns the scratchpad memory descriptor.
    /// @returns scratchpad memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not require
    ///     scratchpad parameter.
    memory::desc scratchpad_desc() const;

    /// Returns the engine on which the scratchpad memory is located.
    /// @returns The engine on which the scratchpad memory is located.
    engine scratchpad_engine() const;

    /// Returns the primitive attributes.
    /// @returns The primitive attributes.
    primitive_attr get_primitive_attr() const;

    /// Returns the kind of the primitive descriptor.
    /// @returns The kind of the primitive descriptor.
    dnnl::primitive::kind get_kind() const;
};

/// @} dnnl_api_primitives_common

/// @addtogroup dnnl_api_reorder Reorder
///
/// A primitive to copy data between two memory objects. This primitive is
/// typically used to change the way the data is laid out in memory.
///
/// @{

/// Reorder primitive.
struct reorder : public primitive {
    /// Primitive descriptor for a reorder primitive.
    struct primitive_desc : public dnnl::primitive_desc_base {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for reorder primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @param src_engine Engine on which the source memory object will be
        ///     located.
        /// @param src_md Source memory descriptor.
        /// @param dst_engine Engine on which the destination memory object
        ///     will be located.
        /// @param dst_md Destination memory descriptor.
        /// @param attr Primitive attributes to use (optional).
        primitive_desc(const engine &src_engine, const memory::desc &src_md,
                const engine &dst_engine, const memory::desc &dst_md,
                const primitive_attr &attr = primitive_attr());

        /// Constructs a primitive descriptor for reorder primitive.
        ///
        /// @param src Source memory object. It is used to obtain the source
        ///     memory descriptor and engine.
        /// @param dst Destination memory object. It is used to obtain the
        ///     destination memory descriptor and engine.
        /// @param attr Primitive attributes to use (optional).
        primitive_desc(const memory &src, const memory &dst,
                const primitive_attr &attr = primitive_attr());

        /// Returns the engine on which the source memory is allocated.
        /// @returns The engine on which the source memory is allocated.
        engine get_src_engine() const;

        /// Returns the engine on which the destination memory is allocated.
        /// @returns The engine on which the destination memory is allocated.
        engine get_dst_engine() const;

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    reorder();

    /// Constructs a reorder primitive.
    /// @param pd Primitive descriptor for reorder primitive.
    reorder(const primitive_desc &pd);

    /// Constructs a reorder primitive that would reorder data between memory
    /// objects having the same memory descriptors as memory objects @p src and
    /// @p dst.
    ///
    /// @param src Source memory object.
    /// @param dst Destination memory object.
    /// @param attr Primitive attributes to use (optional).
    reorder(const memory &src, const memory &dst,
            const primitive_attr &attr = primitive_attr());

    /// Executes the reorder primitive.
    ///
    /// @param stream Stream object. The stream must belong to the same engine
    ///     as the primitive.
    /// @param src Source memory object.
    /// @param dst Destination memory object.
    void execute(const stream &stream, memory &src, memory &dst) const;

    /// Executes the reorder primitive (SYCL-aware version)
    ///
    /// @param stream Stream object. The stream must belong to the same engine
    ///               as the primitive.
    /// @param src Source memory object.
    /// @param dst Destination memory object.
    /// @param deps Vector of SYCL events that the execution should depend on.
    ///
    /// @returns SYCL event that corresponds to the SYCL queue underlying the
    ///          @p stream.
    cl::sycl::event execute_sycl(const stream &stream, memory &src, memory &dst,
            const std::vector<cl::sycl::event> &deps = {}) const;
};

/// @} dnnl_api_reorder

/// @addtogroup dnnl_api_concat Concat
///
/// A primitive to concatenate data by arbitrary dimension.
///
/// @{


/// Tensor concatenation (concat) primitive.
struct concat : public primitive {
    /// Primitive descriptor for a concat primitive.
    struct primitive_desc : public dnnl::primitive_desc_base {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for an out-of-place concatenation
        /// primitive.
        ///
        /// Inputs:
        ///  - `src[0]` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `src[1]` (#dnnl::primitive_desc_base::src_desc(`1`))
        ///  - ...
        ///  - `src[n - 1]` (#dnnl::primitive_desc_base::src_desc(`n - 1`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @param dst Destination memory descriptor.
        /// @param concat_dimension Source tensors will be concatenated over
        ///     dimension with this index. Note that order of dimensions does
        ///     not depend on memory format.
        /// @param srcs Vector of source memory descriptors.
        /// @param engine Engine to perform the operation on.
        /// @param attr Primitive attributes to use (optional).
        primitive_desc(const memory::desc &dst, int concat_dimension,
                const std::vector<memory::desc> &srcs, const engine &engine,
                const primitive_attr &attr = primitive_attr());

        /// Constructs a primitive descriptor for an out-of-place concatenation
        /// primitive.
        ///
        /// This version derives the destination memory descriptor
        /// automatically.
        ///
        /// @param concat_dimension Source tensors will be concatenated over
        ///     dimension with this index. Note that order of dimensions does
        ///     not depend on memory format.
        /// @param srcs Vector of source memory descriptors.
        /// @param engine Engine to perform the operation on.
        /// @param attr Primitive attributes to use (optional).
        primitive_desc(int concat_dimension,
                const std::vector<memory::desc> &srcs, const engine &engine,
                const primitive_attr &attr = primitive_attr());

        /// @copydoc dnnl::primitive_desc_base::src_desc(int)const
        memory::desc src_desc(int idx = 0) const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    concat();

    /// Constructs a concatenation primitive.
    /// @param pd Primitive descriptor for concatenation primitive.
    concat(const primitive_desc &pd);
};

/// @} dnnl_api_concat

/// @addtogroup dnnl_api_sum Sum
///
/// A primitive to sum multiple tensors.
///
/// @{

/// Out-of-place summation (sum) primitive.
struct sum : public primitive {
    /// Primitive descriptor for a sum primitive.
    struct primitive_desc : public dnnl::primitive_desc_base {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a sum primitive.
        ///
        /// Inputs:
        ///  - `src[0]` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `src[1]` (#dnnl::primitive_desc_base::src_desc(`1`))
        ///  - ...
        ///  - `src[n - 1]` (#dnnl::primitive_desc_base::src_desc(`n - 1`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @param dst Destination memory descriptor.
        /// @param scales Vector of scales to multiply data in each source
        ///     memory by.
        /// @param srcs Vector of source memory descriptors.
        /// @param engine Engine to perform the operation on.
        /// @param attr Primitive attributes to use (optional).
        primitive_desc(const memory::desc &dst,
                const std::vector<float> &scales,
                const std::vector<memory::desc> &srcs, const engine &engine,
                const primitive_attr &attr = primitive_attr());

        /// Constructs a primitive descriptor for a sum primitive.
        ///
        /// This version derives the destination memory descriptor
        /// automatically.
        ///
        /// @param scales Vector of scales by which to multiply data in each
        ///     source memory object.
        /// @param srcs Vector of source memory descriptors.
        /// @param engine Engine on which to perform the operation.
        /// @param attr Primitive attributes to use (optional).
        primitive_desc(const std::vector<float> &scales,
                const std::vector<memory::desc> &srcs, const engine &engine,
                const primitive_attr &attr = primitive_attr());

        /// @copydoc dnnl::primitive_desc_base::src_desc(int)const
        memory::desc src_desc(int idx = 0) const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    sum();

    /// Constructs a sum primitive.
    /// @param pd Primitive descriptor for sum primitive.
    sum(const primitive_desc &pd);
};

/// @} dnnl_api_sum

/// @addtogroup dnnl_api_primitives_common
/// @{

/// A base class for descriptors of all primitives that have an operation
/// descriptor and that support iteration over multiple implementations.
struct primitive_desc : public dnnl::primitive_desc_base {
    /// Default constructor. Produces an empty object.
    primitive_desc();
};

/// @} dnnl_api_primitives_common

/// @addtogroup dnnl_api_convolution Convolution
///
/// A primitive to perform 1D, 2D or 3D convolution. Supported variants are
/// forward propagation, backward propagation, and weights gradient with or
/// without bias.
///
/// @{

/// Convolution forward propagation primitive.
struct convolution_forward : public primitive {
    /// Descriptor for a convolution forward propagation primitive.
    struct desc {
        /// Constructs a descriptor for a convolution forward propagation
        /// primitive with bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm Convolution algorithm. Possible values are
        ///     #dnnl::algorithm::convolution_direct,
        ///     #dnnl::algorithm::convolution_winograd, and
        ///     #dnnl::algorithm::convolution_auto.
        /// @param src_desc Source memory descriptor.
        /// @param weights_desc Weights memory descriptor.
        /// @param bias_desc Bias memory descriptor. Passing zero memory
        ///     descriptor disables the bias term.
        /// @param dst_desc Destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(prop_kind prop_kind, algorithm algorithm,
                const memory::desc &src_desc, const memory::desc &weights_desc,
                const memory::desc &bias_desc, const memory::desc &dst_desc,
                const memory::dims &strides, const memory::dims &padding_l,
                const memory::dims &padding_r);

        /// Constructs a descriptor for a convolution forward propagation
        /// primitive without bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm Convolution algorithm. Possible values are
        ///     #dnnl::algorithm::convolution_direct,
        ///     #dnnl::algorithm::convolution_winograd, and
        ///     #dnnl::algorithm::convolution_auto.
        /// @param src_desc Source memory descriptor.
        /// @param weights_desc Weights memory descriptor.
        /// @param dst_desc Destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(prop_kind prop_kind, algorithm algorithm,
                const memory::desc &src_desc, const memory::desc &weights_desc,
                const memory::desc &dst_desc, const memory::dims &strides,
                const memory::dims &padding_l, const memory::dims &padding_r);

        /// Constructs a descriptor for a dilated convolution forward
        /// propagation primitive with bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm Convolution algorithm. Possible values are
        ///     #dnnl::algorithm::convolution_direct,
        ///     #dnnl::algorithm::convolution_winograd, and
        ///     #dnnl::algorithm::convolution_auto.
        /// @param src_desc Source memory descriptor.
        /// @param weights_desc Weights memory descriptor.
        /// @param bias_desc Bias memory descriptor. Passing zero memory
        ///     descriptor disables the bias term.
        /// @param dst_desc Destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param dilates Dilations for each spatial dimension. A zero value
        ///     means no dilation in the corresponding dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(prop_kind prop_kind, algorithm algorithm,
                const memory::desc &src_desc, const memory::desc &weights_desc,
                const memory::desc &bias_desc, const memory::desc &dst_desc,
                const memory::dims &strides, const memory::dims &dilates,
                const memory::dims &padding_l, const memory::dims &padding_r);

        /// Constructs a descriptor for a dilated convolution forward
        /// propagation primitive without bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm Convolution algorithm. Possible values are
        ///     #dnnl::algorithm::convolution_direct,
        ///     #dnnl::algorithm::convolution_winograd, and
        ///     #dnnl::algorithm::convolution_auto.
        /// @param src_desc Source memory descriptor.
        /// @param weights_desc Weights memory descriptor.
        /// @param dst_desc Destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param dilates Dilations for each spatial dimension. A zero value
        ///     means no dilation in the corresponding dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(prop_kind prop_kind, algorithm algorithm,
                const memory::desc &src_desc, const memory::desc &weights_desc,
                const memory::desc &dst_desc, const memory::dims &strides,
                const memory::dims &dilates, const memory::dims &padding_l,
                const memory::dims &padding_r);
    };

    /// Primitive descriptor for a convolution forward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a convolution forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a convolution forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case
        ///     an empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a convolution forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a convolution forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param attr Primitive attributes to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case
        ///     an empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::weights_desc()const
        memory::desc weights_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;

        /// Returns the bias memory descriptor.
        /// @returns The bias memory descriptor.
        /// @returns A zero memory descriptor of the primitive does not have a
        ///     bias parameter.
        memory::desc bias_desc() const;
    };

    /// Default constructor. Produces an empty object.
    convolution_forward();

    /// Constructs a convolution forward propagation primitive.
    /// @param pd Primitive descriptor for a convolution forward propagation
    ///     primitive.
    convolution_forward(const primitive_desc &pd);
};

/// Convolution backward propagation primitive.
struct convolution_backward_data : public primitive {

    /// Descriptor for a convolution backward propagation primitive.
    struct desc {
        /// Constructs a descriptor for a convolution backward propagation
        /// primitive.
        ///
        /// Inputs:
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param algorithm Convolution algorithm. Possible values are
        ///     #dnnl::algorithm::convolution_direct,
        ///     #dnnl::algorithm::convolution_winograd, and
        ///     #dnnl::algorithm::convolution_auto.
        /// @param diff_src_desc Diff source memory descriptor.
        /// @param weights_desc Weights memory descriptor.
        /// @param diff_dst_desc Diff destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(algorithm algorithm, const memory::desc &diff_src_desc,
                const memory::desc &weights_desc,
                const memory::desc &diff_dst_desc, const memory::dims &strides,
                const memory::dims &padding_l, const memory::dims &padding_r);

        /// Constructs a descriptor for dilated convolution backward
        /// propagation primitive.
        ///
        /// Inputs:
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param algorithm Convolution algorithm. Possible values are
        ///     #dnnl::algorithm::convolution_direct,
        ///     #dnnl::algorithm::convolution_winograd, and
        ///     #dnnl::algorithm::convolution_auto.
        /// @param diff_src_desc Diff source memory descriptor.
        /// @param weights_desc Weights memory descriptor.
        /// @param diff_dst_desc Diff destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param dilates Dilations for each spatial dimension. A zero value
        ///     means no dilation in the corresponding dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(algorithm algorithm, const memory::desc &diff_src_desc,
                const memory::desc &weights_desc,
                const memory::desc &diff_dst_desc, const memory::dims &strides,
                const memory::dims &dilates, const memory::dims &padding_l,
                const memory::dims &padding_r);
    };

    /// Primitive descriptor for a convolution backward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a convolution backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a convolution backward propagation
        ///     primitive.
        /// @param engine Engine to perform the operation on.
        /// @param hint_fwd_pd Primitive descriptor for a convolution forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case
        ///     an empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const convolution_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a convolution backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a convolution backward propagation
        ///     primitive.
        /// @param engine Engine to perform the operation on.
        /// @param attr Primitive attributes to use.
        /// @param hint_fwd_pd Primitive descriptor for a convolution forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case
        ///     an empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const convolution_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::diff_src_desc()const
        memory::desc diff_src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::weights_desc()const
        memory::desc weights_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_dst_desc()const
        memory::desc diff_dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    convolution_backward_data();

    /// Constructs a convolution backward propagation primitive.
    /// @param pd Primitive descriptor for a convolution backward propagation
    ///     primitive.
    convolution_backward_data(const primitive_desc &pd);
};

/// Convolution weights gradient primitive.
struct convolution_backward_weights : public primitive {
    /// Descriptor for a convolution weights gradient primitive.
    struct desc {
        /// Constructs a descriptor for a convolution weights gradient primitive
        /// with bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_weights` (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///  - `diff_bias` (#dnnl::primitive_desc_base::diff_weights_desc(`1`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param algorithm Convolution algorithm. Possible values are
        ///     #dnnl::algorithm::convolution_direct,
        ///     #dnnl::algorithm::convolution_winograd, and
        ///     #dnnl::algorithm::convolution_auto.
        /// @param src_desc Source memory descriptor.
        /// @param diff_weights_desc Diff weights memory descriptor.
        /// @param diff_bias_desc Diff bias memory descriptor. Passing zero
        ///     memory descriptor disables the bias term.
        /// @param diff_dst_desc Diff destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(algorithm algorithm, const memory::desc &src_desc,
                const memory::desc &diff_weights_desc,
                const memory::desc &diff_bias_desc,
                const memory::desc &diff_dst_desc, const memory::dims &strides,
                const memory::dims &padding_l, const memory::dims &padding_r);

        /// Constructs a descriptor for a convolution weights gradient primitive
        /// without bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_weights` (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param algorithm Convolution algorithm. Possible values are
        ///     #dnnl::algorithm::convolution_direct,
        ///     #dnnl::algorithm::convolution_winograd, and
        ///     #dnnl::algorithm::convolution_auto.
        /// @param src_desc Source memory descriptor.
        /// @param diff_weights_desc Diff weights memory descriptor.
        /// @param diff_dst_desc Diff destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(algorithm algorithm, const memory::desc &src_desc,
                const memory::desc &diff_weights_desc,
                const memory::desc &diff_dst_desc, const memory::dims &strides,
                const memory::dims &padding_l, const memory::dims &padding_r);

        /// Constructs a descriptor for a dilated convolution weights gradient
        /// primitive with bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_weights` (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///  - `diff_bias` (#dnnl::primitive_desc_base::diff_weights_desc(`1`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param algorithm Convolution algorithm. Possible values are
        ///     #dnnl::algorithm::convolution_direct,
        ///     #dnnl::algorithm::convolution_winograd, and
        ///     #dnnl::algorithm::convolution_auto.
        /// @param src_desc Source memory descriptor.
        /// @param diff_weights_desc Diff weights memory descriptor.
        /// @param diff_bias_desc Diff bias memory descriptor. Passing zero
        ///     memory descriptor disables the bias term.
        /// @param diff_dst_desc Diff destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param dilates Dilations for each spatial dimension. A zero value
        ///     means no dilation in the corresponding dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(algorithm algorithm, const memory::desc &src_desc,
                const memory::desc &diff_weights_desc,
                const memory::desc &diff_bias_desc,
                const memory::desc &diff_dst_desc, const memory::dims &strides,
                const memory::dims &dilates, const memory::dims &padding_l,
                const memory::dims &padding_r);

        /// Constructs a descriptor for a dilated convolution weights gradient
        /// primitive without bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_weights` (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param algorithm Convolution algorithm. Possible values are
        ///     #dnnl::algorithm::convolution_direct,
        ///     #dnnl::algorithm::convolution_winograd, and
        ///     #dnnl::algorithm::convolution_auto.
        /// @param src_desc Source memory descriptor.
        /// @param diff_weights_desc Diff weights memory descriptor.
        /// @param diff_dst_desc Diff destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param dilates Dilations for each spatial dimension. A zero value
        ///     means no dilation in the corresponding dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(algorithm algorithm, const memory::desc &src_desc,
                const memory::desc &diff_weights_desc,
                const memory::desc &diff_dst_desc, const memory::dims &strides,
                const memory::dims &dilates, const memory::dims &padding_l,
                const memory::dims &padding_r);
    };

    /// Primitive descriptor for a convolution weights gradient primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a convolution weights gradient
        /// primitive.
        ///
        /// @param desc Descriptor for a convolution weights gradient primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a convolution forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case
        ///     an empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const convolution_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a convolution weights gradient
        /// primitive.
        ///
        /// @param desc Descriptor for a convolution weights gradient primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a convolution forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case
        ///     an empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const convolution_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_weights_desc()const
        memory::desc diff_weights_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_dst_desc()const
        memory::desc diff_dst_desc() const;

        /// Returns the diff bias memory descriptor.
        /// @returns The diff bias memory descriptor.
        /// @returns A zero memory descriptor of the primitive does not have a
        ///          diff bias parameter.
        memory::desc diff_bias_desc() const;
    };

    /// Default constructor. Produces an empty object.
    convolution_backward_weights();

    /// Constructs a convolution weights gradient primitive.
    /// @param pd Primitive descriptor for a convolution weights gradient
    ///     primitive.
    convolution_backward_weights(const primitive_desc &pd);
};

/// @} dnnl_api_convolution
//
/// @addtogroup dnnl_api_deconvolution Deconvolution
///
/// A primitive to perform 1D, 2D or 3D deconvolution. Supported variants are
/// forward propagation, backward propagation, and weights gradient with or
/// without bias.
///
/// @{

/// Deconvolution forward propagation primitive.
struct deconvolution_forward : public primitive {
    /// Descriptor for a deconvolution forward propagation primitive.
    struct desc {
        /// Constructs a descriptor for a deconvolution forward propagation
        /// primitive with bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm Deconvolution algorithm:
        ///     #dnnl::algorithm::deconvolution_direct, and
        ///     #dnnl::algorithm::deconvolution_winograd.
        /// @param src_desc Source memory descriptor.
        /// @param weights_desc Weights memory descriptor.
        /// @param bias_desc Bias memory descriptor. Passing zero memory
        ///     descriptor disables the bias term.
        /// @param dst_desc Destination memory descriptor.
        /// @param strides Vector of strides for spatial dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(prop_kind prop_kind, algorithm algorithm,
                const memory::desc &src_desc, const memory::desc &weights_desc,
                const memory::desc &bias_desc, const memory::desc &dst_desc,
                const memory::dims &strides, const memory::dims &padding_l,
                const memory::dims &padding_r);

        /// Constructs a descriptor for a deconvolution forward propagation
        /// primitive without bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm Deconvolution algorithm:
        ///     #dnnl::algorithm::deconvolution_direct, and
        ///     #dnnl::algorithm::deconvolution_winograd.
        /// @param src_desc Source memory descriptor.
        /// @param weights_desc Weights memory descriptor.
        /// @param dst_desc Destination memory descriptor.
        /// @param strides Vector of strides for spatial dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(prop_kind prop_kind, algorithm algorithm,
                const memory::desc &src_desc, const memory::desc &weights_desc,
                const memory::desc &dst_desc, const memory::dims &strides,
                const memory::dims &padding_l, const memory::dims &padding_r);

        /// Constructs a descriptor for a dilated deconvolution forward
        /// propagation primitive with bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm Deconvolution algorithm:
        ///     #dnnl::algorithm::deconvolution_direct, and
        ///     #dnnl::algorithm::deconvolution_winograd.
        /// @param src_desc Source memory descriptor.
        /// @param weights_desc Weights memory descriptor.
        /// @param bias_desc Bias memory descriptor. Passing zero memory
        ///     descriptor disables the bias term.
        /// @param dst_desc Destination memory descriptor.
        /// @param strides Vector of strides for spatial dimension.
        /// @param dilates Dilations for each spatial dimension. A zero value
        ///     means no dilation in the corresponding dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(prop_kind prop_kind, algorithm algorithm,
                const memory::desc &src_desc, const memory::desc &weights_desc,
                const memory::desc &bias_desc, const memory::desc &dst_desc,
                const memory::dims &strides, const memory::dims &dilates,
                const memory::dims &padding_l, const memory::dims &padding_r);

        /// Constructs a descriptor for a dilated deconvolution forward
        /// propagation primitive without bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm Deconvolution algorithm:
        ///     #dnnl::algorithm::deconvolution_direct, and
        ///     #dnnl::algorithm::deconvolution_winograd.
        /// @param src_desc Source memory descriptor.
        /// @param weights_desc Weights memory descriptor.
        /// @param dst_desc Destination memory descriptor.
        /// @param strides Vector of strides for spatial dimension.
        /// @param dilates Dilations for each spatial dimension. A zero value
        ///     means no dilation in the corresponding dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(prop_kind prop_kind, algorithm algorithm,
                const memory::desc &src_desc, const memory::desc &weights_desc,
                const memory::desc &dst_desc, const memory::dims &strides,
                const memory::dims &dilates, const memory::dims &padding_l,
                const memory::dims &padding_r);
    };

    /// Primitive descriptor for a deconvolution forward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a deconvolution forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a deconvolution forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a deconvolution forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a deconvolution forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param attr Primitive attributes to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::weights_desc()const
        memory::desc weights_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;

        /// @copydoc dnnl::convolution_forward::primitive_desc::bias_desc()const
        memory::desc bias_desc() const;
    };

    /// Default constructor. Produces an empty object.
    deconvolution_forward();

    /// Constructs a deconvolution forward propagation primitive.
    /// @param pd Primitive descriptor for a deconvolution forward propagation
    ///     primitive.
    deconvolution_forward(const primitive_desc &pd);
};

/// Deconvolution backward propagation primitive.
struct deconvolution_backward_data : public primitive {
    /// Descriptor for a deconvolution backward propagation primitive.
    struct desc {
        /// Constructs a descriptor for a deconvolution backward propagation
        /// primitive.
        ///
        /// Inputs:
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param algorithm Deconvolution algorithm
        ///     (#dnnl::algorithm::convolution_direct,
        ///     #dnnl::algorithm::convolution_winograd).
        /// @param diff_src_desc Diff source memory descriptor.
        /// @param weights_desc Weights memory descriptor.
        /// @param diff_dst_desc Diff destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(algorithm algorithm, const memory::desc &diff_src_desc,
                const memory::desc &weights_desc,
                const memory::desc &diff_dst_desc, const memory::dims &strides,
                const memory::dims &padding_l, const memory::dims &padding_r);

        /// Constructs a descriptor for a dilated deconvolution backward
        /// propagation primitive.
        ///
        /// Inputs:
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param algorithm Deconvolution algorithm
        ///     (#dnnl::algorithm::convolution_direct,
        ///     #dnnl::algorithm::convolution_winograd).
        /// @param diff_src_desc Diff source memory descriptor.
        /// @param weights_desc Weights memory descriptor.
        /// @param diff_dst_desc Diff destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param dilates Dilations for each spatial dimension. A zero value
        ///     means no dilation in the corresponding dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(algorithm algorithm, const memory::desc &diff_src_desc,
                const memory::desc &weights_desc,
                const memory::desc &diff_dst_desc, const memory::dims &strides,
                const memory::dims &dilates, const memory::dims &padding_l,
                const memory::dims &padding_r);
    };

    /// Primitive descriptor for a deconvolution backward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a deconvolution backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a deconvolution backward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a deconvolution forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const deconvolution_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a deconvolution backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a deconvolution backward propagation
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a deconvolution forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const deconvolution_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::diff_src_desc()const
        memory::desc diff_src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::weights_desc()const
        memory::desc weights_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_dst_desc()const
        memory::desc diff_dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    deconvolution_backward_data();

    /// Constructs a deconvolution backward propagation primitive.
    /// @param pd Primitive descriptor for a deconvolution backward propagation
    ///     primitive.
    deconvolution_backward_data(const primitive_desc &pd);
};

/// Deconvolution weights gradient primitive.
struct deconvolution_backward_weights : public primitive {
    /// Descriptor for a deconvolution weights gradient primitive.
    struct desc {
        /// Constructs a descriptor for a deconvolution weights gradient
        /// primitive with bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_weights` (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///  - `diff_bias` (#dnnl::primitive_desc_base::diff_weights_desc(`1`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param algorithm Deconvolution algorithm. Possible values are
        ///     #dnnl::algorithm::deconvolution_direct, and
        ///     #dnnl::algorithm::deconvolution_winograd.
        /// @param src_desc Source memory descriptor.
        /// @param diff_weights_desc Diff weights memory descriptor.
        /// @param diff_bias_desc Diff bias memory descriptor. Passing zero
        ///     memory descriptor disables the bias term.
        /// @param diff_dst_desc Diff destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(algorithm algorithm, const memory::desc &src_desc,
                const memory::desc &diff_weights_desc,
                const memory::desc &diff_bias_desc,
                const memory::desc &diff_dst_desc, const memory::dims &strides,
                const memory::dims &padding_l, const memory::dims &padding_r);

        /// Constructs a descriptor for a deconvolution weights gradient primitive
        /// without bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_weights` (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param algorithm Deconvolution algorithm. Possible values are
        ///     #dnnl::algorithm::deconvolution_direct, and
        ///     #dnnl::algorithm::deconvolution_winograd.
        /// @param src_desc Source memory descriptor.
        /// @param diff_weights_desc Diff weights memory descriptor.
        /// @param diff_dst_desc Diff destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(algorithm algorithm, const memory::desc &src_desc,
                const memory::desc &diff_weights_desc,
                const memory::desc &diff_dst_desc, const memory::dims &strides,
                const memory::dims &padding_l, const memory::dims &padding_r);

        /// Constructs a descriptor for a dilated deconvolution weights gradient
        /// primitive with bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_weights` (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///  - `diff_bias` (#dnnl::primitive_desc_base::diff_weights_desc(`1`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param algorithm Deconvolution algorithm. Possible values are
        ///     #dnnl::algorithm::deconvolution_direct, and
        ///     #dnnl::algorithm::deconvolution_winograd.
        /// @param src_desc Source memory descriptor.
        /// @param diff_weights_desc Diff weights memory descriptor.
        /// @param diff_bias_desc Diff bias memory descriptor. Passing zero
        ///     memory descriptor disables the bias term.
        /// @param diff_dst_desc Diff destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param dilates Dilations for each spatial dimension. A zero value
        ///     means no dilation in the corresponding dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(algorithm algorithm, const memory::desc &src_desc,
                const memory::desc &diff_weights_desc,
                const memory::desc &diff_bias_desc,
                const memory::desc &diff_dst_desc, const memory::dims &strides,
                const memory::dims &dilates, const memory::dims &padding_l,
                const memory::dims &padding_r);

        /// Constructs a descriptor for a dilated deconvolution weights gradient
        /// primitive without bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_weights` (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param algorithm Deconvolution algorithm. Possible values are
        ///     #dnnl::algorithm::deconvolution_direct, and
        ///     #dnnl::algorithm::deconvolution_winograd.
        /// @param src_desc Source memory descriptor.
        /// @param diff_weights_desc Diff weights memory descriptor.
        /// @param diff_dst_desc Diff destination memory descriptor.
        /// @param strides Strides for each spatial dimension.
        /// @param dilates Dilations for each spatial dimension. A zero value
        ///     means no dilation in the corresponding dimension.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(algorithm algorithm, const memory::desc &src_desc,
                const memory::desc &diff_weights_desc,
                const memory::desc &diff_dst_desc, const memory::dims &strides,
                const memory::dims &dilates, const memory::dims &padding_l,
                const memory::dims &padding_r);
    };

    /// Primitive descriptor for a deconvolution weights gradient primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a deconvolution weights
        /// update primitive.
        ///
        /// @param desc Descriptor for a deconvolution weights gradient
        ///     primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a deconvolution forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception.  In this case
        ///     an empty object will be produced.  This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const deconvolution_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a deconvolution weights
        /// update primitive.
        ///
        /// @param desc Descriptor for a deconvolution weights gradient
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a deconvolution forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const deconvolution_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_weights_desc()const
        memory::desc diff_weights_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_dst_desc()const
        memory::desc diff_dst_desc() const;

        /// @copydoc dnnl::convolution_backward_weights::primitive_desc::diff_bias_desc()const
        memory::desc diff_bias_desc() const;
    };

    /// Default constructor. Produces an empty object.
    deconvolution_backward_weights();

    /// Constructs a deconvolution weights gradient primitive.
    /// @param pd Primitive descriptor for a deconvolution weights gradient
    ///     primitive.
    deconvolution_backward_weights(const primitive_desc &pd);
};

/// @} dnnl_api_deconvolution

/// @addtogroup dnnl_api_lrn LRN
///
/// A primitive to perform local response normalization (LRN) across or within
/// channels.
///
/// @{

/// Local response normalization (LRN) forward propagation primitive.
struct lrn_forward : public primitive {
    /// Descriptor for an LRN forward propagation primitive.
    struct desc {
        /// Constructs a descriptor for a LRN forward propagation primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`)),
        ///     if @p alg_kind = #dnnl::algorithm::pooling_max and @p
        ///     prop_kind = #dnnl::prop_kind::forward_training; must be
        ///     queried for using @ref dnnl::primitive_desc_base::query_md()
        ///     after a corresponding primitive descriptor is created
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm LRN algorithm kind: either
        ///     #dnnl::algorithm::lrn_across_channels, or
        ///     #dnnl::algorithm::lrn_within_channel.
        /// @param data_desc Source and destination memory descriptors.
        /// @param local_size Regularization local size.
        /// @param alpha The alpha regularization parameter.
        /// @param beta The beta regularization parameter.
        /// @param k The k regularization parameter.
        desc(prop_kind prop_kind, algorithm algorithm,
                const memory::desc &data_desc, memory::dim local_size,
                float alpha, float beta, float k = 1.f);
    };

    /// Primitive descriptor for an LRN forward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for an LRN forward propagation
        /// primitive.
        ///
        /// @param desc Descriptor for an LRN forward propagation primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for an LRN forward propagation
        /// primitive.
        ///
        /// @param desc Descriptor for an LRN forward propagation primitive.
        /// @param engine Engine to use.
        /// @param attr Primitive attributes to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;

        /// @copydoc dnnl::primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;
    };

    /// Default constructor. Produces an empty object.
    lrn_forward();

    /// Constructs an LRN forward propagation primitive.
    /// @param pd Primitive descriptor for an LRN forward propagation
    ///     primitive.
    lrn_forward(const primitive_desc &pd);
};

/// Local response normalization (LRN) backward propagation primitive.
struct lrn_backward : public primitive {
    /// Descriptor for an LRN backward propagation primitive.
    struct desc {
        /// Constructs a descriptor for an LRN backward propagation primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`)),
        ///     if the underlying implementation requires it; must be queried
        ///     for using @ref dnnl::primitive_desc_base::query_md() after a
        ///     corresponding primitive descriptor is created
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///
        /// @param algorithm LRN algorithm kind: either
        ///     #dnnl::algorithm::lrn_across_channels, or
        ///     #dnnl::algorithm::lrn_within_channel.
        /// @param diff_data_desc Diff source and diff destination memory
        ///     descriptor.
        /// @param data_desc Source memory descriptor.
        /// @param local_size Regularization local size.
        /// @param alpha The alpha regularization parameter.
        /// @param beta The beta regularization parameter.
        /// @param k The k regularization parameter.
        desc(algorithm algorithm, const memory::desc &data_desc,
                const memory::desc &diff_data_desc, memory::dim local_size,
                float alpha, float beta, float k = 1.f);
    };

    /// Primitive descriptor for an LRN backward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for an LRN backward propagation
        /// primitive.
        ///
        /// @param desc Descriptor for an LRN backward propagation primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for an LRN forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const lrn_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for an LRN backward propagation
        /// primitive.
        ///
        /// @param desc Descriptor for an LRN backward propagation primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for an LRN forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const lrn_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc diff_src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_dst_desc()const
        memory::desc diff_dst_desc() const;

        /// @copydoc dnnl::primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;
    };

    /// Default constructor. Produces an empty object.
    lrn_backward();

    /// Constructs an LRN backward propagation primitive.
    /// @param pd Primitive descriptor for an LRN backward propagation
    ///     primitive.
    lrn_backward(const primitive_desc &pd);
};

/// @} dnnl_api_lrn

/// @addtogroup dnnl_api_pooling Pooling
///
/// A primitive to perform max or average pooling.
///
/// @{

/// Pooling forward propagation primitive.
struct pooling_forward : public primitive {
    /// Descriptor for a pooling forward propagation primitive.
    struct desc {
        /// Constructs a descriptor for pooling forward propagation primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`)),
        ///     if @p alg_kind = #dnnl::algorithm::pooling_max and @p
        ///     prop_kind = #dnnl::prop_kind::forward_training; must be
        ///     queried for using @ref dnnl::primitive_desc_base::query_md()
        ///     after a corresponding primitive descriptor is created
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm Pooling algorithm kind: either
        ///     #dnnl::algorithm::pooling_max,
        ///     #dnnl::algorithm::pooling_avg_include_padding,
        ///     or #dnnl::algorithm::pooling_avg (same as
        ///     #dnnl::algorithm::pooling_avg_exclude_padding).
        /// @param src_desc Source memory descriptor.
        /// @param dst_desc Destination memory descriptor.
        /// @param strides Vector of strides for spatial dimension.
        /// @param kernel Vector of kernel spatial dimensions.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(prop_kind prop_kind, algorithm algorithm,
                const memory::desc &src_desc, const memory::desc &dst_desc,
                const memory::dims &strides, const memory::dims &kernel,
                const memory::dims &padding_l, const memory::dims &padding_r);
    };

    /// Primitive descriptor for a pooling forward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a pooling forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a pooling forward propagation primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a pooling forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a pooling forward propagation primitive.
        /// @param engine Engine to use.
        /// @param attr Primitive attributes to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;

        /// @copydoc dnnl::primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;
    };

    /// Default constructor. Produces an empty object.
    pooling_forward();

    /// Constructs a pooling forward propagation primitive.
    /// @param pd Primitive descriptor for a pooling forward propagation
    ///     primitive.
    pooling_forward(const primitive_desc &pd);
};

/// Pooling backward propagation primitive.
struct pooling_backward : public primitive {
    /// Descriptor for a pooling backward propagation primitive.
    struct desc {
        /// Constructs a descriptor for pooling backward propagation primitive.
        ///
        /// Inputs:
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`)),
        ///     if @p alg_kind = #dnnl::algorithm::pooling_max; must be
        ///     queried for using @ref dnnl::primitive_desc_base::query_md()
        ///     after a corresponding primitive descriptor is created
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///
        /// @param algorithm Pooling algorithm kind: either
        ///     #dnnl::algorithm::pooling_max,
        ///     #dnnl::algorithm::pooling_avg_include_padding,
        ///     or #dnnl::algorithm::pooling_avg (same as
        ///     #dnnl::algorithm::pooling_avg_exclude_padding).
        /// @param diff_src_desc Diff source memory descriptor.
        /// @param diff_dst_desc Diff destination memory descriptor.
        /// @param strides Vector of strides for spatial dimension.
        /// @param kernel Vector of kernel spatial dimensions.
        /// @param padding_l Vector of padding values for low indices for each
        ///     spatial dimension (front, top, left).
        /// @param padding_r Vector of padding values for high indices for
        ///     each spatial dimension (back, bottom, right).
        desc(algorithm algorithm, const memory::desc &diff_src_desc,
                const memory::desc &diff_dst_desc, const memory::dims &strides,
                const memory::dims &kernel, const memory::dims &padding_l,
                const memory::dims &padding_r);
    };

    /// Primitive descriptor for a pooling backward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a pooling backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a pooling backward propagation primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a pooling forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const pooling_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a pooling backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a pooling backward propagation primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a pooling forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const pooling_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc diff_src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_dst_desc()const
        memory::desc diff_dst_desc() const;

        /// @copydoc dnnl::primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;
    };

    /// Default constructor. Produces an empty object.
    pooling_backward();

    /// Constructs a pooling backward propagation primitive.
    /// @param pd Primitive descriptor for a pooling backward propagation
    ///     primitive.
    pooling_backward(const primitive_desc &pd);
};

/// @} dnnl_api_pooling

/// @addtogroup dnnl_api_eltwise Eltwise
///
/// A primitive to perform elementwise operations such as the
/// rectifier linear unit (ReLU).
///
/// Both forward and backward propagation primitives support in-place
/// operation; that is, src and dst can refer to the same memory for forward
/// propagation, and diff_dst and diff_src can refer to the same memory for
/// backward propagation.
///
/// @warning
///     Because the original source data is required for backward propagation,
///     in-place forward propagation is not generally supported in the
///     training mode. However, for algorithms supporting destination as input
///     memory, dst can be used for the backward propagation, which makes it
///     possible to get performance benefit even in the training mode.
///
/// @{

/// Elementwise unary operation forward propagation primitive.
struct eltwise_forward : public primitive {
    /// Descriptor for an elementwise forward propagation primitive.
    struct desc {
        /// Constructs a descriptor for an elementwise forward propagation
        /// primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm Elementwise algorithm kind.
        /// @param data_desc Source and destination memory descriptors.
        /// @param alpha The alpha parameter for the elementwise operation.
        ///     Specific meaning depends on the algorithm.
        /// @param beta The beta parameter for the elementwise operation.
        ///     Specific meaning depends on the algorithm.
        desc(prop_kind prop_kind, algorithm algorithm,
                const memory::desc &data_desc, float alpha = 0,
                float beta = 0);
    };

    /// Primitive descriptor for an elementwise forward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for an elementwise forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for an elementwise forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for an elementwise forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for an elementwise forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param attr Primitive attributes to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    eltwise_forward();

    /// Constructs an eltwise forward propagation primitive.
    /// @param pd Primitive descriptor for an eltwise forward propagation
    ///     primitive.
    eltwise_forward(const primitive_desc &pd);
};

/// Elementwise unary operation backward propagation primitive.
///
/// @sa eltwise_forward
struct eltwise_backward : public primitive {
    /// Descriptor for an elementwise backward propagation primitive.
    struct desc {
        /// Constructs a descriptor for an elementwise backward propagation
        /// primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///
        /// @param algorithm Elementwise algorithm kind.
        /// @param diff_data_desc Diff source and destination memory
        ///     descriptors.
        /// @param data_desc Source memory descriptor.
        /// @param alpha The alpha parameter for the elementwise operation.
        ///     Specific meaning depends on the algorithm.
        /// @param beta The beta parameter for the elementwise operation.
        ///     Specific meaning depends on the algorithm.
        desc(algorithm algorithm, const memory::desc &diff_data_desc,
                const memory::desc &data_desc, float alpha = 0,
                float beta = 0);
    };

    /// Primitive descriptor for eltwise backward propagation.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for an elementwise backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for an elementwise backward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for an elementwise forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const eltwise_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for an elementwise backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for an elementwise backward propagation
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for an elementwise forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const eltwise_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_src_desc()const
        memory::desc diff_src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_dst_desc()const
        memory::desc diff_dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    eltwise_backward();

    /// Constructs an eltwise backward propagation primitive.
    /// @param pd Primitive descriptor for an eltwise backward propagation
    ///     primitive.
    eltwise_backward(const primitive_desc &pd);
};

/// @} dnnl_api_eltwise

/// @addtogroup dnnl_api_softmax Softmax
///
/// A primitive to perform softmax.
///
/// @{

/// Softmax forward propagation primitive.
struct softmax_forward : public primitive {
    /// Descriptor for a softmax forward propagation primitive.
    struct desc {
        /// Default constructor. Produces an empty object.
        desc();

        /// Constructs a descriptor for a softmax forward propagation
        /// primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param data_desc Source and destination memory descriptor.
        /// @param softmax_axis Axis over which softmax is computed.
        desc(prop_kind prop_kind, const memory::desc &data_desc,
                int softmax_axis);
    };

    /// Primitive descriptor for a softmax forward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a softmax forward
        /// propagation primitive.
        ///
        /// @param desc descriptor for a softmax forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a softmax forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a softmax forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param attr Primitive attributes to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    softmax_forward();

    /// Constructs a softmax forward propagation primitive.
    /// @param pd Primitive descriptor for a softmax forward propagation
    ///     primitive.
    softmax_forward(const primitive_desc &pd);
};

/// Softmax backward propagation primitive.
struct softmax_backward : public primitive {
    /// Descriptor for a softmax backward propagation primitive.
    struct desc {
        /// Default constructor. Produces an empty object.
        desc();

        /// Constructs a descriptor for a softmax backward propagation
        /// primitive.
        ///
        /// Inputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///
        /// @param diff_data_desc Diff source and diff destination memory
        ///     descriptor.
        /// @param data_desc Destination memory descriptor.
        /// @param softmax_axis Axis over which softmax is computed.
        desc(const memory::desc &diff_data_desc, const memory::desc &data_desc,
                int softmax_axis);
    };

    /// Primitive descriptor for a softmax backward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a softmax backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a softmax backward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a softmax forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const softmax_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a softmax backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a softmax backward propagation
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a softmax forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const softmax_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_src_desc()const
        memory::desc diff_src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc diff_dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    softmax_backward();

    /// Constructs a softmax backward propagation primitive.
    /// @param pd Primitive descriptor for a softmax backward propagation
    ///     primitive.
    softmax_backward(const primitive_desc &pd);
};

/// @} dnnl_api_softmax

/// @addtogroup dnnl_api_logsoftmax LogSoftmax
///
/// A primitive to perform logsoftmax.
///
/// @{

/// Logsoftmax forward propagation primitive.
struct logsoftmax_forward : public primitive {
    /// Descriptor for a logsoftmax forward propagation primitive.
    struct desc {
        /// Default constructor. Produces an empty object.
        desc();

        /// Constructs a descriptor for a logsoftmax forward propagation
        /// primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param data_desc Source and destination memory descriptor.
        /// @param logsoftmax_axis Axis over which softmax is computed.
        desc(prop_kind prop_kind, const memory::desc &data_desc,
                int logsoftmax_axis);
    };

    /// Primitive descriptor for a logsoftmax forward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a logsoftmax forward
        /// propagation primitive.
        ///
        /// @param desc descriptor for a logsoftmax forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a logsoftmax forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a logsoftmax forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param attr Primitive attributes to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    logsoftmax_forward();

    /// Constructs a logsoftmax forward propagation primitive.
    /// @param pd Primitive descriptor for a logsoftmax forward propagation
    ///     primitive.
    logsoftmax_forward(const primitive_desc &pd);
};

/// Logsoftmax backward propagation primitive.
struct logsoftmax_backward : public primitive {
    /// Descriptor for a logsoftmax backward propagation primitive.
    struct desc {
        /// Default constructor. Produces an empty object.
        desc();

        /// Constructs a descriptor for a logsoftmax backward propagation
        /// primitive.
        ///
        /// Inputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///
        /// @param diff_data_desc Diff source and diff destination memory
        ///     descriptors.
        /// @param data_desc Destination memory descriptor.
        /// @param logsoftmax_axis Axis over which softmax is computed.
        desc(const memory::desc &diff_data_desc, const memory::desc &data_desc,
                int logsoftmax_axis);
    };

    /// Primitive descriptor for a logsoftmax backward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a logsoftmax backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a logsoftmax backward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a logsoftmax forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const logsoftmax_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a logsoftmax backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a logsoftmax backward propagation
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a logsoftmax forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const logsoftmax_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_src_desc()const
        memory::desc diff_src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc diff_dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    logsoftmax_backward();

    /// Constructs a logsoftmax backward propagation primitive.
    /// @param pd Primitive descriptor for a logsoftmax backward propagation
    ///     primitive.
    logsoftmax_backward(const primitive_desc &pd);
};

/// @} dnnl_api_logsoftmax

/// @addtogroup dnnl_api_batch_normalization Batch Normalization
///
/// A primitive to perform batch normalization.
///
/// Both forward and backward propagation primitives support in-place
/// operation; that is, src and dst can refer to the same memory for forward
/// propagation, and diff_dst and diff_src can refer to the same memory for
/// backward propagation.
///
/// The batch normalization primitives computations can be controlled by
/// specifying different @ref dnnl::normalization_flags values. For example,
/// batch normalization can compute the mean and variance on its own or take
/// them as inputs.  It can either perform scaling and shifting using gamma
/// and beta parameters or not. Optionally, it can also perform a fused ReLU,
/// which in case of training would also require a workspace.
///
/// @{

/// Batch normalization forward propagation primitive.
struct batch_normalization_forward : public primitive {
    /// Descriptor for a batch normalization forward propagation primitive.
    struct desc {
        /// Constructs a batch normalization descriptor for forward
        /// propagation.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `mean` (#dnnl::primitive_desc_base::src_desc(`1`)),
        ///     if #dnnl::normalization_flags::use_global_stats bit-flag is
        ///     set in @p flags
        ///  - `variance` (#dnnl::primitive_desc_base::src_desc(`2`)),
        ///     if #dnnl::normalization_flags::use_global_stats bit-flag is
        ///     set in @p flags
        ///  - `scale_and_shift` (#dnnl::primitive_desc_base::weights_desc(`0`)),
        ///     if #dnnl::normalization_flags::use_scale_shift bit-flag is set
        ///     in @p flags
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `mean` (#dnnl::primitive_desc_base::dst_desc(`1`)),
        ///     if #dnnl::normalization_flags::use_global_stats bit-flag is
        ///     not set in @p flags and @p prop_kind =
        ///     #dnnl::prop_kind::forward_training
        ///  - `variance` (#dnnl::primitive_desc_base::dst_desc(`2`)),
        ///     if #dnnl::normalization_flags::use_global_stats bit-flag is
        ///     not set in @p flags and @p prop_kind =
        ///     #dnnl::prop_kind::forward_training
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`)),
        ///     if #dnnl::normalization_flags::fuse_norm_relu bit-flag is set
        ///     in @p flags and @p prop_kind =
        ///     #dnnl::prop_kind::forward_training; must be queried
        ///     for using @ref primitive_desc_base::query_md() after a
        ///     corresponding primitive descriptor is created
        ///
        /// @note
        ///     In-place operation is supported: the dst can refer to the same
        ///     memory as the src.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param data_desc Source and destination memory descriptors.
        /// @param epsilon Batch normalization epsilon parameter.
        /// @param flags Batch normalization flags (@ref
        ///     dnnl::normalization_flags).
        desc(prop_kind prop_kind, const memory::desc &data_desc, float epsilon,
                normalization_flags flags);
    };

    /// Primitive descriptor for a batch normalization forward propagation
    /// primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a batch normalization forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a batch normalization forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a batch normalization forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a batch normalization forward propagation
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;

        /// @copydoc dnnl::primitive_desc_base::weights_desc()const
        memory::desc weights_desc() const;

        /// @copydoc dnnl::primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;

        /// Returns memory descriptor for mean.
        /// @returns Memory descriptor for mean.
        memory::desc mean_desc() const;

        /// Returns memory descriptor for variance.
        /// @returns Memory descriptor for variance.
        memory::desc variance_desc() const;
    };

    /// Default constructor. Produces an empty object.
    batch_normalization_forward();

    /// Constructs a batch normalization forward propagation primitive.
    /// @param pd Primitive descriptor for a batch normalization forward
    ///     propagation primitive.
    batch_normalization_forward(const primitive_desc &pd);
};

/// Batch normalization backward propagation primitive.
struct batch_normalization_backward : public primitive {
    /// Descriptor for a batch normalization backward propagation primitive.
    struct desc {
        /// Constructs a batch normalization descriptor for backward
        /// propagation.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `mean` (#dnnl::primitive_desc_base::src_desc(`1`))
        ///  - `variance` (#dnnl::primitive_desc_base::src_desc(`2`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `scale_and_shift` (#dnnl::primitive_desc_base::weights_desc(`0`)),
        ///     if #dnnl::normalization_flags::use_scale_shift bit-flag is
        ///     set in @p flags
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`)),
        ///     if #dnnl::normalization_flags::fuse_norm_relu bit-flag is set
        ///     in @p flags
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///  - `diff_scale_and_shift`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`0`)),
        ///     if #dnnl::normalization_flags::use_scale_shift bit-flag is
        ///     set in @p flags and @p prop_kind = #dnnl::prop_kind::backward
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::backward_data and #dnnl::prop_kind::backward
        ///     (diffs for all parameters are computed in this case).
        /// @param diff_data_desc Diff source and diff destination memory
        ///     descriptor.
        /// @param data_desc Source memory descriptor.
        /// @param epsilon Batch normalization epsilon parameter.
        /// @param flags Batch normalization flags (@ref
        ///     dnnl::normalization_flags).
        desc(prop_kind prop_kind, const memory::desc &diff_data_desc,
                const memory::desc &data_desc, float epsilon,
                normalization_flags flags);
    };

    /// Primitive descriptor for a batch normalization backward propagation
    /// primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a batch normalization backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a batch normalization backward
        ///     propagation primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a batch normalization
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const batch_normalization_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a batch normalization backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a batch normalization backward
        ///     propagation primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a batch normalization
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const batch_normalization_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::weights_desc()const
        memory::desc weights_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_src_desc()const
        memory::desc diff_src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_dst_desc()const
        memory::desc diff_dst_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_weights_desc()const
        memory::desc diff_weights_desc() const;

        /// @copydoc dnnl::batch_normalization_forward::primitive_desc::mean_desc()const
        memory::desc mean_desc() const;

        /// @copydoc dnnl::batch_normalization_forward::primitive_desc::variance_desc()const
        memory::desc variance_desc() const;

        /// @copydoc dnnl::primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;
    };

    /// Default constructor. Produces an empty object.
    batch_normalization_backward();

    /// Constructs a batch normalization backward propagation primitive.
    /// @param pd Primitive descriptor for a batch normalization backward
    ///     propagation primitive.
    batch_normalization_backward(const primitive_desc &pd);
};

/// @} dnnl_api_batch_normalization

/// @addtogroup dnnl_api_layer_normalization Layer Normalization
///
/// A primitive to perform layer normalization. Normalization is performed
/// within the last logical dimension of data tensor.
///
/// Both forward and backward propagation primitives support in-place
/// operation; that is, src and dst can refer to the same memory for forward
/// propagation, and diff_dst and diff_src can refer to the same memory for
/// backward propagation.
///
/// The layer normalization primitives computations can be controlled by
/// specifying different dnnl::normalization_flags values. For example,
/// layer normalization forward propagation can be configured to either
/// compute the mean and variance or take them as arguments. It can either
/// perform scaling and shifting using gamma and beta parameters or not.
/// Optionally, it can also perform a fused ReLU, which in case of training
/// would also require a workspace.
///
/// @{

/// Layer normalization forward propagation primitive.
struct layer_normalization_forward : public primitive {
    /// Descriptor for a layer normalization forward propagation primitive.
    struct desc {
        /// Constructs a descriptor for layer normalization forward
        /// propagation primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `mean` (#dnnl::primitive_desc_base::src_desc(`1`)),
        ///     if #dnnl::normalization_flags::use_global_stats bit-flag is
        ///     set in @p flags
        ///  - `variance` (#dnnl::primitive_desc_base::src_desc(`2`)),
        ///     if #dnnl::normalization_flags::use_global_stats bit-flag is
        ///     set in @p flags
        ///  - `scale_and_shift` (#dnnl::primitive_desc_base::weights_desc(`0`)),
        ///     if #dnnl::normalization_flags::use_scale_shift bit-flag is set
        ///     in @p flags
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `mean` (#dnnl::primitive_desc_base::dst_desc(`1`)),
        ///     if #dnnl::normalization_flags::use_global_stats bit-flag is
        ///     not set in @p flags and @p prop_kind =
        ///     #dnnl::prop_kind::forward_training
        ///  - `variance` (#dnnl::primitive_desc_base::dst_desc(`2`)),
        ///     if #dnnl::normalization_flags::use_global_stats bit-flag is
        ///     not set in @p flags and @p prop_kind =
        ///     #dnnl::prop_kind::forward_training
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param data_desc Source and destination memory descriptor.
        /// @param stat_desc Statistics memory descriptors.
        /// @param epsilon Layer normalization epsilon parameter.
        /// @param flags Layer normalization flags (@ref
        ///     dnnl::normalization_flags).
        desc(prop_kind prop_kind, const memory::desc &data_desc,
                const memory::desc &stat_desc, float epsilon,
                normalization_flags flags);

        /// Constructs a descriptor for layer normalization forward
        /// propagation primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `mean` (#dnnl::primitive_desc_base::src_desc(`1`)),
        ///     if #dnnl::normalization_flags::use_global_stats bit-flag is
        ///     set in @p flags
        ///  - `variance` (#dnnl::primitive_desc_base::src_desc(`2`)),
        ///     if #dnnl::normalization_flags::use_global_stats bit-flag is
        ///     set in @p flags
        ///  - `scale_and_shift` (#dnnl::primitive_desc_base::weights_desc(`0`)),
        ///     if #dnnl::normalization_flags::use_scale_shift bit-flag is set
        ///     in @p flags
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `mean` (#dnnl::primitive_desc_base::dst_desc(`1`)),
        ///     if #dnnl::normalization_flags::use_global_stats bit-flag is
        ///     not set in @p flags and @p prop_kind =
        ///     #dnnl::prop_kind::forward_training
        ///  - `variance` (#dnnl::primitive_desc_base::dst_desc(`2`)),
        ///     if #dnnl::normalization_flags::use_global_stats bit-flag is
        ///     not set in @p flags and @p prop_kind =
        ///     #dnnl::prop_kind::forward_training
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param data_desc Source and destination memory descriptor.
        /// @param epsilon Layer normalization epsilon parameter.
        /// @param flags Layer normalization flags (@ref
        ///     dnnl::normalization_flags).
        desc(prop_kind prop_kind, const memory::desc &data_desc, float epsilon,
                normalization_flags flags);
    };

    /// Primitive descriptor for a layer normalization forward propagation
    /// primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a layer normalization forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a layer normalization forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a layer normalization forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a layer normalization forward propagation
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;

        /// @copydoc dnnl::primitive_desc_base::weights_desc()const
        memory::desc weights_desc() const;

        /// @copydoc dnnl::primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;

        /// @copydoc dnnl::batch_normalization_forward::primitive_desc::mean_desc()const
        memory::desc mean_desc() const;

        /// @copydoc dnnl::batch_normalization_forward::primitive_desc::variance_desc()const
        memory::desc variance_desc() const;
    };

    /// Default constructor. Produces an empty object.
    layer_normalization_forward();

    /// Constructs a layer normalization forward propagation primitive.
    /// @param pd Primitive descriptor for a layer normalization forward
    ///     propagation primitive.
    layer_normalization_forward(const primitive_desc &pd);
};

/// Layer normalization backward propagation primitive.
struct layer_normalization_backward : public primitive {
    /// Descriptor for a layer normalization backward propagation primitive.
    struct desc {
        /// Constructs a descriptor for layer normalization backward
        /// propagation primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `mean` (#dnnl::primitive_desc_base::src_desc(`1`))
        ///  - `variance` (#dnnl::primitive_desc_base::src_desc(`2`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `scale_and_shift` (#dnnl::primitive_desc_base::weights_desc(`0`)),
        ///     if #dnnl::normalization_flags::use_scale_shift bit-flag is
        ///     set in @p flags
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///  - `diff_scale_and_shift`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`0`)), if
        ///     #dnnl::normalization_flags::use_scale_shift bit-flag is set
        ///     in @p flags and @p prop_kind = #dnnl::prop_kind::backward
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::backward_data and #dnnl::prop_kind::backward
        ///     (diffs for all parameters are computed in this case).
        /// @param diff_data_desc Diff source and diff destination memory
        ///     descriptor.
        /// @param data_desc Source memory descriptor.
        /// @param stat_desc Statistics memory descriptors.
        /// @param epsilon Layer normalization epsilon parameter.
        /// @param flags Layer normalization flags (@ref
        ///     dnnl::normalization_flags).
        desc(prop_kind prop_kind, const memory::desc &diff_data_desc,
                const memory::desc &data_desc, const memory::desc &stat_desc,
                float epsilon, normalization_flags flags);

        /// Constructs a descriptor for layer normalization backward
        /// propagation primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `mean` (#dnnl::primitive_desc_base::src_desc(`1`))
        ///  - `variance` (#dnnl::primitive_desc_base::src_desc(`2`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `scale_and_shift` (#dnnl::primitive_desc_base::weights_desc(`0`)),
        ///     if #dnnl::normalization_flags::use_scale_shift bit-flag is
        ///     set in @p flags
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///  - `diff_scale_and_shift`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`0`)), if
        ///     #dnnl::normalization_flags::use_scale_shift bit-flag is set
        ///     in @p flags and @p prop_kind = #dnnl::prop_kind::backward
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::backward_data and #dnnl::prop_kind::backward
        ///     (diffs for all parameters are computed in this case).
        /// @param diff_data_desc Diff source and diff destination memory
        ///     descriptor.
        /// @param data_desc Source memory descriptor.
        /// @param epsilon Layer normalization epsilon parameter.
        /// @param flags Layer normalization flags (@ref
        ///     dnnl::normalization_flags).
        desc(prop_kind prop_kind, const memory::desc &diff_data_desc,
                const memory::desc &data_desc, float epsilon,
                normalization_flags flags);
    };

    /// Primitive descriptor for a layer normalization backward propagation
    /// primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a layer normalization backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a layer normalization backward
        ///     propagation primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a layer normalization
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const layer_normalization_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a layer normalization backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a layer normalization backward
        ///     propagation primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a layer normalization
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const layer_normalization_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::weights_desc()const
        memory::desc weights_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_src_desc()const
        memory::desc diff_src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_dst_desc()const
        memory::desc diff_dst_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_weights_desc()const
        memory::desc diff_weights_desc() const;

        /// @copydoc dnnl::batch_normalization_forward::primitive_desc::mean_desc()const
        memory::desc mean_desc() const;

        /// @copydoc dnnl::batch_normalization_forward::primitive_desc::variance_desc()const
        memory::desc variance_desc() const;

        /// @copydoc dnnl::primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;
    };

    /// Default constructor. Produces an empty object.
    layer_normalization_backward();

    /// Constructs a layer normalization backward propagation primitive.
    /// @param pd Primitive descriptor for a layer normalization backward
    ///     propagation primitive.
    layer_normalization_backward(const primitive_desc &pd);
};

/// @} dnnl_api_layer_normalization

/// @addtogroup dnnl_api_inner_product Inner Product
///
/// A primitive to compute an inner product.
///
/// @{

/// Inner product forward propagation primitive.
struct inner_product_forward : public primitive {
    /// Descriptor for an inner product forward propagation primitive.
    struct desc {
        /// Constructs a descriptor for an inner product forward propagation
        /// primitive with bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param src_desc Memory descriptor for src.
        /// @param weights_desc Memory descriptor for diff weights.
        /// @param bias_desc Memory descriptor for diff bias.
        /// @param dst_desc Memory descriptor for diff dst.
        desc(prop_kind prop_kind, const memory::desc &src_desc,
                const memory::desc &weights_desc, const memory::desc &bias_desc,
                const memory::desc &dst_desc);

        /// Constructs a descriptor for an inner product forward propagation
        /// primitive without bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param src_desc Memory descriptor for src.
        /// @param weights_desc Memory descriptor for diff weights.
        /// @param dst_desc Memory descriptor for dst.
        desc(prop_kind prop_kind, const memory::desc &src_desc,
                const memory::desc &weights_desc,
                const memory::desc &dst_desc);
    };

    /// Primitive descriptor for an inner product forward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for an inner product forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for an inner product forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for an inner product forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for an inner product forward propagation
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::weights_desc()const
        memory::desc weights_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;

        /// @copydoc dnnl::convolution_forward::primitive_desc::bias_desc()const
        memory::desc bias_desc() const;
    };

    /// Default constructor. Produces an empty object.
    inner_product_forward();

    /// Constructs an inner product forward propagation primitive.
    /// @param pd Primitive descriptor for an inner product forward
    ///     propagation primitive.
    inner_product_forward(const primitive_desc &pd);
};

/// Inner product backward propagation primitive.
struct inner_product_backward_data : public primitive {
    /// Descriptor for an inner product backward propagation primitive.
    struct desc {
        /// Constructs a descriptor for an inner product backward propagation
        /// primitive.
        ///
        /// Inputs:
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param diff_src_desc Memory descriptor for diff src.
        /// @param weights_desc Memory descriptor for weights.
        /// @param diff_dst_desc Memory descriptor for diff dst.
        desc(const memory::desc &diff_src_desc,
                const memory::desc &weights_desc,
                const memory::desc &diff_dst_desc);
    };

    /// Primitive descriptor for an inner product backward propagation
    /// primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for an inner product backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for an inner product backward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for an inner product
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const inner_product_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for an inner product backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for an inner product backward propagation
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for an inner product
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const inner_product_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::diff_src_desc()const
        memory::desc diff_src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::weights_desc()const
        memory::desc weights_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_dst_desc()const
        memory::desc diff_dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    inner_product_backward_data();

    /// Constructs an inner product backward propagation primitive.
    /// @param pd Primitive descriptor for an inner product backward
    ///     propagation primitive.
    inner_product_backward_data(const primitive_desc &pd);
};

/// Inner product weights gradient primitive.
struct inner_product_backward_weights : public primitive {
    /// Descriptor for an inner product weights gradient primitive.
    struct desc {
        /// Constructs a descriptor for an inner product descriptor weights
        /// update primitive with bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_weights` (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///  - `diff_bias` (#dnnl::primitive_desc_base::diff_weights_desc(`1`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param src_desc Memory descriptor for src.
        /// @param diff_weights_desc Memory descriptor for diff weights.
        /// @param diff_bias_desc Memory descriptor for diff bias.
        /// @param diff_dst_desc Memory descriptor for diff dst.
        desc(const memory::desc &src_desc,
                const memory::desc &diff_weights_desc,
                const memory::desc &diff_bias_desc,
                const memory::desc &diff_dst_desc);

        /// Constructs a descriptor for an inner product descriptor weights
        /// update primitive without bias.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_weights` (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param src_desc Memory descriptor for src.
        /// @param diff_weights_desc Memory descriptor for diff weights.
        /// @param diff_dst_desc Memory descriptor for diff dst.
        desc(const memory::desc &src_desc,
                const memory::desc &diff_weights_desc,
                const memory::desc &diff_dst_desc);
    };

    /// Primitive descriptor for an inner product weights gradient primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for an inner product weights
        /// update primitive.
        ///
        /// @param desc Descriptor for an inner product weights gradient
        ///     primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for an inner product
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const inner_product_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for an inner product weights
        /// update primitive.
        ///
        /// @param desc Descriptor for an inner product weights gradient
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for an inner product
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const inner_product_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_weights_desc()const
        memory::desc diff_weights_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_dst_desc()const
        memory::desc diff_dst_desc() const;

        /// @copydoc dnnl::convolution_backward_weights::primitive_desc::diff_bias_desc()const
        memory::desc diff_bias_desc() const;
    };

    /// Default constructor. Produces an empty object.
    inner_product_backward_weights();

    /// Constructs an inner product weights gradient primitive.
    /// @param pd Primitive descriptor for an inner product weights gradient
    ///     primitive.
    inner_product_backward_weights(const primitive_desc &pd);
};

/// @} dnnl_api_inner_product

/// @addtogroup dnnl_api_rnn RNN
///
/// A primitive to compute recurrent neural network layers.
///
/// @{

/// Base class for primitive descriptors for RNN primitives.
struct rnn_primitive_desc_base : public primitive_desc {
    /// Default constructor. Produces an empty object.
    rnn_primitive_desc_base();

    /// Returns source layer memory descriptor.
    /// @returns Source layer memory descriptor.
    memory::desc src_layer_desc() const;

    /// Returns source iteration memory descriptor.
    /// @returns Source iteration memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///          source iteration parameter.
    memory::desc src_iter_desc() const;

    /// Returns source recurrent cell state memory descriptor.
    /// @returns Source recurrent cell state memory descriptor.
    memory::desc src_iter_c_desc() const;

    /// Returns weights layer memory descriptor.
    /// @returns Weights layer memory descriptor.
    memory::desc weights_layer_desc() const;

    /// Returns weights iteration memory descriptor.
    /// @returns Weights iteration memory descriptor.
    memory::desc weights_iter_desc() const;

    /// Returns bias memory descriptor.
    /// @returns Bias memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///          bias parameter.
    memory::desc bias_desc() const;

    /// Returns destination layer memory descriptor.
    /// @returns Destination layer memory descriptor.
    memory::desc dst_layer_desc() const;

    /// Returns destination iteration memory descriptor.
    /// @returns Destination iteration memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///          destination iteration parameter.
    memory::desc dst_iter_desc() const;

    /// Returns destination recurrent cell state memory descriptor.
    /// @returns Destination recurrent cell state memory descriptor.
    memory::desc dst_iter_c_desc() const;

    /// Returns diff source layer memory descriptor.
    /// @returns Diff source layer memory descriptor.
    memory::desc diff_src_layer_desc() const;

    /// Returns diff source iteration memory descriptor.
    /// @returns Diff source iteration memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///          diff source iteration parameter.
    memory::desc diff_src_iter_desc() const;

    /// Returns diff source recurrent cell state memory descriptor.
    /// @returns Diff source recurrent cell state memory descriptor.
    memory::desc diff_src_iter_c_desc() const;

    /// Returns diff weights layer memory descriptor.
    /// @returns Diff weights layer memory descriptor.
    memory::desc diff_weights_layer_desc() const;

    /// Returns diff weights iteration memory descriptor.
    /// @returns Diff weights iteration memory descriptor.
    memory::desc diff_weights_iter_desc() const;

    /// Returns diff bias memory descriptor.
    /// @returns Diff bias memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///          diff bias parameter.
    memory::desc diff_bias_desc() const;

    /// Returns diff destination layer memory descriptor.
    /// @returns Diff destination layer memory descriptor.
    memory::desc diff_dst_layer_desc() const;

    /// Returns diff destination iteration memory descriptor.
    /// @returns Diff destination iteration memory descriptor.
    /// @returns A zero memory descriptor if the primitive does not have a
    ///          diff destination iteration parameter.
    memory::desc diff_dst_iter_desc() const;

    /// Returns diff destination recurrent cell state memory descriptor.
    /// @returns Diff destination recurrent cell state memory descriptor.
    memory::desc diff_dst_iter_c_desc() const;
};

/// Vanilla RNN forward propagation primitive.
struct vanilla_rnn_forward : public primitive {
    /// Descriptor for a vanilla RNN forward propagation primitive.
    struct desc {
        /// Constructs a descriptor for a vanilla RNN forward propagation
        /// primitive.
        ///
        /// The @p src_iter_desc, @p bias_desc, and @p dst_iter_desc may point
        /// to a zero memory descriptor. This would then indicate that the RNN
        /// forward propagation primitive should not use them and should
        /// default to zero values instead.
        ///
        /// Inputs:
        ///  - `src_layer` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `src_iter` (#dnnl::primitive_desc_base::src_desc(`1`)), if used
        ///  - `weights_layer` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `weights_iter` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`2`)), if used
        ///
        /// Outputs:
        ///  - `dst_layer` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `dst_iter` (#dnnl::primitive_desc_base::dst_desc(`1`)), if used
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`)),
        ///     if @p prop_kind equals #dnnl::prop_kind::forward_training;
        ///     must be queried for using @ref
        ///     dnnl::primitive_desc_base::query_md() after a corresponding
        ///     primitive descriptor is created
        ///
        /// @note
        ///     All memory descriptors except @p src_iter_desc can be
        ///     initialized with an #dnnl::memory::format_tag::any value of @p
        ///     format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param activation Activation kind. Possible values are
        ///     #dnnl::algorithm::eltwise_relu,
        ///     #dnnl::algorithm::eltwise_tanh, or
        ///     #dnnl::algorithm::eltwise_logistic.
        /// @param direction RNN direction. See @ref dnnl::rnn_direction for
        ///     more info.
        /// @param src_layer_desc Memory descriptor for the input vector.
        /// @param src_iter_desc Memory descriptor for the input recurrent
        ///     hidden state vector.
        /// @param weights_layer_desc Memory descriptor for the weights
        ///     applied to the layer input.
        /// @param weights_iter_desc Memory descriptor for the weights applied
        ///     to the recurrent input.
        /// @param bias_desc Bias memory descriptor.
        /// @param dst_layer_desc Memory descriptor for the output vector.
        /// @param dst_iter_desc Memory descriptor for the output recurrent
        ///     hidden state vector.
        /// @param flags Unused.
        /// @param alpha Negative slope if activation is
        ///     #dnnl::algorithm::eltwise_relu.
        /// @param beta Unused.
        desc(prop_kind prop_kind, algorithm activation, rnn_direction direction,
                const memory::desc &src_layer_desc,
                const memory::desc &src_iter_desc,
                const memory::desc &weights_layer_desc,
                const memory::desc &weights_iter_desc,
                const memory::desc &bias_desc,
                const memory::desc &dst_layer_desc,
                const memory::desc &dst_iter_desc,
                rnn_flags flags = rnn_flags::undef, float alpha = 0.0f,
                float beta = 0.0f);
    };

    /// Primitive descriptor for a vanilla RNN forward propagation primitive.
    struct primitive_desc : public rnn_primitive_desc_base {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a vanilla RNN forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a vanilla RNN forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a vanilla RNN forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a vanilla RNN forward propagation
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::rnn_primitive_desc_base::src_layer_desc()const
        memory::desc src_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::src_iter_desc()const
        memory::desc src_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_layer_desc()const
        memory::desc weights_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_iter_desc()const
        memory::desc weights_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::bias_desc()const
        memory::desc bias_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_layer_desc()const
        memory::desc dst_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_iter_desc()const
        memory::desc dst_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;
    };

    /// Default constructor. Produces an empty object.
    vanilla_rnn_forward();

    /// Constructs a vanilla RNN forward propagation primitive.
    /// @param pd Primitive descriptor for a vanilla RNN forward
    ///     propagation primitive.
    vanilla_rnn_forward(const primitive_desc &pd);
};

/// Vanilla RNN backward propagation primitive.
struct vanilla_rnn_backward : public primitive {
    /// Descriptor for a vanilla RNN backward propagation primitive.
    struct desc {
        /// Constructs a descriptor for a vanilla RNN backward propagation
        /// primitive.
        ///
        /// The @p src_iter_desc together with @p diff_src_iter_desc, @p
        /// bias_desc together with @p diff_bias_desc, and @p dst_iter_desc
        /// together with @p diff_src_iter_desc, may point to a zero memory
        /// descriptor. This would then indicate that the RNN backward
        /// propagation primitive should not use the respective data and
        /// should use zero values instead.
        ///
        /// Inputs:
        ///  - `src_layer` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `src_iter` (#dnnl::primitive_desc_base::src_desc(`1`)), if used
        ///  - `weights_layer` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `weights_iter` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`2`)), if used
        ///  - `dst_layer` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `dst_iter` (#dnnl::primitive_desc_base::dst_desc(`1`)), if used
        ///  - `diff_dst_layer` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `diff_dst_iter`
        ///     (#dnnl::primitive_desc_base::diff_dst_desc(`1`)), if used
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src_layer`
        ///     (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///  - `diff_src_iter`
        ///     (#dnnl::primitive_desc_base::diff_src_desc(`1`)), if used
        ///  - `diff_weights_layer`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///  - `diff_weights_iter`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`1`))
        ///  - `diff_bias`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`2`)), if used
        ///
        /// @note
        ///     All the memory descriptors may be initialized with the
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Must be
        ///     #dnnl::prop_kind::backward.
        /// @param activation Activation kind. Possible values are
        ///     #dnnl::algorithm::eltwise_relu,
        ///     #dnnl::algorithm::eltwise_tanh, or
        ///     #dnnl::algorithm::eltwise_logistic.
        /// @param direction RNN direction. See @ref dnnl::rnn_direction for
        ///     more info.
        /// @param src_layer_desc Memory descriptor for the input vector.
        /// @param src_iter_desc Memory descriptor for the input recurrent
        ///     hidden state vector.
        /// @param weights_layer_desc Memory descriptor for the weights
        ///     applied to the layer input.
        /// @param weights_iter_desc Memory descriptor for the weights applied
        ///     to the recurrent input.
        /// @param bias_desc Bias memory descriptor.
        /// @param dst_layer_desc Memory descriptor for the output vector.
        /// @param dst_iter_desc Memory descriptor for the output recurrent
        ///     hidden state vector.
        /// @param diff_src_layer_desc Memory descriptor for the diff of input
        ///     vector.
        /// @param diff_src_iter_desc Memory descriptor for the diff of input
        ///     recurrent hidden state vector.
        /// @param diff_weights_layer_desc Memory descriptor for the diff of
        ///     weights applied to the layer input.
        /// @param diff_weights_iter_desc Memory descriptor for the diff of
        ///     weights applied to the recurrent input.
        /// @param diff_bias_desc Diff bias memory descriptor.
        /// @param diff_dst_layer_desc Memory descriptor for the diff of
        ///     output vector.
        /// @param diff_dst_iter_desc Memory descriptor for the diff of output
        ///     recurrent hidden state vector.
        /// @param flags Unused.
        /// @param alpha Negative slope if activation is
        ///     #dnnl::algorithm::eltwise_relu.
        /// @param beta Unused.
        desc(prop_kind prop_kind, algorithm activation, rnn_direction direction,
                const memory::desc &src_layer_desc,
                const memory::desc &src_iter_desc,
                const memory::desc &weights_layer_desc,
                const memory::desc &weights_iter_desc,
                const memory::desc &bias_desc,
                const memory::desc &dst_layer_desc,
                const memory::desc &dst_iter_desc,
                const memory::desc &diff_src_layer_desc,
                const memory::desc &diff_src_iter_desc,
                const memory::desc &diff_weights_layer_desc,
                const memory::desc &diff_weights_iter_desc,
                const memory::desc &diff_bias_desc,
                const memory::desc &diff_dst_layer_desc,
                const memory::desc &diff_dst_iter_desc,
                rnn_flags flags = rnn_flags::undef, float alpha = 0.0f,
                float beta = 0.0f);
    };

    /// Primitive descriptor for an RNN backward propagation primitive.
    struct primitive_desc : public rnn_primitive_desc_base {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a vanilla RNN backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a vanilla RNN backward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a vanilla RNN
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const vanilla_rnn_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a vanilla RNN backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a vanilla RNN backward propagation
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a vanilla RNN
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const vanilla_rnn_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::rnn_primitive_desc_base::src_layer_desc()const
        memory::desc src_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::src_iter_desc()const
        memory::desc src_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_layer_desc()const
        memory::desc weights_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_iter_desc()const
        memory::desc weights_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::bias_desc()const
        memory::desc bias_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_layer_desc()const
        memory::desc dst_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_iter_desc()const
        memory::desc dst_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_src_layer_desc()const
        memory::desc diff_src_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_src_iter_desc()const
        memory::desc diff_src_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_weights_layer_desc()const
        memory::desc diff_weights_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_weights_iter_desc()const
        memory::desc diff_weights_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_bias_desc()const
        memory::desc diff_bias_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_dst_layer_desc()const
        memory::desc diff_dst_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_dst_iter_desc()const
        memory::desc diff_dst_iter_desc() const;
    };

    /// Default constructor. Produces an empty object.
    vanilla_rnn_backward();

    /// Constructs a vanilla RNN backward propagation primitive.
    /// @param pd Primitive descriptor for a vanilla RNN backward
    ///     propagation primitive.
    vanilla_rnn_backward(const primitive_desc &pd);
};

/// LSTM forward propagation primitive.
struct lstm_forward : public primitive {
    /// Descriptor for an LSTM forward propagation primitive.
    struct desc {
        /// Constructs a descriptor for an LSTM (with or without peephole and
        /// with or without projection) forward propagation primitive.
        ///
        /// The @p src_iter_desc, @p src_iter_c_desc, @p weights_peephole_desc,
        /// @p bias_desc, @p dst_iter_desc, and @p dst_iter_c_desc may point to
        /// a zero memory descriptor. This would then indicate that the LSTM
        /// forward propagation primitive should not use them and should
        /// default to zero values instead.
        ///
        /// The @p weights_projection_desc may point to a zero memory
        /// descriptor. This would then indicate that the LSTM doesn't have
        /// recurrent projection layer.
        ///
        /// @note
        ///     All memory descriptors can be initialized with an
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// Inputs:
        ///  - src_layer (#dnnl::primitive_desc_base::src_desc (0))
        ///  - src_iter (#dnnl::primitive_desc_base::src_desc (1)), if used
        ///  - src_iter_c (#dnnl::primitive_desc_base::src_desc (2)), if used
        ///  - weights_layer (#dnnl::primitive_desc_base::weights_desc (0))
        ///  - weights_iter (#dnnl::primitive_desc_base::weights_desc (1))
        ///  - weights_peephole (#dnnl::primitive_desc_base::weights_desc (2)),
        ///    if used
        ///  - weights_projection
        ///    (#dnnl::primitive_desc_base::weights_desc (index)), if used and
        ///    index is:
        ///    - 2, if there is no weights_peephole
        ///    - 3, otherwise
        ///  - bias (#dnnl::primitive_desc_base::weights_desc (index)), if used
        ///    and index is:
        ///    - 2, if neither weights_peephole nor weights_projection is used
        ///    - 3, if one of weights_peephole or weights_projection is used
        ///    - 4, if both weights_peephole and weights_projection are used
        ///
        /// Outputs:
        ///  - dst_layer (#dnnl::primitive_desc_base::dst_desc (0))
        ///  - dst_iter (#dnnl::primitive_desc_base::dst_desc (1)), if used
        ///  - dst_iter_c (#dnnl::primitive_desc_base::dst_desc (2)), if used
        ///  - workspace (#dnnl::primitive_desc_base::workspace_desc (0)),
        ///     if @p prop_kind equals #dnnl::prop_kind::forward_training;
        ///     must be queried for using @ref
        ///     dnnl::primitive_desc_base::query_md() after a corresponding
        ///     primitive descriptor is created
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param direction RNN direction. See @ref dnnl::rnn_direction for
        ///     more info.
        /// @param src_layer_desc Memory descriptor for the input vector.
        /// @param src_iter_desc Memory descriptor for the input recurrent
        ///     hidden state vector.
        /// @param src_iter_c_desc Memory descriptor for the input recurrent
        ///     cell state vector.
        /// @param weights_layer_desc Memory descriptor for the weights
        ///     applied to the layer input.
        /// @param weights_iter_desc Memory descriptor for the weights applied
        ///     to the recurrent input.
        /// @param weights_peephole_desc Memory descriptor for the weights
        ///     applied to the cell states (according to the Peephole LSTM
        ///     formula).
        /// @param weights_projection_desc Memory descriptor for the weights
        ///     applied to the hidden states to get the recurrent projection
        ///     (according to the Projection LSTM formula).
        /// @param bias_desc Bias memory descriptor.
        /// @param dst_layer_desc Memory descriptor for the output vector.
        /// @param dst_iter_desc Memory descriptor for the output recurrent
        ///     hidden state vector.
        /// @param dst_iter_c_desc Memory descriptor for the output recurrent
        ///     cell state vector.
        /// @param flags Unused.
        desc(prop_kind prop_kind, rnn_direction direction,
                const memory::desc &src_layer_desc,
                const memory::desc &src_iter_desc,
                const memory::desc &src_iter_c_desc,
                const memory::desc &weights_layer_desc,
                const memory::desc &weights_iter_desc,
                const memory::desc &weights_peephole_desc,
                const memory::desc &weights_projection_desc,
                const memory::desc &bias_desc,
                const memory::desc &dst_layer_desc,
                const memory::desc &dst_iter_desc,
                const memory::desc &dst_iter_c_desc,
                rnn_flags flags = rnn_flags::undef);

        /// Constructs a descriptor for an LSTM (with or without peephole)
        /// forward propagation primitive.
        ///
        /// The @p src_iter_desc, @p src_iter_c_desc, @p weights_peephole_desc,
        /// @p bias_desc, @p dst_iter_desc, and @p dst_iter_c_desc may point to
        /// a zero memory descriptor. This would then indicate that the LSTM
        /// forward propagation primitive should not use them and should
        /// default to zero values instead.
        ///
        /// Inputs:
        ///  - `src_layer` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `src_iter` (#dnnl::primitive_desc_base::src_desc(`1`)), if used
        ///  - `src_iter_c` (#dnnl::primitive_desc_base::src_desc(`2`)), if used
        ///  - `weights_layer` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `weights_iter` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///  - `weights_peephole` (#dnnl::primitive_desc_base::weights_desc(`2`)),
        ///    if used
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`2`)), if used and
        ///    LSTM is without peephole
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`3`)), if used and
        ///    LSTM is with peephole
        ///
        /// Outputs:
        ///  - `dst_layer` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `dst_iter` (#dnnl::primitive_desc_base::dst_desc(`1`)), if used
        ///  - `dst_iter_c` (#dnnl::primitive_desc_base::dst_desc(`2`)), if used
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`)),
        ///     if @p prop_kind equals #dnnl::prop_kind::forward_training;
        ///     must be queried for using @ref
        ///     dnnl::primitive_desc_base::query_md() after a corresponding
        ///     primitive descriptor is created
        ///
        /// @note
        ///     All memory descriptors can be initialized with an
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param direction RNN direction. See @ref dnnl::rnn_direction for
        ///     more info.
        /// @param src_layer_desc Memory descriptor for the input vector.
        /// @param src_iter_desc Memory descriptor for the input recurrent
        ///     hidden state vector.
        /// @param src_iter_c_desc Memory descriptor for the input recurrent
        ///     cell state vector.
        /// @param weights_layer_desc Memory descriptor for the weights
        ///     applied to the layer input.
        /// @param weights_iter_desc Memory descriptor for the weights applied
        ///     to the recurrent input.
        /// @param weights_peephole_desc Memory descriptor for the weights
        ///     applied to the cell states (according to the Peephole LSTM
        ///     formula).
        /// @param bias_desc Bias memory descriptor.
        /// @param dst_layer_desc Memory descriptor for the output vector.
        /// @param dst_iter_desc Memory descriptor for the output recurrent
        ///     hidden state vector.
        /// @param dst_iter_c_desc Memory descriptor for the output recurrent
        ///     cell state vector.
        /// @param flags Unused.
        desc(prop_kind prop_kind, rnn_direction direction,
                const memory::desc &src_layer_desc,
                const memory::desc &src_iter_desc,
                const memory::desc &src_iter_c_desc,
                const memory::desc &weights_layer_desc,
                const memory::desc &weights_iter_desc,
                const memory::desc &weights_peephole_desc,
                const memory::desc &bias_desc,
                const memory::desc &dst_layer_desc,
                const memory::desc &dst_iter_desc,
                const memory::desc &dst_iter_c_desc,
                rnn_flags flags = rnn_flags::undef);

        /// Constructs a descriptor for an LSTM forward propagation primitive.
        ///
        /// The @p src_iter_desc, @p src_iter_c_desc, @p bias_desc, @p
        /// dst_iter_desc, and @p dst_iter_c_desc may point to a zero memory
        /// descriptor. This would then indicate that the LSTM forward
        /// propagation primitive should not use them and should default to
        /// zero values instead.
        ///
        /// Inputs:
        ///  - `src_layer` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `src_iter` (#dnnl::primitive_desc_base::src_desc(`1`)), if used
        ///  - `src_iter_c` (#dnnl::primitive_desc_base::src_desc(`2`)), if used
        ///  - `weights_layer` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `weights_iter` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`2`)), if used
        ///
        /// Outputs:
        ///  - `dst_layer` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `dst_iter` (#dnnl::primitive_desc_base::dst_desc(`1`)), if used
        ///  - `dst_iter_c` (#dnnl::primitive_desc_base::dst_desc(`2`)), if used
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`)),
        ///     if @p prop_kind equals #dnnl::prop_kind::forward_training;
        ///     must be queried for using @ref
        ///     dnnl::primitive_desc_base::query_md() after a
        ///     corresponding primitive descriptor is created
        ///
        /// @note
        ///     All memory descriptors can be initialized with an
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param direction RNN direction. See @ref dnnl::rnn_direction for
        ///     more info.
        /// @param src_layer_desc Memory descriptor for the input vector.
        /// @param src_iter_desc Memory descriptor for the input recurrent
        ///     hidden state vector.
        /// @param src_iter_c_desc Memory descriptor for the input recurrent
        ///     cell state vector.
        /// @param weights_layer_desc Memory descriptor for the weights
        ///     applied to the layer input.
        /// @param weights_iter_desc Memory descriptor for the weights applied
        ///     to the recurrent input.
        /// @param bias_desc Bias memory descriptor.
        /// @param dst_layer_desc Memory descriptor for the output vector.
        /// @param dst_iter_desc Memory descriptor for the output recurrent
        ///     hidden state vector.
        /// @param dst_iter_c_desc Memory descriptor for the output recurrent
        ///     cell state vector.
        /// @param flags Unused.
        desc(prop_kind prop_kind, rnn_direction direction,
                const memory::desc &src_layer_desc,
                const memory::desc &src_iter_desc,
                const memory::desc &src_iter_c_desc,
                const memory::desc &weights_layer_desc,
                const memory::desc &weights_iter_desc,
                const memory::desc &bias_desc,
                const memory::desc &dst_layer_desc,
                const memory::desc &dst_iter_desc,
                const memory::desc &dst_iter_c_desc,
                rnn_flags flags = rnn_flags::undef);
    };

    /// Primitive descriptor for an LSTM forward propagation primitive.
    struct primitive_desc : public rnn_primitive_desc_base {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for an LSTM forward propagation
        /// primitive.
        ///
        /// @param desc Descriptor for an LSTM forward propagation primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for an LSTM forward propagation
        /// primitive.
        ///
        /// @param desc Descriptor for an LSTM forward propagation primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::rnn_primitive_desc_base::src_layer_desc()const
        memory::desc src_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::src_iter_desc()const
        memory::desc src_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::src_iter_desc()const
        memory::desc src_iter_c_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_layer_desc()const
        memory::desc weights_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_iter_desc()const
        memory::desc weights_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::bias_desc()const
        memory::desc bias_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_layer_desc()const
        memory::desc dst_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_iter_desc()const
        memory::desc dst_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::src_iter_desc()const
        memory::desc dst_iter_c_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;
    };

    /// Default constructor. Produces an empty object.
    lstm_forward();

    /// Constructs an LSTM forward propagation primitive.
    /// @param pd Primitive descriptor for an LSTM forward propagation
    ///     primitive.
    lstm_forward(const primitive_desc &pd);
};

/// LSTM backward propagation primitive.
struct lstm_backward : public primitive {
    /// Descriptor for an LSTM backward propagation primitive.
    struct desc {
        /// projection) descriptor for backward propagation using @p prop_kind,
        /// @p direction, and memory descriptors.
        ///
        /// The @p src_iter_desc together with @p diff_iter_desc, @p
        /// src_iter_c_desc together with @p src_iter_c_desc, @p
        /// weights_peephole_desc together with @p diff_weights_peephole_desc,
        /// @p bias_desc together with @p diff_bias_desc, @p dst_iter_desc
        /// together with @p diff_dst_iter_desc, and @p dst_iter_c_desc
        /// together with @p diff_dst_iter_c_desc, may point to a zero memory
        /// descriptor. This would then indicate that the LSTM backward
        /// propagation primitive should not use them and should default to
        /// zero values instead.
        ///
        /// The @p weights_projection_desc together with @p
        /// diff_weights_projection_desc may point to a zero memory descriptor.
        /// This would then indicate that the LSTM doesn't have recurrent
        /// projection layer.
        ///
        /// @note
        ///     All memory descriptors can be initialized with
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// Inputs:
        ///  - src_layer (#dnnl::primitive_desc_base::src_desc (0))
        ///  - src_iter (#dnnl::primitive_desc_base::src_desc (1)), if used
        ///  - src_iter_c (#dnnl::primitive_desc_base::src_desc (2)), if used
        ///  - weights_layer (#dnnl::primitive_desc_base::weights_desc (0))
        ///  - weights_iter (#dnnl::primitive_desc_base::weights_desc (1))
        ///  - weights_peephole (#dnnl::primitive_desc_base::weights_desc (2)),
        ///    if used
        ///  - weights_projection
        ///    (#dnnl::primitive_desc_base::weights_desc (index)), if used and
        ///    index is:
        ///    - 2, if there is no weights_peephole
        ///    - 3, otherwise
        ///  - bias (#dnnl::primitive_desc_base::weights_desc (index)), if used
        ///    and index is:
        ///    - 2, if neither weights_peephole nor weights_projection is used
        ///    - 3, if one of weights_peephole or weights_projection is used
        ///    - 4, if both weights_peephole and weights_projection are used
        ///  - dst_layer (#dnnl::primitive_desc_base::dst_desc (0))
        ///  - dst_iter (#dnnl::primitive_desc_base::dst_desc (1)), if used
        ///  - dst_iter_c (#dnnl::primitive_desc_base::dst_desc (2)), if used
        ///  - diff_dst_layer (#dnnl::primitive_desc_base::diff_dst_desc (0))
        ///  - diff_dst_iter
        ///     (#dnnl::primitive_desc_base::diff_dst_desc (1)), if used
        ///  - diff_dst_iter_c
        ///     (#dnnl::primitive_desc_base::diff_dst_desc (2)), if used
        ///  - workspace (#dnnl::primitive_desc_base::workspace_desc (0))
        ///
        /// Outputs:
        ///  - diff_src_layer (#dnnl::primitive_desc_base::diff_src_desc (0))
        ///  - diff_src_iter
        ///     (#dnnl::primitive_desc_base::diff_src_desc (1)), if used
        ///  - diff_src_iter_c
        ///     (#dnnl::primitive_desc_base::diff_src_desc (2)), if used
        ///  - diff_weights_layer
        ///     (#dnnl::primitive_desc_base::diff_weights_desc (0))
        ///  - diff_weights_iter
        ///     (#dnnl::primitive_desc_base::diff_weights_desc (1))
        ///  - diff_weights_peephole
        ///    (#dnnl::primitive_desc_base::diff_weights_desc (2)), if used
        ///  - diff_weights_projection
        ///    (#dnnl::primitive_desc_base::diff_weights_desc (index)), if used
        ///    and index is:
        ///    - 2, if there is no diff_weights_peephole
        ///    - 3, otherwise
        ///  - diff_bias
        ///    (#dnnl::primitive_desc_base::diff_weights_desc (index)), if used
        ///    and index is:
        ///    - 2, if neither diff_weights_peephole nor
        ///         diff_weights_projection is used
        ///    - 3, if one of diff_weights_peephole or diff_weights_projection
        ///         is used
        ///    - 4, if both diff_weights_peephole and diff_weights_projection
        ///         are used
        ///
        /// @param prop_kind Propagation kind. Must be
        ///     #dnnl::prop_kind::backward.
        /// @param direction RNN direction. See @ref dnnl::rnn_direction for
        ///     more info.
        /// @param src_layer_desc Memory descriptor for the input vector.
        /// @param src_iter_desc Memory descriptor for the input recurrent
        ///     hidden state vector.
        /// @param src_iter_c_desc Memory descriptor for the input recurrent
        ///     cell state vector.
        /// @param weights_layer_desc Memory descriptor for the weights
        ///     applied to the layer input.
        /// @param weights_iter_desc Memory descriptor for the weights applied
        ///     to the recurrent input.
        /// @param weights_peephole_desc Memory descriptor for the weights
        ///     applied to the cell states (according to the Peephole LSTM
        ///     formula).
        /// @param weights_projection_desc Memory descriptor for the weights
        ///     applied to the hidden states to get the recurrent projection
        ///     (according to the Projection LSTM formula).
        /// @param bias_desc Bias memory descriptor.
        /// @param dst_layer_desc Memory descriptor for the output vector.
        /// @param dst_iter_desc Memory descriptor for the output recurrent
        ///     hidden state vector.
        /// @param dst_iter_c_desc Memory descriptor for the output recurrent
        ///     cell state vector.
        /// @param diff_src_layer_desc Memory descriptor for the diff of input
        ///     vector.
        /// @param diff_src_iter_desc Memory descriptor for the diff of input
        ///     recurrent hidden state vector.
        /// @param diff_src_iter_c_desc Memory descriptor for the diff of
        ///     input recurrent cell state vector.
        /// @param diff_weights_layer_desc Memory descriptor for the diff of
        ///     weights applied to the layer input.
        /// @param diff_weights_iter_desc Memory descriptor for the diff of
        ///     weights applied to the recurrent input.
        /// @param diff_weights_peephole_desc Memory descriptor for the diff of
        ///     weights applied to the cell states (according to the Peephole
        ///     LSTM formula).
        /// @param diff_weights_projection_desc Memory descriptor for the diff
        ///     of weights applied to the hidden states to get the recurrent
        ///     projection (according to the Projection LSTM formula).
        /// @param diff_bias_desc Diff bias memory descriptor.
        /// @param diff_dst_layer_desc Memory descriptor for the diff of
        ///     output vector.
        /// @param diff_dst_iter_desc Memory descriptor for the diff of output
        ///     recurrent hidden state vector.
        /// @param diff_dst_iter_c_desc Memory descriptor for the diff of
        ///     output recurrent cell state vector.
        /// @param flags Unused.
        desc(prop_kind prop_kind, rnn_direction direction,
                const memory::desc &src_layer_desc,
                const memory::desc &src_iter_desc,
                const memory::desc &src_iter_c_desc,
                const memory::desc &weights_layer_desc,
                const memory::desc &weights_iter_desc,
                const memory::desc &weights_peephole_desc,
                const memory::desc &weights_projection_desc,
                const memory::desc &bias_desc,
                const memory::desc &dst_layer_desc,
                const memory::desc &dst_iter_desc,
                const memory::desc &dst_iter_c_desc,
                const memory::desc &diff_src_layer_desc,
                const memory::desc &diff_src_iter_desc,
                const memory::desc &diff_src_iter_c_desc,
                const memory::desc &diff_weights_layer_desc,
                const memory::desc &diff_weights_iter_desc,
                const memory::desc &diff_weights_peephole_desc,
                const memory::desc &diff_weights_projection_desc,
                const memory::desc &diff_bias_desc,
                const memory::desc &diff_dst_layer_desc,
                const memory::desc &diff_dst_iter_desc,
                const memory::desc &diff_dst_iter_c_desc,
                rnn_flags flags = rnn_flags::undef);

        /// Constructs an LSTM (with or without peephole) descriptor for
        /// backward propagation using @p prop_kind, @p direction, and memory
        /// descriptors.
        ///
        /// The @p src_iter_desc together with @p diff_iter_desc, @p
        /// src_iter_c_desc together with @p src_iter_c_desc, @p
        /// weights_peephole_desc together with @p diff_weights_peephole_desc,
        /// @p bias_desc together with @p diff_bias_desc, @p dst_iter_desc
        /// together with @p diff_dst_iter_desc, and @p dst_iter_c_desc
        /// together with @p diff_dst_iter_c_desc, may point to a zero memory
        /// descriptor. This would then indicate that the LSTM backward
        /// propagation primitive should not use them and should default to
        /// zero values instead.
        ///
        /// @note
        ///     All memory descriptors may be initialized with
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// Inputs:
        ///  - `src_layer` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `src_iter` (#dnnl::primitive_desc_base::src_desc(`1`)), if used
        ///  - `src_iter_c` (#dnnl::primitive_desc_base::src_desc(`2`)), if used
        ///  - `weights_layer` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `weights_iter` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///  - `weights_peephole` (#dnnl::primitive_desc_base::weights_desc(`2`)),
        ///    if used
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`2`)), if used and
        ///    LSTM is without peephole
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`3`)), if used and
        ///    LSTM is with peephole
        ///  - `dst_layer` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `dst_iter` (#dnnl::primitive_desc_base::dst_desc(`1`)), if used
        ///  - `dst_iter_c` (#dnnl::primitive_desc_base::dst_desc(`2`)), if used
        ///  - `diff_dst_layer` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `diff_dst_iter`
        ///     (#dnnl::primitive_desc_base::diff_dst_desc(`1`)), if used
        ///  - `diff_dst_iter_c`
        ///     (#dnnl::primitive_desc_base::diff_dst_desc(`2`)), if used
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src_layer` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///  - `diff_src_iter`
        ///     (#dnnl::primitive_desc_base::diff_src_desc(`1`)), if used
        ///  - `diff_src_iter_c`
        ///     (#dnnl::primitive_desc_base::diff_src_desc(`2`)), if used
        ///  - `diff_weights_layer`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///  - `diff_weights_iter`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`1`))
        ///  - `diff_weights_peephole`
        ///    (#dnnl::primitive_desc_base::diff_weights_desc(`2`)), if used
        ///  - `diff_bias` (#dnnl::primitive_desc_base::diff_weights_desc(`2`)),
        ///    if used and LSTM is without peephole
        ///  - `diff_bias` (#dnnl::primitive_desc_base::diff_weights_desc(`3`)),
        ///    if used and LSTM is with peephole
        ///
        /// @param prop_kind Propagation kind. Must be
        ///     #dnnl::prop_kind::backward.
        /// @param direction RNN direction. See @ref dnnl::rnn_direction for
        ///     more info.
        /// @param src_layer_desc Memory descriptor for the input vector.
        /// @param src_iter_desc Memory descriptor for the input recurrent
        ///     hidden state vector.
        /// @param src_iter_c_desc Memory descriptor for the input recurrent
        ///     cell state vector.
        /// @param weights_layer_desc Memory descriptor for the weights
        ///     applied to the layer input.
        /// @param weights_iter_desc Memory descriptor for the weights applied
        ///     to the recurrent input.
        /// @param weights_peephole_desc Memory descriptor for the weights
        ///     applied to the cell states (according to the Peephole LSTM
        ///     formula).
        /// @param bias_desc Bias memory descriptor.
        /// @param dst_layer_desc Memory descriptor for the output vector.
        /// @param dst_iter_desc Memory descriptor for the output recurrent
        ///     hidden state vector.
        /// @param dst_iter_c_desc Memory descriptor for the output recurrent
        ///     cell state vector.
        /// @param diff_src_layer_desc Memory descriptor for the diff of input
        ///     vector.
        /// @param diff_src_iter_desc Memory descriptor for the diff of input
        ///     recurrent hidden state vector.
        /// @param diff_src_iter_c_desc Memory descriptor for the diff of
        ///     input recurrent cell state vector.
        /// @param diff_weights_layer_desc Memory descriptor for the diff of
        ///     weights applied to the layer input.
        /// @param diff_weights_iter_desc Memory descriptor for the diff of
        ///     weights applied to the recurrent input.
        /// @param diff_weights_peephole_desc Memory descriptor for the diff of
        ///     weights applied to the cell states (according to the Peephole
        ///     LSTM formula).
        /// @param diff_bias_desc Diff bias memory descriptor.
        /// @param diff_dst_layer_desc Memory descriptor for the diff of
        ///     output vector.
        /// @param diff_dst_iter_desc Memory descriptor for the diff of output
        ///     recurrent hidden state vector.
        /// @param diff_dst_iter_c_desc Memory descriptor for the diff of
        ///     output recurrent cell state vector.
        /// @param flags Unused.
        desc(prop_kind prop_kind, rnn_direction direction,
                const memory::desc &src_layer_desc,
                const memory::desc &src_iter_desc,
                const memory::desc &src_iter_c_desc,
                const memory::desc &weights_layer_desc,
                const memory::desc &weights_iter_desc,
                const memory::desc &weights_peephole_desc,
                const memory::desc &bias_desc,
                const memory::desc &dst_layer_desc,
                const memory::desc &dst_iter_desc,
                const memory::desc &dst_iter_c_desc,
                const memory::desc &diff_src_layer_desc,
                const memory::desc &diff_src_iter_desc,
                const memory::desc &diff_src_iter_c_desc,
                const memory::desc &diff_weights_layer_desc,
                const memory::desc &diff_weights_iter_desc,
                const memory::desc &diff_weights_peephole_desc,
                const memory::desc &diff_bias_desc,
                const memory::desc &diff_dst_layer_desc,
                const memory::desc &diff_dst_iter_desc,
                const memory::desc &diff_dst_iter_c_desc,
                rnn_flags flags = rnn_flags::undef);

        /// Constructs an LSTM descriptor for backward propagation using @p
        /// prop_kind, @p direction, and memory descriptors.
        ///
        /// The @p src_iter_desc together with @p diff_iter_desc, @p
        /// src_iter_c_desc together with @p src_iter_c_desc, @p bias_desc
        /// together with @p diff_bias_desc, @p dst_iter_desc together with @p
        /// diff_dst_iter_desc, and @p dst_iter_c_desc together with @p
        /// diff_dst_iter_c_desc, may point to a zero memory descriptor. This
        /// would then indicate that the LSTM backward propagation primitive
        /// should not use them and should default to zero values instead.
        ///
        /// @note
        ///     All memory descriptors may be initialized with
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// Inputs:
        ///  - `src_layer` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `src_iter` (#dnnl::primitive_desc_base::src_desc(`1`)), if used
        ///  - `src_iter_c` (#dnnl::primitive_desc_base::src_desc(`2`)), if used
        ///  - `weights_layer` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `weights_iter` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`2`)), if used
        ///  - `dst_layer` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `dst_iter` (#dnnl::primitive_desc_base::dst_desc(`1`)), if used
        ///  - `dst_iter_c` (#dnnl::primitive_desc_base::dst_desc(`2`)), if used
        ///  - `diff_dst_layer` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `diff_dst_iter`
        ///     (#dnnl::primitive_desc_base::diff_dst_desc(`1`)), if used
        ///  - `diff_dst_iter_c`
        ///     (#dnnl::primitive_desc_base::diff_dst_desc(`2`)), if used
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src_layer` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///  - `diff_src_iter`
        ///     (#dnnl::primitive_desc_base::diff_src_desc(`1`)), if used
        ///  - `diff_src_iter_c`
        ///     (#dnnl::primitive_desc_base::diff_src_desc(`2`)), if used
        ///  - `diff_weights_layer`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///  - `diff_weights_iter`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`1`))
        ///  - `diff_bias`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`2`)), if used
        ///
        /// @param prop_kind Propagation kind. Must be
        ///     #dnnl::prop_kind::backward.
        /// @param direction RNN direction. See @ref dnnl::rnn_direction for
        ///     more info.
        /// @param src_layer_desc Memory descriptor for the input vector.
        /// @param src_iter_desc Memory descriptor for the input recurrent
        ///     hidden state vector.
        /// @param src_iter_c_desc Memory descriptor for the input recurrent
        ///     cell state vector.
        /// @param weights_layer_desc Memory descriptor for the weights
        ///     applied to the layer input.
        /// @param weights_iter_desc Memory descriptor for the weights applied
        ///     to the recurrent input.
        /// @param bias_desc Bias memory descriptor.
        /// @param dst_layer_desc Memory descriptor for the output vector.
        /// @param dst_iter_desc Memory descriptor for the output recurrent
        ///     hidden state vector.
        /// @param dst_iter_c_desc Memory descriptor for the output recurrent
        ///     cell state vector.
        /// @param diff_src_layer_desc Memory descriptor for the diff of input
        ///     vector.
        /// @param diff_src_iter_desc Memory descriptor for the diff of input
        ///     recurrent hidden state vector.
        /// @param diff_src_iter_c_desc Memory descriptor for the diff of
        ///     input recurrent cell state vector.
        /// @param diff_weights_layer_desc Memory descriptor for the diff of
        ///     weights applied to the layer input.
        /// @param diff_weights_iter_desc Memory descriptor for the diff of
        ///     weights applied to the recurrent input.
        /// @param diff_bias_desc Diff bias memory descriptor.
        /// @param diff_dst_layer_desc Memory descriptor for the diff of
        ///     output vector.
        /// @param diff_dst_iter_desc Memory descriptor for the diff of output
        ///     recurrent hidden state vector.
        /// @param diff_dst_iter_c_desc Memory descriptor for the diff of
        ///     output recurrent cell state vector.
        /// @param flags Unused.
        desc(prop_kind prop_kind, rnn_direction direction,
                const memory::desc &src_layer_desc,
                const memory::desc &src_iter_desc,
                const memory::desc &src_iter_c_desc,
                const memory::desc &weights_layer_desc,
                const memory::desc &weights_iter_desc,
                const memory::desc &bias_desc,
                const memory::desc &dst_layer_desc,
                const memory::desc &dst_iter_desc,
                const memory::desc &dst_iter_c_desc,
                const memory::desc &diff_src_layer_desc,
                const memory::desc &diff_src_iter_desc,
                const memory::desc &diff_src_iter_c_desc,
                const memory::desc &diff_weights_layer_desc,
                const memory::desc &diff_weights_iter_desc,
                const memory::desc &diff_bias_desc,
                const memory::desc &diff_dst_layer_desc,
                const memory::desc &diff_dst_iter_desc,
                const memory::desc &diff_dst_iter_c_desc,
                rnn_flags flags = rnn_flags::undef);
    };

    /// Primitive descriptor for LSTM backward propagation.
    struct primitive_desc : public rnn_primitive_desc_base {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for an LSTM backward propagation
        /// primitive.
        ///
        /// @param desc Descriptor for LSTM backward propagation primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for an LSTM
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const lstm_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for an LSTM backward propagation
        /// primitive.
        ///
        /// @param desc Descriptor for an LSTM backward propagation primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for an LSTM
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const lstm_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::rnn_primitive_desc_base::src_layer_desc()const
        memory::desc src_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::src_iter_desc()const
        memory::desc src_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::src_iter_desc()const
        memory::desc src_iter_c_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_layer_desc()const
        memory::desc weights_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_iter_desc()const
        memory::desc weights_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::bias_desc()const
        memory::desc bias_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_layer_desc()const
        memory::desc dst_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_iter_desc()const
        memory::desc dst_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::src_iter_desc()const
        memory::desc dst_iter_c_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_src_layer_desc()const
        memory::desc diff_src_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_src_iter_desc()const
        memory::desc diff_src_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_src_iter_c_desc()const
        memory::desc diff_src_iter_c_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_weights_layer_desc()const
        memory::desc diff_weights_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_weights_iter_desc()const
        memory::desc diff_weights_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_bias_desc()const
        memory::desc diff_bias_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_dst_layer_desc()const
        memory::desc diff_dst_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_dst_iter_desc()const
        memory::desc diff_dst_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_dst_iter_c_desc()const
        memory::desc diff_dst_iter_c_desc() const;
    };

    /// Default constructor. Produces an empty object.
    lstm_backward();

    /// Constructs an LSTM backward propagation primitive.
    /// @param pd Primitive descriptor for an LSTM backward propagation
    ///     primitive.
    lstm_backward(const primitive_desc &pd);
};

/// GRU forward propagation primitive.
struct gru_forward : public primitive {
    /// Descriptor for a GRU forward propagation primitive.
    struct desc {
        /// Constructs a descriptor for a GRU forward propagation primitive.
        ///
        /// The @p src_iter_desc, @p bias_desc, and @p dst_iter, may point to
        /// a zero memory descriptor. This would then indicate that the GRU
        /// forward propagation primitive should not use them and should
        /// default to zero values instead.
        ///
        /// Inputs:
        ///  - `src_layer` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `src_iter` (#dnnl::primitive_desc_base::src_desc(`1`)), if used
        ///  - `weights_layer` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `weights_iter` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`2`)), if used
        ///
        /// Outputs:
        ///  - `dst_layer` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `dst_iter` (#dnnl::primitive_desc_base::dst_desc(`1`)), if used
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`)),
        ///     if @p prop_kind equals #dnnl::prop_kind::forward_training;
        ///     must be queried for using @ref
        ///     dnnl::primitive_desc_base::query_md() after a corresponding
        ///     primitive descriptor is created
        ///
        /// @note
        ///     All memory descriptors except @p src_iter_desc may be
        ///     initialized with an #dnnl::memory::format_tag::any value of @p
        ///     format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param direction RNN direction. See @ref dnnl::rnn_direction for
        ///     more info.
        /// @param src_layer_desc Memory descriptor for the input vector.
        /// @param src_iter_desc Memory descriptor for the input recurrent
        ///     hidden state vector.
        /// @param weights_layer_desc Memory descriptor for the weights
        ///     applied to the layer input.
        /// @param weights_iter_desc Memory descriptor for the weights applied
        ///     to the recurrent input.
        /// @param bias_desc Bias memory descriptor.
        /// @param dst_layer_desc Memory descriptor for the output vector.
        /// @param dst_iter_desc Memory descriptor for the output recurrent
        ///     hidden state vector.
        /// @param flags Unused.
        desc(prop_kind prop_kind, rnn_direction direction,
                const memory::desc &src_layer_desc,
                const memory::desc &src_iter_desc,
                const memory::desc &weights_layer_desc,
                const memory::desc &weights_iter_desc,
                const memory::desc &bias_desc,
                const memory::desc &dst_layer_desc,
                const memory::desc &dst_iter_desc,
                rnn_flags flags = rnn_flags::undef);
    };

    /// Primitive descriptor GRU forward propagation primitive.
    struct primitive_desc : public rnn_primitive_desc_base {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a GRU forward propagation
        /// primitive.
        ///
        /// @param desc Descriptor for a GRU forward propagation primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a GRU forward propagation
        /// primitive.
        ///
        /// @param desc Descriptor for a GRU forward propagation primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::rnn_primitive_desc_base::src_layer_desc()const
        memory::desc src_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::src_iter_desc()const
        memory::desc src_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_layer_desc()const
        memory::desc weights_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_iter_desc()const
        memory::desc weights_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::bias_desc()const
        memory::desc bias_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_layer_desc()const
        memory::desc dst_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_iter_desc()const
        memory::desc dst_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;
    };

    /// Default constructor. Produces an empty object.
    gru_forward();

    /// Constructs a GRU forward propagation primitive.
    /// @param pd Primitive descriptor for a GRU forward propagation
    ///     primitive.
    gru_forward(const primitive_desc &pd);
};

/// GRU backward propagation primitive.
struct gru_backward : public primitive {
    /// Descriptor for a GRU backward propagation primitive.
    struct desc {
        /// Constructs a descriptor for a GRU backward propagation primitive.
        ///
        /// The @p src_iter_desc together with @p diff_src_iter_desc, @p
        /// bias_desc together with @p diff_bias_desc, and @p dst_iter
        /// together with @p diff_dst_iter, may point to a zero memory
        /// descriptor.  This would then indicate that the GRU backward
        /// propagation primitive should not use them and should default to
        /// zero values instead.
        ///
        /// Inputs:
        ///  - `src_layer` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `src_iter` (#dnnl::primitive_desc_base::src_desc(`1`)), if used
        ///  - `weights_layer` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `weights_iter` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`2`)), if used
        ///  - `dst_layer` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `dst_iter` (#dnnl::primitive_desc_base::dst_desc(`1`)), if used
        ///  - `diff_dst_layer` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `diff_dst_iter`
        ///     (#dnnl::primitive_desc_base::diff_dst_desc(`1`)), if used
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src_layer` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///  - `diff_src_iter`
        ///     (#dnnl::primitive_desc_base::diff_src_desc(`1`)), if used
        ///  - `diff_weights_layer`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///  - `diff_weights_iter`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`1`))
        ///  - `diff_bias`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`2`)), if used
        ///
        /// @note
        ///     All memory descriptors may be initialized with
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Must be
        ///     #dnnl::prop_kind::backward.
        /// @param direction RNN direction. See @ref dnnl::rnn_direction for
        ///     more info.
        /// @param src_layer_desc Memory descriptor for the input vector.
        /// @param src_iter_desc Memory descriptor for the input recurrent
        ///     hidden state vector.
        /// @param weights_layer_desc Memory descriptor for the weights
        ///     applied to the layer input.
        /// @param weights_iter_desc Memory descriptor for the weights applied
        ///     to the recurrent input.
        /// @param bias_desc Bias memory descriptor.
        /// @param dst_layer_desc Memory descriptor for the output vector.
        /// @param dst_iter_desc Memory descriptor for the output recurrent
        ///     hidden state vector.
        /// @param diff_src_layer_desc Memory descriptor for the diff of input
        ///     vector.
        /// @param diff_src_iter_desc Memory descriptor for the diff of input
        ///     recurrent hidden state vector.
        /// @param diff_weights_layer_desc Memory descriptor for the diff of
        ///     weights applied to the layer input.
        /// @param diff_weights_iter_desc Memory descriptor for the diff of
        ///     weights applied to the recurrent input.
        /// @param diff_bias_desc Diff bias memory descriptor.
        /// @param diff_dst_layer_desc Memory descriptor for the diff of
        ///     output vector.
        /// @param diff_dst_iter_desc Memory descriptor for the diff of output
        ///     recurrent hidden state vector.
        /// @param flags Unused.
        desc(prop_kind prop_kind, rnn_direction direction,
                const memory::desc &src_layer_desc,
                const memory::desc &src_iter_desc,
                const memory::desc &weights_layer_desc,
                const memory::desc &weights_iter_desc,
                const memory::desc &bias_desc,
                const memory::desc &dst_layer_desc,
                const memory::desc &dst_iter_desc,
                const memory::desc &diff_src_layer_desc,
                const memory::desc &diff_src_iter_desc,
                const memory::desc &diff_weights_layer_desc,
                const memory::desc &diff_weights_iter_desc,
                const memory::desc &diff_bias_desc,
                const memory::desc &diff_dst_layer_desc,
                const memory::desc &diff_dst_iter_desc,
                rnn_flags flags = rnn_flags::undef);
    };

    /// Primitive descriptor for a GRU backward propagation primitive.
    struct primitive_desc : public rnn_primitive_desc_base {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a GRU backward propagation
        /// primitive.
        ///
        /// @param desc Descriptor for a GRU backward propagation primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a GRU
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const gru_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a GRU backward propagation
        /// primitive.
        ///
        /// @param desc Descriptor for a GRU backward propagation primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a GRU
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const gru_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::rnn_primitive_desc_base::src_layer_desc()const
        memory::desc src_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::src_iter_desc()const
        memory::desc src_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_layer_desc()const
        memory::desc weights_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_iter_desc()const
        memory::desc weights_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::bias_desc()const
        memory::desc bias_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_layer_desc()const
        memory::desc dst_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_iter_desc()const
        memory::desc dst_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_src_layer_desc()const
        memory::desc diff_src_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_src_iter_desc()const
        memory::desc diff_src_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_weights_layer_desc()const
        memory::desc diff_weights_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_weights_iter_desc()const
        memory::desc diff_weights_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_bias_desc()const
        memory::desc diff_bias_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_dst_layer_desc()const
        memory::desc diff_dst_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_dst_iter_desc()const
        memory::desc diff_dst_iter_desc() const;
    };

    /// Default constructor. Produces an empty object.
    gru_backward();

    /// Constructs a GRU backward propagation primitive.
    /// @param pd Primitive descriptor for a GRU backward propagation
    ///     primitive.
    gru_backward(const primitive_desc &pd);
};

/// LBR GRU forward propagation primitive.
struct lbr_gru_forward : public primitive {
    /// Descriptor for an LBR GRU forward propagation primitive.
    struct desc {
        /// Constructs a descriptor for LBR GRU forward propagation primitive.
        ///
        /// The @p src_iter_desc, @p bias_desc, and @p dst_iter, may point to
        /// a zero memory descriptor. This would then indicate that the LBR
        /// GRU forward propagation primitive should not use them and should
        /// default to zero values instead.
        ///
        /// Inputs:
        ///  - `src_layer` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `src_iter` (#dnnl::primitive_desc_base::src_desc(`1`)), if used
        ///  - `weights_layer` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `weights_iter` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`2`)), if used
        ///
        /// Outputs:
        ///  - `dst_layer` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `dst_iter` (#dnnl::primitive_desc_base::dst_desc(`1`)), if used
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`)),
        ///     if @p prop_kind equals #dnnl::prop_kind::forward_training;
        ///     must be queried for using @ref
        ///     dnnl::primitive_desc_base::query_md() after a corresponding
        ///     primitive descriptor is created
        ///
        /// @note
        ///     All memory descriptors except @p src_iter_desc may be
        ///     initialized with an #dnnl::memory::format_tag::any value of @p
        ///     format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param direction RNN direction. See @ref dnnl::rnn_direction for
        ///     more info.
        /// @param src_layer_desc Memory descriptor for the input vector.
        /// @param src_iter_desc Memory descriptor for the input recurrent
        ///     hidden state vector.
        /// @param weights_layer_desc Memory descriptor for the weights
        ///     applied to the layer input.
        /// @param weights_iter_desc Memory descriptor for the weights applied
        ///     to the recurrent input.
        /// @param bias_desc Bias memory descriptor.
        /// @param dst_layer_desc Memory descriptor for the output vector.
        /// @param dst_iter_desc Memory descriptor for the output recurrent
        ///     hidden state vector.
        /// @param flags Unused.
        desc(prop_kind prop_kind, rnn_direction direction,
                const memory::desc &src_layer_desc,
                const memory::desc &src_iter_desc,
                const memory::desc &weights_layer_desc,
                const memory::desc &weights_iter_desc,
                const memory::desc &bias_desc,
                const memory::desc &dst_layer_desc,
                const memory::desc &dst_iter_desc,
                rnn_flags flags = rnn_flags::undef);
    };

    /// Primitive descriptor for an LBR GRU forward propagation primitive.
    struct primitive_desc : public rnn_primitive_desc_base {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a LBR GRU forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a LBR GRU forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a LBR GRU forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a LBR GRU forward propagation
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);


        /// @copydoc dnnl::rnn_primitive_desc_base::src_layer_desc()const
        memory::desc src_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::src_iter_desc()const
        memory::desc src_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_layer_desc()const
        memory::desc weights_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_iter_desc()const
        memory::desc weights_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::bias_desc()const
        memory::desc bias_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_layer_desc()const
        memory::desc dst_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_iter_desc()const
        memory::desc dst_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;
    };

    /// Default constructor. Produces an empty object.
    lbr_gru_forward();

    /// Constructs an LBR GRU forward propagation primitive.
    /// @param pd Primitive descriptor for an LBR GRU forward propagation
    ///     primitive.
    lbr_gru_forward(const primitive_desc &pd);
};

/// LBR GRU backward propagation primitive.
struct lbr_gru_backward : public primitive {
    /// Descriptor for a LBR GRU backward propagation primitive.
    struct desc {
        /// Constructs a descriptor for LBR GRU backward propagation
        /// primitive.
        ///
        /// The @p src_iter_desc together with @p diff_src_iter_desc, @p
        /// bias_desc together with @p diff_bias_desc, and @p dst_iter
        /// together with @p diff_dst_iter, may point to a zero memory
        /// descriptor.  This would then indicate that the LBR GRU backward
        /// propagation primitive should not use them and should default to
        /// zero values instead.
        ///
        /// Inputs:
        ///  - `src_layer` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `src_iter` (#dnnl::primitive_desc_base::src_desc(`1`)), if used
        ///  - `weights_layer` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `weights_iter` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`2`)), if used
        ///  - `dst_layer` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///  - `dst_iter` (#dnnl::primitive_desc_base::dst_desc(`1`)), if used
        ///  - `diff_dst_layer` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///  - `diff_dst_iter`
        ///     (#dnnl::primitive_desc_base::diff_dst_desc(`1`)), if used
        ///  - `workspace` (#dnnl::primitive_desc_base::workspace_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src_layer` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///  - `diff_src_iter`
        ///     (#dnnl::primitive_desc_base::diff_src_desc(`1`)), if used
        ///  - `diff_weights_layer`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`0`))
        ///  - `diff_weights_iter`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`1`))
        ///  - `diff_bias`
        ///     (#dnnl::primitive_desc_base::diff_weights_desc(`2`)), if used
        ///
        /// @note
        ///     All memory descriptors may be initialized with
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Must be
        ///     #dnnl::prop_kind::backward.
        /// @param direction RNN direction. See @ref dnnl::rnn_direction for
        ///     more info.
        /// @param src_layer_desc Memory descriptor for the input vector.
        /// @param src_iter_desc Memory descriptor for the input recurrent
        ///     hidden state vector.
        /// @param weights_layer_desc Memory descriptor for the weights
        ///     applied to the layer input.
        /// @param weights_iter_desc Memory descriptor for the weights applied
        ///     to the recurrent input.
        /// @param bias_desc Bias memory descriptor.
        /// @param dst_layer_desc Memory descriptor for the output vector.
        /// @param dst_iter_desc Memory descriptor for the output recurrent
        ///     hidden state vector.
        /// @param diff_src_layer_desc Memory descriptor for the diff of input
        ///     vector.
        /// @param diff_src_iter_desc Memory descriptor for the diff of input
        ///     recurrent hidden state vector.
        /// @param diff_weights_layer_desc Memory descriptor for the diff of
        ///     weights applied to the layer input.
        /// @param diff_weights_iter_desc Memory descriptor for the diff of
        ///     weights applied to the recurrent input.
        /// @param diff_bias_desc Diff bias memory descriptor.
        /// @param diff_dst_layer_desc Memory descriptor for the diff of
        ///     output vector.
        /// @param diff_dst_iter_desc Memory descriptor for the diff of output
        ///     recurrent hidden state vector.
        /// @param flags Unused.
        desc(prop_kind prop_kind, rnn_direction direction,
                const memory::desc &src_layer_desc,
                const memory::desc &src_iter_desc,
                const memory::desc &weights_layer_desc,
                const memory::desc &weights_iter_desc,
                const memory::desc &bias_desc,
                const memory::desc &dst_layer_desc,
                const memory::desc &dst_iter_desc,
                const memory::desc &diff_src_layer_desc,
                const memory::desc &diff_src_iter_desc,
                const memory::desc &diff_weights_layer_desc,
                const memory::desc &diff_weights_iter_desc,
                const memory::desc &diff_bias_desc,
                const memory::desc &diff_dst_layer_desc,
                const memory::desc &diff_dst_iter_desc,
                rnn_flags flags = rnn_flags::undef);
    };

    /// Primitive descriptor for an LBR GRU backward propagation primitive.
    struct primitive_desc : public rnn_primitive_desc_base {
        /// Default constructor. Produces an empty object.
        primitive_desc() = default;

        /// Constructs a primitive descriptor for an LBR GRU backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for an LBR GRU backward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for an LBR GRU
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const lbr_gru_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for an LBR GRU backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for an LBR GRU backward propagation
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for an LBR GRU
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const lbr_gru_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::rnn_primitive_desc_base::src_layer_desc()const
        memory::desc src_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::src_iter_desc()const
        memory::desc src_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_layer_desc()const
        memory::desc weights_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::weights_iter_desc()const
        memory::desc weights_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::bias_desc()const
        memory::desc bias_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_layer_desc()const
        memory::desc dst_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::dst_iter_desc()const
        memory::desc dst_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::workspace_desc()const
        memory::desc workspace_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_src_layer_desc()const
        memory::desc diff_src_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_src_iter_desc()const
        memory::desc diff_src_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_weights_layer_desc()const
        memory::desc diff_weights_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_weights_iter_desc()const
        memory::desc diff_weights_iter_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_bias_desc()const
        memory::desc diff_bias_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_dst_layer_desc()const
        memory::desc diff_dst_layer_desc() const;

        /// @copydoc dnnl::rnn_primitive_desc_base::diff_dst_iter_desc()const
        memory::desc diff_dst_iter_desc() const;
    };

    /// Default constructor. Produces an empty object.
    lbr_gru_backward();

    /// Constructs an LBR GRU backward propagation primitive.
    /// @param pd Primitive descriptor for an LBR GRU backward propagation
    ///     primitive.
    lbr_gru_backward(const primitive_desc &pd);
};

/// @} dnnl_api_rnn

/// @addtogroup dnnl_api_shuffle Shuffle
///
/// A primitive to shuffle tensor data along an axis.
///
/// @{

/// Shuffle forward propagation primitive.
struct shuffle_forward : public primitive {
    /// Descriptor for a shuffle forward propagation primitive.
    struct desc {
        /// Constructs a descriptor for a shuffle forward propagation
        /// primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param data_desc Source and destination memory descriptor.
        /// @param axis The axis along which the data is shuffled.
        /// @param group_size Shuffle group size.
        desc(prop_kind prop_kind, const memory::desc &data_desc, int axis,
                int group_size);
    };

    /// Primitive descriptor for a shuffle forward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a shuffle forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a shuffle forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param attr Primitive attributes to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const primitive_attr &attr = primitive_attr(),
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    shuffle_forward();

    /// Constructs a shuffle forward propagation primitive.
    /// @param pd Primitive descriptor for a shuffle forward propagation
    ///     primitive.
    shuffle_forward(const primitive_desc &pd);
};

/// Shuffle backward propagation primitive.
struct shuffle_backward : public primitive {
    /// Descriptor for a shuffle primitive backward propagation
    /// primitive.
    struct desc {
        /// Constructs a descriptor for a shuffle backward propagation
        /// primitive.
        ///
        /// Inputs:
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///
        /// @param diff_data_desc Diff source and diff destination memory
        ///     descriptor.
        /// @param axis The axis along which the data is shuffled.
        /// @param group_size Shuffle group size.
        desc(const memory::desc &diff_data_desc, int axis, int group_size);
    };

    /// Primitive descriptor for a shuffle backward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a shuffle backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a shuffle backward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param attr Primitive attributes to use.
        /// @param hint_fwd_pd Primitive descriptor for a shuffle
        ///     forward propagation primitive. It is used as a hint for
        ///     deciding which memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const shuffle_forward::primitive_desc &hint_fwd_pd,
                const primitive_attr &attr = primitive_attr(),
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::diff_src_desc()const
        memory::desc diff_src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_dst_desc()const
        memory::desc diff_dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    shuffle_backward();

    /// Constructs a shuffle backward propagation primitive.
    /// @param pd Primitive descriptor for a shuffle backward propagation
    ///     primitive.
    shuffle_backward(const primitive_desc &pd);
};

/// @} dnnl_api_shuffle

/// @addtogroup dnnl_api_binary Binary
///
/// A primitive to perform tensor operations over two tensors.
///
/// @{

/// Elementwise binary operator primitive.
struct binary : public primitive {
    /// Descriptor for an elementwise binary operator primitive.
    struct desc {
        /// Constructs a descriptor for an elementwise binary operator
        /// primitive.
        ///
        /// Inputs:
        ///  - `src0` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `src1` (#dnnl::primitive_desc_base::src_desc(`1`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @param algorithm Elementwise algorithm.
        /// @param src0 Memory descriptor for source tensor #0.
        /// @param src1 Memory descriptor for source tensor #1.
        /// @param dst Memory descriptor for destination tensor.
        desc(algorithm algorithm, const memory::desc &src0,
                const memory::desc &src1, const memory::desc &dst);
    };

    /// Primitive descriptor for an elementwise binary operator primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for an elementwise binary operator
        /// primitive.
        ///
        /// @param desc Descriptor for an elementwise binary operator primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for an elementwise binary operator
        /// primitive.
        ///
        /// @param desc Descriptor for an elementwise binary operator primitive.
        /// @param engine Engine to use.
        /// @param attr Primitive attributes to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc(int)const
        memory::desc src_desc(int idx = 0) const;

        /// Returns the memory descriptor for source #0.
        memory::desc src0_desc() const;

        /// Returns the memory descriptor for source #1.
        memory::desc src1_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    binary();

    /// Constructs an elementwise binary operation primitive.
    /// @param pd Primitive descriptor for an elementwise binary operation
    ///     primitive.
    binary(const primitive_desc &pd);
};

/// @} dnnl_api_binary

/// @addtogroup dnnl_api_matmul Matrix Multiplication
///
/// A primitive to perform matrix-matrix multiplication. The batched mode
/// is supported with 3D tensors.
///
/// @{

/// Matrix multiplication (matmul) primitive.
struct matmul : public primitive {
    /// Descriptor for a matmul primitive.
    struct desc {
        /// Constructs a descriptor for a matmul primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @param src_desc Memory descriptor for source (matrix A).
        /// @param weights_desc Memory descriptor for weights (matrix B).
        /// @param dst_desc Memory descriptor for destination (matrix C).
        desc(const memory::desc &src_desc, const memory::desc &weights_desc,
                const memory::desc &dst_desc);

        /// Constructs a descriptor for a matmul primitive.
        ///
        /// Inputs:
        ///  - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///  - `weights` (#dnnl::primitive_desc_base::weights_desc(`0`))
        ///  - `bias` (#dnnl::primitive_desc_base::weights_desc(`1`))
        ///
        /// Outputs:
        ///  - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @param src_desc Memory descriptor for source (matrix A).
        /// @param weights_desc Memory descriptor for weights (matrix B).
        /// @param dst_desc Memory descriptor for destination (matrix C).
        /// @param bias_desc Memory descriptor for bias.
        desc(const memory::desc &src_desc, const memory::desc &weights_desc,
                const memory::desc &bias_desc, const memory::desc &dst_desc);
    };

    /// Primitive descriptor for a matmul primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a matmul primitive.
        ///
        /// @param desc Descriptor for a matmul primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a matmul primitive.
        ///
        /// @param desc Descriptor for a matmul primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::weights_desc()const
        memory::desc weights_desc() const;

        /// @copydoc dnnl::convolution_forward::primitive_desc::bias_desc()const
        memory::desc bias_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    matmul();

    /// Constructs a matmul primitive.
    /// @param pd Primitive descriptor for a matmul primitive.
    matmul(const primitive_desc &pd);
};

/// @} dnnl_api_matmul

/// @addtogroup dnnl_api_resampling Resampling
///
/// A primitive to compute resampling operation on 1D, 2D or 3D data tensor
/// using Nearest Neighbor, or Linear (Bilinear, Trilinear) interpolation
/// method.
///
/// @{

/// Resampling forward propagation.
struct resampling_forward : public primitive {
    /// Descriptor for resampling forward propagation.
    struct desc {
        /// Constructs a descriptor for a resampling forward propagation
        /// primitive using source and destination memory descriptors.
        ///
        /// Inputs:
        /// - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///
        /// Outputs:
        /// - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @note
        ///     The destination memory descriptor may be initialized with
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm resampling algorithm kind: either
        ///     #dnnl::algorithm::resampling_nearest, or
        ///     #dnnl::algorithm::resampling_linear
        /// @param src_desc Source memory descriptor.
        /// @param dst_desc Destination memory descriptor.
        desc(prop_kind prop_kind, algorithm algorithm,
                const memory::desc &src_desc, const memory::desc &dst_desc);

        /// Constructs a descriptor for a resampling forward propagation
        /// primitive using source memory descriptor and factors.
        ///
        /// Inputs:
        /// - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm resampling algorithm kind: either
        ///     #dnnl::algorithm::resampling_nearest, or
        ///     #dnnl::algorithm::resampling_linear
        /// @param factors Vector of scaling factors for spatial dimension.
        /// @param src_desc Source memory descriptor.
        desc(prop_kind prop_kind, algorithm algorithm,
                const std::vector<float> &factors,
                const memory::desc &src_desc);

        /// Constructs a descriptor for a resampling forward propagation
        /// primitive.
        ///
        /// Inputs:
        /// - `src` (#dnnl::primitive_desc_base::src_desc(`0`))
        ///
        /// Outputs:
        /// - `dst` (#dnnl::primitive_desc_base::dst_desc(`0`))
        ///
        /// @note
        ///     The destination memory descriptor may be initialized with
        ///     #dnnl::memory::format_tag::any value of @p format_tag.
        ///
        /// @param prop_kind Propagation kind. Possible values are
        ///     #dnnl::prop_kind::forward_training, and
        ///     #dnnl::prop_kind::forward_inference.
        /// @param algorithm resampling algorithm kind: either
        ///     #dnnl::algorithm::resampling_nearest, or
        ///     #dnnl::algorithm::resampling_linear
        /// @param factors Vector of scaling factors for spatial dimension.
        /// @param src_desc Source memory descriptor.
        /// @param dst_desc Destination memory descriptor.
        desc(prop_kind prop_kind, algorithm algorithm,
                const std::vector<float> &factors, const memory::desc &src_desc,
                const memory::desc &dst_desc);
    };

    /// Primitive descriptor for a resampling forward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a resampling forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a resampling forward propagation
        /// primitive.
        /// @param engine Engine to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a resampling forward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a resampling forward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param attr Primitive attributes to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine, bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::src_desc()const
        memory::desc src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::dst_desc()const
        memory::desc dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    resampling_forward();

    /// Constructs a resampling forward propagation primitive.
    /// @param pd Primitive descriptor for a resampling forward propagation
    ///     primitive.
    resampling_forward(const primitive_desc &pd);
};

/// Resampling backward propagation primitive.
struct resampling_backward : public primitive {
    /// Descriptor for a resampling backward propagation primitive.
    struct desc {
        /// Constructs a descriptor for a resampling backward propagation
        /// primitive using source and destination memory descriptors.
        ///
        /// Inputs:
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///
        /// @param algorithm resampling algorithm kind: either
        ///     #dnnl::algorithm::resampling_nearest, or
        ///     #dnnl::algorithm::resampling_linear
        /// @param diff_src_desc Diff source memory descriptor.
        /// @param diff_dst_desc Diff destination memory descriptor.
        desc(algorithm algorithm, const memory::desc &diff_src_desc,
                const memory::desc &diff_dst_desc);

        /// Constructs a descriptor for resampling backward propagation
        /// primitive.
        ///
        /// Inputs:
        ///  - `diff_dst` (#dnnl::primitive_desc_base::diff_dst_desc(`0`))
        ///
        /// Outputs:
        ///  - `diff_src` (#dnnl::primitive_desc_base::diff_src_desc(`0`))
        ///
        /// @param algorithm resampling algorithm kind: either
        ///     #dnnl::algorithm::resampling_nearest, or
        ///     #dnnl::algorithm::resampling_linear
        /// @param factors Vector of scaling factors for spatial dimension.
        /// @param diff_src_desc Diff source memory descriptor.
        /// @param diff_dst_desc Diff destination memory descriptor.
        desc(algorithm algorithm, const std::vector<float> &factors,
                const memory::desc &diff_src_desc,
                const memory::desc &diff_dst_desc);
    };

    /// Primitive descriptor for resampling backward propagation primitive.
    struct primitive_desc : public dnnl::primitive_desc {
        /// Default constructor. Produces an empty object.
        primitive_desc();

        /// Constructs a primitive descriptor for a resampling backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a resampling backward propagation
        ///     primitive.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a resampling forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const engine &engine,
                const resampling_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// Constructs a primitive descriptor for a resampling backward
        /// propagation primitive.
        ///
        /// @param desc Descriptor for a resampling backward propagation
        ///     primitive.
        /// @param attr Primitive attributes to use.
        /// @param engine Engine to use.
        /// @param hint_fwd_pd Primitive descriptor for a resampling forward
        ///     propagation primitive. It is used as a hint for deciding which
        ///     memory format to use.
        /// @param allow_empty A flag signifying whether construction is
        ///     allowed to fail without throwing an exception. In this case an
        ///     empty object will be produced. This flag is optional and
        ///     defaults to false.
        primitive_desc(const desc &desc, const primitive_attr &attr,
                const engine &engine,
                const resampling_forward::primitive_desc &hint_fwd_pd,
                bool allow_empty = false);

        /// @copydoc dnnl::primitive_desc_base::diff_src_desc()const
        memory::desc diff_src_desc() const;

        /// @copydoc dnnl::primitive_desc_base::diff_dst_desc()const
        memory::desc diff_dst_desc() const;
    };

    /// Default constructor. Produces an empty object.
    resampling_backward();

    /// Constructs a resampling backward propagation primitive.
    /// @param pd Primitive descriptor for a resampling backward propagation
    ///     primitive.
    resampling_backward(const primitive_desc &pd);
};

/// @} dnnl_api_resampling

/// @} dnnl_api_primitives

} // namespace dnnl

/// @} dnnl_api

#endif
