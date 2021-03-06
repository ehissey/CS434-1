// generated by Fast Light User Interface Designer (fluid) version 1.0110

#ifndef gui_h
#define gui_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>

class GUI {
public:
  GUI();
  Fl_Double_Window *uiw;
private:
  void cb_DBG_i(Fl_Button*, void*);
  static void cb_DBG(Fl_Button*, void*);
  void cb_Quit_i(Fl_Button*, void*);
  static void cb_Quit(Fl_Button*, void*);
  void cb_Pan_i(Fl_Button*, void*);
  static void cb_Pan(Fl_Button*, void*);
  void cb_Pan1_i(Fl_Button*, void*);
  static void cb_Pan1(Fl_Button*, void*);
  void cb_Tilt_i(Fl_Button*, void*);
  static void cb_Tilt(Fl_Button*, void*);
  void cb_Tilt1_i(Fl_Button*, void*);
  static void cb_Tilt1(Fl_Button*, void*);
  void cb_Roll_i(Fl_Button*, void*);
  static void cb_Roll(Fl_Button*, void*);
  void cb_Roll1_i(Fl_Button*, void*);
  static void cb_Roll1(Fl_Button*, void*);
  void cb_Trans_i(Fl_Button*, void*);
  static void cb_Trans(Fl_Button*, void*);
  void cb_Trans1_i(Fl_Button*, void*);
  static void cb_Trans1(Fl_Button*, void*);
  void cb_Trans2_i(Fl_Button*, void*);
  static void cb_Trans2(Fl_Button*, void*);
  void cb_Trans3_i(Fl_Button*, void*);
  static void cb_Trans3(Fl_Button*, void*);
  void cb_Trans4_i(Fl_Button*, void*);
  static void cb_Trans4(Fl_Button*, void*);
  void cb_Trans5_i(Fl_Button*, void*);
  static void cb_Trans5(Fl_Button*, void*);
  void cb_Zoom_i(Fl_Button*, void*);
  static void cb_Zoom(Fl_Button*, void*);
  void cb_Zoom1_i(Fl_Button*, void*);
  static void cb_Zoom1(Fl_Button*, void*);
public:
  Fl_Input *scaleFactor;
private:
  void cb_Rot_i(Fl_Button*, void*);
  static void cb_Rot(Fl_Button*, void*);
  void cb_Rot1_i(Fl_Button*, void*);
  static void cb_Rot1(Fl_Button*, void*);
  void cb_Rot2_i(Fl_Button*, void*);
  static void cb_Rot2(Fl_Button*, void*);
  void cb_Wireframe_i(Fl_Button*, void*);
  static void cb_Wireframe(Fl_Button*, void*);
  void cb_Wireframe1_i(Fl_Button*, void*);
  static void cb_Wireframe1(Fl_Button*, void*);
  void cb_Save_i(Fl_Button*, void*);
  static void cb_Save(Fl_Button*, void*);
  void cb_Load_i(Fl_Button*, void*);
  static void cb_Load(Fl_Button*, void*);
  void cb_Load1_i(Fl_Button*, void*);
  static void cb_Load1(Fl_Button*, void*);
  void cb_Save1_i(Fl_Button*, void*);
  static void cb_Save1(Fl_Button*, void*);
  void cb_Go_i(Fl_Button*, void*);
  static void cb_Go(Fl_Button*, void*);
  void cb_Go1_i(Fl_Button*, void*);
  static void cb_Go1(Fl_Button*, void*);
  void cb_Gouraud_i(Fl_Button*, void*);
  static void cb_Gouraud(Fl_Button*, void*);
  void cb_Phong_i(Fl_Button*, void*);
  static void cb_Phong(Fl_Button*, void*);
  void cb_Light_i(Fl_Button*, void*);
  static void cb_Light(Fl_Button*, void*);
  void cb_Light1_i(Fl_Button*, void*);
  static void cb_Light1(Fl_Button*, void*);
  void cb_Light2_i(Fl_Button*, void*);
  static void cb_Light2(Fl_Button*, void*);
  void cb_Light3_i(Fl_Button*, void*);
  static void cb_Light3(Fl_Button*, void*);
  void cb_Light4_i(Fl_Button*, void*);
  static void cb_Light4(Fl_Button*, void*);
  void cb_Light5_i(Fl_Button*, void*);
  static void cb_Light5(Fl_Button*, void*);
  void cb_Set_i(Fl_Button*, void*);
  static void cb_Set(Fl_Button*, void*);
  void cb_Set1_i(Fl_Button*, void*);
  static void cb_Set1(Fl_Button*, void*);
public:
  Fl_Input *ambientInput;
  Fl_Input *exponentInput;
private:
  void cb_Ref_i(Fl_Button*, void*);
  static void cb_Ref(Fl_Button*, void*);
  void cb_Ref1_i(Fl_Button*, void*);
  static void cb_Ref1(Fl_Button*, void*);
  void cb_Ref2_i(Fl_Button*, void*);
  static void cb_Ref2(Fl_Button*, void*);
  void cb_Ref3_i(Fl_Button*, void*);
  static void cb_Ref3(Fl_Button*, void*);
  void cb_Ref4_i(Fl_Button*, void*);
  static void cb_Ref4(Fl_Button*, void*);
  void cb_Ref5_i(Fl_Button*, void*);
  static void cb_Ref5(Fl_Button*, void*);
  void cb_Ref6_i(Fl_Button*, void*);
  static void cb_Ref6(Fl_Button*, void*);
  void cb_Ref7_i(Fl_Button*, void*);
  static void cb_Ref7(Fl_Button*, void*);
  void cb_Ref8_i(Fl_Button*, void*);
  static void cb_Ref8(Fl_Button*, void*);
  void cb_Ref9_i(Fl_Button*, void*);
  static void cb_Ref9(Fl_Button*, void*);
  void cb_Refa_i(Fl_Button*, void*);
  static void cb_Refa(Fl_Button*, void*);
  void cb_Refb_i(Fl_Button*, void*);
  static void cb_Refb(Fl_Button*, void*);
  void cb_Refc_i(Fl_Button*, void*);
  static void cb_Refc(Fl_Button*, void*);
  void cb_Refd_i(Fl_Button*, void*);
  static void cb_Refd(Fl_Button*, void*);
  void cb_Refe_i(Fl_Button*, void*);
  static void cb_Refe(Fl_Button*, void*);
  void cb_Reff_i(Fl_Button*, void*);
  static void cb_Reff(Fl_Button*, void*);
  void cb_Ref10_i(Fl_Button*, void*);
  static void cb_Ref10(Fl_Button*, void*);
  void cb_Ref11_i(Fl_Button*, void*);
  static void cb_Ref11(Fl_Button*, void*);
  void cb_Ref12_i(Fl_Button*, void*);
  static void cb_Ref12(Fl_Button*, void*);
  void cb_Ref13_i(Fl_Button*, void*);
  static void cb_Ref13(Fl_Button*, void*);
public:
  Fl_Input *refScaleFactor;
private:
  void cb_Real_i(Fl_Button*, void*);
  static void cb_Real(Fl_Button*, void*);
  void cb_Virtual_i(Fl_Button*, void*);
  static void cb_Virtual(Fl_Button*, void*);
  void cb_Effect_i(Fl_Button*, void*);
  static void cb_Effect(Fl_Button*, void*);
  void cb_Effect1_i(Fl_Button*, void*);
  static void cb_Effect1(Fl_Button*, void*);
  void cb_No_i(Fl_Button*, void*);
  static void cb_No(Fl_Button*, void*);
public:
  void show();
  void DBG_cb();
  void quit_cb();
  void panLeft_cb();
  void panRight_cb();
  void tiltLeft_cb();
  void tiltRight_cb();
  void rollForward_cb();
  void rollBackward_cb();
  void transLeft_cb();
  void transRight_cb();
  void transUp_cb();
  void transDown_cb();
  void transForward_cb();
  void transBackward_cb();
  void zoomIn_cb();
  void zoomOut_cb();
  void rotObjX_cb();
  void rotObjY_cb();
  void rotObjZ_cb();
  void wireframeOn_cb();
  void wireframeOff_cb();
  void saveCam1_cb();
  void loadCam1_cb();
  void saveCam2_cb();
  void loadCam2_cb();
  void goToCam1_cb();
  void goToCam2_cb();
  void gouraud_cb();
  void phong_cb();
  void lightLeft_cb();
  void lightRight_cb();
  void lightUp_cb();
  void lightDown_cb();
  void lightForward_cb();
  void lightBackward_cb();
  void setAmbient_cb();
  void setExponent_cb();
  void refPanLeft_cb();
  void refPanRight_cb();
  void refTiltLeft_cb();
  void refTiltRight_cb();
  void refRollForward_cb();
  void refRollBackward_cb();
  void refTransLeft_cb();
  void refTransRight_cb();
  void refTransUp_cb();
  void refTransDown_cb();
  void refTransForward_cb();
  void refTransBackward_cb();
  void refZoomIn_cb();
  void refZoomOut_cb();
  void refGoToCam1_cb();
  void refGoToCam2_cb();
  void refSaveCam1_cb();
  void refLoadCam1_cb();
  void refSaveCam2_cb();
  void refLoadCam2_cb();
  void real_cb();
  void virtual_cb();
  void effectOne_cb();
  void effectTwo_cb();
  void noEffect_cb();
};
#endif
