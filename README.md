currying
========

A simple currying library for automatically currying functions and function objects (such as lambdas)

Currently there are two separate entry points until the best option can be determined. Use curry() to simply curry a
function, meaning that the arguments shall now be passed one at a time (only). Use curry_multiapply() to wrap the
curried function in an function object that can automatically handle getting multiple arguments at a time.
