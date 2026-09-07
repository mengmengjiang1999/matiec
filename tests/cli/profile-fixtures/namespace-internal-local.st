NAMESPACE INTERNAL Secret
TYPE
  Value : INT;
END_TYPE

FUNCTION Identity : Value
  VAR_INPUT
    Input : Value;
  END_VAR
  Identity := Input;
END_FUNCTION
END_NAMESPACE
