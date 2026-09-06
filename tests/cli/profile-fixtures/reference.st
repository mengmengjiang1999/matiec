PROGRAM reference_profile_test
  VAR
    value : INT;
    value_ref : REF_TO INT;
  END_VAR

  value_ref := REF(value);
  value := value_ref^;
END_PROGRAM
