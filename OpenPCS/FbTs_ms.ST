FUNCTION_BLOCK FbTs_ms
VAR_EXTERNAL

END_VAR

VAR_INPUT
RTC_in :DATE_AND_TIME;
Reset :BOOL  := FALSE;
END_VAR

VAR_OUTPUT
Ts_ms :UDINT := 0;
END_VAR

VAR
RTC_cur :DATE_AND_TIME;
RTC_prev :DATE_AND_TIME;
RTC_delta_time :TIME;
RTC_delta_dint :DINT;
END_VAR
(*PLC previous scan time ms*)
IF (Reset) THEN
RTC_cur := RTC_in;
RTC_prev := RTC_in;
ELSE
RTC_cur := RTC_in;
RTC_delta_time := RTC_cur - RTC_prev;
RTC_prev := RTC_cur;
RTC_delta_dint := TIME_TO_DINT(RTC_delta_time);

  (*Limit 0...1500ms*)
  IF ((RTC_delta_dint >= 0) AND (RTC_delta_dint <= 1500)) THEN
  Ts_ms := DINT_TO_UDINT(RTC_delta_dint);
  ELSE
  Ts_ms := 0;
  END_IF;

END_IF;



END_FUNCTION_BLOCK