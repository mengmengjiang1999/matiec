PROGRAM AssertionFormatting
  VAR
    Ready : BOOL := TRUE;
  END_VAR

  ASSERT (* condition remains ordinary ST *) (
    Ready
  );
END_PROGRAM
