PROGRAM AccessDemo
  VAR Value : INT; END_VAR
  Value := Value;
END_PROGRAM

CONFIGURATION AccessConfig
  VAR_GLOBAL
    Setpoint : INT;
  END_VAR
  RESOURCE Resource1 ON PLC
    PROGRAM Instance0 : AccessDemo;
  END_RESOURCE
  VAR_ACCESS
    MissingExport : Missing : INT READ_ONLY;
  END_VAR
END_CONFIGURATION
