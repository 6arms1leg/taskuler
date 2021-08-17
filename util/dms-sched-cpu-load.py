# Schedulability analysis
# =======================
#
# The goal of the schedulability analysis is to assess whether a given task set
# (tasks within a task list) can be scheduled, i.e., whether its schedulability
# is feasable.
#
# For that, the two main objektives are:
#
# * OBJECTIVE 1 - Assert that the total CPU load is below the target limit
# * OBJECTIVE 2 - Assert that each task’s WCRT is less than or equal to its
#   deadline
#
# Note on objective 1:
#
# Upper bounds, e.g. least uppper bound (harmonic/non-harmonic/harmonic
# chains) or hyperbolic bound, are neither applicable to assess schedulability
# feasibility nor the CPU load budget.
# They are only valid for the rate-monotonic scheduling scheme (with
# preemption).
# However, schedulability can be guaranteed (as a "sufficient condition") by
# objective 2.
# Unfortunately, objective 2 does not allow for a CPU load budget assessment.
#
# References
# ----------
#
# [1] *Scheduling Algorithms for Multiprogramming in a Hard-Real-Time
#     Environment* (Journal of the Association for Computing Machinery, Vol.
#     20, No. 1, 1973-01-01, pp. 46--61; C. L. Liu, J. W. Layland)
# [2] *Finding Response Times in a Real-Time System* (The Computer Journal,
#     Vol. 29, No. 5, 1986; M. Joseph, P. Pandya)

import argparse
import pandas as pd
import math
import sys

# WCRT calc. iteration limit after which no convergence is assumed
convTryLim = 50

# Return function handle of argument type function for ArgumentParser checking
# float range: min <= arg <= max
def floatRange(min, max):

    # Define the function with default arguments
    def floatRangeChecker(arg):
        # New type function for argparse - float within predefined range

        try:
            floatVal = float(arg)
        except ValueError:
            raise argparse.ArgumentTypeError('Must be a floating point number')
        if floatVal < min or floatVal > max:
            raise argparse.ArgumentTypeError('Must be in range [' + str(min)
                                             + '..' + str(max)+']')
        return floatVal

    # Return function handle to checking function
    return floatRangeChecker

# Handle positional and optional arguments
parser = argparse.ArgumentParser(description='Perform schedulability analysis \
                                 for partly preemptive DMS based on CSV input \
                                 file')
parser.add_argument('-t', '--timeTick', type=floatRange(0.0, 10.0),
                    help='Seconds corresponding to one time tick (improves \
                    accuracy of the analysis)')
parser.add_argument('-l', '--cpuLoadLim', type=floatRange(0.0, 100.0),
                    help='CPU load limit in %% (if exceeded, script returns \
                    non-zero exit code, e.g. for CI purposes)')
parser.add_argument('inputFile', help='CSV input file')
parser.add_argument('outputFileBase', help='MD output file base name')
args = parser.parse_args()

# If time tick argument is not provided ...
if not args.timeTick:
    args.timeTick = 0

# Read timing table CSV input file
df = pd.read_csv(args.inputFile, skipinitialspace=True, comment='#')

# Print read input for visual confirmation
print(df.to_string())

# Sort timing table by deadline
# "Mergesort" algorithm is used as it only changes the order if necessary.
# Tasks with equal deadlines stay in the same (maybe intentional) order as in
# the input timing table.
df.sort_values(by=['Deadline in s'], inplace=True, kind='mergesort')
df = df.reset_index(drop=True)

# Add priority column
df.insert(1, 'Prio.', list(range(1, len(df) + 1)))

# Calc. and add period column
df['Period in s'] = 1 / df['Freq. in Hz']

# OBJECTIVE 1 - Calculate total CPU load
# --------------------------------------
#
# The CPU utilization per task is simply its WCET divided by its period.
# The total CPU load is then the sum of all task’s CPU utilizations.
# See [1].

# Calc. CPU utilization for each task (in %)
df['CPU util. in %'] = df['WCET in s'] / df['Period in s'] * 100

# Calc. total CPU load
cpuLoad = df['CPU util. in %'].sum()

# OBJECTIVE 2 - Calculate WCRT
# ----------------------------
#
# Depending on the scheduling strategy, two different WCRT calculation
# algorithms must be employed:
#
# * ALGORITHM 1 - Taskuler with only cooperative tasks,
# * ALGORITHM 2 - Taskuler augmented with (nested) interrupts.
#
# Algorithm 2 gives an upper boundary (worst-case), and therefore can also be
# used for systems without interrupt priorization (no nesting).
# All algorithms assume a timing table that is already sorted by deadline.

# Count
#
# * cooperative tasks, and
# * preemptive tasks (with priority; nested interrupts)
#
# in order to decide which algorithm to employ
schedule = df['Sched.'].value_counts()

# ALGORITHM 1 - Taskuler with only cooperative tasks
#
# The Taskuler is used to schedule all tasks without augmentation by
# interrupts.
# Note, however, that the Taskuler needs a relative system time tick, which is
# normally implemented via an interrupt.
#
# This algorithm consists of STEPs 1--3.
if 'co' in schedule and 'pe' not in schedule:
    print('\nWCRT calc. for cooperative tasks ...\n')
    wcrt = []
    for idx, wcet in enumerate(df['WCET in s']): # For each task: ...

        # ... STEP1 - Sum up all the WCETs of all higher prio. tasks
        wcrtTmp = df['WCET in s'].loc[:idx].sum()

        # ... If it is not the lowest prio. task:
        # STEP2 - Find and add the one longest WCET out of of all lower prio.
        # tasks and
        # STEP3 - Substract the time of 1 time tick
        if idx is not len(df.index)-1:
            wcrtTmp += df['WCET in s'].loc[idx+1:].max() - args.timeTick

        wcrt.append(wcrtTmp)
    df['WCRT in s'] = wcrt

