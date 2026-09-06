PROGRAM AssertDemo
  VAR
    Result : INT;
  END_VAR
  ASSERT(TRUE);
  ASSERT(FALSE);
  Result := 42;
END_PROGRAM

CONFIGURATION config
  RESOURCE resource1 ON PLC
    TASK main_task(INTERVAL := T#1000ms, PRIORITY := 0);
    PROGRAM instance0 WITH main_task : AssertDemo;
  END_RESOURCE
END_CONFIGURATION
