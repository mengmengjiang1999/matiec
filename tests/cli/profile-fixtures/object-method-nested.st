FUNCTION_BLOCK NestedCounter
  VAR Count : INT := 0; END_VAR
  Count := Count;

  METHOD PUBLIC Increment : INT
    VAR_INPUT Delta : INT; END_VAR
    Count := Count + Delta;
    Increment := Count;
  END_METHOD
END_FUNCTION_BLOCK

PROGRAM NestedDemo
  VAR
    Instance : NestedCounter;
    Result : INT;
  END_VAR
  Result := Instance.Increment(Instance.Increment(1));
END_PROGRAM
