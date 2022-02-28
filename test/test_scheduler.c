/** \file */

#ifdef TEST

#include "unity.h"

#include "mock_scheduler_testMocks.h"

#include "scheduler.h"
#include "taskType.h"

/* `"` used intentionally.  This allows the user to override and provide his
 * own type definitions before falling back to libc.
 */
#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

/** \brief Run before every test */
void setUp(void)
{
    return; /* Do nothing */
}

/** \brief Run after every test */
void tearDown(void)
{
    /* Reset all private variables (i.e., `static` globals) of module under
     * test back to their initial values.
     */
    fn_sdlr_setTickCountSource(NULL);
    fn_sdlr_setTaskAttributes(NULL, (uint8_t)0u);
    fn_sdlr_resetTaskOverrunCount();

    return;
}

/**
 * \brief Verfiy unsigned integer rollover arithmetic of relative system time
 * tick count
 *
 * This test does not exercise the code base but just documents and verifies
 * the understanding of unsigned integer arithmetic--in this case substractions
 * that would yield negative results with signed integers.
 * This arithmetic with unsigned integers is an integral part of the scheduling
 * algorithm.
 * For ease of understanding, `uint8_t` is used here.
 */
void test_sdlr_verfiyUnsignedIntegerTickRolloverArithmetic(void)
{
    const uint8_t u8_varA = (uint8_t)5u;
    const uint8_t u8_varB = (uint8_t)250u;

    const uint8_t u8_resultExpectedBA = (uint8_t)245u;
    const uint8_t u8_resultExpectedAB = (uint8_t)11u; /* -245 with signed int. */

    TEST_ASSERT_EQUAL_UINT8( u8_resultExpectedBA, (u8_varB - u8_varA) );
    TEST_ASSERT_EQUAL_UINT8( u8_resultExpectedAB, (u8_varA - u8_varB) );

    return;
}

/** \brief Test if relative system time tick count source is set correctly */
void test_sdlr_tickCountSourceIsSet(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { false, (uint32_t)1u, (uint32_t)1u, (uint32_t)0u, fn_taskRunner }
    };

    fn_getTickCount_ExpectAndReturn( (uint32_t)0u );

    fn_sdlr_setTickCountSource(fn_getTickCount); /* Set tick count source ... */
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    fn_sdlr_execute(); /* ... and test if it gets called (exactly once) */

    return;
}

/** \brief Test that task list is initialized with `Null` pointer */
void test_sdlr_taskListIsInitializedToNull(void)
{
    TEST_ASSERT_NULL( fn_sdlr_getTaskList() );

    return;
}

/** \brief Test if task list is set and returned correctly */
void test_sdlr_taskListIsSetAndReturned(void)
{
    stc_tsk_t stc_tsk_taskListExpected = {false,
                                          (uint32_t)1u,
                                          (uint32_t)1u,
                                          (uint32_t)0u,
                                          fn_taskRunner};
    stc_tsk_t* const p_stc_tsk_taskListExpected = &stc_tsk_taskListExpected;
    stc_tsk_t* p_stc_tsk_taskListActual = NULL;

    fn_sdlr_setTaskAttributes(p_stc_tsk_taskListExpected, (uint8_t)1u);
    p_stc_tsk_taskListActual = fn_sdlr_getTaskList();

    TEST_ASSERT_EQUAL_PTR(p_stc_tsk_taskListExpected, p_stc_tsk_taskListActual);

    return;
}

/**
 * \brief Test that task count (number of task entries in the connected task
 * list) is initialized to `0`
 */
void test_sdlr_taskCountIsInitializedTo0(void)
{
    const uint8_t u8_taskCountExpected = (uint8_t)0u;

    TEST_ASSERT_EQUAL_UINT8( u8_taskCountExpected, fn_sdlr_getTaskCount() );

    return;
}

/**
 * \brief Test if task Count (number of task entries in the connected task
 * list) is set and returned correctly
 */
