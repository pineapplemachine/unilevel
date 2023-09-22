/**
 * This code was used to quickly develop and debug a fuzzy string matching
 * algorithm for use with Unilevel's command palette matching function.
 * The C++ implementation is based on the scoreMatrix function in this file.
 * 
 * The steps of figuring out a suitable algorithm and how to optimize its
 * performance are recorded here.
 * 
 * This test script can be run like so, if Node.js is installed:
 * $ node scratch/stringdist.js
 */

// This function was used to compute a running average,
// before I realized that factoring in the average of
// match run lengths into the fuzzy match score was not
// especially tenable.
function runAvg(avg, next, count) {
    return next === 0 ? avg : ((avg * count) + next) / (count + 1);
}

// First version of the algorithm. Greedy and naive.
function scoreNaive(needle, haystack) {
    let runLengthCurrent = 0;
    let runLengthMax = 0;
    let runLengthMean = 0.0;
    let runCount = 0;
    let matchCount = 0;
    let needleIndex = 0;
    let haystackIndex = 0;
    while(needleIndex < needle.length && haystackIndex < haystack.length) {
        if(needle[needleIndex] === haystack[haystackIndex]) {
            runLengthCurrent++;
            needleIndex++;
            matchCount++;
        }
        else if(runLengthCurrent) {
            runLengthMean = (
                ((runLengthMean * runCount) + runLengthCurrent) /
                (runCount + 1)
            );
            runLengthMax = Math.max(runLengthMax, runLengthCurrent);
            runLengthCurrent = 0;
            runCount++;
            // console.log(needle[needleIndex], "==", haystack[haystackIndex]);
            // console.log(".runLengthMean:", runLengthMean);
            // console.log(".runLengthMax:", runLengthMax);
        }
        haystackIndex++;
    }
    if(runLengthCurrent > 0) {
        runLengthMean = (
            ((runLengthMean * runCount) + runLengthCurrent) /
            (runCount + 1)
        );
        runLengthMax = Math.max(runLengthMax, runLengthCurrent);
        runLengthCurrent = 0;
        runCount++;
        // console.log("$runLengthMean:", runLengthMean);
        // console.log("$runLengthMax:", runLengthMax);
    }
    return runLengthMax + runLengthMean + matchCount;
}

// First iteration of a recursive algorithm based on the naive one.
// Note that a recursive string search algorithm like this one is very
// far from ideal, due to its exponential computational complexity.
function scoreRecursive(needle, haystack, useState = null) {
    let state = useState || {
        runCurrent: 0,
        runLongest: 0,
        runAverage: 0.0,
        runCount: 0,
        matchCount: 0,
        needleIndex: 0,
        haystackIndex: 0,
    };
    while(state.needleIndex < needle.length && state.haystackIndex < haystack.length) {
        if(needle[state.needleIndex] === haystack[state.haystackIndex]) {
            const score0 = scoreRecursive(needle, haystack, {
                runCurrent: state.runCurrent + 1,
                runLongest: state.runLongest,
                runAverage: state.runAverage,
                runCount: state.runCount,
                matchCount: state.matchCount + 1,
                needleIndex: state.needleIndex + 1,
                haystackIndex: state.haystackIndex + 1,
            });
            const score1 = scoreRecursive(needle, haystack, {
                runCurrent: 0,
                runLongest: Math.max(state.runLongest, state.runCurrent),
                runAverage: runAvg(state.runAverage, state.runCurrent, state.runCount),
                runCount: state.runCount + (state.runCurrent > 0 ? 1 : 0),
                matchCount: state.matchCount,
                needleIndex: state.needleIndex,
                haystackIndex: state.haystackIndex + 1,
            });
            return Math.max(score0, score1);
        }
        else if(!state.runCurrent) {
            const score0 = scoreRecursive(needle, haystack, {
                runCurrent: 0,
                runLongest: state.runLongest,
                runAverage: state.runAverage,
                runCount: state.runCount,
                matchCount: state.matchCount,
                needleIndex: state.needleIndex,
                haystackIndex: state.haystackIndex + 1,
            });
            const score1 = scoreRecursive(needle, haystack, {
                runCurrent: 0,
                runLongest: state.runLongest,
                runAverage: state.runAverage,
                runCount: state.runCount,
                matchCount: state.matchCount,
                needleIndex: state.needleIndex + 1,
                haystackIndex: state.haystackIndex,
            });
            return Math.max(score0, score1);
        }
        else {
            state = {
                runCurrent: 0,
                runLongest: Math.max(state.runLongest, state.runCurrent),
                runAverage: runAvg(state.runAverage, state.runCurrent, state.runCount),
                runCount: state.runCount + 1,
                matchCount: state.matchCount,
                needleIndex: state.needleIndex,
                haystackIndex: state.haystackIndex,
            };
        }
    }
    if(state.runCurrent > 0) {
        state = {
            runCurrent: 0,
            runLongest: Math.max(state.runLongest, state.runCurrent),
            runAverage: runAvg(state.runAverage, state.runCurrent, state.runCount),
            runCount: state.runCount + 1,
            matchCount: state.matchCount,
            needleIndex: state.needleIndex,
            haystackIndex: state.haystackIndex,
        };
    }
    return state.runLongest + state.runAverage + state.matchCount;
}

