NAME=vgui_controls
SRCROOT=../..
TARGET_PLATFORM=linux32
TARGET_PLATFORM_EXT=
USE_VALVE_BINDIR=0
PWD:=$(shell pwd)
# If no configuration is specified, "release" will be used.
ifeq "$(CFG)" ""
	CFG = release
endif

GCC_ExtraCompilerFlags=
GCC_ExtraLinkerFlags=
SymbolVisibility=hidden
OptimizerLevel=-gdwarf-2 -g2 $(OptimizerLevel_CompilerSpecific)
SystemLibraries=
DLL_EXT=.so
SYM_EXT=.dbg
FORCEINCLUDES= 
ifeq "$(CFG)" "debug"
DEFINES += -DVPC -DRAD_TELEMETRY_DISABLED -DDEBUG -D_DEBUG -DGNUC -DPOSIX -DCOMPILER_GCC -D_DLL_EXT=.so -D_LINUX -DLINUX -DPOSIX -D_POSIX -DBINK_VIDEO -DGL_GLEXT_PROTOTYPES -DDX_TO_GL_ABSTRACTION -DUSE_SDL -DDEV_BUILD -D_EXTERNAL_DLL_EXT=.so -DVPCGAMECAPS=HL2MP -DPROJECTDIR=/root/source2/vgui2/vgui_controls -D_DLL_EXT=.so -DSOURCE1=1 -DVPCGAME=hl2mp -D_LINUX=1 -D_POSIX=1 -DLINUX=1 -DPOSIX=1 
else
DEFINES += -DVPC -DRAD_TELEMETRY_DISABLED -DNDEBUG -DGNUC -DPOSIX -DCOMPILER_GCC -D_DLL_EXT=.so -D_LINUX -DLINUX -DPOSIX -D_POSIX -DBINK_VIDEO -DGL_GLEXT_PROTOTYPES -DDX_TO_GL_ABSTRACTION -DUSE_SDL -DDEV_BUILD -D_EXTERNAL_DLL_EXT=.so -DVPCGAMECAPS=HL2MP -DPROJECTDIR=/root/source2/vgui2/vgui_controls -D_DLL_EXT=.so -DSOURCE1=1 -DVPCGAME=hl2mp -D_LINUX=1 -D_POSIX=1 -DLINUX=1 -DPOSIX=1 
endif
INCLUDEDIRS += ../../common ../../public ../../public/tier0 ../../public/tier1 ../../thirdparty/SDL2 generated_proto ../../thirdparty/protobuf-2.3.0/src ../../thirdparty ../../thirdparty/cef generated_proto 
CONFTYPE=lib
OUTPUTFILE=../../lib/public/linux32/vgui_controls.a


POSTBUILDCOMMAND=true



CPPFILES= \
    ../../public/filesystem_helpers.cpp \
    ../../public/html/htmlprotobuf.cpp \
    ../../vgui2/src/vgui_key_translation.cpp \
    AnalogBar.cpp \
    AnimatingImagePanel.cpp \
    AnimationController.cpp \
    BitmapImagePanel.cpp \
    BuildFactoryHelper.cpp \
    BuildGroup.cpp \
    BuildModeDialog.cpp \
    Button.cpp \
    CheckButton.cpp \
    CheckButtonList.cpp \
    CircularProgressBar.cpp \
    ComboBox.cpp \
    consoledialog.cpp \
    ControllerMap.cpp \
    controls.cpp \
    cvartogglecheckbutton.cpp \
    DirectorySelectDialog.cpp \
    Divider.cpp \
    EditablePanel.cpp \
    ExpandButton.cpp \
    FileOpenDialog.cpp \
    FileOpenStateMachine.cpp \
    FocusNavGroup.cpp \
    Frame.cpp \
    generated_proto/htmlmessages.pb.cc \
    GraphPanel.cpp \
    HTML.cpp \
    Image.cpp \
    ImageList.cpp \
    ImagePanel.cpp \
    InputDialog.cpp \
    KeyBindingHelpDialog.cpp \
    KeyBoardEditorDialog.cpp \
    KeyRepeat.cpp \
    Label.cpp \
    ListPanel.cpp \
    ListViewPanel.cpp \
    Menu.cpp \
    MenuBar.cpp \
    MenuButton.cpp \
    MenuItem.cpp \
    MessageBox.cpp \
    MessageDialog.cpp \
    Panel.cpp \
    PanelListPanel.cpp \
    PerforceFileExplorer.cpp \
    PerforceFileList.cpp \
    perforcefilelistframe.cpp \
    ProgressBar.cpp \
    ProgressBox.cpp \
    PropertyDialog.cpp \
    PropertyPage.cpp \
    PropertySheet.cpp \
    QueryBox.cpp \
    RadioButton.cpp \
    RichText.cpp \
    RotatingProgressBar.cpp \
    savedocumentquery.cpp \
    ScalableImagePanel.cpp \
    ScrollableEditablePanel.cpp \
    ScrollBar.cpp \
    ScrollBarSlider.cpp \
    SectionedListPanel.cpp \
    Slider.cpp \
    Splitter.cpp \
    subrectimage.cpp \
    TextEntry.cpp \
    TextImage.cpp \
    ToggleButton.cpp \
    Tooltip.cpp \
    ToolWindow.cpp \
    TreeView.cpp \
    TreeViewListControl.cpp \
    URLLabel.cpp \
    WizardPanel.cpp \
    WizardSubPanel.cpp \


