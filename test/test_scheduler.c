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
    fn_sdlr_setTaskAttributes(NULL, (uint8_t)0U);
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
    const uint8_t u8_varA = (uint8_t)5U;
    const uint8_t u8_varB = (uint8_t)250U;

    const uint8_t u8_resultExpectedBA = (uint8_t)245U;
    const uint8_t u8_resultExpectedAB = (uint8_t)11U; /* -245 with signed int. */

    TEST_ASSERT_EQUAL_UINT8( u8_resultExpectedBA, (u8_varB - u8_varA) );
    TEST_ASSERT_EQUAL_UINT8( u8_resultExpectedAB, (u8_varA - u8_varB) );

    return;
}

/** \brief Test if relative system time tick count source is set correctly */
void test_sdlr_tickCountSourceIsSet(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { en_act_OFF, (uint32_t)1U, (uint32_t)1U, (uint32_t)0U, fn_taskRunner }
    };

    fn_getTickCount_ExpectAndReturn( (uint32_t)0U );

    fn_sdlr_setTickCountSource(fn_getTickCount); /* Set tick count source ... */
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
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
    stc_tsk_t stc_tsk_taskListExpected = {en_act_OFF,
                                          (uint32_t)1U,
                                          (uint32_t)1U,
                                          (uint32_t)0U,
                                          fn_taskRunner};
    stc_tsk_t* const p_stc_tsk_taskListExpected = &stc_tsk_taskListExpected;
    stc_tsk_t* p_stc_tsk_taskListActual = NULL;

    fn_sdlr_setTaskAttributes(p_stc_tsk_taskListExpected, (uint8_t)1U);
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
    const uint8_t u8_taskCountExpected = (uint8_t)0U;

    TEST_ASSERT_EQUAL_UINT8( u8_taskCountExpected, fn_sdlr_getTaskCount() );

    return;
}

/**
 * \brief Test if task Count (number of task entries in the connected task
 * list) is set and returned correctly
 */
void test_sdlr_taskCountIsSetAndReturned(void)
{
    stc_tsk_t stc_tsk_taskList = {en_act_OFF,
                                  (uint32_t)1U,
                                  (uint32_t)1U,
                                  (uint32_t)0U,
                                  fn_taskRunner};
    const uint8_t u8_taskCountExpected = (uint8_t)1U;
    uint8_t u8_taskCountActual = (uint8_t)0U;

    fn_sdlr_setTaskAttributes(&stc_tsk_taskList, u8_taskCountExpected);
    u8_taskCountActual = fn_sdlr_getTaskCount();

    TEST_ASSERT_EQUAL_UINT8(u8_taskCountExpected, u8_taskCountActual);

    return;
}

