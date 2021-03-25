/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace RakNet {

using System;
using System.Runtime.InteropServices;

public class AddressOrGUID : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal AddressOrGUID(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(AddressOrGUID obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~AddressOrGUID() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          RakNetPINVOKE.delete_AddressOrGUID(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

	public static implicit operator AddressOrGUID(SystemAddress systemAddress)
	{
		return new AddressOrGUID(systemAddress);
	} 

	public static implicit operator AddressOrGUID(RakNetGUID guid)
	{
		return new AddressOrGUID(guid);
	} 



  public RakNetGUID rakNetGuid {
    set {
      RakNetPINVOKE.AddressOrGUID_rakNetGuid_set(swigCPtr, RakNetGUID.getCPtr(value));
    } 
    get {
      IntPtr cPtr = RakNetPINVOKE.AddressOrGUID_rakNetGuid_get(swigCPtr);
      RakNetGUID ret = (cPtr == IntPtr.Zero) ? null : new RakNetGUID(cPtr, false);
      return ret;
    } 
  }

  public SystemAddress systemAddress {
    set {
      RakNetPINVOKE.AddressOrGUID_systemAddress_set(swigCPtr, SystemAddress.getCPtr(value));
    } 
    get {
      IntPtr cPtr = RakNetPINVOKE.AddressOrGUID_systemAddress_get(swigCPtr);
      SystemAddress ret = (cPtr == IntPtr.Zero) ? null : new SystemAddress(cPtr, false);
      return ret;
    } 
  }

  public ushort GetSystemIndex() {
    ushort ret = RakNetPINVOKE.AddressOrGUID_GetSystemIndex(swigCPtr);
    return ret;
  }

  public bool IsUndefined() {
    bool ret = RakNetPINVOKE.AddressOrGUID_IsUndefined(swigCPtr);
    return ret;
  }

  public void SetUndefined() {
    RakNetPINVOKE.AddressOrGUID_SetUndefined(swigCPtr);
  }

  public static uint ToInteger(AddressOrGUID aog) {
    uint ret = RakNetPINVOKE.AddressOrGUID_ToInteger(AddressOrGUID.getCPtr(aog));
    if (RakNetPINVOKE.SWIGPendingException.Pending) throw RakNetPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string ToString(bool writePort) {
    string ret = RakNetPINVOKE.AddressOrGUID_ToString__SWIG_0(swigCPtr, writePort);
    return ret;
  }

  public void ToString(bool writePort, string dest, uint destLength) {
    RakNetPINVOKE.AddressOrGUID_ToString__SWIG_1(swigCPtr, writePort, dest, destLength);
  }

  public AddressOrGUID() : this(RakNetPINVOKE.new_AddressOrGUID__SWIG_0(), true) {
  }

  public AddressOrGUID(AddressOrGUID input) : this(RakNetPINVOKE.new_AddressOrGUID__SWIG_1(AddressOrGUID.getCPtr(input)), true) {
    if (RakNetPINVOKE.SWIGPendingException.Pending) throw RakNetPINVOKE.SWIGPendingException.Retrieve();
  }

  public AddressOrGUID(SystemAddress input) : this(RakNetPINVOKE.new_AddressOrGUID__SWIG_2(SystemAddress.getCPtr(input)), true) {
    if (RakNetPINVOKE.SWIGPendingException.Pending) throw RakNetPINVOKE.SWIGPendingException.Retrieve();
  }

  public AddressOrGUID(Packet packet) : this(RakNetPINVOKE.new_AddressOrGUID__SWIG_3(Packet.getCPtr(packet)), true) {
  }

  public AddressOrGUID(RakNetGUID input) : this(RakNetPINVOKE.new_AddressOrGUID__SWIG_4(RakNetGUID.getCPtr(input)), true) {
    if (RakNetPINVOKE.SWIGPendingException.Pending) throw RakNetPINVOKE.SWIGPendingException.Retrieve();
  }

  public AddressOrGUID CopyData(AddressOrGUID input) {
    AddressOrGUID ret = new AddressOrGUID(RakNetPINVOKE.AddressOrGUID_CopyData__SWIG_0(swigCPtr, AddressOrGUID.getCPtr(input)), false);
    if (RakNetPINVOKE.SWIGPendingException.Pending) throw RakNetPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public AddressOrGUID CopyData(SystemAddress input) {
    AddressOrGUID ret = new AddressOrGUID(RakNetPINVOKE.AddressOrGUID_CopyData__SWIG_1(swigCPtr, SystemAddress.getCPtr(input)), false);
    if (RakNetPINVOKE.SWIGPendingException.Pending) throw RakNetPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public AddressOrGUID CopyData(RakNetGUID input) {
    AddressOrGUID ret = new AddressOrGUID(RakNetPINVOKE.AddressOrGUID_CopyData__SWIG_2(swigCPtr, RakNetGUID.getCPtr(input)), false);
    if (RakNetPINVOKE.SWIGPendingException.Pending) throw RakNetPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public bool Equals(AddressOrGUID right) {
    bool ret = RakNetPINVOKE.AddressOrGUID_Equals(swigCPtr, AddressOrGUID.getCPtr(right));
    if (RakNetPINVOKE.SWIGPendingException.Pending) throw RakNetPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}

}