void test_sdlr_taskCountIsSetAndReturned(void)
{
    stc_tsk_t stc_tsk_taskList = {false,
                                  (uint32_t)1u,
                                  (uint32_t)1u,
                                  (uint32_t)0u,
                                  fn_taskRunner};
    const uint8_t u8_taskCountExpected = (uint8_t)1u;
    uint8_t u8_taskCountActual = (uint8_t)0u;

    fn_sdlr_setTaskAttributes(&stc_tsk_taskList, u8_taskCountExpected);
    u8_taskCountActual = fn_sdlr_getTaskCount();

    TEST_ASSERT_EQUAL_UINT8(u8_taskCountExpected, u8_taskCountActual);

    return;
}

/** \brief Test that task deadline overrun is initialized to `0` */
void test_sdlr_taskOverrunIsInitializedTo0(void)
{
    const uint8_t u8_taskOverrunExpected = (uint8_t)0u;

    TEST_ASSERT_EQUAL_UINT8( u8_taskOverrunExpected,
                             fn_sdlr_getTaskOverrunCount() );

    return;
}

/**
 * \brief Test that first scheduling execution cycle without resetting the
 * relative system time tick count before is not counted as a task deadline
 * overrun
 */
void test_sdlr_firstLateSchedulerExecutionIsNoTaskOverrun(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { true, (uint32_t)100u, (uint32_t)100u, (uint32_t)0u,
          fn_taskRunner }
    };
    const uint8_t u8_taskOverrunExpected = (uint8_t)0u;

    fn_getTickCount_ExpectAndReturn( (uint32_t)1000u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)1099u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    fn_sdlr_execute();

    TEST_ASSERT_EQUAL_UINT8( u8_taskOverrunExpected,
                             fn_sdlr_getTaskOverrunCount() );

    return;
}

/**
 * \brief Test if single task deadline overrun is detected and count returned
 * correctly
 */
void test_sdlr_singleTaskOverrunIsDetectedAndCountReturned(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { true, (uint32_t)100u, (uint32_t)50u, (uint32_t)0u,
          fn_taskRunner }
    };
    const uint8_t u8_taskOverrunExpected = (uint8_t)1u;

    fn_getTickCount_ExpectAndReturn( (uint32_t)1000u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)1051u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    fn_sdlr_execute();

    TEST_ASSERT_EQUAL_UINT8( u8_taskOverrunExpected,
                             fn_sdlr_getTaskOverrunCount() );

    return;
}

/**
 * \brief Test if multiple task deadline overruns are detected and counts
 * returned correctly
 */
void test_sdlr_multipleTaskOverrunsAreDetectedAndCountsReturned(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { true, (uint32_t)100u, (uint32_t)100u, (uint32_t)0u,
          fn_taskRunner }
    };
    uint8_t u8_idx = (uint8_t)0u;
    const uint8_t u8_taskOverrunExpected = (uint8_t)2u;

    /* Task overrun */
    fn_getTickCount_ExpectAndReturn( (uint32_t)1000u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)1101u );

    /* No task overrun */
    fn_getTickCount_ExpectAndReturn( (uint32_t)1110u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)1120u );

    /* Task overrun */
    fn_getTickCount_ExpectAndReturn( (uint32_t)1200 );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)1301u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    for(u8_idx = (uint8_t)0u; u8_idx < 3; u8_idx++)
    {
        fn_sdlr_execute();
    }

    TEST_ASSERT_EQUAL_UINT8( u8_taskOverrunExpected,
                             fn_sdlr_getTaskOverrunCount() );

    return;
}

/**
 * \brief Test if multiple task deadline overruns are detected and counts
 * returned correctly on relative system time tick count rollover
 */
