TYPE
  SampleArray : ARRAY [1..3] OF INT;
  DeviceState : STRUCT
    Value : INT;
    Samples : SampleArray;
  END_STRUCT;
END_TYPE

PROGRAM AccessProducer
  VAR_OUTPUT
    Snapshot : DeviceState;
  END_VAR
  Snapshot.Value := Snapshot.Value;
END_PROGRAM

CONFIGURATION HierarchicalAccessConfig
  VAR_GLOBAL
    State : DeviceState;
    Values : SampleArray;
  END_VAR

  RESOURCE Resource1 ON PLC
    VAR_GLOBAL
      LocalState : DeviceState;
    END_VAR
    TASK MainTask(INTERVAL := T#1000ms, PRIORITY := 0);
    PROGRAM Producer WITH MainTask : AccessProducer;
  END_RESOURCE

  VAR_ACCESS
    ConfigField : State.Value : INT READ_WRITE;
    ConfigArray : Values[2] : INT;
    NestedArray : State.Samples[3] : INT;
    ResourceField : Resource1.LocalState.Value : INT;
    ProgramField : Resource1.Producer.Snapshot.Value : INT;
  END_VAR
END_CONFIGURATION