LIBFILES = \


LIBFILENAMES = \


# Include the base makefile now.
include $(SRCROOT)/devtools/makefile_base_posix.mak



OTHER_DEPENDENCIES = \
	generated_proto/htmlmessages.pb.cc \
	generated_proto/htmlmessages.pb.h


$(OBJ_DIR)/_other_deps.P : $(OTHER_DEPENDENCIES)
	$(GEN_OTHER_DEPS)

-include $(OBJ_DIR)/_other_deps.P



ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/filesystem_helpers.P
endif

$(OBJ_DIR)/filesystem_helpers.o : $(PWD)/../../public/filesystem_helpers.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/htmlprotobuf.P
endif

$(OBJ_DIR)/htmlprotobuf.o : $(PWD)/../../public/html/htmlprotobuf.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)
generated_proto/htmlmessages.pb.cc generated_proto/htmlmessages.pb.h : /root/source2/vgui2/chromehtml/htmlmessages.proto
	 @echo "Running Protocol Buffer Compiler on htmlmessages.proto...";mkdir -p $(OBJ_DIR) 2> /dev/null;mkdir generated_proto 2> /dev/null;../../gcsdk/bin/linux/protoc --proto_path=../../thirdparty/protobuf-2.3.0/src --proto_path=/root/source2/vgui2/chromehtml/ --proto_path=../../gcsdk --cpp_out=generated_proto /root/source2/vgui2/chromehtml/htmlmessages.proto


ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/vgui_key_translation.P
endif

$(OBJ_DIR)/vgui_key_translation.o : $(PWD)/../../vgui2/src/vgui_key_translation.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/AnalogBar.P
endif

$(OBJ_DIR)/AnalogBar.o : $(PWD)/AnalogBar.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/AnimatingImagePanel.P
endif

$(OBJ_DIR)/AnimatingImagePanel.o : $(PWD)/AnimatingImagePanel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/AnimationController.P
endif

$(OBJ_DIR)/AnimationController.o : $(PWD)/AnimationController.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/BitmapImagePanel.P
endif

$(OBJ_DIR)/BitmapImagePanel.o : $(PWD)/BitmapImagePanel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/BuildFactoryHelper.P
endif

$(OBJ_DIR)/BuildFactoryHelper.o : $(PWD)/BuildFactoryHelper.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/BuildGroup.P
endif

$(OBJ_DIR)/BuildGroup.o : $(PWD)/BuildGroup.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/BuildModeDialog.P
endif

$(OBJ_DIR)/BuildModeDialog.o : $(PWD)/BuildModeDialog.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/Button.P
endif

$(OBJ_DIR)/Button.o : $(PWD)/Button.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/CheckButton.P
endif

$(OBJ_DIR)/CheckButton.o : $(PWD)/CheckButton.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/CheckButtonList.P
endif

$(OBJ_DIR)/CheckButtonList.o : $(PWD)/CheckButtonList.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/CircularProgressBar.P
endif

