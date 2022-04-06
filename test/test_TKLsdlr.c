/** \file */

#ifdef TEST

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>
#include "assert.h" /* Sanity checks (Design by Contract); replaced for unit
                       testing */

#include "unity.h"

#include "TKLsdlr.h"

#include "TKLtyp.h"
#include "mock_TKLtick.h"

#include "mock_TKLtsk.h"

/* "Invisible" API for unit tests to reset internal state (private vars.) */
extern void TKLsdlr_utClrTickSrc(void);
extern void TKLsdlr_utClrTskLst(void);
extern void TKLsdlr_utClrTskCnt(void);

/* OPERATIONS
 * ==========
 */

/** \brief Run before every test */
void setUp(void) {
    /* Do nothing */
}

/** \brief Run after every test */
void tearDown(void) {
    /* Reset internal state (private vars.) */
    TKLsdlr_utClrTickSrc();
    TKLsdlr_utClrTskLst();
    TKLsdlr_utClrTskCnt();
    TKLsdlr_clrTskOverrun();
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
void test_TKLsdlr_checkUintTickRolloverArith(void) {
    const uint8_t a = 5u;
    const uint8_t b = 250u;
    const uint8_t resExpBMinusA = 245u;
    const uint8_t resExpAMinusB = 11u; /* `-245` with signed int. */

    TEST_ASSERT_EQUAL_UINT8(resExpBMinusA, (b - a));
    TEST_ASSERT_EQUAL_UINT8(resExpAMinusB, (a - b));
}

/**
 * \brief Test that assert fires on attempt to set relative system time tick
 * count source to `NULL`
 */
void test_TKLsdlr_assertNoNullPtrOnSetTickSrc(void) {
    TEST_ASSERT_FAIL_ASSERT(TKLsdlr_setTickSrc(NULL));
}

/**
 * \brief Test that assert fires on attempt to set task list to `NULL` or its
 * length to `0`
 */
void test_TKLsdlr_assertNoNullPtrNo0TskCntOnSetTskLst(void) {
    TKLtyp_tsk_t tskLst[1] = {0};

    TEST_ASSERT_FAIL_ASSERT(TKLsdlr_setTskLst(NULL, 1u));
    TEST_ASSERT_FAIL_ASSERT(TKLsdlr_setTskLst(tskLst, 0u));
}

/**
 * \brief Test that assert fires on attempt to set task activation status with
 * `NULL` pointer task runner or incomplete initialization
 */
void test_TKLsdlr_assertNoNullPtrNo0TskCntOnSetTskAct(void) {
    TKLtyp_tsk_t tskLst[1] = {0};

    /* Fully init. */
    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);

    TEST_ASSERT_FAIL_ASSERT(TKLsdlr_setTskAct(NULL, false, false));

    TKLsdlr_utClrTickSrc(); /* Tick source not set */

    TEST_ASSERT_FAIL_ASSERT(TKLsdlr_setTskAct(&TKLtsk_runner, false, false));

    /* Task list not set */
    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_utClrTskLst();

    TEST_ASSERT_FAIL_ASSERT(TKLsdlr_setTskAct(&TKLtsk_runner, false, false));

    /* Task count not set */
    TKLsdlr_setTskLst(tskLst, 1u);
    TKLsdlr_utClrTskCnt();

    TEST_ASSERT_FAIL_ASSERT(TKLsdlr_setTskAct(&TKLtsk_runner, false, false));
}

/**
 * \brief Test that assert fires on attempt to execute scheduler cycle with
 * incomplete initialization
 */
void test_TKLsdlr_assertNoNullPtrNo0TskCntOnExec(void) {
    TKLtyp_tsk_t tskLst[1] = {0};

    TKLsdlr_setTskLst(tskLst, 1u); /* Tick source not set */

    TEST_ASSERT_FAIL_ASSERT(TKLsdlr_exec());

    /* Task list not set */
    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_utClrTskLst();

    TEST_ASSERT_FAIL_ASSERT(TKLsdlr_exec());

    /* Task count not set */
    TKLsdlr_setTskLst(tskLst, 1u);
    TKLsdlr_utClrTskCnt();

    TEST_ASSERT_FAIL_ASSERT(TKLsdlr_exec());
}

