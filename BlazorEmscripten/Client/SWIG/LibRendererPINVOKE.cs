//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 4.0.2
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


class LibRendererPINVOKE {


  static LibRendererPINVOKE() {
  }


  [global::System.Runtime.InteropServices.DllImport("LibRenderer", EntryPoint="CSharp_Renderer_m_shaders_loaded_set")]
  public static extern void Renderer_m_shaders_loaded_set(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("LibRenderer", EntryPoint="CSharp_Renderer_m_shaders_loaded_get")]
  public static extern int Renderer_m_shaders_loaded_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("LibRenderer", EntryPoint="CSharp_Renderer_getProgram")]
  public static extern uint Renderer_getProgram(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("LibRenderer", EntryPoint="CSharp_Renderer_init")]
  public static extern void Renderer_init(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("LibRenderer", EntryPoint="CSharp_new_Renderer")]
  public static extern global::System.IntPtr new_Renderer(int jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("LibRenderer", EntryPoint="CSharp_Renderer_change_cubemap")]
  public static extern void Renderer_change_cubemap(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("LibRenderer", EntryPoint="CSharp_Renderer_render")]
  public static extern void Renderer_render(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("LibRenderer", EntryPoint="CSharp_Renderer_Set_iMouse")]
  public static extern void Renderer_Set_iMouse(global::System.Runtime.InteropServices.HandleRef jarg1, float jarg2, float jarg3);

  [global::System.Runtime.InteropServices.DllImport("LibRenderer", EntryPoint="CSharp_Renderer_Set_iEnv")]
  public static extern void Renderer_Set_iEnv(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("LibRenderer", EntryPoint="CSharp_Renderer_Set_iTime")]
  public static extern void Renderer_Set_iTime(global::System.Runtime.InteropServices.HandleRef jarg1, float jarg2);

  [global::System.Runtime.InteropServices.DllImport("LibRenderer", EntryPoint="CSharp_delete_Renderer")]
  public static extern void delete_Renderer(global::System.Runtime.InteropServices.HandleRef jarg1);
}