# ALGORITHM 2 - Taskuler augmented with (nested) interrupts
#
# The Taskuler is used to schedule most tasks but augmented by (nested)
# interrupts for the scheduling of some high priority tasks.
#
# This algorithm consists of STEPs 1--4.3.
elif 'co' in schedule and 'pe' in schedule:
    print('\nWCRT calc. for cooperative and preemptive tasks (with priority;')
    print('  nested interrupts) ...\n')

    # STEP 1 - Divide timing table in two seperate tables, one with all
    # preemptive and one with all cooperative tasks
    co = df[df['Sched.'] == 'co']
    pe = df[df['Sched.'] == 'pe']
    co = co.reset_index(drop=True)
    pe = pe.reset_index(drop=True)

    # STEP 2 - Calculate the WCRT for each task in the preemptive timing table
    # as per [2], eq. (1)

    # The WCRT of the highest priority preemptive task is its WCET
    wcrtPn = [pe['WCET in s'].loc[0]]

    for idx_1, elem in enumerate(pe['WCET in s'].loc[1:]): # For each task
                                                           # except the first:
                                                           # ...
        wcrtPrev = 0
        wcrtNow = elem
        convTryCnt = 0
        while wcrtNow != wcrtPrev:
            wcrtPrev = wcrtNow
            wcrtNow = elem
            for idx_2 in range(idx_1+1):
                wcrtNow += math.ceil(wcrtPrev / pe['Period in s'].loc[idx_2]) \
                           * pe['WCET in s'].loc[idx_2]
            if convTryCnt >= convTryLim:
                print('\nNo convergence in PE WCRT calc. for '
                      + pe['Task'].loc[idx_1] + ' after ' + str(convTryLim)
                      + ' iterations.')
                sys.exit(1)
            else:
                convTryCnt += 1
        wcrtPn.append(wcrtNow)

    # STEP 3 - Calculate WCRT' for each task in the cooperative timing table.
    #
    # WCRT' is the resulting WCRT of a cooperative task caused by pre-empting
    # tasks, with the effect of other cooperative tasks not yet included.
    #
    # This is done by appending each cooperative task seperately (as the lowest
    # priority task) to the preemptive timing table and calculate its WCRT' as
    # per [2], eq. (1).
    wcrtCo = []
    for idx_1, elem_1 in enumerate(co['WCET in s']):
        wcrtPrev = 0
        wcrtNow = elem_1
        convTryCnt = 0
        while wcrtNow != wcrtPrev:
            wcrtPrev = wcrtNow
            wcrtNow = elem_1
            for idx_2, elem_2 in enumerate(pe['WCET in s']):
                wcrtNow += math.ceil(wcrtPrev / pe['Period in s'].loc[idx_2]) \
                           * elem_2
            if convTryCnt >= convTryLim:
                print('\nNo convergence in CO WCRT calc. for '
                      + co['Task'].loc[idx_1] + ' after ' + str(convTryLim)
                      + ' iterations.')
                sys.exit(1)
            else:
                convTryCnt += 1
        wcrtCo.append(wcrtNow)

    # STEP 4 - Calculate the final WCRT for each cooperative task by performing
    # the following steps for the WCRT' for each task in the cooperative timing
    # table:
    wcrt = []
    for idx, elem in enumerate(wcrtCo):

        # STEP4.1 - Add sum of all higher priority cooperative task’s WCRT'.
        wcrtTmp = elem + sum(wcrtCo[:idx])

        # ... If it is not the lowest prio. task:
        # STEP4.2 - Find and add the one longest WCRT' out of of all lower
        # priority cooperative tasks and
        # STEP4.3 - Substract the time of 1 time tick
        if idx is not len(wcrtCo)-1:
            wcrtTmp += max(wcrtCo[idx+1:]) - args.timeTick

        wcrt.append(wcrtTmp)
    wcrtCo = wcrt

    pe['WCRT in s'] = wcrtPn
    co['WCRT in s'] = wcrtCo
    df = pd.concat([pe, co])

    # Sort by deadline
    df.sort_values(by=['Prio.'], inplace=True)
    df = df.reset_index(drop=True)
else:
    print('Invalid mix of co/pe')

# Add deadline overrun/violation column and count all violations (`True`s)
df['Deadline overrun?'] = df['Deadline in s'] < df['WCRT in s']
deadlineOverrunCnt = df['Deadline overrun?'].sum()

# Print final timing table for visual confirmation
print(df.to_string())

# Create table with the result summary
res = pd.DataFrame({'Objective': ['Deadline overruns', 'Total CPU load'],
                    'Value': [str(deadlineOverrunCnt), str(cpuLoad) + ' %']})

# Print schedulability result (table with the result summary)
print('\n' + res.to_string(index=False))

# Write final Markdown input timing table
df.loc[:,:'WCET in s'].to_markdown(buf=args.outputFileBase + '-in.md',
                                   index=False)

# Write final Markdown output timing table
pd.concat([df['Task'], df.loc[:,'Period in s':]],
          axis=1).to_markdown(buf=args.outputFileBase + '-out.md', index=False)

# Write final Markdown result timing table
res.to_markdown(buf=args.outputFileBase + '-res.md', index=False)

# If CPU load limit argument is provided ...
if args.cpuLoadLim:
    # Use non-zero exit code if schedulability is unfeasible.
    # This allows for easy employment in continuous integration systems.
    if deadlineOverrunCnt == 0 and cpuLoad <= args.cpuLoadLim:
        print('\n=> Schedulability seems feasible')
        sys.exit(0)
    else:
        print('\n=> Schedulability unfeasible')
        sys.exit(1)

sys.exit(0)