// Second, simplified implementation of the recursive algorithm.
function scoreRecursive2(needle, haystack, useState = null) {
    let state = useState || {
        runCurrent: 0,
        runLongest: 0,
        runAverage: 0.0,
        runCount: 0,
        matchCount: 0,
        needleIndex: 0,
        haystackIndex: 0,
    };
    while(state.needleIndex < needle.length && state.haystackIndex < haystack.length) {
        const scoreNext = scoreRecursive(needle, haystack, {
            runCurrent: 0,
            runLongest: Math.max(state.runLongest, state.runCurrent),
            runAverage: runAvg(state.runAverage, state.runCurrent, state.runCount),
            runCount: state.runCount + (state.runCurrent > 0 ? 1 : 0),
            matchCount: state.matchCount,
            needleIndex: state.needleIndex,
            haystackIndex: state.haystackIndex + 1,
        });
        if(needle[state.needleIndex] === haystack[state.haystackIndex]) {
            const scoreMatch = scoreRecursive(needle, haystack, {
                runCurrent: state.runCurrent + 1,
                runLongest: state.runLongest,
                runAverage: state.runAverage,
                runCount: state.runCount,
                matchCount: state.matchCount + 1,
                needleIndex: state.needleIndex + 1,
                haystackIndex: state.haystackIndex + 1,
            });
            return Math.max(scoreNext, scoreMatch);
        }
        else {
            const scoreSkip = scoreRecursive(needle, haystack, {
                runCurrent: 0,
                runLongest: Math.max(state.runLongest, state.runCurrent),
                runAverage: runAvg(state.runAverage, state.runCurrent, state.runCount),
                runCount: state.runCount + (state.runCurrent > 0 ? 1 : 0),
                matchCount: state.matchCount,
                needleIndex: state.needleIndex + 1,
                haystackIndex: state.haystackIndex,
            });
            return Math.max(scoreNext, scoreSkip);
        }
    }
    state = {
        runCurrent: 0,
        runLongest: Math.max(state.runLongest, state.runCurrent),
        runAverage: runAvg(state.runAverage, state.runCurrent, state.runCount),
        runCount: state.runCount + (state.runCurrent > 0 ? 1 : 0),
        matchCount: state.matchCount,
        needleIndex: state.needleIndex,
        haystackIndex: state.haystackIndex,
    };
    return state.runLongest + state.runAverage + state.matchCount;
}

