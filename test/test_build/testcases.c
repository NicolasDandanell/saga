#include <aceunit.h>

extern void beforeEach_push(void);
extern void test_saga_call_push_full_offset_wrap(void);
extern void test_saga_call_push_full_wrap(void);
extern void test_saga_call_push_simple_add(void);
extern void test_saga_call_push_simple_wrap(void);
extern void beforeEach_pull(void);
extern void test_simple_one_log_pull(void);

const aceunit_func_t testcases_saga_add_test[] = {
    test_saga_call_push_full_offset_wrap,
    test_saga_call_push_full_wrap,
    test_saga_call_push_simple_add,
    test_saga_call_push_simple_wrap,
    NULL,
};

const AceUnit_Fixture_t fixture_saga_add_test = {
    NULL,
    NULL,
    beforeEach_push,
    NULL,
    testcases_saga_add_test,
};


const aceunit_func_t testcases_saga_pull_test[] = {
    test_simple_one_log_pull,
    NULL,
};

const AceUnit_Fixture_t fixture_saga_pull_test = {
    NULL,
    NULL,
    beforeEach_pull,
    NULL,
    testcases_saga_pull_test,
};

const AceUnit_Fixture_t *fixtures[] = {
    &fixture_saga_add_test,
    &fixture_saga_pull_test,
    NULL,
};
