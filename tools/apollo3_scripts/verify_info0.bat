
jlink -CommanderScript jlink-read-info0.txt
diff info0.bin info0_dump.bin
@ECHO OFF
if ERRORLEVEL 1 (
    echo "INFO0 Invalid"
) else (
    echo "INFO0 Valid"
)
pause