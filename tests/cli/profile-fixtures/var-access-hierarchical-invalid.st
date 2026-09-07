TYPE
  IntArray : ARRAY [1..3] OF INT;
  DeviceState : STRUCT
    Value : INT;
  END_STRUCT;
END_TYPE

PROGRAM InvalidAccessProducer
  VAR_OUTPUT
    Value : INT;
  END_VAR
  Value := Value;
END_PROGRAM

CONFIGURATION InvalidHierarchicalAccess
  VAR_GLOBAL
    State : DeviceState;
    Values : IntArray;
  END_VAR
  RESOURCE Resource1 ON PLC
    TASK MainTask(INTERVAL := T#1000ms, PRIORITY := 0);
    PROGRAM Producer WITH MainTask : InvalidAccessProducer;
  END_RESOURCE
  VAR_ACCESS
    MissingField : State.Missing : INT;
    InvalidSelector : State[1] : INT;
    OutsideBounds : Values[4] : INT;
    MissingOutput : Resource1.Producer.Missing : INT;
    WrongType : State.Value : BOOL;
  END_VAR
END_CONFIGURATION
