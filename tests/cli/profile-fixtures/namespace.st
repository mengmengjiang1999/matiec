NAMESPACE Factory.Motion
TYPE
  Speed : INT;
END_TYPE

FUNCTION Clamp : Speed
  VAR_INPUT
    Value : Speed;
  END_VAR
  Clamp := Value;
END_FUNCTION
END_NAMESPACE

PROGRAM Main
  VAR
    Current : Factory.Motion.Speed := 7;
  END_VAR
  Current := Factory.Motion.Clamp(Current);
END_PROGRAM