/** \brief Test that task deadline overrun is initialized to `0` */
void test_sdlr_taskOverrunIsInitializedTo0(void)
{
    const uint8_t u8_taskOverrunExpected = (uint8_t)0U;

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
        { en_act_ON, (uint32_t)100U, (uint32_t)100U, (uint32_t)0U,
          fn_taskRunner }
    };
    const uint8_t u8_taskOverrunExpected = (uint8_t)0U;

    fn_getTickCount_ExpectAndReturn( (uint32_t)1000U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)1099U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
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
        { en_act_ON, (uint32_t)100U, (uint32_t)50U, (uint32_t)0U,
          fn_taskRunner }
    };
    const uint8_t u8_taskOverrunExpected = (uint8_t)1U;

    fn_getTickCount_ExpectAndReturn( (uint32_t)1000U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)1051U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
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
        { en_act_ON, (uint32_t)100U, (uint32_t)100U, (uint32_t)0U,
          fn_taskRunner }
    };
    uint8_t u8_idx = (uint8_t)0U;
    const uint8_t u8_taskOverrunExpected = (uint8_t)2U;

    /* Task overrun */
    fn_getTickCount_ExpectAndReturn( (uint32_t)1000U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)1101U );

    /* No task overrun */
    fn_getTickCount_ExpectAndReturn( (uint32_t)1110U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)1120U );

    /* Task overrun */
    fn_getTickCount_ExpectAndReturn( (uint32_t)1200 );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)1301U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
    for(u8_idx = (uint8_t)0U; u8_idx < 3; u8_idx++)
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
        { en_act_ON, (uint32_t)100U, (uint32_t)100U, (uint32_t)0U,
          fn_taskRunner }
    };
    uint8_t u8_idx = (uint8_t)0U;
    const uint8_t u8_taskOverrunExpected = (uint8_t)2U;

    /* Task overrun (at rollover) */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX - (uint32_t)10U); /* 4294967285 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)100U );

    /* No task overrun */
    fn_getTickCount_ExpectAndReturn( (uint32_t)20U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)30U );

    /* Task overrun */
    fn_getTickCount_ExpectAndReturn( (uint32_t)130 );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)231U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
    for(u8_idx = (uint8_t)0U; u8_idx < 3; u8_idx++)
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
        { en_act_ON, (uint32_t)100U, (uint32_t)100U,
          UINT32_MAX - (uint32_t)200U, fn_taskRunner }
    };
    const uint8_t u8_taskOverrunExpected = (uint8_t)1U;

    /* Task overrun (at rollover) */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX - (uint32_t)10U); /* 4294967285 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)10U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
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
        { en_act_ON, (uint32_t)10U, (uint32_t)10U, (uint32_t)0U, fn_taskRunner }
    };
    const uint8_t u8_taskOverrunExpectedA = (uint8_t)1U;
    const uint8_t u8_taskOverrunExpectedB = (uint8_t)0U;

    /* Task overrun */
    fn_getTickCount_ExpectAndReturn( (uint32_t)10U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)21U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
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
        { en_act_OFF, (uint32_t)1U, (uint32_t)1U, (uint32_t)0U, fn_taskRunner }
    };
    const unsigned int en_act_activeExpected = (unsigned int)en_act_ON;

    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
    fn_sdlr_setTaskAct(fn_taskRunner, en_act_ON);

    TEST_ASSERT_EQUAL_UINT(en_act_activeExpected,
                           (unsigned int)a_stc_tsk_taskList[0].en_act_active);

    return;
}

/** \brief Test if single task is disabled correctly */
void test_sdlr_singleTaskIsDisabled(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { en_act_ON, (uint32_t)1U, (uint32_t)1U, (uint32_t)0U, fn_taskRunner }
    };
    const unsigned int en_act_activeExpected = (unsigned int)en_act_OFF;

    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
    fn_sdlr_setTaskAct(fn_taskRunner, en_act_OFF);

    TEST_ASSERT_EQUAL_UINT(en_act_activeExpected,
                           (unsigned int)a_stc_tsk_taskList[0].en_act_active);

    return;
}

/**
 * \brief Test if multiple identical task runners are enabled and disabled
 * correctly
 */
