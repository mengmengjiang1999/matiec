FUNCTION_BLOCK Counter
  VAR
    Count : INT := 0;
  END_VAR

  Count := Count;

  METHOD PUBLIC Increment : INT
    VAR_INPUT
      Delta : INT;
    END_VAR
    VAR
      Amount : INT;
    END_VAR
    Amount := Delta;
    Count := Count + Amount;
    Increment := Count;
  END_METHOD
END_FUNCTION_BLOCK

PROGRAM MethodDemo
  VAR
    CounterInstance : Counter;
    Result : INT;
  END_VAR
  Result := CounterInstance.Increment(Delta := 2);
END_PROGRAM

CONFIGURATION config
  RESOURCE resource1 ON PLC
    TASK main_task(INTERVAL := T#1000ms, PRIORITY := 0);
    PROGRAM instance0 WITH main_task : MethodDemo;
  END_RESOURCE
END_CONFIGURATION
