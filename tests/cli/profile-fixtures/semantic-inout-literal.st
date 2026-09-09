FUNCTION Bump : INT
  VAR_IN_OUT Target : INT; END_VAR
  Target := Target + 1;
  Bump := Target;
END_FUNCTION

PROGRAM InvalidInOut
  VAR Result : INT; END_VAR
  Result := Bump(1);
END_PROGRAM
