PROGRAM AccessDemo
  VAR
    Value : INT;
  END_VAR
  Value := Value;
END_PROGRAM

CONFIGURATION AccessConfig
  VAR_GLOBAL
    Setpoint : INT := 7;
  END_VAR

  RESOURCE Resource1 ON PLC
    TASK MainTask(INTERVAL := T#1000ms, PRIORITY := 0);
    PROGRAM Instance0 WITH MainTask : AccessDemo;
  END_RESOURCE

  VAR_ACCESS
    RemoteSetpoint : Setpoint : INT READ_WRITE;
    MonitorSetpoint : Setpoint : INT;
  END_VAR
END_CONFIGURATION
