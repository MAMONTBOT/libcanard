// This software is distributed under the terms of the MIT License.
// Copyright (c) OpenCyphal Development Team.

#include "helpers.h"
#include <unity.h>
#include <cstdint>
#include <cstdlib>
#include <memory>

// Simple allocator that always fails.
static void* dummy_alloc_mem(const canard_mem_t mem, const size_t size) { return dummy_alloc(mem.context, size); }
static void  dummy_free_mem(const canard_mem_t mem, const size_t size, void* const pointer)
{
    dummy_free(mem.context, size, pointer);
}

static void test_canard_publish_validation(void)
{
    canard_t self = {};

    // Invalid interface bitmap.
    const canard_bytes_chain_t payload = { .bytes = { .size = 0, .data = NULL }, .next = NULL };
    TEST_ASSERT_FALSE(canard_publish(&self, 0, 0, canard_prio_nominal, 0, 0, payload, CANARD_USER_CONTEXT_NULL, false));

    // Invalid payload.
    const canard_bytes_chain_t bad_payload = { .bytes = { .size = 1, .data = NULL }, .next = NULL };
    TEST_ASSERT_FALSE(
      canard_publish(&self, 0, 1, canard_prio_nominal, 0, 0, bad_payload, CANARD_USER_CONTEXT_NULL, false));
}

static void test_canard_publish_oom(void)
{
    canard_mem_vtable_t vtable = {};
    vtable.free                = dummy_free_mem;
    vtable.alloc               = dummy_alloc_mem;
    canard_t self              = {};
    self.mem.tx_transfer       = canard_mem_t{ &vtable, NULL };
    self.mem.tx_frame          = canard_mem_t{ &vtable, NULL };

    // Allocation failure in txfer_new should return false.
    const canard_bytes_chain_t payload = { .bytes = { .size = 0, .data = NULL }, .next = NULL };
    TEST_ASSERT_FALSE(canard_publish(&self, 0, 1, canard_prio_nominal, 0, 0, payload, CANARD_USER_CONTEXT_NULL, false));
}

static void test_canard_0v1_publish_requires_node_id(void)
{
    canard_t self = {};

    // Node-ID zero should reject the request.
    const canard_bytes_chain_t payload = { .bytes = { .size = 0, .data = NULL }, .next = NULL };
    TEST_ASSERT_FALSE(canard_0v1_publish(&self, 0, 1, canard_prio_nominal, 1, 0xFFFF, 0, payload));
}

extern "C" void setUp() {}
extern "C" void tearDown() {}

int main()
{
    UNITY_BEGIN();

    // canard_publish/canard_0v1_publish validation.
    RUN_TEST(test_canard_publish_validation);
    RUN_TEST(test_canard_publish_oom);
    RUN_TEST(test_canard_0v1_publish_requires_node_id);

    return UNITY_END();
}