void test_sdlr_multipleSameTasksAreEnabledAndDisabled(void)
{
    unsigned int u_UNTOUCHED = (unsigned int)en_act_OFF + (unsigned int)1U;
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { en_act_ON, (uint32_t)1U, (uint32_t)1U, (uint32_t)0U, fn_taskRunner0 },
        { en_act_OFF, (uint32_t)1U, (uint32_t)1U, (uint32_t)0U, fn_taskRunner1 },
        { en_act_ON, (uint32_t)1U, (uint32_t)1U, (uint32_t)0U, fn_taskRunner0 },
        { u_UNTOUCHED, (uint32_t)1U, (uint32_t)1U, (uint32_t)0U,
          fn_taskRunner2 },
        { en_act_OFF, (uint32_t)1U, (uint32_t)1U, (uint32_t)0U, fn_taskRunner1 },
        { en_act_ON, (uint32_t)1U, (uint32_t)1U, (uint32_t)0U, fn_taskRunner0 },
        { en_act_OFF, (uint32_t)1U, (uint32_t)1U, (uint32_t)0U, fn_taskRunner1 }
    };
    unsigned int u_activeExpectedA = (unsigned int)en_act_OFF;
    unsigned int u_activeExpectedB = (unsigned int)en_act_ON;

    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)7U);
    fn_sdlr_setTaskAct(fn_taskRunner0, u_activeExpectedA);
    fn_sdlr_setTaskAct(fn_taskRunner1, u_activeExpectedB);

    /* Should have become disabled */
    TEST_ASSERT_EQUAL_UINT(u_activeExpectedA,
                           (unsigned int)a_stc_tsk_taskList[0].en_act_active);
    TEST_ASSERT_EQUAL_UINT(u_activeExpectedA,
                           (unsigned int)a_stc_tsk_taskList[2].en_act_active);
    TEST_ASSERT_EQUAL_UINT(u_activeExpectedA,
                           (unsigned int)a_stc_tsk_taskList[5].en_act_active);

    /* Should have become enabled */
    TEST_ASSERT_EQUAL_UINT(u_activeExpectedB,
                           (unsigned int)a_stc_tsk_taskList[1].en_act_active);
    TEST_ASSERT_EQUAL_UINT(u_activeExpectedB,
                           (unsigned int)a_stc_tsk_taskList[4].en_act_active);
    TEST_ASSERT_EQUAL_UINT(u_activeExpectedB,
                           (unsigned int)a_stc_tsk_taskList[6].en_act_active);

    /* Should have stayed untouched */
    TEST_ASSERT_EQUAL_UINT(u_UNTOUCHED,
                           (unsigned int)a_stc_tsk_taskList[3].en_act_active);

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
        { en_act_ON, (uint32_t)1U, (uint32_t)1U, (uint32_t)0U, fn_taskRunner }
    };
    uint8_t u8_idx = (uint8_t)0U;

    /* No run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)0U );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)1U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)1U );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)2U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)2U );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)3U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)3U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
    for(u8_idx = (uint8_t)0U; u8_idx < 4; u8_idx++)
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
        { en_act_ON, (uint32_t)2U, (uint32_t)2U, (uint32_t)0U, fn_taskRunner }
    };
    uint8_t u8_idx = (uint8_t)0U;

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)201U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)201U );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)202U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)202U );

    /* No run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)203U );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)204U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)204U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
    for(u8_idx = (uint8_t)0U; u8_idx < 4; u8_idx++)
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
        { en_act_ON, (uint32_t)2U, (uint32_t)2U, (uint32_t)0U, fn_taskRunner }
    };
    uint8_t u8_idx = (uint8_t)0U;

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)201U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)201U );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)302U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)302U );

    /* No run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)303U );

    /* Run */
    fn_getTickCount_ExpectAndReturn( (uint32_t)507U );
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)507U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
    for(u8_idx = (uint8_t)0U; u8_idx < 4; u8_idx++)
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
        { en_act_ON, (uint32_t)3U, (uint32_t)3U, (uint32_t)0U, fn_taskRunner0 },
        { en_act_ON, (uint32_t)4U, (uint32_t)4U, (uint32_t)0U, fn_taskRunner1 },
        { en_act_ON, (uint32_t)9U, (uint32_t)9U, (uint32_t)0U, fn_taskRunner2 }
    };
    uint8_t u8_idx = (uint8_t)0U;

    /* Run task runner 0, 1, 2 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)201U );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)202U );
    fn_getTickCount_ExpectAndReturn( (uint32_t)202U );
    fn_taskRunner1_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)203U );
    fn_getTickCount_ExpectAndReturn( (uint32_t)203U );
    fn_taskRunner2_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)204U );

    /* Run task runner 0, 0, 1, 0, 2 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)206U );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)207U );
    fn_getTickCount_ExpectAndReturn( (uint32_t)207U );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)208U );
    fn_getTickCount_ExpectAndReturn( (uint32_t)209U );
    fn_taskRunner1_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)210U );
    fn_getTickCount_ExpectAndReturn( (uint32_t)210U );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)211U );
    fn_getTickCount_ExpectAndReturn( (uint32_t)211U );
    fn_taskRunner2_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)212U );

    /* Run task runner 0, 1 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)214U );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)215U );
    fn_getTickCount_ExpectAndReturn( (uint32_t)215U );
    fn_taskRunner1_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)216U );

    /* Run task runner 0, 1, 2 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)501U );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)502U );
    fn_getTickCount_ExpectAndReturn( (uint32_t)502U );
    fn_taskRunner1_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)503U );
    fn_getTickCount_ExpectAndReturn( (uint32_t)503U );
    fn_taskRunner2_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)504U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)3U);
    for(u8_idx = (uint8_t)0U; u8_idx < 13; u8_idx++)
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
        { en_act_ON,
          (uint32_t)30U,
          (uint32_t)30U,
          (uint32_t)( (uint32_t)0U - (uint32_t)20U ), /* Offset 20 */
          fn_taskRunner0 },
        /* Run at: 30, 70, 110, ... */
        { en_act_ON,
          (uint32_t)40U,
          (uint32_t)40U,
          (uint32_t)( (uint32_t)0U - (uint32_t)10U ), /* Offset 10 */
          fn_taskRunner1 },
        /* Run at: 90, 180, ... */
        { en_act_ON,
          (uint32_t)90U,
          (uint32_t)90U,
          (uint32_t)0U, /* No offset (0) */
          fn_taskRunner2 }
    };
    uint8_t u8_idx = (uint8_t)0U;

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)0U );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)9U );

    /* Run task runner 0 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)10U );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)10U );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)11U );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)29U );

    /* Run task runner 1 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)30U );
    fn_taskRunner1_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)30U );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)31U );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)39U );

    /* Run task runner 0 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)40U );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)40U );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)41U );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)69U );

    /* Run task runner 0, 1 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)70U );
    fn_taskRunner0_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)70U );
    fn_getTickCount_ExpectAndReturn( (uint32_t)70U );
    fn_taskRunner1_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)70U );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)71U );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)89U );

    /* Run task runner 2 */
    fn_getTickCount_ExpectAndReturn( (uint32_t)90U );
    fn_taskRunner2_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)90U );

    /* Run no task runner */
    fn_getTickCount_ExpectAndReturn( (uint32_t)91U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)3U);
    for(u8_idx = (uint8_t)0U; u8_idx < 17; u8_idx++)
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
        { en_act_OFF, (uint32_t)10U, (uint32_t)10U, (uint32_t)0U, fn_taskRunner }
    };

    /* Expect only one call to `getTickCount` as no task runner is supposed to
     * run.
     * Now, the test will fail (as it should), if disabled task’s
     * `fn_taskRunner` is called.
     */
    fn_getTickCount_ExpectAndReturn( (uint32_t)10U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
    fn_sdlr_execute();

    return;
}

