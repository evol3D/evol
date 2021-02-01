/*!
 * \file vec.h
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>

#ifndef VEC_INIT_CAP
/*!
 * \brief Initial capacity that is first reserved when a vector is initialized
 */
#define VEC_INIT_CAP 8
#endif

#ifndef VEC_GROWTH_RATE
/*!
 * \brief Rate at which a vector grows whenever a resize is needed
 */
#define VEC_GROWTH_RATE 3 / 2
#endif

typedef void *vec_t;

/*!
 * \brief Signature of a function that copies data from one address to another.
 * \details For copying elements of type `T`, an equivalent signature should be:
 * ```
 * void T_copy(T *dst, const T *src)
 * ```
 * \param dst Address at which the data should be copied
 * \param src Address from which the data should be copied
 */
typedef void (*elem_copy)(void *dst, const void *src);

/*!
 * \brief Signature of a function that destroys data referenced by a pointer
 * \details For destroying elements of type `T`, an equivalent signature should be:
 * ```
 * void T_destr(T *d)
 * ```
 *
 * \param d Pointer to data that should be destroyed
 */
typedef void (*elem_destr)(void *d);

/*!
 * \param elemsize Memory (in bytes) that should be reserved per element in the vector
 * \param copy A function pointer to the function that should be used to copy an element
 * to and from the vector. Can be NULL
 * \param destr A function pointer to the function that should be used to destroy a
 * vector element. Can be NULL
 *
 * \returns A vector object
 */
vec_t
vec_init_impl(size_t elemsize, elem_copy copy, elem_destr destr);


/*!
 * \brief Syntactic sugar for `vec_init_impl()`
 * \details Sample usage:
 * ```
 * vec_init(int, 0, 0)
 * ```
 */
#define vec_init(type, copy, destr) vec_init_impl(sizeof(type), copy, destr)

/*!
 * \param v The vector that we want an iterator for
 *
 * \returns A pointer to the first element in a vector
 */
void *
vec_iter_begin(vec_t v);

/*!
 * \param v The vector that we want an iterator for
 *
 * \returns A pointer to the memory block right after the last element in the vector
 */
void *
vec_iter_end(vec_t v);

/*!
 * \brief A function that increments an iterator to make it point to the next
 * element in the vector
 *
 * \param v The vector that is being iterated over
 * \param iter Reference to the iterator that is being incremented
 */
void
vec_iter_next(vec_t v, void **iter);

/*!
 * \brief A function that destroys a vector object. If a destructor function was
 * passed while initializing the vector, then this function is called on every
 * element before all reserved memory is freed.
 *
 * \param v The vector that is being destroyed
 */
void
vec_fini(vec_t v);

/*!
 * \brief A function that copies a value to the end of a vector. If a copy
 * function was passed while initializing the vector, then this function is
 * called to copy the new element into the vector. Otherwise, memcpy is used
 * with a length of `vec_meta.elemsize`. If a resize is needed but fails due to
 * 'OOM' issues, then the vector is left unchanged and a non-zero is returned.
 *
 * \param v The vector object
 * \param val A pointer to the element that is to be copied to the end of the
 * vector
 *
 * \returns An error code. If the operation was successful, then `0` is returned.
 */
int32_t
vec_push(vec_t *v, void *val);

/*!
 * \brief A function that returns the length of a vector
 *
 * \param v The vector object
 */
size_t
vec_len(vec_t v);

/*!
 * \brief A function that returns the capacity of a vector
 *
 * \param v The vector object
 */
size_t
vec_capacity(vec_t v);

/*!
 * \brief Calls the free operation (if exists) on every element, then sets
 * the length to 0.
 *
 * \param v The vector object
 */
int32_t
vec_clear(vec_t v);

/*!
 * \brief Sets the length of the vector to `len`.
 *
 * \details If `len` is less than `v`'s current length, then `v`'s length is
 * amended. Otherwise, the capcity is checked to make sure that there is enough
 * space for the new len.
 *
 * \param v The vector object
 * \param len The desired new length
 */
int32_t
vec_setlen(vec_t *v, size_t len);

/*!
 * \brief Sets the capacity of the vector to `cap`.
 *
 * \param v The vector object
 * \param cap The desired new capacity
 */
int32_t
vec_setcapacity(vec_t *v, size_t cap);