// Initial, not quite successful translation of the simplified recursive
// algorithm to an iterative matrix-based one.
// Some credit goes to this resource for helping me to conceptualize this
// matrix-based algorithm:
// https://people.cs.umass.edu/~mccallum/courses/cl2006/lect4-stredit.pdf
function scoreMatrixRough(needle, haystack) {
    const rowLength = 1 + needle.length;
    const colLength = 1 + haystack.length;
    const stateMatrix = new Array(rowLength * colLength).fill(undefined);
    const getStateScore = (state) => {
        return state.runLongest + state.runAverage + state.matchCount;
    };
    const stateInitObject = () => ({
        path: ".",
        runCurrent: 0,
        runLongest: 0,
        runAverage: 0.0,
        runCount: 0,
        matchCount: 0,
        score: 0,
    });
    function printStateMatrix(map) {
        for(let i = 0; i < stateMatrix.length; i += rowLength) {
            console.log(stateMatrix.slice(i, i + rowLength).map(map).join(" "));
        }
        console.log("");
    }
    for(let i = 0; i < rowLength; i++) {
        stateMatrix[i] = stateInitObject();
    }
    for(let j = 1; j < colLength; j++) {
        stateMatrix[j * rowLength] = stateInitObject();
    }
    for(let i = 1; i < rowLength; i++) {
        for(let j = 1; j < colLength; j++) {
            const cellIndex = i + j * rowLength;
            const stateNextPrev = stateMatrix[cellIndex - rowLength];
            const stateNext = {
                path: "n",
                runCurrent: 0,
                runLongest: Math.max(stateNextPrev.runLongest, stateNextPrev.runCurrent),
                runAverage: runAvg(stateNextPrev.runAverage, stateNextPrev.runCurrent, stateNextPrev.runCount),
                runCount: stateNextPrev.runCount + (stateNextPrev.runCurrent > 0 ? 1 : 0),
                matchCount: stateNextPrev.matchCount,
                // needleIndex: state.needleIndex,
                // haystackIndex: state.haystackIndex + 1,
            };
            stateNext.score = getStateScore(stateNext);
            if(needle[i - 1] === haystack[j - 1]) {
                const stateMatchPrev = stateMatrix[cellIndex - rowLength - 1];
                const stateMatch = {
                    path: "=",
                    runCurrent: stateMatchPrev.runCurrent + 1,
                    runLongest: stateMatchPrev.runLongest,
                    runAverage: stateMatchPrev.runAverage,
                    runCount: stateMatchPrev.runCount,
                    matchCount: stateMatchPrev.matchCount + 1,
                    // needleIndex: state.needleIndex + 1,
                    // haystackIndex: state.haystackIndex + 1,
                };
                stateMatch.score = getStateScore(stateMatch);
                stateMatrix[cellIndex] = (
                    stateMatch.score > stateNext.score ?
                    stateMatch : stateNext
                );
            }
            else {
                const stateSkipPrev = stateMatrix[cellIndex - 1];
                const stateSkip = {
                    path: "s",
                    runCurrent: 0,
                    runLongest: Math.max(stateSkipPrev.runLongest, stateSkipPrev.runCurrent),
                    runAverage: runAvg(stateSkipPrev.runAverage, stateSkipPrev.runCurrent, stateSkipPrev.runCount),
                    runCount: stateSkipPrev.runCount + (stateSkipPrev.runCurrent > 0 ? 1 : 0),
                    matchCount: stateSkipPrev.matchCount,
                    // needleIndex: state.needleIndex + 1,
                    // haystackIndex: state.haystackIndex,
                };
                stateSkip.score = getStateScore(stateSkip);
                stateMatrix[cellIndex] = (
                    stateSkip.score > stateNext.score ?
                    stateSkip : stateNext
                );
            }
            printStateMatrix(c => (!c ? ".." : c.path + String(c.score)));
        }
    }
    const lastStatePrev = stateMatrix[stateMatrix.length - 1];
    const lastState = {
        runCurrent: 0,
        runLongest: Math.max(lastStatePrev.runLongest, lastStatePrev.runCurrent),
        runAverage: runAvg(lastStatePrev.runAverage, lastStatePrev.runCurrent, lastStatePrev.runCount),
        runCount: lastStatePrev.runCount + (lastStatePrev.runCurrent > 0 ? 1 : 0),
        matchCount: lastStatePrev.matchCount,
        needleIndex: lastStatePrev.needleIndex,
        haystackIndex: lastStatePrev.haystackIndex,
    };
    console.log("lastStatePrev:", lastStatePrev);
    return getStateScore(lastState);
}

function isBoundaryChar(ch) {
    return !ch || (" -+.,_/()[]{}'\"\\\t".indexOf(ch) >= 0);
}