$(OBJ_DIR)/CircularProgressBar.o : $(PWD)/CircularProgressBar.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ComboBox.P
endif

$(OBJ_DIR)/ComboBox.o : $(PWD)/ComboBox.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/consoledialog.P
endif

$(OBJ_DIR)/consoledialog.o : $(PWD)/consoledialog.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ControllerMap.P
endif

$(OBJ_DIR)/ControllerMap.o : $(PWD)/ControllerMap.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/controls.P
endif

$(OBJ_DIR)/controls.o : $(PWD)/controls.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/cvartogglecheckbutton.P
endif

$(OBJ_DIR)/cvartogglecheckbutton.o : $(PWD)/cvartogglecheckbutton.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/DirectorySelectDialog.P
endif

$(OBJ_DIR)/DirectorySelectDialog.o : $(PWD)/DirectorySelectDialog.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/Divider.P
endif

$(OBJ_DIR)/Divider.o : $(PWD)/Divider.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/EditablePanel.P
endif

$(OBJ_DIR)/EditablePanel.o : $(PWD)/EditablePanel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ExpandButton.P
endif

$(OBJ_DIR)/ExpandButton.o : $(PWD)/ExpandButton.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/FileOpenDialog.P
endif

$(OBJ_DIR)/FileOpenDialog.o : $(PWD)/FileOpenDialog.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/FileOpenStateMachine.P
endif

$(OBJ_DIR)/FileOpenStateMachine.o : $(PWD)/FileOpenStateMachine.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/FocusNavGroup.P
endif

$(OBJ_DIR)/FocusNavGroup.o : $(PWD)/FocusNavGroup.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/Frame.P
endif

$(OBJ_DIR)/Frame.o : $(PWD)/Frame.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/htmlmessages.pb.P
endif

$(OBJ_DIR)/htmlmessages.pb.o : $(PWD)/generated_proto/htmlmessages.pb.cc $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/GraphPanel.P
endif

$(OBJ_DIR)/GraphPanel.o : $(PWD)/GraphPanel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/HTML.P
endif

$(OBJ_DIR)/HTML.o : $(PWD)/HTML.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/Image.P
endif

$(OBJ_DIR)/Image.o : $(PWD)/Image.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ImageList.P
endif

$(OBJ_DIR)/ImageList.o : $(PWD)/ImageList.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ImagePanel.P
endif

$(OBJ_DIR)/ImagePanel.o : $(PWD)/ImagePanel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/InputDialog.P
endif

$(OBJ_DIR)/InputDialog.o : $(PWD)/InputDialog.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/KeyBindingHelpDialog.P
endif

$(OBJ_DIR)/KeyBindingHelpDialog.o : $(PWD)/KeyBindingHelpDialog.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/KeyBoardEditorDialog.P
endif

$(OBJ_DIR)/KeyBoardEditorDialog.o : $(PWD)/KeyBoardEditorDialog.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/KeyRepeat.P
endif

$(OBJ_DIR)/KeyRepeat.o : $(PWD)/KeyRepeat.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/Label.P
endif

$(OBJ_DIR)/Label.o : $(PWD)/Label.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ListPanel.P
endif

$(OBJ_DIR)/ListPanel.o : $(PWD)/ListPanel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ListViewPanel.P
endif

$(OBJ_DIR)/ListViewPanel.o : $(PWD)/ListViewPanel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/Menu.P
endif

$(OBJ_DIR)/Menu.o : $(PWD)/Menu.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/MenuBar.P
endif

$(OBJ_DIR)/MenuBar.o : $(PWD)/MenuBar.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/MenuButton.P
endif

$(OBJ_DIR)/MenuButton.o : $(PWD)/MenuButton.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/MenuItem.P
endif

$(OBJ_DIR)/MenuItem.o : $(PWD)/MenuItem.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/MessageBox.P
endif

$(OBJ_DIR)/MessageBox.o : $(PWD)/MessageBox.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/MessageDialog.P
endif

$(OBJ_DIR)/MessageDialog.o : $(PWD)/MessageDialog.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/Panel.P
endif

$(OBJ_DIR)/Panel.o : $(PWD)/Panel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/PanelListPanel.P
endif

$(OBJ_DIR)/PanelListPanel.o : $(PWD)/PanelListPanel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/PerforceFileExplorer.P
endif

