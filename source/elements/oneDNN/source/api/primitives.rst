..
  Copyright 2019 Intel Corporation

.. default-domain:: cpp

Primitives
----------

The library has multiple levels of abstractions for primitives and memory
objects in order to expose maximum flexibility to its users.

On the logical level, the library provides the following abstractions:

* *Operation descriptors* (one for each supported primitive) describe an
  operation's most basic properties without specifying, for example, which
  engine will be used to compute them. For example, convolution descriptor
  describes shapes of source, destination, and weights tensors, propagation
  kind (forward, backward with respect to data or weights), and other
  implementation-independent parameters.

* *Primitive descriptors* (one for each supported primitive) are at an
  abstraction level in between operation descriptors and primitives and can be
  used to inspect details of a specific primitive implementation like expected
  memory formats via queries to implement memory format propagation (see
  Memory format propagation) without having to fully instantiate a primitive.

* *Primitives* are functor objects that encapsulates a particular computation
  such as forward convolution, backward LSTM computations, or a data
  transformation operation. A single primitive can sometimes represent more
  complex fused computations such as a forward convolution followed by a ReLU.

  The most important difference between a primitive and a pure function is
  that a primitive can store state.

  One part of the primitive’s state is immutable. For example, convolution
  primitives store parameters like tensor shapes and can pre-compute other
  dependent parameters like cache blocking. This approach allows DNNL primitives
  to pre-generate code specifically tailored for the operation to be performed.
  The DNNL programming model assumes that the time it takes to perform the
  pre-computations is amortized by reusing the same primitive to perform
  computations multiple times.

  The mutable part of the primitive’s state is referred to as a scratchpad. It
  is a memory buffer that a primitive may use for temporary storage only
  during computations. The scratchpad can either be owned by a primitive
  object (which makes that object non-thread safe) or be an execution-time
  parameter.

.. toctree::
   :maxdepth: 1

   primitives/common.rst
   primitives/convolution.rst