// Final algorithm, iterative and matrix-based.
function scoreMatrix(needle, haystack) {
    const rowLength = 1 + needle.length;
    const colLength = 1 + haystack.length;
    const stateMatrix = new Array(rowLength * colLength).fill(undefined);
    const getStateScore = (state) => {
        return 1 + (
            state.runLongest * 2 +
            state.runFirst +
            state.runInitial +
            state.runMatchCount * 2 +
            state.matchCount * 2 +
            state.matchBoundaryCount
        );
    };
    const stateInitObject = (path = ".") => ({
        path: path,
        runCurrent: 0,
        runLongest: 0,
        runFirst: 0,
        runInitial: 0,
        matchCount: 0,
        runMatchCount: 0,
        matchBoundaryCount: 0,
        score: 0,
    });
    function printStateMatrix(map) {
        for(let i = 0; i < stateMatrix.length; i += rowLength) {
            console.log(stateMatrix.slice(i, i + rowLength).map(map).join(" "));
        }
        console.log("");
    }
    for(let i = 0; i < rowLength; i++) {
        stateMatrix[i] = stateInitObject(needle[i - 1] || ".");
    }
    for(let j = 1; j < colLength; j++) {
        stateMatrix[j * rowLength] = stateInitObject(haystack[j - 1] || ".");
    }
    for(let i = 1; i < rowLength; i++) {
        for(let j = 1; j < colLength; j++) {
            const cellIndex = i + j * rowLength;
            const stateNextPrev = stateMatrix[cellIndex - rowLength];
            const stateNext = Object.assign({}, stateNextPrev, {
                path: "n",
                runCurrent: 0,
                // needleIndex: state.needleIndex,
                // haystackIndex: state.haystackIndex + 1,
            });
            stateNext.score = getStateScore(stateNext);
            if(needle[i - 1] === haystack[j - 1]) {
                const stateMatchPrev = stateMatrix[cellIndex - rowLength - 1];
                const runCurrent = stateMatchPrev.runCurrent + 1;
                const matchCount = stateMatchPrev.matchCount + 1;
                const stateMatch = {
                    path: "=",
                    runCurrent: runCurrent,
                    runLongest: Math.max(runCurrent, stateMatchPrev.runLongest),
                    runFirst: (matchCount === runCurrent) ? runCurrent : stateMatchPrev.runFirst,
                    runInitial: (
                        (runCurrent >= j) ?
                        runCurrent : stateMatchPrev.runInitial
                    ),
                    matchCount: matchCount,
                    runMatchCount: stateMatchPrev.runMatchCount + (stateMatchPrev.runCurrent > 0 ? 1 : 0),
                    matchBoundaryCount: stateMatchPrev.matchBoundaryCount + (
                        (
                            stateMatchPrev.runCurrent === 0 &&
                            isBoundaryChar(haystack[j - 2])
                        ) ? 1 : 0
                    ),
                    // needleIndex: state.needleIndex + 1,
                    // haystackIndex: state.haystackIndex + 1,
                };
                stateMatch.score = getStateScore(stateMatch);
                stateMatrix[cellIndex] = (
                    stateMatch.score >= stateNext.score ?
                    stateMatch : stateNext
                );
            }
            else {
                const stateSkipPrev = stateMatrix[cellIndex - 1];
                const stateSkip = Object.assign({}, stateSkipPrev, {
                    path: "s",
                    runCurrent: 0,
                    // needleIndex: state.needleIndex + 1,
                    // haystackIndex: state.haystackIndex,
                });
                stateSkip.score = getStateScore(stateSkip);
                stateMatrix[cellIndex] = (
                    stateSkip.score > stateNext.score ?
                    stateSkip : stateNext
                );
            }
            // printStateMatrix(c => (!c ? ".." : c.path + String(c.score)));
        }
    }
    const lastState = stateMatrix[stateMatrix.length - 1];
    console.log(needle, ":", haystack, "lastState:", lastState);
    return (needle.length * 8) - getStateScore(lastState);
}

function testScore(needle, haystack) {
    const naive = scoreNaive(needle, haystack);
    console.log(`Naive:  "${needle}", "${haystack}": ${naive}`);
    const recur = scoreRecursive(needle, haystack);
    console.log(`Recur:  "${needle}", "${haystack}": ${recur}`);
    const recur2 = scoreRecursive2(needle, haystack);
    console.log(`Recur2: "${needle}", "${haystack}": ${recur2}`);
    const matrix = scoreMatrix(needle, haystack);
    console.log(`Matrix: "${needle}", "${haystack}": ${matrix}`);
    console.log("");
}
function testSearch(needle, haystacks) {
    const scores = haystacks.map((haystack) => (
        [haystack, scoreMatrix(needle.toUpperCase(), haystack.toUpperCase())]
    ));
    scores.sort((a, b) => (a[1] - b[1]));
    console.log(`Needle "${needle}":`);
    for(const item of scores) {
        const haystack = item[0];
        const score = item[1];
        console.log(`  Haystack "${haystack}": ${score}`);
    }
}

testScore("h", "hello");
testScore("e", "hello");
testScore("l", "hello");
testScore("o", "hello");
testScore("?", "hello");
testScore("he", "hello");
testScore("ll", "hello");
testScore("lo", "hello");
testScore("h?", "hello");
testScore("?l", "hello");
testScore("?o", "hello");
testScore("??", "hello");
testScore("llo", "hello");
testScore("?llo", "hello");
testScore("llo?", "hello");
testScore("hello", "hello");
testScore("?hello", "hello");
testScore("hello?", "hello");
testScore("?ello", "hello");
testScore("h?llo", "hello");
testScore("he?lo", "hello");
testScore("hel?o", "hello");

const testCommands = [
    // "create new project",
    // "open project",
    // "save project",
    // "save project as",
    // "add new scene to project",
    // "add new chunk to scene",
    // "add new entity to scene",
    // "add new cube primitive to scene",
    // "add new sphere primitive to scene",
    // "add new cylinder primitive to scene",
    // "focus camera on selection",
    // "subdivide selection",
    // "delete selection",
    // "hide selection",
    // "find and replace entities in scene",
    // "test alphabet",
    "Print Goodbye Message",
    "Print Hello World Message",
    "Set Test GUI Message to Alphabet",
    "Set Test GUI Message to Hello World",
    "Set Test GUI Message to Test Message #2",
];
testSearch("a", testCommands);
testSearch("al", testCommands);
// testSearch("entity", testCommands);
// testSearch("cube", testCommands);
// testSearch("add scene", testCommands);
// testSearch("new scene", testCommands);
// testSearch("new pr", testCommands);
// testSearch("re", testCommands);
// testSearch("i", testCommands);