/** \brief Test that `lastRun` value of disabled task is still updated */
void test_sdlr_lastRunOfDisabledTaskIsStillUpdated(void)
{
    stc_tsk_t a_stc_tsk_taskList[] =
    {
        { en_act_OFF, (uint32_t)10U, (uint32_t)10U, (uint32_t)0U, fn_taskRunner }
    };
    const uint32_t u32_lastRunExpected = (uint32_t)320U;

    fn_getTickCount_ExpectAndReturn( (uint32_t)327U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
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
        { en_act_ON, (uint32_t)10U, (uint32_t)10U, (uint32_t)0U, fn_taskRunner }
    };
    /* Begin of period for this test’s use case scenario (see comment below) */
    const uint32_t u32_lastRunExpected = (uint32_t)320U;

    fn_getTickCount_ExpectAndReturn( (uint32_t)327U ); /* Begin of period: 320 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn( (uint32_t)329U );

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
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
        { en_act_ON, (uint32_t)3U, (uint32_t)3U, (uint32_t)0U, fn_taskRunner }
    };

    uint8_t u8_idx = (uint8_t)0U;

    /* Run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX - (uint32_t)3U); /* 4294967292 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn(UINT32_MAX - (uint32_t)3U);

    /* No run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX - (uint32_t)2U); /* 4294967293 */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX - (uint32_t)1U); /* 4294967294 */

    /* Run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX); /* 4294967295 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn(UINT32_MAX);

    /* No run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)1U); /* 0 */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)2U); /* 1 */

    /* Run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)3U); /* 2 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)3U);

    /* No run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)4U); /* 3 */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)5U); /* 4 */

    /* Run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)6U); /* 5 */
    fn_taskRunner_Expect();
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)6U);

    /* No run */
    fn_getTickCount_ExpectAndReturn(UINT32_MAX + (uint32_t)7U); /* 6 */

    fn_sdlr_setTickCountSource(fn_getTickCount);
    fn_sdlr_setTaskAttributes(a_stc_tsk_taskList, (uint8_t)1U);
    for(u8_idx = (uint8_t)0U; u8_idx < 11; u8_idx++)
    {
        fn_sdlr_execute();
    }

    return;
}

#endif /* TEST */
