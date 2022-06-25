FUNCTION_BLOCK FbTask1
VAR_EXTERNAL

END_VAR

VAR_INPUT
Ts_ms :UDINT := 100;
Reset :BOOL  := FALSE;
END_VAR

VAR_OUTPUT
Blink:BOOL :=FALSE;
END_VAR

VAR
Timer1 :UDINT;
END_VAR
(*Blink meandr period 10000ms*)

IF ((Timer1 >= 10000) OR Reset) THEN
Timer1 := 0;
ELSE
Timer1 := Timer1 + Ts_ms;
END_IF;

IF (Timer1 <= 5000) THEN
Blink := TRUE;
ELSE
Blink := FALSE;
END_IF;



END_FUNCTION_BLOCK