$(OBJ_DIR)/PerforceFileExplorer.o : $(PWD)/PerforceFileExplorer.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/PerforceFileList.P
endif

$(OBJ_DIR)/PerforceFileList.o : $(PWD)/PerforceFileList.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/perforcefilelistframe.P
endif

$(OBJ_DIR)/perforcefilelistframe.o : $(PWD)/perforcefilelistframe.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ProgressBar.P
endif

$(OBJ_DIR)/ProgressBar.o : $(PWD)/ProgressBar.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ProgressBox.P
endif

$(OBJ_DIR)/ProgressBox.o : $(PWD)/ProgressBox.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/PropertyDialog.P
endif

$(OBJ_DIR)/PropertyDialog.o : $(PWD)/PropertyDialog.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/PropertyPage.P
endif

$(OBJ_DIR)/PropertyPage.o : $(PWD)/PropertyPage.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/PropertySheet.P
endif

$(OBJ_DIR)/PropertySheet.o : $(PWD)/PropertySheet.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/QueryBox.P
endif

$(OBJ_DIR)/QueryBox.o : $(PWD)/QueryBox.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/RadioButton.P
endif

$(OBJ_DIR)/RadioButton.o : $(PWD)/RadioButton.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/RichText.P
endif

$(OBJ_DIR)/RichText.o : $(PWD)/RichText.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/RotatingProgressBar.P
endif

$(OBJ_DIR)/RotatingProgressBar.o : $(PWD)/RotatingProgressBar.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/savedocumentquery.P
endif

$(OBJ_DIR)/savedocumentquery.o : $(PWD)/savedocumentquery.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ScalableImagePanel.P
endif

$(OBJ_DIR)/ScalableImagePanel.o : $(PWD)/ScalableImagePanel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ScrollableEditablePanel.P
endif

$(OBJ_DIR)/ScrollableEditablePanel.o : $(PWD)/ScrollableEditablePanel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ScrollBar.P
endif

$(OBJ_DIR)/ScrollBar.o : $(PWD)/ScrollBar.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ScrollBarSlider.P
endif

$(OBJ_DIR)/ScrollBarSlider.o : $(PWD)/ScrollBarSlider.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/SectionedListPanel.P
endif

$(OBJ_DIR)/SectionedListPanel.o : $(PWD)/SectionedListPanel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/Slider.P
endif

$(OBJ_DIR)/Slider.o : $(PWD)/Slider.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/Splitter.P
endif

$(OBJ_DIR)/Splitter.o : $(PWD)/Splitter.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/subrectimage.P
endif

$(OBJ_DIR)/subrectimage.o : $(PWD)/subrectimage.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/TextEntry.P
endif

$(OBJ_DIR)/TextEntry.o : $(PWD)/TextEntry.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/TextImage.P
endif

$(OBJ_DIR)/TextImage.o : $(PWD)/TextImage.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ToggleButton.P
endif

$(OBJ_DIR)/ToggleButton.o : $(PWD)/ToggleButton.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/Tooltip.P
endif

$(OBJ_DIR)/Tooltip.o : $(PWD)/Tooltip.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/ToolWindow.P
endif

$(OBJ_DIR)/ToolWindow.o : $(PWD)/ToolWindow.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/TreeView.P
endif

$(OBJ_DIR)/TreeView.o : $(PWD)/TreeView.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/TreeViewListControl.P
endif

$(OBJ_DIR)/TreeViewListControl.o : $(PWD)/TreeViewListControl.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/URLLabel.P
endif

$(OBJ_DIR)/URLLabel.o : $(PWD)/URLLabel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/WizardPanel.P
endif

$(OBJ_DIR)/WizardPanel.o : $(PWD)/WizardPanel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)

ifneq (clean, $(findstring clean, $(MAKECMDGOALS)))
-include $(OBJ_DIR)/WizardSubPanel.P
endif

$(OBJ_DIR)/WizardSubPanel.o : $(PWD)/WizardSubPanel.cpp $(PWD)/vgui_controls_linux32.mak $(SRCROOT)/devtools/makefile_base_posix.mak
	$(PRE_COMPILE_FILE)
	$(COMPILE_FILE) $(POST_COMPILE_FILE)