void
test_sdlr_multiTaskOverrunsAreDetectedAndCountsReturnedOnTickRollover(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { true, (uint32_t)100u, (uint32_t)100u, (uint32_t)0u,
          fn_taskRunner }
    };
    uint8_t u8_idx = (uint8_t)0u;
    const uint8_t u8_taskOverrunExpected = (uint8_t)2u;

    /* Task overrun (at rollover) */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX - (uint32_t)10u); /* 4294967285 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)100u );

    /* No task overrun */
    fn_getTickCount_ExpectAndReturn( (uint32_t)20u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)30u );

    /* Task overrun */
    fn_getTickCount_ExpectAndReturn( (uint32_t)130 );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)231u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    for(u8_idx = (uint8_t)0u; u8_idx < 3; u8_idx++)
    {
        fn_sdlr_execute();
    }

    TEST_ASSERT_EQUAL_UINT8( u8_taskOverrunExpected,
                             fn_sdlr_getTaskOverrunCount() );

    return;
}

/**
 * \brief Test if single task deadline overrun is detected and count returned
 * on inconvenient relative system time tick count rollover
 *
 * Special "task deadline overrun at time tick rollover" use case scenario.
 * This is an important edge case as it made a previously implemented detailed
 * design solution fail.
 */
void
test_sdlr_singleTaskOverrunIsDetectedAndCountRetOnInconvTickRollover(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        /* Last run (offset) is 4294967095 */
        { true, (uint32_t)100u, (uint32_t)100u,
          UINT32_MAX - (uint32_t)200u, fn_taskRunner }
    };
    const uint8_t u8_taskOverrunExpected = (uint8_t)1u;

    /* Task overrun (at rollover) */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX - (uint32_t)10u); /* 4294967285 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)10u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    fn_sdlr_execute();

    TEST_ASSERT_EQUAL_UINT8( u8_taskOverrunExpected,
                             fn_sdlr_getTaskOverrunCount() );

    return;
}

/** \brief Test if task deadline overrun count is reset correctly */
void test_sdlr_taskOverrunCountIsReset(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { true, (uint32_t)10u, (uint32_t)10u, (uint32_t)0u, fn_taskRunner }
    };
    const uint8_t u8_taskOverrunExpectedA = (uint8_t)1u;
    const uint8_t u8_taskOverrunExpectedB = (uint8_t)0u;

    /* Task overrun */
    fn_getTickCount_ExpectAndReturn( (uint32_t)10u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)21u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    fn_sdlr_execute();

    /* First, assert that task overrun is detected (counter not 0) before reset
     */
    TEST_ASSERT_EQUAL_UINT8( u8_taskOverrunExpectedA,
                             fn_sdlr_getTaskOverrunCount() );

    fn_sdlr_resetTaskOverrunCount();

    TEST_ASSERT_EQUAL_UINT8( u8_taskOverrunExpectedB,
                             fn_sdlr_getTaskOverrunCount() );

    return;
}

/** \brief Test if single task is enabled correctly */
void test_sdlr_singleTaskIsEnabled(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { false, (uint32_t)1u, (uint32_t)1u, (uint32_t)0u, fn_taskRunner }
    };
    const bool b_activeExpected = true;

    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    fn_sdlr_setTaskAct(fn_taskRunner, true, false);

    TEST_ASSERT_EQUAL_UINT(b_activeExpected, a_stc_tsk_taskList[0].b_active);

    return;
}

/** \brief Test if single task is disabled correctly */
void test_sdlr_singleTaskIsDisabled(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { true, (uint32_t)1u, (uint32_t)1u, (uint32_t)0u, fn_taskRunner }
    };
    const bool b_activeExpected = false;

    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    fn_sdlr_setTaskAct(fn_taskRunner, false, false);

    TEST_ASSERT_EQUAL_UINT(b_activeExpected, a_stc_tsk_taskList[0].b_active);

    return;
}

/**
 * \brief Test if multiple identical task runners are enabled and disabled
 * correctly
 */
