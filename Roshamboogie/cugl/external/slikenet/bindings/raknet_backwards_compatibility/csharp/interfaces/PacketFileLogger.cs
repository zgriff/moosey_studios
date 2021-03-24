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

public class PacketFileLogger : PacketLogger {
  private HandleRef swigCPtr;

  internal PacketFileLogger(IntPtr cPtr, bool cMemoryOwn) : base(RakNetPINVOKE.PacketFileLogger_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(PacketFileLogger obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~PacketFileLogger() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          RakNetPINVOKE.delete_PacketFileLogger(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public new static PacketFileLogger GetInstance() {
    IntPtr cPtr = RakNetPINVOKE.PacketFileLogger_GetInstance();
    PacketFileLogger ret = (cPtr == IntPtr.Zero) ? null : new PacketFileLogger(cPtr, false);
    return ret;
  }

  public static void DestroyInstance(PacketFileLogger i) {
    RakNetPINVOKE.PacketFileLogger_DestroyInstance(PacketFileLogger.getCPtr(i));
  }

  public PacketFileLogger() : this(RakNetPINVOKE.new_PacketFileLogger(), true) {
  }

  public void StartLog(string filenamePrefix) {
    RakNetPINVOKE.PacketFileLogger_StartLog(swigCPtr, filenamePrefix);
  }

  public override void WriteLog(string str) {
    RakNetPINVOKE.PacketFileLogger_WriteLog(swigCPtr, str);
  }

}

}
