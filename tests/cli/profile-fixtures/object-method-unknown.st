FUNCTION_BLOCK Counter
  VAR
    Count : INT;
  END_VAR
  Count := Count;

  METHOD PUBLIC Increment : INT
    Increment := Count;
  END_METHOD
END_FUNCTION_BLOCK

PROGRAM BadMethod
  VAR
    CounterInstance : Counter;
    Result : INT;
  END_VAR
  Result := CounterInstance.Missing();
END_PROGRAM
