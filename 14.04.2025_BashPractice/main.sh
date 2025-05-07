#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Usage: provide source file and arguments"
    exit 1
fi

SOURCE_FILE="$1"
shift
PROGRAM_ARGS="$@"

mkdir -p src bin logs

TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
STUDENT_NUMBERS="8_18"

OUTPUT_FILE="bin/${STUDENT_NUMBERS}_${TIMESTAMP}.out"
LOG_FILE="logs/${STUDENT_NUMBERS}_${TIMESTAMP}.log"

echo "Compiling $SOURCE_FILE..." | tee -a "$LOG_FILE"
gcc "src/$SOURCE_FILE" -o "$OUTPUT_FILE" 2>&1 | tee -a "$LOG_FILE"

if [ ${PIPESTATUS[0]} -eq 0 ]; then
    echo "Compilation successful" | tee -a "$LOG_FILE"
    
    echo "Running program with arguments: $PROGRAM_ARGS" | tee -a "$LOG_FILE"
    "./$OUTPUT_FILE" $PROGRAM_ARGS 2>&1 | tee -a "$LOG_FILE"
    EXIT_CODE=${PIPESTATUS[0]}
    
    echo "Program execution completed with exit code: $EXIT_CODE" | tee -a "$LOG_FILE"
    if [ $EXIT_CODE -eq 0 ]; then
        echo "Program executed successfully" | tee -a "$LOG_FILE"
    else
        echo "Program execution failed" | tee -a "$LOG_FILE"
    fi
else
    echo "Compilation failed" | tee -a "$LOG_FILE"
fi

echo "Timestamp: $(date)" | tee -a "$LOG_FILE" 