/** \brief Test if relative system time tick count source is set correctly */
void test_TKLsdlr_setTickSrc(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = false,
         .period = 1u,
         .deadline = 1u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };

    TKLtick_getTick_ExpectAndReturn(0u);

    TKLsdlr_setTickSrc(&TKLtick_getTick); /* Set tick count source ... */
    TKLsdlr_setTskLst(tskLst, 1u);
    TKLsdlr_exec(); /* ... and test if it gets called (exactly once) */
}

/** \brief Test if task list is set and returned correctly */
void test_TKLsdlr_setAndReturnTskLst(void) {
    TKLtyp_tsk_t tskLstExp[1] = {0};
    TKLtyp_tsk_t* p_tskLstAct = NULL;

    TKLsdlr_setTskLst(tskLstExp, 1u);
    p_tskLstAct = TKLsdlr_getTskLst();

    TEST_ASSERT_EQUAL_PTR(tskLstExp, p_tskLstAct);
}

/**
 * \brief Test if task count (number of task entries in the connected task
 * list) is set and returned correctly
 */
void test_TKLsdlr_setAndReturnTskCnt(void) {
    TKLtyp_tsk_t tskLst[1] = {0};
    const uint8_t tskCntExp = 1u;
    uint8_t tskCntAct = 0u;

    TKLsdlr_setTskLst(tskLst, tskCntExp);
    tskCntAct = TKLsdlr_cntTsk();

    TEST_ASSERT_EQUAL_UINT8(tskCntExp, tskCntAct);
}

/** \brief Test that task deadline overrun is initialized to `0` */
void test_TKLsdlr_initTskOverrunCntTo0(void) {
    const uint8_t overrunExp = 0u;

    TEST_ASSERT_EQUAL_UINT8(overrunExp, TKLsdlr_cntTskOverrun());
}

/**
 * \brief Test that first scheduling execution cycle without resetting the
 * relative system time tick count before is not counted as a task deadline
 * overrun
 */
void test_TKLsdlr_checkFirstLateSdlrExecIsNoTskOverrun(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = true,
         .period = 100u,
         .deadline = 100u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };
    const uint8_t overrunExp = 0u;

    TKLtick_getTick_ExpectAndReturn(1000u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(1099u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    TKLsdlr_exec();

    TEST_ASSERT_EQUAL_UINT8(overrunExp, TKLsdlr_cntTskOverrun());
}

/**
 * \brief Test if single task deadline overrun is detected and count returned
 * correctly
 */
void test_TKLsdlr_detectAndCntSingleTskOverrun(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = true,
         .period = 100u,
         .deadline = 50u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };
    const uint8_t overrunExp = 1u;

    TKLtick_getTick_ExpectAndReturn(1000u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(1051u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    TKLsdlr_exec();

    TEST_ASSERT_EQUAL_UINT8(overrunExp, TKLsdlr_cntTskOverrun());
}

/**
 * \brief Test if multiple task deadline overruns are detected and counts
 * returned correctly
 */
void test_TKLsdlr_detectAndCntMultiTskOverrun(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = true,
         .period = 100u,
         .deadline = 100u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };
    const uint8_t overrunExp = 2u;

    /* Task overrun */
    TKLtick_getTick_ExpectAndReturn(1000u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(1101u);

    /* No task overrun */
    TKLtick_getTick_ExpectAndReturn(1110u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(1120u);

    /* Task overrun */
    TKLtick_getTick_ExpectAndReturn(1200);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(1301u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    for (uint8_t i = 0u; i < 3; i++) {
        TKLsdlr_exec();
    }

    TEST_ASSERT_EQUAL_UINT8(overrunExp, TKLsdlr_cntTskOverrun());
}

/**
 * \brief Test if multiple task deadline overruns are detected and counts
 * returned correctly on relative system time tick count rollover
 */
void test_TKLsdlr_detectAndCntMultiTskOverrunOnTickRollover(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = true,
         .period = 100u,
         .deadline = 100u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };
    const uint8_t overrunExp = 2u;

    /* Task overrun (at rollover) */
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX - 10u); /* `4294967285` */
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(100u);

    /* No task overrun */
    TKLtick_getTick_ExpectAndReturn(20u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(30u);

    /* Task overrun */
    TKLtick_getTick_ExpectAndReturn(130);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(231u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    for (uint8_t i = 0u; i < 3; i++) {
        TKLsdlr_exec();
    }

    TEST_ASSERT_EQUAL_UINT8(overrunExp, TKLsdlr_cntTskOverrun());
}

/**
 * \brief Test if single task deadline overrun is detected and count returned
 * on inconvenient relative system time tick count rollover
 *
 * Special "task deadline overrun at time tick rollover" use case scenario.
 * This is an important edge case as it made a previously implemented detailed
 * design solution fail.
 */
void test_TKLsdlr_detectAndCntSingleTskOverrunOnInconvTickRollover(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = true,
         .period = 100u,
         .deadline = 100u,
         .lastRun = UINT32_MAX - 200u, /* Last run (offset) `4294967095` */
         .p_tskRunner = &TKLtsk_runner}
    };
    const uint8_t overrunExp = 1u;

    /* Task overrun (at rollover) */
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX - 10u); /* `4294967285` */
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(10u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    TKLsdlr_exec();

    TEST_ASSERT_EQUAL_UINT8(overrunExp, TKLsdlr_cntTskOverrun());
}

/**
 * \brief Test if task deadline overrun counter gets correctly saturated
 * (instead of rolling over)
 */
void test_TKLsdlr_saturateTskOverrun(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = true,
         .period = 100u,
         .deadline = 10u,
         .lastRun = 0u - 100u, /* Offset 0 */
         .p_tskRunner = &TKLtsk_runner}
    };
    const uint8_t overrunExp = UCHAR_MAX; /* Saturated task overrun count */

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);

    /* Run more scheduler exec. cycles with task overruns than task deadline
       overrun counter can hold so it gets saturated */
    for (uint16_t i = 0u; i < UCHAR_MAX + 3u; i++) {
        /* Task overruns */
        TKLtick_getTick_ExpectAndReturn(tskLst[0].period * i);
        TKLtsk_runner_Expect();
        TKLtick_getTick_ExpectAndReturn((tskLst[0].period * i)
                                        + tskLst[0].deadline + 1u);

        TKLsdlr_exec();
    }

    TEST_ASSERT_EQUAL_UINT8(overrunExp, TKLsdlr_cntTskOverrun());
}