void test_sdlr_multipleSameTasksAreEnabledAndDisabled(void)
{
    const bool b_untouched = false;
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { true, (uint32_t)1u, (uint32_t)1u, (uint32_t)0u, fn_taskRunner0 },
        { false, (uint32_t)1u, (uint32_t)1u, (uint32_t)0u, fn_taskRunner1 },
        { true, (uint32_t)1u, (uint32_t)1u, (uint32_t)0u, fn_taskRunner0 },
        { b_untouched, (uint32_t)1u, (uint32_t)1u, (uint32_t)0u, fn_taskRunner2 },
        { false, (uint32_t)1u, (uint32_t)1u, (uint32_t)0u, fn_taskRunner1 },
        { true, (uint32_t)1u, (uint32_t)1u, (uint32_t)0u, fn_taskRunner0 },
        { false, (uint32_t)1u, (uint32_t)1u, (uint32_t)0u, fn_taskRunner1 }
    };
    const bool b_activeExpectedA = false;
    const bool b_activeExpectedB = true;

    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)7u);
    fn_sdlr_setTaskAct(fn_taskRunner0, b_activeExpectedA, false);
    fn_sdlr_setTaskAct(fn_taskRunner1, b_activeExpectedB, false);

    /* Should have become disabled */
    TEST_ASSERT_EQUAL_UINT(b_activeExpectedA, a_stc_tsk_taskList[0].b_active);
    TEST_ASSERT_EQUAL_UINT(b_activeExpectedA, a_stc_tsk_taskList[2].b_active);
    TEST_ASSERT_EQUAL_UINT(b_activeExpectedA, a_stc_tsk_taskList[5].b_active);

    /* Should have become enabled */
    TEST_ASSERT_EQUAL_UINT(b_activeExpectedB, a_stc_tsk_taskList[1].b_active);
    TEST_ASSERT_EQUAL_UINT(b_activeExpectedB, a_stc_tsk_taskList[4].b_active);
    TEST_ASSERT_EQUAL_UINT(b_activeExpectedB, a_stc_tsk_taskList[6].b_active);

    /* Should have stayed untouched */
    TEST_ASSERT_EQUAL_UINT(b_untouched, a_stc_tsk_taskList[3].b_active);

    return;
}

/** \brief Test if time stamp of last task run is updated correctly */
void test_sdlr_lastRunOfsingleTaskIsUpdated(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { false, (uint32_t)1u, (uint32_t)1u, (uint32_t)0u, fn_taskRunner }
    };
    const uint32_t u32_lastRunExpected = (uint32_t)111;

    fn_getTickCount_ExpectAndReturn(u32_lastRunExpected);

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    fn_sdlr_setTaskAct(fn_taskRunner, true, true);

    TEST_ASSERT_EQUAL_UINT32(u32_lastRunExpected,
                             a_stc_tsk_taskList[0].u32_lastRun);

    return;
}

/**
 * \brief Test correct execution of due-to-run task with 1 time tick period and
 * start at 0 time ticks
 */
void test_sdlr_executeDueToRunTaskAt1TickPeriodOn0TickStart(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { true, (uint32_t)1u, (uint32_t)1u, (uint32_t)0u, fn_taskRunner }
    };
    uint8_t u8_idx = (uint8_t)0u;

    /* No run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)0u );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)1u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)1u );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)2u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)2u );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)3u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)3u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    for(u8_idx = (uint8_t)0u; u8_idx < 4; u8_idx++)
    {
        fn_sdlr_execute();
    }

    return;
}

/**
 * \brief Test correct execution of due-to-run task with 2 time ticks period
 * and start at non-0 time tick
 */
void test_sdlr_executeDueToRunTaskAt2TicksPeriodOnNon0TickStart(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { true, (uint32_t)2u, (uint32_t)2u, (uint32_t)0u, fn_taskRunner }
    };
    uint8_t u8_idx = (uint8_t)0u;

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)201u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)201u );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)202u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)202u );

    /* No run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)203u );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)204u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)204u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    for(u8_idx = (uint8_t)0u; u8_idx < 4; u8_idx++)
    {
        fn_sdlr_execute();
    }

    return;
}

/**
 * \brief Test correct execution of due-to-run task with 2 time ticks period
 * and start at non-0 time tick and skipped time ticks
 */
