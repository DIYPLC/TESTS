PROGRAM main
VAR_EXTERNAL

END_VAR

VAR_GLOBAL

END_VAR

VAR
Reset  :BOOL := TRUE; (*PLC First scan*)
DbTask1: FbTask1;
DbGetTaskInfo :GetTaskInfo;
DbTs_ms :FbTs_ms; (*PLC previous scan time ms*)
DbGETSYSTEMDATEANDTIME :GETSYSTEMDATEANDTIME;
END_VAR
(*
HELLO WORD BLINK PROJECT.
VA 25-06-2022
infoteam OpenPCS 7.1
https://www.ascontecnologic.com/product/openpcs/?lang=en
https://www.gcanbus.com/products/plc-con-ru/
https://github.com/DIYPLC/LibPlc
*)

DbGetTaskInfo();

DbGETSYSTEMDATEANDTIME();
DbTs_ms.RTC_in := DbGETSYSTEMDATEANDTIME.ODT;
DbTs_ms.Reset := Reset;
DbTs_ms();

DbTask1.Ts_ms := DbTs_ms.Ts_ms;
DbTask1.Reset := Reset;
DbTask1();

IF (Reset = TRUE) THEN
Reset := FALSE;
END_IF;


END_PROGRAM