/** \brief Test if task deadline overrun count is reset correctly */
void test_TKLsdlr_clearTskOverrun(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = true,
         .period = 10u,
         .deadline = 10u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };
    const uint8_t overrunExpA = 1u;
    const uint8_t overrunExpB = 0u;

    /* Task overrun */
    TKLtick_getTick_ExpectAndReturn(10u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(21u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    TKLsdlr_exec();

    /* First, assert task overrun is detected (counter not `0`) before reset */
    TEST_ASSERT_EQUAL_UINT8(overrunExpA, TKLsdlr_cntTskOverrun());

    TKLsdlr_clrTskOverrun();

    TEST_ASSERT_EQUAL_UINT8(overrunExpB, TKLsdlr_cntTskOverrun());
}

/** \brief Test if single task is enabled correctly */
void test_TKLsdlr_enaSingleTsk(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = false,
         .period = 1u,
         .deadline = 1u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    TKLsdlr_setTskAct(TKLtsk_runner, true, false);

    TEST_ASSERT_TRUE(tskLst[0].active);
}

/** \brief Test if single task is disabled correctly */
void test_TKLsdlr_disSingleTsk(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = true,
         .period = 1u,
         .deadline = 1u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    TKLsdlr_setTskAct(TKLtsk_runner, false, false);

    TEST_ASSERT_FALSE(tskLst[0].active);
}

/**
 * \brief Test if multiple identical task runners are enabled and disabled
 * correctly
 */
void test_TKLsdlr_enaAndDisMultiSameTsk(void) {
    const bool untouched = false;
    TKLtyp_tsk_t tskLst[] = {
        /* .active, .period, .deadline, .lastRun, .p_tskRunner */
        {true, 1u, 1u, 0u, &TKLtsk_runner0},
        {false, 1u, 1u, 0u, &TKLtsk_runner1},
        {true, 1u, 1u, 0u, &TKLtsk_runner0},
        {untouched, 1u, 1u, 0u, &TKLtsk_runner2},
        {false, 1u, 1u, 0u, &TKLtsk_runner1},
        {true, 1u, 1u, 0u, &TKLtsk_runner0},
        {false, 1u, 1u, 0u, &TKLtsk_runner1}
    };
    const bool activeExpA = false;
    const bool activeExpB = true;

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 7u);
    TKLsdlr_setTskAct(TKLtsk_runner0, activeExpA, false);
    TKLsdlr_setTskAct(TKLtsk_runner1, activeExpB, false);

    /* Should have become disabled */
    TEST_ASSERT_EQUAL_UINT(activeExpA, tskLst[0].active);
    TEST_ASSERT_EQUAL_UINT(activeExpA, tskLst[2].active);
    TEST_ASSERT_EQUAL_UINT(activeExpA, tskLst[5].active);

    /* Should have become enabled */
    TEST_ASSERT_EQUAL_UINT(activeExpB, tskLst[1].active);
    TEST_ASSERT_EQUAL_UINT(activeExpB, tskLst[4].active);
    TEST_ASSERT_EQUAL_UINT(activeExpB, tskLst[6].active);

    /* Should have stayed untouched */
    TEST_ASSERT_EQUAL_UINT(untouched, tskLst[3].active);
}