void
test_sdlr_execDueToRunTaskAt2TicksPeriodOnNon0TickStartAndTickLoss(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { true, (uint32_t)2u, (uint32_t)2u, (uint32_t)0u, fn_taskRunner }
    };
    uint8_t u8_idx = (uint8_t)0u;

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)201u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)201u );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)302u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)302u );

    /* No run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)303u );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)507u );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)507u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    for(u8_idx = (uint8_t)0u; u8_idx < 4; u8_idx++)
    {
        fn_sdlr_execute();
    }

    return;
}

/**
 * \brief Test correct execution of multiple due-to-run tasks with different
 * time tick periods and start at non-0 time tick and skipped time ticks
 */
void
test_sdlr_execDueToRunTasksAtDiffPeriodsOnNon0TickStartAndTickLoss(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { true, (uint32_t)3u, (uint32_t)3u, (uint32_t)0u, fn_taskRunner0 },
        { true, (uint32_t)4u, (uint32_t)4u, (uint32_t)0u, fn_taskRunner1 },
        { true, (uint32_t)9u, (uint32_t)9u, (uint32_t)0u, fn_taskRunner2 }
    };
    uint8_t u8_idx = (uint8_t)0u;

    /* Run task runner 0, 1, 2 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)201u );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)202u );
    fn_getTickCount_ExpectAndReturn( (uint32_t)202u );
    fn_taskRunner1_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)203u );
    fn_getTickCount_ExpectAndReturn( (uint32_t)203u );
    fn_taskRunner2_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)204u );

    /* Run task runner 0, 0, 1, 0, 2 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)206u );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)207u );
    fn_getTickCount_ExpectAndReturn( (uint32_t)207u );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)208u );
    fn_getTickCount_ExpectAndReturn( (uint32_t)209u );
    fn_taskRunner1_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)210u );
    fn_getTickCount_ExpectAndReturn( (uint32_t)210u );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)211u );
    fn_getTickCount_ExpectAndReturn( (uint32_t)211u );
    fn_taskRunner2_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)212u );

    /* Run task runner 0, 1 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)214u );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)215u );
    fn_getTickCount_ExpectAndReturn( (uint32_t)215u );
    fn_taskRunner1_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)216u );

    /* Run task runner 0, 1, 2 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)501u );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)502u );
    fn_getTickCount_ExpectAndReturn( (uint32_t)502u );
    fn_taskRunner1_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)503u );
    fn_getTickCount_ExpectAndReturn( (uint32_t)503u );
    fn_taskRunner2_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)504u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)3u);
    for(u8_idx = (uint8_t)0u; u8_idx < 13; u8_idx++)
    {
        fn_sdlr_execute();
    }

    return;
}

/**
 * \brief Test correct execution of multiple due-to-run tasks with different
 * time tick periods and offsets and start at 0 time ticks
 */
