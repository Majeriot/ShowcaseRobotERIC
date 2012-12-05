
//------------------------------------------------------------------------------
// 
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// 
//------------------------------------------------------------------------------

// Generated from: messageExample.proto
namespace Magic.Sensors
{
    [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"ExampleMessage")]
    public partial class ExampleMessage : global::ProtoBuf.IExtensible
    {
      public ExampleMessage() {}
      
    private string _robot;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"robot", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string robot
    {
      get { return _robot; }
      set { _robot = value; }
    }
    private int _id;
    [global::ProtoBuf.ProtoMember(2, IsRequired = true, Name=@"id", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public int id
    {
      get { return _id; }
      set { _id = value; }
    }

    private string _name = "";
    [global::ProtoBuf.ProtoMember(3, IsRequired = false, Name=@"name", DataFormat = global::ProtoBuf.DataFormat.Default)][global::System.ComponentModel.DefaultValue("")]
    public string name
    {
      get { return _name; }
      set { _name = value; }
    }
    private readonly global::System.Collections.Generic.List<Magic.Sensors.ExampleMessage.StatusCode> _codes = new global::System.Collections.Generic.List<Magic.Sensors.ExampleMessage.StatusCode>();
    [global::ProtoBuf.ProtoMember(4, Name=@"codes", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public global::System.Collections.Generic.List<Magic.Sensors.ExampleMessage.StatusCode> codes
    {
      get { return _codes; }
    }
  
    [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"StatusCode")]
    public partial class StatusCode : global::ProtoBuf.IExtensible
    {
      public StatusCode() {}
      
    private string _number;
    [global::ProtoBuf.ProtoMember(1, IsRequired = true, Name=@"number", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public string number
    {
      get { return _number; }
      set { _number = value; }
    }

    private Magic.Sensors.ExampleMessage.SensorType _type = Magic.Sensors.ExampleMessage.SensorType.LIDAR;
    [global::ProtoBuf.ProtoMember(2, IsRequired = false, Name=@"type", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)][global::System.ComponentModel.DefaultValue(Magic.Sensors.ExampleMessage.SensorType.LIDAR)]
    public Magic.Sensors.ExampleMessage.SensorType type
    {
      get { return _type; }
      set { _type = value; }
    }
      private global::ProtoBuf.IExtension extensionObject;
      global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
        { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
    }
  
    public enum SensorType
    {
      LIDAR = 0,
    RADAR = 1,
    GPS = 2
    }
  
      private global::ProtoBuf.IExtension extensionObject;
      global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
        { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
    }
  
}