/** \brief Test if time stamp of last task run is updated correctly */
void test_TKLsdlr_updateSingleTskLastRun(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = false,
         .period = 1u,
         .deadline = 1u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };
    const uint32_t lastRunExp = 111u;

    TKLtick_getTick_ExpectAndReturn(lastRunExp);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    TKLsdlr_setTskAct(TKLtsk_runner, true, true);

    TEST_ASSERT_EQUAL_UINT32(lastRunExp, tskLst[0].lastRun);
}

/**
 * \brief Test correct execution of due-to-run task with 1 time tick period and
 * start at 0 time ticks
 */
void test_TKLsdlr_execDueToRunTskAt1TickPeriodOn0TickStart(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = true,
         .period = 1u,
         .deadline = 1u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };

    /* No run */
    TKLtick_getTick_ExpectAndReturn(0u);

    /* Run */
    TKLtick_getTick_ExpectAndReturn(1u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(1u);

    /* Run */
    TKLtick_getTick_ExpectAndReturn(2u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(2u);

    /* Run */
    TKLtick_getTick_ExpectAndReturn(3u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(3u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    for (uint8_t i = 0u; i < 4; i++) {
        TKLsdlr_exec();
    }
}

/**
 * \brief Test correct execution of due-to-run task with 2 time ticks period
 * and start at non-0 time tick
 */
void test_TKLsdlr_execDueToRunTskAt2TicksPeriodOnNon0TickStart(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = true,
         .period = 2u,
         .deadline = 2u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };

    /* Run */
    TKLtick_getTick_ExpectAndReturn(201u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(201u);

    /* Run */
    TKLtick_getTick_ExpectAndReturn(202u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(202u);

    /* No run */
    TKLtick_getTick_ExpectAndReturn(203u);

    /* Run */
    TKLtick_getTick_ExpectAndReturn(204u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(204u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    for (uint8_t i = 0u; i < 4; i++) {
        TKLsdlr_exec();
    }
}

/**
 * \brief Test correct execution of due-to-run task with 2 time ticks period
 * and start at non-0 time tick and skipped time ticks
 */
void test_TKLsdlr_execDueToRunTskAt2TicksPeriodOnNon0TickStartAndTickLoss(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = true,
         .period = 2u,
         .deadline = 2u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };

    /* Run */
    TKLtick_getTick_ExpectAndReturn(201u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(201u);

    /* Run */
    TKLtick_getTick_ExpectAndReturn(302u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(302u);

    /* No run */
    TKLtick_getTick_ExpectAndReturn(303u);

    /* Run */
    TKLtick_getTick_ExpectAndReturn(507u);
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(507u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    for (uint8_t i = 0u; i < 4; i++) {
        TKLsdlr_exec();
    }
}

/**
 * \brief Test correct execution of multiple due-to-run tasks with different
 * time tick periods and start at non-0 time tick and skipped time ticks
 */
void test_TKLsdlr_execDueToRunTskAtDiffPeriodOnNon0TickStartAndTickLoss(void) {
    TKLtyp_tsk_t tskLst[] = {
        /* .active, .period, .deadline, .lastRun, .p_tskRunner */
        {true, 3u, 3u, 0u, TKLtsk_runner0},
        {true, 4u, 4u, 0u, TKLtsk_runner1},
        {true, 9u, 9u, 0u, TKLtsk_runner2}
    };

    /* Run task runner 0, 1, 2 */
    TKLtick_getTick_ExpectAndReturn(201u);
    TKLtsk_runner0_Expect();
    TKLtick_getTick_ExpectAndReturn(202u);
    TKLtick_getTick_ExpectAndReturn(202u);
    TKLtsk_runner1_Expect();
    TKLtick_getTick_ExpectAndReturn(203u);
    TKLtick_getTick_ExpectAndReturn(203u);
    TKLtsk_runner2_Expect();
    TKLtick_getTick_ExpectAndReturn(204u);

    /* Run task runner 0, 0, 1, 0, 2 */
    TKLtick_getTick_ExpectAndReturn(206u);
    TKLtsk_runner0_Expect();
    TKLtick_getTick_ExpectAndReturn(207u);
    TKLtick_getTick_ExpectAndReturn(207u);
    TKLtsk_runner0_Expect();
    TKLtick_getTick_ExpectAndReturn(208u);
    TKLtick_getTick_ExpectAndReturn(209u);
    TKLtsk_runner1_Expect();
    TKLtick_getTick_ExpectAndReturn(210u);
    TKLtick_getTick_ExpectAndReturn(210u);
    TKLtsk_runner0_Expect();
    TKLtick_getTick_ExpectAndReturn(211u);
    TKLtick_getTick_ExpectAndReturn(211u);
    TKLtsk_runner2_Expect();
    TKLtick_getTick_ExpectAndReturn(212u);

    /* Run task runner 0, 1 */
    TKLtick_getTick_ExpectAndReturn(214u);
    TKLtsk_runner0_Expect();
    TKLtick_getTick_ExpectAndReturn(215u);
    TKLtick_getTick_ExpectAndReturn(215u);
    TKLtsk_runner1_Expect();
    TKLtick_getTick_ExpectAndReturn(216u);

    /* Run task runner 0, 1, 2 */
    TKLtick_getTick_ExpectAndReturn(501u);
    TKLtsk_runner0_Expect();
    TKLtick_getTick_ExpectAndReturn(502u);
    TKLtick_getTick_ExpectAndReturn(502u);
    TKLtsk_runner1_Expect();
    TKLtick_getTick_ExpectAndReturn(503u);
    TKLtick_getTick_ExpectAndReturn(503u);
    TKLtsk_runner2_Expect();
    TKLtick_getTick_ExpectAndReturn(504u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 3u);
    for (uint8_t i = 0u; i < 13; i++) {
        TKLsdlr_exec();
    }
}

/**
 * \brief Test correct execution of multiple due-to-run tasks with different
 * time tick periods and offsets and start at 0 time ticks
 */
void test_TKLsdlr_execDueToRunTskAtDiffPeriodAndOffsetOn0TickStart(void) {
    TKLtyp_tsk_t tskLst[] = {
        /* Run at: `10`, `40`, `70`, `100`, ... */
        {.active = true,
         .period = 30u,
         .deadline = 30u,
         .lastRun = 0u - 20u, /* Offset `20` */
         .p_tskRunner = &TKLtsk_runner0},
        /* Run at: `30`, `70`, `110`, ... */
        {.active = true,
         .period = 40u,
         .deadline = 40u,
         .lastRun = 0u - 10u, /* Offset `10` */
         .p_tskRunner = &TKLtsk_runner1},
        /* Run at: `90`, `180`, ... */
        {.active = true,
         .period = 90u,
         .deadline = 90u,
         .lastRun = 0u, /* No offset (`0`) */
         .p_tskRunner = &TKLtsk_runner2}
    };

    /* Run no task runner */
    TKLtick_getTick_ExpectAndReturn(0u);

    /* Run no task runner */
    TKLtick_getTick_ExpectAndReturn(9u);

    /* Run task runner 0 */
    TKLtick_getTick_ExpectAndReturn(10u);
    TKLtsk_runner0_Expect();
    TKLtick_getTick_ExpectAndReturn(10u);

    /* Run no task runner */
    TKLtick_getTick_ExpectAndReturn(11u);

    /* Run no task runner */
    TKLtick_getTick_ExpectAndReturn(29u);

    /* Run task runner 1 */
    TKLtick_getTick_ExpectAndReturn(30u);
    TKLtsk_runner1_Expect();
    TKLtick_getTick_ExpectAndReturn(30u);

    /* Run no task runner */
    TKLtick_getTick_ExpectAndReturn(31u);

    /* Run no task runner */
    TKLtick_getTick_ExpectAndReturn(39u);

    /* Run task runner 0 */
    TKLtick_getTick_ExpectAndReturn(40u);
    TKLtsk_runner0_Expect();
    TKLtick_getTick_ExpectAndReturn(40u);

    /* Run no task runner */
    TKLtick_getTick_ExpectAndReturn(41u);

    /* Run no task runner */
    TKLtick_getTick_ExpectAndReturn(69u);

    /* Run task runner 0, 1 */
    TKLtick_getTick_ExpectAndReturn(70u);
    TKLtsk_runner0_Expect();
    TKLtick_getTick_ExpectAndReturn(70u);
    TKLtick_getTick_ExpectAndReturn(70u);
    TKLtsk_runner1_Expect();
    TKLtick_getTick_ExpectAndReturn(70u);

    /* Run no task runner */
    TKLtick_getTick_ExpectAndReturn(71u);

    /* Run no task runner */
    TKLtick_getTick_ExpectAndReturn(89u);

    /* Run task runner 2 */
    TKLtick_getTick_ExpectAndReturn(90u);
    TKLtsk_runner2_Expect();
    TKLtick_getTick_ExpectAndReturn(90u);

    /* Run no task runner */
    TKLtick_getTick_ExpectAndReturn(91u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 3u);
    for (uint8_t i = 0u; i < 17; i++) {
        TKLsdlr_exec();
    }
}

/** \brief Test that task runner of disabled task is not run */
void test_TKLsdlr_checkRunnerOfDisTskIsNotRun(void) {
    TKLtyp_tsk_t tskLst[] = {
        /* If task runner is run, test will fail (as it should), complaining
           that function `TKLtsk_runner` is called more times than expected */
        {.active = false,
         .period = 10u,
         .deadline = 10u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };

    /* Expect only one call as no task runner is supposed to run.  Now, test
       will fail (as it should), if disabled task’s runner is called. */
    TKLtick_getTick_ExpectAndReturn(10u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    TKLsdlr_exec();
}

/** \brief Test that `lastRun` value of disabled task is still updated */
void test_TKLsdlr_checkLastRunOfDisTskIsStillUpdated(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = false,
         .period = 10u,
         .deadline = 10u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };
    const uint32_t lastRunExp = 320u;

    TKLtick_getTick_ExpectAndReturn(327u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    TKLsdlr_exec();

    TEST_ASSERT_EQUAL_UINT32(lastRunExp, tskLst[0].lastRun);
}

/**
 * \brief Test that `lastRun` value is always set to begin of period (time
 * slot)
 */
void test_TKLsdlr_checkLastRunIsAlwaysSetToBeginOfPeriod(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = true,
         .period = 10u,
         .deadline = 10u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };
    const uint32_t lastRunExp = 320u; /* Begin of period for this test’s use
                                         case scenario (see comment below) */

    TKLtick_getTick_ExpectAndReturn(327u); /* Begin of period: `320` */
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(329u);

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    TKLsdlr_exec();

    TEST_ASSERT_EQUAL_UINT32(lastRunExp, tskLst[0].lastRun);
}

/**
 * \brief Test correct execution of due-to-run task at 3 time ticks period with
 * relative system time tick count rollover
 */
void test_TKLsdlr_execDueToRunTskAt3TickPeriodOnTickRollover(void) {
    TKLtyp_tsk_t tskLst[] = {
        {.active = true,
         .period = 3u,
         .deadline = 3u,
         .lastRun = 0u,
         .p_tskRunner = &TKLtsk_runner}
    };

    /* Run */
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX - 3u); /* `4294967292` */
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX - 3u);

    /* No run */
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX - 2u); /* `4294967293` */
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX - 1u); /* `4294967294` */

    /* Run */
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX); /* `4294967295` */
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX);

    /* No run */
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX + 1u); /* `0` */
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX + 2u); /* `1` */

    /* Run */
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX + 3u); /* `2` */
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX + 3u);

    /* No run */
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX + 4u); /* `3` */
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX + 5u); /* `4` */

    /* Run */
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX + 6u); /* `5` */
    TKLtsk_runner_Expect();
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX + 6u);

    /* No run */
    TKLtick_getTick_ExpectAndReturn(UINT32_MAX + 7u); /* `6` */

    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(tskLst, 1u);
    for (uint8_t i = 0u; i < 11; i++) {
        TKLsdlr_exec();
    }
}

#endif /* TEST */