void test_sdlr_execDueToRunTasksAtDiffPeriodsAndOffsetsOn0TickStart(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        /* Run at: 10, 40, 70, 100, ... */
        { true,
          (uint32_t)30u,
          (uint32_t)30u,
          (uint32_t)( (uint32_t)0u - (uint32_t)20u ), /* Offset 20 */
          fn_taskRunner0 },
        /* Run at: 30, 70, 110, ... */
        { true,
          (uint32_t)40u,
          (uint32_t)40u,
          (uint32_t)( (uint32_t)0u - (uint32_t)10u ), /* Offset 10 */
          fn_taskRunner1 },
        /* Run at: 90, 180, ... */
        { true,
          (uint32_t)90u,
          (uint32_t)90u,
          (uint32_t)0u, /* No offset (0) */
          fn_taskRunner2 }
    };
    uint8_t u8_idx = (uint8_t)0u;

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)0u );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)9u );

    /* Run task runner 0 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)10u );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)10u );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)11u );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)29u );

    /* Run task runner 1 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)30u );
    fn_taskRunner1_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)30u );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)31u );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)39u );

    /* Run task runner 0 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)40u );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)40u );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)41u );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)69u );

    /* Run task runner 0, 1 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)70u );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)70u );
    fn_getTickCount_ExpectAndReturn( (uint32_t)70u );
    fn_taskRunner1_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)70u );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)71u );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)89u );

    /* Run task runner 2 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)90u );
    fn_taskRunner2_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)90u );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)91u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)3u);
    for(u8_idx = (uint8_t)0u; u8_idx < 17; u8_idx++)
    {
        fn_sdlr_execute();
    }

    return;
}

/** \brief Test that task runner of disabled task is not run */
void test_sdlr_taskRunnerOfDisabledTaskIsNotRun(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        /* If task runner is run, test will fail (as it should), complaining
         * that function `fn_taskRunner` is called more times than expected
         */
        { false, (uint32_t)10u, (uint32_t)10u, (uint32_t)0u, fn_taskRunner }
    };

    /* Expect only one call to `getTickCount` as no task runner is supposed to
     * run.
     * Now, the test will fail (as it should), if disabled task’s
     * `fn_taskRunner` is called.
     */
    fn_getTickCount_ExpectAndReturn( (uint32_t)10u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    fn_sdlr_execute();

    return;
}

/** \brief Test that `lastRun` value of disabled task is still updated */
void test_sdlr_lastRunOfDisabledTaskIsStillUpdated(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { false, (uint32_t)10u, (uint32_t)10u, (uint32_t)0u, fn_taskRunner }
    };
    const uint32_t u32_lastRunExpected = (uint32_t)320u;

    fn_getTickCount_ExpectAndReturn( (uint32_t)327u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    fn_sdlr_execute();

    TEST_ASSERT_EQUAL_UINT32(u32_lastRunExpected,
                             a_stc_tsk_taskList[0].u32_lastRun);

    return;
}

/**
 * \brief Test that `lastRun` value is always set to begin of period (time
 * slot)
 */
void test_sdlr_lastRunIsAlwaysSetToBeginOfPeriod(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { true, (uint32_t)10u, (uint32_t)10u, (uint32_t)0u, fn_taskRunner }
    };
    const uint32_t u32_lastRunExpected = (uint32_t)320u; /* Begin of period for
                                                            this test’s use
                                                            case scenario (see
                                                            comment below) */

    fn_getTickCount_ExpectAndReturn( (uint32_t)327u ); /* Begin of period: 320 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)329u );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    fn_sdlr_execute();

    TEST_ASSERT_EQUAL_UINT32(u32_lastRunExpected,
                             a_stc_tsk_taskList[0].u32_lastRun);

    return;
}

/**
 * \brief Test correct execution of due-to-run task at 3 time ticks period with
 * relative system time tick count rollover
 */
void test_sdlr_executeDueToRunTaskAt3TicksPeriodOnTickRollover(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { true, (uint32_t)3u, (uint32_t)3u, (uint32_t)0u, fn_taskRunner }
    };
    uint8_t u8_idx = (uint8_t)0u;

    /* Run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX - (uint32_t)3u); /* 4294967292 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn(UINT32_MAX - (uint32_t)3u);

    /* No run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX - (uint32_t)2u); /* 4294967293 */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX - (uint32_t)1u); /* 4294967294 */

    /* Run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX); /* 4294967295 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn(UINT32_MAX);

    /* No run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)1u); /* 0 */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)2u); /* 1 */

    /* Run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)3u); /* 2 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)3u);

    /* No run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)4u); /* 3 */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)5u); /* 4 */

    /* Run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)6u); /* 5 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)6u);

    /* No run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)7u); /* 6 */

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1u);
    for(u8_idx = (uint8_t)0u; u8_idx < 11; u8_idx++)
    {
        fn_sdlr_execute();
    }

    return;
}

#endif /* TEST */
